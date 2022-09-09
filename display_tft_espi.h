// Adapter class for the OLED-display to work with RPi Pico
// adapter to enable compatibility between ESP8266/ESP32 and RPi Pico
// Programmer: Roger Buehler, tscha70@gmail.com, 2012/May/14
// Code: https://github.com/tscha70/MegaGamesCompilationPicoAdafruit

#include <SPI.h>
#include <TFT_eSPI.h>       // Hardware-specific library
TFT_eSPI tft = TFT_eSPI();  // Invoke custom library

// This is the game defined display buffer - do not mess with this...
#define BUFFER_LENGTH 1024
#define PIXEL_WIDTH 128
#define PIXEL_HEIGHT 64

// get the screen dimensions and set rotation to display in landscape mode
#if TFT_WIDTH > TFT_HEIGHT
// already in landscape
#define SCREEN_WIDTH TFT_WIDTH
#define SCREEN_HEIGHT TFT_HEIGHT
#define SCREEN_ROTATE 0
#else
// portrait display - swap dimensions and request rotation
#define SCREEN_WIDTH TFT_HEIGHT
#define SCREEN_HEIGHT TFT_WIDTH
#define SCREEN_ROTATE 1
#endif

// define buttons here to keep all platform specific stuff in one place
// these are for WaveShare PicoLCD 1.14
// If using these, you may want to copy the included User_Setup_Pico_LCD_114.h to your TFT_eSPI library folder
// and set up 
#define SCREEN_PIN_SOUND    0
#define SCEEN_BUTTON_START  15
#define SCREEN_BUTTON_RIGHT 20
#define SCREEN_BUTTON_DOWN  18
#define SCREEN_BUTTON_LEFT  16
#define SCREEN_BUTTON_UP    2

class DisplayEmulator
{
  private:
    uint16_t _img[SCREEN_WIDTH * 2]; // line buffer for decoded display lines (2 off for double buffering)

  public:
    uint8_t buffer[BUFFER_LENGTH];

    void display(void)
    {
      // abuse Besenham for scaling (LIMITED ALGORITHM ONLY SCALES UP!!!)
      int32_t Dsy = (2 * (PIXEL_HEIGHT - 1)) - (SCREEN_HEIGHT - 1);
      uint32_t py = 0;
      uint32_t lpy = 1;
      uint16_t * img;

      // itterate through every screen row
      for(uint32_t sy = 0; sy < SCREEN_HEIGHT; sy++)
      {
        // the scaling algorithm may pick the same soure row twice when scaling up - only decode when we get a new row
        if(py != lpy) {
          // pick next line buffer to render into
          img = (img == _img) ? _img + SCREEN_WIDTH : _img; // before we render a new line, pick the next buffer

          // convert pixel row to offset and bitmask
          uint8_t mask = 0x01 << (py % 8);
          uint32_t py8W = (py / 8) * PIXEL_WIDTH;

          // calculate Bresenham paramteres for row scaling
          int32_t Dsx = (2 * (PIXEL_WIDTH-1)) - (SCREEN_WIDTH-1);
          uint32_t px = 0;
          uint32_t lpx = 1;

          // itterate through every screen prow pixel
          for(uint32_t sx = 0; sx < SCREEN_WIDTH; sx++)
          {
            // if it is a new source pixel, pick it from the source buffer
            if(px != lpx)
            {
              img[sx] = (buffer[px + py8W] & mask) ? TFT_WHITE : TFT_BLACK;
              lpx = px;
            }
            else
            {
              // otherwise, just copy the previous decode
              img[sx] = img[sx - 1];
            }
            // get the new source pixel
            if(Dsx > 0) {
                px++;
                Dsx -= 2 * (SCREEN_WIDTH-1);
            }
            Dsx += 2 * (PIXEL_WIDTH-1);
          }
          lpy = py;
        }
        
        // render the row
        // these options include (needless) windowing and clipping - rather render them directly
        //tft.pushImage(0, sy, SCREEN_WIDTH, 1, _img);
        //tft.pushImageDMA(0, sy, SCREEN_WIDTH, 1, _img);

        // wait for previous row dma to complete (setWindow writes to SPI bus too)
        tft.dmaWait();
        // start new SPI session
        tft.startWrite();
        // set the window to the target row
        tft.setWindow(0, sy, SCREEN_WIDTH - 1, sy);
        // fire up the DMA engine
        tft.pushPixelsDMA(img, SCREEN_WIDTH);
        //tft.pushPixels(img, SCREEN_WIDTH);
        // finish the SPI session
        tft.endWrite();

        // get the next source row
        if(Dsy > 0) {
            py++;
            Dsy -= 2 * (SCREEN_HEIGHT-1);
        }
        Dsy += 2 * (PIXEL_HEIGHT-1);
      }
    }

    void init(void)
    {
      tft.init();
      tft.initDMA();
      // you may need to add 2 here to invert the display
      tft.setRotation(SCREEN_ROTATE);
      //tft.setRotation(SCREEN_ROTATE + 2);
      tft.fillScreen(TFT_BLACK);
    }

    void flipScreenVertically()
    {
      // do nothing
    }

    void setContrast(uint8_t contrast)
    {
      // do nothing
    }
};

// just a dummy in order to keep the existing code running
class ESPDummy
{
  public:
    void wdtFeed(void)
    {
      // do nothing;
    }
    void wdtDisable(void)
    {
      // do nothing
    }

};
