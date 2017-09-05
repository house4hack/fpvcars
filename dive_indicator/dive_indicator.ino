// NeoPixel Ring simple sketch (c) 2013 Shae Erisson
// released under the GPLv3 license to match the rest of the AdaFruit NeoPixel library

#include <Adafruit_NeoPixel.h>
#ifdef __AVR__
  #include <avr/power.h>
#endif

#define PIN            0

#define NUMPIXELS      1
#define RC_PIN 2
Adafruit_NeoPixel pixels = Adafruit_NeoPixel(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);

#define UP 0
#define DOWN 1

#define NORMAL 3

#define OFF 0
#define ON 1


int state = NORMAL;

void setup() {
  // This is for Trinket 5V 16MHz, you can remove these three lines if you are not using a Trinket
#if defined (__AVR_ATtiny85__)
  if (F_CPU == 16000000) clock_prescale_set(clock_div_1);
#endif
  // End of trinket special code

  pinMode(RC_PIN,INPUT);

  pixels.begin(); // This initializes the NeoPixel library.

    pixels.setPixelColor(0, pixels.Color(0,0,0)); // Moderately bright green color.
   // pixels.setPixelColor(1, pixels.Color(0,0,0)); // Moderately bright green color.
    pixels.show(); // This sends the updated pixel color to the hardware.    

}



void loop() {

  int time = pulseIn(RC_PIN,HIGH, 30000);
  //int time = PRIGHT;
  if(time >1000 && time<2000){
    if(time > 1600.0f){
      state = UP;
    } else if (time< 1400.0f){
      state = DOWN;
    } else {
      state = NORMAL;
    }
  }  
  if(state == UP){
    pixels.setPixelColor(0, pixels.Color(0,255,0)); // Moderately bright green color.
    pixels.show(); // This sends the updated pixel color to the hardware.    
  }
  if(state == DOWN){
    pixels.setPixelColor(0, pixels.Color(255,0,0)); // Moderately bright green color.
    pixels.show(); // This sends the updated pixel color to the hardware.    
  }
  if(state == NORMAL){
    pixels.setPixelColor(0, pixels.Color(0,0,0)); // Moderately bright green color.
    pixels.show(); // This sends the updated pixel color to the hardware.    
  } 
  

}
