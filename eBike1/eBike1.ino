#include <Adafruit_NeoPixel.h>
#ifdef __AVR__
  #include <avr/power.h>
#endif

#define PIN 8
#define buttonPin 2
#define piezoPin 9

Adafruit_NeoPixel strip = Adafruit_NeoPixel(47, PIN, NEO_RGB + NEO_KHZ800);

// Generally, you should use "unsigned long" for variables that hold time
// The value will quickly become too large for an int to store
unsigned long previousMillis = 0;        // will store last time LED was updated

// constants won't change :
const long interval = 100;           // interval at which to play notes

int dixieNotes[] = {784, 659, 523, 0, 523, 0, 523, 587, 659, 698, 783, 0, 783, 0, 783, 0, 659, 0, 0, 0, 0, 0, 0, 0};
int cucarachaNotes[] = {261, 0, 261, 0, 261, 349, 349, 440, 0, 
                        261, 0, 261, 0, 261, 349, 349, 440, 0, 
                        349, 0, 349, 0, 329, 0, 329, 0, 
                        293, 0, 293, 261, 261, 0, 0, 0, 0, 0, 0, 0, 0};
byte currentNote = 0;

byte rainbowCycleVar = 0;

void setup() {
  // This is for Trinket 5V 16MHz, you can remove these three lines if you are not using a Trinket
  #if defined (__AVR_ATtiny85__)
    if (F_CPU == 16000000) clock_prescale_set(clock_div_1);
  #endif
  // End of trinket special code
  pinMode(buttonPin, INPUT);
  pinMode(piezoPin, OUTPUT);
  tone(piezoPin, 784, 100);
  //delay(100);
  strip.begin();
  strip.show(); // Initialize all pixels to 'off'
}

void loop() {
  rainbowCycle(rainbowCycleVar);
  int buttonState = digitalRead(buttonPin);

 
  if (buttonState == HIGH) {
    playSong();
    
  } else {
    // turn LED off:
    noTone(piezoPin);
    currentNote = 0;
  }
  rainbowCycleVar++;
  delay(20);
}

void playSong() {
  unsigned long currentMillis = millis();

  if (currentMillis - previousMillis >= interval) {
    // save the last time you blinked the LED
    previousMillis = currentMillis;
    int note = cucarachaNotes[currentNote];
    tone(piezoPin, note);
    currentNote++;
    if(currentNote >= (sizeof(cucarachaNotes)/sizeof(int))) currentNote = 0;
    if(!note) previousMillis += 100;
  }
}

// Slightly different, this makes the rainbow equally distributed throughout
void rainbowCycle(uint8_t wait) {
  uint16_t i, j;
  for(i=0; i< strip.numPixels(); i++) {
    strip.setPixelColor(i, Wheel(((i * 256 / strip.numPixels()) + rainbowCycleVar) & 255));
  }
  strip.show();
}

//Theatre-style crawling lights.
void theaterChase(uint32_t c, uint8_t wait) {
  for (int j=0; j<10; j++) {  //do 10 cycles of chasing
    for (int q=0; q < 3; q++) {
      for (int i=0; i < strip.numPixels(); i=i+3) {
        strip.setPixelColor(i+q, c);    //turn every third pixel on
      }
      strip.show();

      delay(wait);

      for (int i=0; i < strip.numPixels(); i=i+3) {
        strip.setPixelColor(i+q, 0);        //turn every third pixel off
      }
    }
  }
}

//Theatre-style crawling lights with rainbow effect
void theaterChaseRainbow(uint8_t wait) {
  for (int j=0; j < 256; j++) {     // cycle all 256 colors in the wheel
    for (int q=0; q < 3; q++) {
      for (int i=0; i < strip.numPixels(); i=i+3) {
        strip.setPixelColor(i+q, Wheel( (i+j) % 255));    //turn every third pixel on
      }
      strip.show();

      delay(wait);

      for (int i=0; i < strip.numPixels(); i=i+3) {
        strip.setPixelColor(i+q, 0);        //turn every third pixel off
      }
    }
  }
}

// Input a value 0 to 255 to get a color value.
// The colours are a transition r - g - b - back to r.
uint32_t Wheel(byte WheelPos) {
  WheelPos = 255 - WheelPos;
  if(WheelPos < 85) {
    return strip.Color(255 - WheelPos * 3, 0, WheelPos * 3);
  }
  if(WheelPos < 170) {
    WheelPos -= 85;
    return strip.Color(0, WheelPos * 3, 255 - WheelPos * 3);
  }
  WheelPos -= 170;
  return strip.Color(WheelPos * 3, 255 - WheelPos * 3, 0);
}
