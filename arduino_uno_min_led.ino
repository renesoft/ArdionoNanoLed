#include <FastLED.h>

#define NUM_LEDS 13
#define LED_PIN2 7
#define CHIPSET WS2811
#define COLOR_ORDER GRB
CRGB leds[NUM_LEDS];
int potPin = A3; // Potentiometer output connected to analog pin 3
int potVal = 0; 

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  FastLED.addLeds<CHIPSET, LED_PIN2, COLOR_ORDER>(leds, NUM_LEDS).setCorrection(TypicalLEDStrip);
  FastLED.setBrightness(255);
}

void loop() {
  for (int i = 0 ; i < 1 ; i++){
    if (i%3==0){
      leds[i].r = 255;
      leds[i].b = 0;
      leds[i].g = 0;
    }
    if (i%3==1){
      leds[i].r = 0;
      leds[i].b = 255;
      leds[i].g = 0;
    }
    if (i%3==2){
      leds[i].r = 0;
      leds[i].b = 0;
      leds[i].g = 255;
    }
  }
  FastLED.show();
  // put your main code here, to run repeatedly:
 // potVal = analogRead(potPin);
 // Serial.println(potVal);
 // delay (100);
}
