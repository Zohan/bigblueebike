#include <FastLED.h>
#include <EEPROM.h>
#ifdef __AVR__
  #include <avr/power.h>
#endif

#define LED_PIN     5
#define COLOR_ORDER GRB
#define CHIPSET     WS2811
#define NUM_LEDS    64

#define BRIGHTNESS  200
#define FRAMES_PER_SECOND 60

#define COOLING  55
#define SPARKING 120

bool gReverseDirection = false;

CRGB leds[NUM_LEDS];
CRGBPalette16 gPal;
byte mode;
uint8_t gHue = 0; // rotating "base color" used by many of the patterns

#define buttonPin 2
#define piezoPin 9

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
  mode = EEPROM.read(0);
  if(mode >= 2) {
    mode = 0;
  } else mode++;
  EEPROM.write(0, mode);
  FastLED.addLeds<CHIPSET, LED_PIN, COLOR_ORDER>(leds, NUM_LEDS).setCorrection( TypicalLEDStrip );
  if(mode < 3) FastLED.setBrightness(20);
  else FastLED.setBrightness(15);
  FastLED.clear(true);
  delay(500);
  leds[0] = CRGB::Aqua;
  delay(250);
  FastLED.show();
  
  //gPal = CRGBPalette16( CRGB::DarkBlue, CRGB::MediumBlue, CRGB::SeaGreen, CRGB::Teal );
  gPal = CRGBPalette16( CRGB::Teal, CRGB::SeaGreen, CRGB::MediumBlue, CRGB::DarkBlue );
  
}

void loop() {
  int buttonState = digitalRead(buttonPin);

 
  if (buttonState == HIGH) {
    playSong();
    
  } else {
    // turn LED off:
    noTone(piezoPin);
    currentNote = 0;
  }
  random16_add_entropy( random());
  Fire2012WithPalette(); // run simulation frame, using palette colors
  FastLED.show(); // display this frame
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

void Fire2012WithPalette()
{
// Array of temperature readings at each simulation cell
  static byte heat[NUM_LEDS];

  // Step 1.  Cool down every cell a little
    for( int i = 0; i < NUM_LEDS; i++) {
      heat[i] = qsub8( heat[i],  random8(0, ((COOLING * 10) / NUM_LEDS) + 2));
    }
  
    // Step 2.  Heat from each cell drifts 'up' and diffuses a little
    for( int k= NUM_LEDS - 1; k >= 2; k--) {
      heat[k] = (heat[k - 1] + heat[k - 2] + heat[k - 2] ) / 3;
    }
    
    // Step 3.  Randomly ignite new 'sparks' of heat near the bottom
    if( random8() < SPARKING ) {
      int y = random8(7);
      heat[y] = qadd8( heat[y], random8(160,255) );
    }

    // Step 4.  Map from heat cells to LED colors
    for( int j = 0; j < NUM_LEDS; j++) {
      // Scale the heat value from 0-255 down to 0-240
      // for best results with color palettes.
      byte colorindex = scale8( heat[j], 240);
      CRGB color = ColorFromPalette( gPal, colorindex);
      int pixelnumber;
      pixelnumber = j;
      leds[pixelnumber] = color;
    }
}
