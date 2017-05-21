#include <FastLED.h>
#define DATA_PIN 5
#define NUM_LEDS 4

CRGB leds[NUM_LEDS];
String animation = "twinkle";

//Manual settings
typedef struct {
  uint8_t hue;
  uint8_t sat;
  uint8_t val;
  int8_t delay;
  int8_t fadeAmount;
} ManualSettings;
ManualSettings mSettings;

///////////////// Setup /////////////////////////////////
void setup() {
    FastLED.addLeds<NEOPIXEL, DATA_PIN>(leds, NUM_LEDS);
    mSettings = {196, 255, 255, 20, 2};
    Serial.begin(115200);
  }

///////////////////Main loop //////////////////////////////
void loop() {
  if ( animation.equals("pulsade")){
    pulseColor();
  }
  else if( animation.equals("twinkle")){
    twinkle();
  }
  else if (animation.equals("manual")){
    manual();
  }
  if (Serial.available() > 0){
    String btData;
    btData = Serial.readStringUntil(';');
    Serial.print(btData);
    procesBtData(btData);
  }
}

//////////////// Pulsade effect //////////////////////////
int16_t currentVal = 0;

void pulseColor() {

  currentVal += mSettings.fadeAmount;
  if (currentVal > 255) { currentVal = 255;}
  if (currentVal < 0) { currentVal = 0;}
  if(currentVal <= 0 || currentVal >= 255)
  {
    mSettings.fadeAmount = -mSettings.fadeAmount ; 
  }
  for(int i = 0; i < NUM_LEDS; i++ )
  {
   leds[i].setHSV(mSettings.hue,mSettings.sat,currentVal);  // Set Color HERE!!!
  }
  FastLED.show();
  delay(mSettings.delay);
}

/////////////////// Twinkel effect /////////////////////////
typedef struct {
  int16_t current;
  int16_t target;
  int8_t rate;
} TwinkleStars;

TwinkleStars tStars[NUM_LEDS];

inline bool rand_new_led() {
  return random16(1000) < 7;
}

void twinkle(){
  for ( int i=0; i < NUM_LEDS; i++){
    if (tStars[i].current == 0) {
      if (rand_new_led()){
        tStars[i].current = 1;
        tStars[i].rate = random8(2,8);
        tStars[i].target = random8(96, 255);
      }
    }
    else {
      tStars[i].current += tStars[i].rate;
      if (tStars[i].current <= 0) {
        tStars[i].current = 0;
      }
      else if (tStars[i].current > tStars[i].target){
        tStars[i].current = tStars[i].target;
        tStars[i].rate = -tStars[i].rate;      
      }
    }
  }

  for (int i=0; i < NUM_LEDS; i++){
    leds[i] = CHSV(mSettings.hue, mSettings.sat, tStars[i].current);
  }

  FastLED.show();
  FastLED.delay(mSettings.delay);
}

////////////////////// Manual mode /////////////////////////////
void manual() {
  for ( int i=0; i < NUM_LEDS; i++){
    leds[i] = CHSV(mSettings.hue, mSettings.sat, mSettings.val);
  }
  FastLED.show();
}

/////////////////Proces the bluetooth data received//////////////////
void procesBtData(String data){
  if (data.startsWith("mode")){
    data.remove(0,4);
    animation = data;
  }
    if (data.startsWith("hue")){
    data.remove(0,3);
    mSettings.hue = data.toInt();
  }
    if (data.startsWith("sat")){
    data.remove(0,3);
    mSettings.sat = data.toInt();
  }
    if (data.startsWith("val")){
    data.remove(0,3);
    mSettings.val = data.toInt();
  }
}

