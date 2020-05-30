#include <FastLED.h>

#define NUM_LEDS 95

// Data pin that led data will be written out over
#define DATA_PIN 3

// Clock pin only needed for SPI based chipsets when not using hardware SPI
#define CLOCK_PIN 13

// If its's too bright with all the LEDS you can skip every nth led
#define EVERY_NTH 1

// This is an array of leds.  One item for each led in your strip.
CRGB leds[NUM_LEDS];

// specified under `rate` in the `[device]` section of /etc/boblight.conf
#define serialRate 115200

// boblightd sends a prefix (defined in /etc/boblight.conf) before sending the pixel data
uint8_t prefix[] = {0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA};

void setup() {
  delay(1000);
  FastLED.addLeds<WS2801, DATA_PIN, CLOCK_PIN, RGB>(leds, NUM_LEDS);
  Serial.begin(serialRate);

  turnoff_all_leds();
  light_leds(NUM_LEDS, 0xff, 0x00, 0x07);
  delay(1000);
  light_leds(NUM_LEDS, 0, 0xff, 0x00);
  delay(1000);
  light_leds(NUM_LEDS, 0, 0, 0xff);
  delay(1000);
  slideshow_animation();
}

void loop() {      
  wait_prefix();
  
  turnoff_all_leds();
  
  read_rgb_data();
  
  LEDS.show();
}

void turnoff_all_leds() {
  memset(leds, 0, NUM_LEDS * sizeof(struct CRGB));  
}

void slideshow_animation() {
  for(int whiteLed = 0; whiteLed < NUM_LEDS; whiteLed = whiteLed + 1) {
      // Turn our current led on to white, then show the leds
      leds[whiteLed] = CRGB::White;

      // Show the leds (only one of which is set to white, from above)
  
      FastLED.show();
      // Wait a little bit
      delay(10);

      // Turn our current led back to black for the next loop around
      leds[whiteLed] = CRGB::Black;         
   }   
}
void light_leds(int nb_leds, CRGB color) { 
    for(int i = 0 ; i < nb_leds ; i++) {
        leds[i] = color;
    }
    FastLED.show();
}

void light_leds(int nb_leds, int red, int green, int blue) {
    for(int i = 0 ; i < nb_leds ; i++) {
        leds[i].r = red;
        leds[i].g = green;
        leds[i].b = blue;
    }
    FastLED.show();
}

void blink_leds(int nb_leds, CRGB color) {
  light_leds(nb_leds, CRGB::Red);
  
  // Wait a little bit
  delay(500);

  turnoff_all_leds();
  delay(500);  
}

void wait_prefix() {
  // wait until we see the prefix
  for(int i = 0; i < sizeof prefix; ++i) {
    waitLoop: while (!Serial.available()) ;;
    
    // look for the next byte in the sequence if we see the one we want
    if(prefix[i] == Serial.read()) continue;
    
    // otherwise, start over
    i = 0;
    goto waitLoop;
  }
}

void read_rgb_data() {
    for (int iLed = 0; iLed < NUM_LEDS; iLed++) {
    byte r, g, b;    
    while(!Serial.available());
    r = Serial.read();
    while(!Serial.available());
    g = Serial.read();
    while(!Serial.available());
    b = Serial.read();
    if(!(iLed % EVERY_NTH)) {
      leds[iLed].r = r;
      leds[iLed].g = g;
      leds[iLed].b = b;
    }
  }
}

int StrToHex(char str[])
{
  return (int) strtol(str, 0, 16);
}
