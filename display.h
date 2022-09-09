// Adapter class for the OLED-display to work with RPi Pico
// adapter to enable compatibility between ESP8266/ESP32 and RPi Pico
// Programmer: Roger Buehler, tscha70@gmail.com, 2012/May/14
// Code: https://github.com/tscha70/MegaGamesCompilationPicoAdafruit

#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// move the hardware settings to one place
#define SCREEN_PIN_SOUND    10
#define SCEEN_BUTTON_START  11
#define SCREEN_BUTTON_RIGHT 12
#define SCREEN_BUTTON_DOWN  14
#define SCREEN_BUTTON_LEFT  13
#define SCREEN_BUTTON_UP    15

#define BUFFER_LENGTH 1024
#define PIXELWIDTH 128

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

// Declaration for SSD1306 display connected using software SPI (default case):
#define OLED_MOSI  19
#define OLED_CLK   18
#define OLED_DC    16
#define OLED_CS    17
#define OLED_RESET 20
Adafruit_SSD1306 _adafruitDisplay(SCREEN_WIDTH, SCREEN_HEIGHT, OLED_MOSI, OLED_CLK, OLED_DC, OLED_RESET, OLED_CS);

// emulator class in order to emulate the behavior of the SSD1306 library with the U8G2 library
class DisplayEmulator
{

  public:
    uint8_t buffer[BUFFER_LENGTH];

    void display(void)
    {
      _convertVerticalToHorizontalBitmap();
      _adafruitDisplay.clearDisplay();
      _adafruitDisplay.drawBitmap(0, 0, _bitmap, SCREEN_WIDTH, SCREEN_HEIGHT, WHITE);
      _adafruitDisplay.display();
    }

    void init(void)
    {
      _adafruitDisplay.begin(SSD1306_SWITCHCAPVCC);
      _adafruitDisplay.clearDisplay();
    }

    void flipScreenVertically()
    {
      _adafruitDisplay.setRotation(180);
    }

    void setContrast(uint8_t contrast)
    {
      // do nothing
    }

  private:
    uint8_t _bitmap[BUFFER_LENGTH];

    // Bibsi Babsi Algo: (this method converts the vertical bitmap into horizontal bitmap)
    void _convertVerticalToHorizontalBitmap()
    {
      const uint8_t COLS = PIXELWIDTH / 8;
      uint8_t mask;

      // initialize the array (_bitmap)
      for (int r = 0; r < BUFFER_LENGTH; r++) _bitmap[r] = 0;

      for (uint8_t y = 0; y < BUFFER_LENGTH / PIXELWIDTH; y++)
      {
        // for a byte scan all 8 lines
        for (uint8_t line = 0; line < 8; line++)
        {
          // mask must be shifted for each line
          mask = 0x01 << line;

          // scan all pixels from left to right and then shift one line down
          for (uint8_t pix = 0; pix < PIXELWIDTH; pix++)
          {
            // add up the bits for the new byte (vertical to horizontal transformation)
            _bitmap[(pix / 8) + (line * COLS) + (y * PIXELWIDTH)] += ( (buffer[pix + (y * PIXELWIDTH)] & mask) != 0) << (7 - (pix % 8));
          }
        }
      }
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
