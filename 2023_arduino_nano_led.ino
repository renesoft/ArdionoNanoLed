#include <FastLED.h>
#define NUM_LEDS 256
#define LED_PIN2 7

#define coal_pin 2
#define ranbow_pin 3
#define dotbeat_pin 4

#define IN_coal_pin A2
#define IN_ranbow_pin A3
#define IN_dotbeat_pin A4

#define COLOR_ORDER GRB
#define CHIPSET WS2811
#define BRIGHTNESS 255
#define FRAMES_PER_SECOND 12

CRGB leds[NUM_LEDS];
uint8_t ledsVector [NUM_LEDS];
CRGBPalette16 currentPalette;

uint8_t thisdelay = 5;
unsigned long time;
unsigned long timeLast = 0;
unsigned long timeTotal = 0;
uint8_t bpm = 30;
uint8_t fadeval = 224;
uint8_t thishue = 0;    // Starting hue value.
uint8_t deltahue = 10;  // Hue change between pixels.

uint8_t mode_coal_enable = 0 ;
uint8_t mode_rambow_enable = 0 ;
uint8_t mode_beat_enable = 0 ;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  delay(3000);  // sanity delay
  randomSeed(analogRead(A1));
  digitalWrite(coal_pin,1);
  digitalWrite(ranbow_pin,1);
  digitalWrite(dotbeat_pin,1);
  //FastLED.addLeds<CHIPSET, LED_PIN, COLOR_ORDER>(leds, NUM_LEDS).setCorrection( TypicalLEDStrip );
  //FastLED.addLeds<CHIPSET, LED_PIN2, COLOR_ORDER>(leds, NUM_LEDS).setCorrection( TypicalLEDStrip );
  FastLED.addLeds<CHIPSET, LED_PIN2, COLOR_ORDER>(leds, NUM_LEDS).setCorrection(TypicalLEDStrip);
  FastLED.setBrightness(BRIGHTNESS);

  currentPalette = CRGBPalette16(
    CRGB::Black, CRGB::Black, CRGB::Black, CHSV(0, 255, 4),
    CHSV(0, 255, 8), CRGB::Red, CRGB::Red, CRGB::Red,
    CRGB::DarkOrange, CRGB::Orange, CRGB::Orange, CRGB::Orange,
    CRGB::Yellow, CRGB::Yellow, CRGB::Gray, CRGB::Gray);

   
   if (analogRead(IN_coal_pin)>1000) mode_coal_enable = 1;
   if (analogRead(IN_ranbow_pin)>1000) mode_rambow_enable = 1;
   if (analogRead(IN_dotbeat_pin)>1000) mode_beat_enable = 1;
   Serial.println(mode_coal_enable);
   Serial.println(mode_rambow_enable);
   Serial.println(mode_beat_enable);
   
}

void SHOW() {
  FastLED.show();
}

void reset() {
  for (int j = 0; j < NUM_LEDS; j++) {
    leds[j].r = 0;
    leds[j].b = 0;
    leds[j].g = 0;
  }
  //FastLED.show();
  SHOW();
}

void doTime() {
  time = millis();
  if (timeLast == 0) timeLast = time;
  timeTotal += time - timeLast;
  timeLast = time;
}
void timeReset() {
  timeTotal = 0;
}
int timeTotalSec() {
  return timeTotal / 1000;
}

void coals() {
  for (int j = 0; j < NUM_LEDS; j++) {    
    if (leds[j].r > 150) {
      ledsVector[j] = random(-3, -1);
    }
    if (leds[j].r < 4) {
      ledsVector[j] = random(1, 3);
      //ledShift[j] = random(-3,3);
    }

    if (ledsVector[j] == 0) {
      ledsVector[j] = random(-3, 3);
      leds[j].r = 128;
      //leds[j].b = 25;
      if (ledsVector[j] == 0)
        ledsVector[j] = 1;
    }

    leds[j].r += ledsVector[j];
  }
}

void rainbow_beat() {

  uint8_t beatA = beatsin8(17, 0, 255);  // Starting hue
  uint8_t beatB = beatsin8(13, 0, 255);
  fill_rainbow(leds, NUM_LEDS, (beatA + beatB) / 2, 8);  // Use FastLED's fill_rainbow routine.

}  // rainbow_beat()

void rainbow_march() {                              // The fill_rainbow call doesn't support brightness levels
  thishue++;                                        // Increment the starting hue.
  fill_rainbow(leds, NUM_LEDS, thishue, deltahue);  // Use FastLED's fill_rainbow routine.
}  // rainbow_march()

void dot_beat() {

  uint8_t inner = beatsin8(bpm, NUM_LEDS / 4, NUM_LEDS / 4 * 3);   // Move 1/4 to 3/4
  uint8_t outer = beatsin8(bpm, 0, NUM_LEDS - 1);                  // Move entire length
  uint8_t middle = beatsin8(bpm, NUM_LEDS / 3, NUM_LEDS / 3 * 2);  // Move 1/3 to 2/3

  leds[middle] = CRGB::Purple;
  leds[inner] = CRGB::Blue;
  leds[outer] = CRGB::Aqua;

  nscale8(leds, NUM_LEDS, fadeval);  // Fade the entire array. Or for just a few LED's, use  nscale8(&leds[2], 5, fadeval);

}  // dot_beat()


void loop() {
  int mode = random(0, 5);
  //mode = 4;
  Serial.print("Mode:");
  Serial.println(mode);
  if (( mode == 1 || mode == 0 ) && (mode_beat_enable == 1) ) {
    while (timeTotalSec() < 20) {
      doTime();
      rainbow_beat();
      //FastLED.show();
      SHOW();
    }
    timeReset();
    return;
  }

  if ( (mode == 2 || mode == 3) &&  (mode_rambow_enable == 1)) {
    while (timeTotalSec() < 20) {
      doTime();
      EVERY_N_MILLISECONDS(thisdelay) {  // FastLED based non-blocking routine to update/display the sequence.
        rainbow_march();
      }
      SHOW();
      //FastLED.show();
    }
    timeReset();
    return;
  }

  if (mode == 4 && mode_coal_enable == 1) {
    reset();
    FastLED.setBrightness(BRIGHTNESS);
    while (timeTotalSec() < 70) {
      doTime();
      EVERY_N_MILLISECONDS(100) {
        coals();
      }
      if (BRIGHTNESS - (timeTotalSec() * 2) < 10)
        break;
        //Serial.println(leds[0].r);
      FastLED.setBrightness(BRIGHTNESS - (timeTotalSec() * 2));
      //FastLED.show();
      SHOW();
    }
    FastLED.setBrightness(BRIGHTNESS);
    timeReset();
    return;
  }
}
