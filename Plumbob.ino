// NeoPixel test program showing use of the WHITE channel for RGBW
// pixels only (won't look correct on regular RGB NeoPixel strips).

#include <Adafruit_NeoPixel.h>
#ifdef __AVR__
 #include <avr/power.h> // Required for 16 MHz Adafruit Trinket
#endif

// Which pin on the Arduino is connected to the NeoPixels?
// On a Trinket or Gemma we suggest changing this to 1:
#define LED_PIN     2

// How many NeoPixels are attached to the Arduino?
#define LED_COUNT  27

// NeoPixel brightness, 0 (min) to 255 (max)
#define BRIGHTNESS 255 // Set BRIGHTNESS to about 1/5 (max = 255)

// Declare our NeoPixel strip object:
Adafruit_NeoPixel strip(LED_COUNT, LED_PIN, NEO_GRBW + NEO_KHZ800);
// Argument 1 = Number of pixels in NeoPixel strip
// Argument 2 = Arduino pin number (most are valid)
// Argument 3 = Pixel type flags, add together as needed:
//   NEO_KHZ800  800 KHz bitstream (most NeoPixel products w/WS2812 LEDs)
//   NEO_KHZ400  400 KHz (classic 'v1' (not v2) FLORA pixels, WS2811 drivers)
//   NEO_GRB     Pixels are wired for GRB bitstream (most NeoPixel products)
//   NEO_RGB     Pixels are wired for RGB bitstream (v1 FLORA pixels, not v2)
//   NEO_RGBW    Pixels are wired for RGBW bitstream (NeoPixel RGBW products)

#define modeButton 3
#define colorButton 4

bool modePress = false;
bool colorPress = false;

int mode = 0;
int color = 3;
double hsvList[4] = {20000, 8500, 0, 55000};
int brightnessMod = 67;//100

long t;

//DEBOUNCE FUCKING BULLSHIT
int buttonStateM = LOW; //this variable tracks the state of the button, low if not pressed, high if pressed
int modeChange = -1;
long lastDebounceTimeM = 0;  // the last time the output pin was toggled
long debounceDelayM = 50;    // the debounce time; increase if the output flickers

int buttonStateC = LOW; //this variable tracks the state of the button, low if not pressed, high if pressed
int colorChange = -1;
long lastDebounceTimeC = 0;  // the last time the output pin was toggled
long debounceDelayC = 50;    // the debounce time; increase if the output flickers

void setup() {
  // These lines are specifically to support the Adafruit Trinket 5V 16 MHz.
  // Any other board, you can remove this part (but no harm leaving it):
#if defined(__AVR_ATtiny85__) && (F_CPU == 16000000)
  clock_prescale_set(clock_div_1);
#endif
  // END of Trinket-specific code.

  strip.begin();           // INITIALIZE NeoPixel strip object (REQUIRED)
  strip.show();            // Turn OFF all pixels ASAP
  strip.setBrightness(BRIGHTNESS);

  pinMode(modeButton, INPUT);
  pinMode(colorButton, INPUT);

  Serial.begin(9600);

}

void loop() {
  //Serial.println("t:"+String(t)+"mode:"+String(mode)+"color:"+String(color));

  //sample the state of the button - is it pressed or not?
  buttonStateM = digitalRead(modeButton);
  //filter out any noise by setting a time buffer
  if ( (millis() - lastDebounceTimeM) > debounceDelayM) {

    if (buttonStateM == HIGH && buttonStateC == HIGH && modeChange < 0) {
      modeChange = -modeChange;
      brightnessMod -= 33;
      if(brightnessMod <= 0) {
        brightnessMod = 100;
      }
      Serial.println(brightnessMod);
      strip.setBrightness(BRIGHTNESS*(float(brightnessMod)/float(100)));
      color -= 1;
      if(color == -1) {
        color = 4;
      }
      lastDebounceTimeM = millis(); 
    }

    else if ( (buttonStateM == HIGH) && (modeChange < 0) ) {
      mode += 1;
      modeChange = -modeChange;
      lastDebounceTimeM = millis(); //set the current time
    }
    else if ( (buttonStateM == LOW) && (modeChange > 0) ) {
      modeChange = -modeChange;
      lastDebounceTimeM = millis(); //set the current time
    }
  }

  //sample the state of the button - is it pressed or not?
  buttonStateC = digitalRead(colorButton);
  //filter out any noise by setting a time buffer
  if ( (millis() - lastDebounceTimeC) > debounceDelayC) {

    if ( (buttonStateC == HIGH) && (colorChange < 0) ) {
      color += 1;
      color = color%5;
      colorChange = -colorChange;
      lastDebounceTimeC = millis(); //set the current time
    }
    else if ( (buttonStateC == LOW) && (colorChange > 0) ) {
      colorChange = -colorChange;
      lastDebounceTimeC = millis(); //set the current time
    }
  }

  t = millis();

  //check mode and display
  if (mode == 0) { //solid light
    if (color == 4) {
      strip.fill(strip.Color(0, 0, 0, 255));
    }
    else {
      strip.fill(strip.gamma32(strip.ColorHSV(hsvList[color])));
    }
  }
  if (mode == 1) { //slow fade
    float mod = (sin(float(t)/float(300))+1)/2;
    if (mod>1){mod=1;}
    if (color == 4) {
      strip.fill(strip.Color(0, 0, 0, 255*mod));
    }
    else {
      //strip.fill(strip.gamma32(strip.ColorHSV(hsvList[color], 255, 255*mod)));
      strip.fill(strip.gamma32(strip.ColorHSV(hsvList[color], 255, 255*mod)));
    }
    
  }
  if (mode == 2) { //strobe
    int mod = 255*(sin(t/25)>0);
    if (color == 4) {
      strip.fill(strip.Color(0, 0, 0, mod));
    }
    else {
      strip.fill(strip.gamma32(strip.ColorHSV(hsvList[color], 255, mod)));
    }
  }
  if (mode == 3) {//RGB
    int mod = (t/300)%3;
    double list[] = {20000, 40000, 0};
    strip.fill(strip.gamma32(strip.ColorHSV(list[mod])));
    Serial.println(mod);
  }
  if (mode == 4) {//rainbow
    int offset = 100;
    for(int i = 0; i<strip.numPixels(); i++){
      strip.fill(strip.ColorHSV(t*offset));
    }
  }
  if (mode == 5) {//unicorn
    int offset = 1000;
    double mod = (float(t)*float(100));
    Serial.println(mod);
    for(int i = 0; i<strip.numPixels(); i++){
      strip.setPixelColor(i, strip.ColorHSV(mod+float(offset)*float(i)));
    }
  }
  if (mode >= 6) {
    mode = 0; 
  }
  strip.show();
}

// Fill the dots one after the other with a color
void colorWipe(uint32_t c, uint16_t wait) {
  for(uint16_t i=0; i<strip.numPixels(); i++) {
    strip.setPixelColor(i, c);
    strip.show();
    delay(wait);
  }
}
