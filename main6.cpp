#include "FastLED.h"                                          // FastLED library.
#include "arduinoFFT.h"
#include <time.h>
#if FASTLED_VERSION < 3001000
#error "Requires FastLED 3.1 or later; check github for latest code."
#endif
#define CHANNEL A0
#define LED_DT 12                                             // Data pin to connect to the strip.
#define COLOR_ORDER GRB                                       // It's GRB for WS2812 and BGR for APA102.
#define LED_TYPE WS2812                                       // Using APA102, WS2812, WS2801. Don't forget to change LEDS.addLeds.
#define NUM_LEDS 175                                           // Number of LED's.
#define qsubd(x, b)  ((x>b)?b:0)                              // Digital unsigned subtraction macro. if result <0, then => 0. Otherwise, take on fixed value.
#define qsuba(x, b)  ((x>b)?x-b:0)                            // Analog Unsigned subtraction macro. if result <0, then => 0
uint8_t max_bright = 128;                                     // Overall brightness definition. It can be changed on the fly.
//new stuff
srand(time(NULL))
int _sample_size = 0; 
int max_bins = 4;
bool auto_find_frequency_band = true;
long CHANGE_FREQUENCY_EVAL_CYCLES = 10;
float STANDARD_DEV_WIDTH = 0;
bool STD_DEV_INCREASE_MOVES_UP_SAMPLING_FREQUENCY = false;
int SAMPLE_SIZE_TO_FREQUENCY_RATION = 60;
int sample_hertz_ratio =  SAMPLE_SIZE_TO_FREQUENCY_RATION*8;
double color_cycle_factor = pow(2.0, -10);
double amplifier = 1.0 * pow(2.0, -6);
int SIGNAL_MAX = 1;
bool enable_sigmoid = false;
double sigmoid_smooth_factor = 10; //higher means smoother
bool enable_1_over_x = false;
int bluetooth_signal_pin = 4;
int bluetooth_strength = 0;
const uint16_t samples_0 = 8; 
const uint16_t samples_1 = 16; 
const uint16_t samples_2 = 32; 
const uint16_t samples_3 = 64; 
const uint16_t samples_4 = 128; 
int count=0;
double samplingFrequency = 0;
unsigned int sampling_period_us;
double last_std_dev = 0;
double standard_deviation = 0;
double vReal_0[samples_0];
double vImag_0[samples_0];
double pass_value_0[samples_0];
double vReal_1[samples_1];
double vImag_1[samples_1];
double pass_value_1[samples_1];
double vReal_2[samples_2];
double vImag_2[samples_2];
double pass_value_2[samples_2];
double vReal_3[samples_3];
double vImag_3[samples_3];
double pass_value_3[samples_3];
double vReal_4[samples_4];
double vImag_4[samples_4];
double pass_value_4[samples_4];
unsigned long microseconds;
arduinoFFT FFT = arduinoFFT();
int sum = 0;
//new
CRGB leds[NUM_LEDS];
CRGB ledsA[NUM_LEDS];
CRGB ledsB[NUM_LEDS];

int get_sample_size(int _sample_index){
  if(_sample_size==0){
    return 8;
  }else if(_sample_size==1){
    return 16;
  }else if(_sample_size==2){
    return 32;
  }else if(_sample_size==3){
    return 64;
  }else if(_sample_size==4){
    return 128;
  }
}
CRGBPalette16 currentPalette;
void setup() {
  delay(1000);                                                // Power-up safety delay.
  Serial.begin(57600);                                        // Initialize serial port for debugging.
  LEDS.addLeds<LED_TYPE, LED_DT, COLOR_ORDER>(leds, NUM_LEDS);  // Use this for WS2812
  FastLED.setBrightness(max_bright);
  set_max_power_in_volts_and_milliamps(5, 500);               // FastLED Power management set at 5V, 500mA.
  samplingFrequency = get_sample_size(_sample_size)*sample_hertz_ratio; //Hz, must be less than 10000 due to ADC
  sampling_period_us = round(1000000*(1.0/samplingFrequency));

  currentPalette = CRGBPalette16(
  CRGB::Black, CRGB::Black, CRGB::Black, CHSV(0, 255,4),
  CHSV(0, 255, 8), CRGB::Red, CRGB::Red, CRGB::Red,                                   
  CRGB::DarkOrange,CRGB::Orange, CRGB::Orange, CRGB::Orange,
  CRGB::Yellow, CRGB::Yellow, CRGB::Gray, CRGB::Gray);
  
}


// define new animations here that affect A

// color swipe A
void animationAColorSwipeRed() {                                             // running red stripe.
  for (int i = 0; i < NUM_LEDS; i++) {
    uint8_t red = (millis() / 10) + (i * 12);                    // speed, length
    if (red > 128) red = 0;
    ledsA[i] = CRGB(red+(int)(rand()*10), 0, 0);
  }
}
void animationAColorSwipeGreen() {                                               // running green stripe in opposite direction.
  for (int i = 0; i < NUM_LEDS; i++) {
    uint8_t green = (millis() / 5) - (i * 12);                    // speed, length
    if (green > 128) green = 0;
    ledsA[i] = CRGB(0, green+(int)(rand()*10), 0);
  }
}
void animationAColorSwipeBlue() {                                               // running green stripe in opposite direction.
  for (int i = 0; i < NUM_LEDS; i++) {
    uint8_t blue = (millis() / 5) - (i * 12);                    // speed, length
    if (blue > 128) blue = 0;
    ledsA[i] = CRGB(0, 0, blue+(int)(rand()*10));
  }
}

CRGB clr1;
CRGB clr2;
uint8_t speed;
uint8_t loc1;
uint8_t loc2;
uint8_t ran1;
uint8_t ran2;
void animationAblendwave() {

  speed = beatsin8(6,0,255);

  clr1 = blend(CHSV(beatsin8(3,0,255),255,255), CHSV(beatsin8(4,0,255),255,255), speed);
  clr2 = blend(CHSV(beatsin8(4,0,255),255,255), CHSV(beatsin8(3,0,255),255,255), speed);

  loc1 = beatsin8(10,0,NUM_LEDS-1);
  
  fill_gradient_RGB(ledsA, 0, clr2, loc1, clr1);
  fill_gradient_RGB(ledsA, loc1, clr2, NUM_LEDS-1, clr1);

} // blendwave()



void animationAblue() {
  uint8_t blurAmount = dim8_raw( beatsin8(3,64, 192) );       // A sinewave at 3 Hz with values ranging from 64 to 192.
  blur1d( leds, NUM_LEDS, blurAmount);                        // Apply some blurring to whatever's already on the strip, which will eventually go black.
  
  uint8_t  i = beatsin8(  9, 0, NUM_LEDS);
  uint8_t  j = beatsin8( 7, 0, NUM_LEDS);
  uint8_t  k = beatsin8(  5, 0, NUM_LEDS);
  
  // The color of each point shifts over time, each at a different speed.
  uint16_t ms = millis();  
  leds[(i+j)/2] = CHSV( ms / 29, 200, 255);
  leds[(j+k)/2] = CHSV( ms / 41, 200, 255);
  leds[(k+i)/2] = CHSV( ms / 73, 200, 255);
  leds[(k+i+j)/3] = CHSV( ms / 53, 200, 255);
}


uint8_t  thisfade = 8;                                        // How quickly does it fade? Lower = slower fade rate.
int       thishue = 50;                                       // Starting hue.
uint8_t   thisinc = 1;                                        // Incremental value for rotating hues
uint8_t   thissat = 100;                                      // The saturation, where 255 = brilliant colours.
uint8_t   thisbri = 255;                                      // Brightness of a sequence. Remember, max_bright is the overall limiter.
int       huediff = 256;                                      // Range of random #'s to use for hue
uint8_t thisdelay = 5;                                        // We don't need much delay (if any)


void confetti() {                                             // random colored speckles that blink in and fade smoothly
  fadeToBlackBy(leds, NUM_LEDS, thisfade);                    // Low values = slower fade.
  int pos = random16(NUM_LEDS);                               // Pick an LED at random.
  ledsA[pos] += CHSV((thishue + random16(huediff))/4 , thissat, thisbri);  // I use 12 bits for hue so that the hue increment isn't too quick.
  thishue = thishue + thisinc;                                // It increments here.
} // confetti()


void ChangeMe() {                                             // A time (rather than loop) based demo sequencer. This gives us full control over the length of each sequence.
  uint8_t secondHand = (millis() / 1000) % 15;                // IMPORTANT!!! Change '15' to a different value to change duration of the loop.
  static uint8_t lastSecond = 99;                             // Static variable, means it's only defined once. This is our 'debounce' variable.
  if (lastSecond != secondHand) {                             // Debounce to make sure we're not repeating an assignment.
    lastSecond = secondHand;
    switch(secondHand) {
      case  0: thisinc=1; thishue=192; thissat=255; thisfade=2; huediff=256; break;  // You can change values here, one at a time , or altogether.
      case  5: thisinc=2; thishue=128; thisfade=8; huediff=64; break;
      case 10: thisinc=1; thishue=random16(255); thisfade=1; huediff=16; break;      // Only gets called once, and not continuously for the next several seconds. Therefore, no rainbows.
      case 15: break;                                                                // Here's the matching 15 for the other one.
    }
  }
} // ChangeMe()

void animationAconfetti(){
  EVERY_N_MILLISECONDS(thisdelay) {                           // FastLED based non-blocking delay to update/display the sequence.
    confetti();
  }
}


uint8_t fadeval = 224;                                        // Trail behind the LED's. Lower => faster fade.
uint8_t bpm = 30;

void animationAdotbeat(){
  uint8_t inner = beatsin8(bpm, NUM_LEDS/4, NUM_LEDS/4*3);    // Move 1/4 to 3/4
  uint8_t outer = beatsin8(bpm, 0, NUM_LEDS-1);               // Move entire length
  uint8_t middle = beatsin8(bpm, NUM_LEDS/3, NUM_LEDS/3*2);   // Move 1/3 to 2/3

  leds[middle] = CRGB::Purple;
  leds[inner] = CRGB::Blue;
  leds[outer] = CRGB::Aqua;

  nscale8(ledsA,NUM_LEDS,fadeval);     
}



void ease() {

  static uint8_t easeOutVal = 0;
  static uint8_t easeInVal  = 0;
  static uint8_t lerpVal    = 0;

  easeOutVal = ease8InOutQuad(easeInVal);                     // Start with easeInVal at 0 and then go to 255 for the full easing.
  easeInVal++;

  lerpVal = lerp8by8(0, NUM_LEDS, easeOutVal);                // Map it to the number of LED's you have.

  ledsA[lerpVal] = CRGB::Red;
  fadeToBlackBy(ledsA, NUM_LEDS, 16);                          // 8 bit, 1 = slow fade, 255 = fast fade
  
} // ease()
void animationAeasing(){
  EVERY_N_MILLISECONDS(20) {                           // FastLED based non-blocking delay to update/display the sequence.
    ease();
  }
}

uint32_t xscale = 20;                                          // How far apart they are
uint32_t yscale = 3;                                           // How fast they move
uint8_t index = 0;

void inoise8_fire() {
  
  for(int i = 0; i < NUM_LEDS; i++) {
    index = inoise8(i*xscale,millis()*yscale*NUM_LEDS/255);                                           // X location is constant, but we move along the Y at the rate of millis()
    ledsA[i] = ColorFromPalette(currentPalette, min(i*(index)>>6, 255), i*255/NUM_LEDS, LINEARBLEND);  // With that value, look up the 8 bit colour palette value and assign it to the current LED.
  }                                                                                                   // The higher the value of i => the higher up the palette index (see palette definition).
                                                                                                      // Also, the higher the value of i => the brighter the LED.
} //

void animationAinoise8_fire(){
  inoise8_fire();
}


CRGBPalette16 currentPalette2 = LavaColors_p;
CRGBPalette16 targetPalette = LavaColors_p;
TBlendType    currentBlending = LINEARBLEND;

void matrix_ray(uint8_t colorIndex) {                                                 // Send a PWM'd sinewave instead of a random happening of LED's down the strand.

  static uint8_t thisdir = 0;                                                         // We could change the direction if we want to. Static means to assign that value only once.
  static int thisphase = 0;                                                           // Phase change value gets calculated. Static means to assign that value only once.
  uint8_t thiscutoff;                                                                 // You can change the cutoff value to display this wave. Lower value = longer wave.

  thisphase += beatsin8(1,20, 50);                                                    // You can change direction and speed individually.
  thiscutoff = beatsin8(50,164,248);                                                  // This variable is used for the PWM of the lighting with the qsubd command below.
  
  int thisbright = qsubd(cubicwave8(thisphase), thiscutoff);                          // It's PWM time. qsubd sets a minimum value called thiscutoff. If < thiscutoff, then thisbright = 0. Otherwise, thisbright = thiscutoff.
 
  if (thisdir == 0) {                                                                 // Depending on the direction, we'll put that brightness in one end of the array. Andrew Tuline wrote this.
    ledsA[0] = ColorFromPalette(currentPalette2, colorIndex, thisbright, currentBlending); 
    memmove(ledsA+1, ledsA, (NUM_LEDS-1)*3);                                            // Oh look, the FastLED method of copying LED values up/down the strand.
  } else {
    ledsA[NUM_LEDS-1] = ColorFromPalette( currentPalette2, colorIndex, thisbright, currentBlending);
    memmove(ledsA, ledsA+1, (NUM_LEDS-1)*3);    
  }

} // matrix_ray()


void animationAmatrixray(){
  EVERY_N_MILLIS_I(thisTimer,100) {                           // This only sets the Initial timer delay. To change this value, you need to use thisTimer.setPeriod(); You could also call it thatTimer and so on.
    uint8_t timeval = beatsin8(10,20,50);                     // Create/modify a variable based on the beastsin8() function.
    thisTimer.setPeriod(timeval);                             // Use that as our update timer value.

    matrix_ray(millis()>>4);                                  // This is the main function that's called. We could have not passed the millis()>>4, but it's a quick example of passing an argument.
  }

  EVERY_N_MILLISECONDS(100) {                                 // Fixed rate of a palette blending capability.
    uint8_t maxChanges = 24; 
    nblendPaletteTowardPalette(currentPalette, targetPalette, maxChanges);
  }

  EVERY_N_SECONDS(5) {                                        // Change the target palette to a random one every 5 seconds.
    static uint8_t baseC = random8();                         // You can use this as a baseline colour if you want similar hues in the next line.
    targetPalette = CRGBPalette16(CHSV(random8(), 255, random8(128,255)), CHSV(random8(), 255, random8(128,255)), CHSV(random8(), 192, random8(128,255)), CHSV(random8(), 255, random8(128,255)));
  }}



CRGBPalette16 currentPalette3 = OceanColors_p;                                 // Palette definitions
CRGBPalette16 targetPalette3;
TBlendType currentBlending3 = LINEARBLEND;

void plasma() {                                                 // This is the heart of this program. Sure is short. . . and fast.

  int thisPhase = beatsin8(6,-64,64);                           // Setting phase change for a couple of waves.
  int thatPhase = beatsin8(7,-64,64);

  for (int k=0; k<NUM_LEDS; k++) {                              // For each of the LED's in the strand, set a brightness based on a wave as follows:

    int colorIndex = cubicwave8((k*23)+thisPhase)/2 + cos8((k*15)+thatPhase)/2;           // Create a wave and add a phase change and add another wave with its own phase change.. Hey, you can even change the frequencies if you wish.
    int thisBright = qsuba(colorIndex, beatsin8(7,0,96));                                 // qsub gives it a bit of 'black' dead space by setting sets a minimum value. If colorIndex < current value of beatsin8(), then bright = 0. Otherwise, bright = colorIndex..

    ledsA[k] = ColorFromPalette(currentPalette, colorIndex, thisBright, currentBlending);  // Let's now add the foreground colour.
  }

} // plasma()

void animationAplasma(){
  EVERY_N_MILLISECONDS(50) {                                  // FastLED based non-blocking delay to update/display the sequence.
    plasma();
  }

  EVERY_N_MILLISECONDS(1000) {
    Serial.println(LEDS.getFPS());                            // Optional check of our fps.
  }

  EVERY_N_MILLISECONDS(100) {
    uint8_t maxChanges = 24; 
    nblendPaletteTowardPalette(currentPalette, targetPalette, maxChanges);   // AWESOME palette blending capability.
  }


  EVERY_N_SECONDS(5) {                                 // Change the target palette to a random one every 5 seconds.
    uint8_t baseC = random8();                         // You can use this as a baseline colour if you want similar hues in the next line.
    targetPalette = CRGBPalette16(CHSV(baseC+random8(32), 192, random8(128,255)), CHSV(baseC+random8(32), 255, random8(128,255)), CHSV(baseC+random8(32), 192, random8(128,255)), CHSV(baseC+random8(32), 255, random8(128,255)));
  }  
}



uint8_t thisdelay4 = 5;                                        // A delay value for the sequence(s)
uint8_t thishue4 = 0;                                          // Starting hue value.
uint8_t deltahue4 = 10; 

void rainbow_march() {                                        // The fill_rainbow call doesn't support brightness levels
  thishue4++;                                                  // Increment the starting hue.
  fill_rainbow(ledsA, NUM_LEDS, thishue4, deltahue4);            // Use FastLED's fill_rainbow routine.
} // rainbow_march()
void animationArainbowmarch(){
  rainbow_march();
}



// original audio analysis
void animationBAudio() {                                               // running green stripe in opposite direction.

  int sample_size = get_sample_size(_sample_size);
  for(int i=0; i<sample_size; i++)
  {
    microseconds = micros();    //Overflows after around 70 minutes!
  
    if(_sample_size==0){
      vReal_0[i] = analogRead(CHANNEL);
      vImag_0[i] = 0;
    }else if(_sample_size==1){
      vReal_1[i] = analogRead(CHANNEL);
      vImag_1[i] = 0;
    }else if(_sample_size==2){
      vReal_2[i] = analogRead(CHANNEL);
      vImag_2[i] = 0;
    }else if(_sample_size==3){
      vReal_3[i] = analogRead(CHANNEL);
      vImag_3[i] = 0;
    }else if(_sample_size==4){
      vReal_4[i] = analogRead(CHANNEL);
      vImag_4[i] = 0;
    }
    while(micros() < (microseconds + sampling_period_us)){}
  }

  //if(serial) Serial.println(sample_hertz_ratio);
  if(_sample_size==0){
    FFT.Windowing(vReal_0, sample_size, FFT_WIN_TYP_HAMMING, FFT_FORWARD);
    FFT.Compute(vReal_0, vImag_0, sample_size, FFT_FORWARD);
    FFT.ComplexToMagnitude(vReal_0, vImag_0, sample_size);
    
    int led_no = 0;
    double total_pass_value=0;
    for(uint16_t i = 0; i < sample_size; i++)
    {
      pass_value_0[i] = i == 0 ? vReal_0[0] : 0.8*pass_value_0[i-1] + 0.2*vReal_0[i];
      total_pass_value += pass_value_0[i];
    }  

    double average = total_pass_value/sample_size;
    standard_deviation = 0;
    for(uint16_t i = 0; i < sample_size; i++)
    {
      standard_deviation += pow(pass_value_0[i] - average, 2);
    }
    standard_deviation = pow(standard_deviation/sample_size, 0.5);

    for(uint16_t i = 0; i < sample_size; i++)
    {
      double pass_value = pass_value_0[i];

      while(led_no<(i+1)*NUM_LEDS/sample_size){
        ledsB[led_no].red = min((int) (1 + pass_value * amplifier), SIGNAL_MAX);
        ledsB[led_no].green = min((int) (1 + pass_value * amplifier), SIGNAL_MAX);
        ledsB[led_no].blue = min((int) (1 + pass_value * amplifier), SIGNAL_MAX);
        led_no++;
      }
    }  

  }else if(_sample_size==1){
    FFT.Windowing(vReal_1, sample_size, FFT_WIN_TYP_HAMMING, FFT_FORWARD);
    FFT.Compute(vReal_1, vImag_1, sample_size, FFT_FORWARD);
    FFT.ComplexToMagnitude(vReal_1, vImag_1, sample_size);

    int led_no = 0;
    double total_pass_value=0;
    for(uint16_t i = 0; i < sample_size; i++)
    {
      pass_value_1[i] = i == 0 ? vReal_0[0] : 0.8*pass_value_1[i-1] + 0.2*vReal_1[i];
      total_pass_value += pass_value_1[i];
    }  

    double average = total_pass_value/sample_size;
    standard_deviation = 0;
    for(uint16_t i = 0; i < sample_size; i++)
    {
      standard_deviation += pow(pass_value_1[i] - average, 2);
    }
    standard_deviation = pow(standard_deviation/sample_size, 0.5);

    for(uint16_t i = 0; i < sample_size; i++)
    {
      double pass_value = pass_value_1[i];
      while(led_no<(i+1)*NUM_LEDS/sample_size){
        ledsB[led_no].red = (int) (1 + pass_value * amplifier);
        ledsB[led_no].green = (int) (1 + pass_value * amplifier);
        ledsB[led_no].blue = (int) (1 + pass_value * amplifier);
        led_no++;
      }
    }  

  }else if(_sample_size==2){
    FFT.Windowing(vReal_2, sample_size, FFT_WIN_TYP_HAMMING, FFT_FORWARD);
    FFT.Compute(vReal_2, vImag_2, sample_size, FFT_FORWARD);
    FFT.ComplexToMagnitude(vReal_2, vImag_2, sample_size);

    int led_no = 0;
    double total_pass_value=0;
    for(uint16_t i = 0; i < sample_size; i++)
    {
      pass_value_2[i] = i == 0 ? vReal_2[0] : 0.8*pass_value_2[i-1] + 0.2*vReal_2[i];
      total_pass_value += pass_value_2[i];
    }  

    double average = total_pass_value/sample_size;
    standard_deviation = 0;
    for(uint16_t i = 0; i < sample_size; i++)
    {
      standard_deviation += pow(pass_value_2[i] - average, 2);
    }
    standard_deviation = pow(standard_deviation/sample_size, 0.5);
    for(uint16_t i = 0; i < sample_size; i++)
    {
      double pass_value = pass_value_2[i];
      while(led_no<(i+1)*NUM_LEDS/sample_size){
        ledsB[led_no].red = (int) (1 + pass_value * amplifier);
        ledsB[led_no].green = (int) (1 + pass_value * amplifier);
        ledsB[led_no].blue = (int) (1 + pass_value * amplifier);
        led_no++;
      }
    }  

  }else if(_sample_size==3){
    FFT.Windowing(vReal_3, sample_size, FFT_WIN_TYP_HAMMING, FFT_FORWARD);
    FFT.Compute(vReal_3, vImag_3, sample_size, FFT_FORWARD);
    FFT.ComplexToMagnitude(vReal_3, vImag_3, sample_size);

    int led_no = 0;
    double total_pass_value=0;
    for(uint16_t i = 0; i < sample_size; i++)
    {
      pass_value_3[i] = i == 0 ? vReal_3[0] : 0.8*pass_value_3[i-1] + 0.2*vReal_3[i];
      total_pass_value += pass_value_3[i];
    }  

    double average = total_pass_value/sample_size;
    standard_deviation = 0;
    for(uint16_t i = 0; i < sample_size; i++)
    {
      standard_deviation += pow(pass_value_3[i] - average, 2);
    }
    standard_deviation = pow(standard_deviation/sample_size, 0.5);
    for(uint16_t i = 0; i < sample_size; i++)
    {
      double pass_value = pass_value_3[i];
      while(led_no<(i+1)*NUM_LEDS/sample_size){
        ledsB[led_no].red = (int) (1 + pass_value * amplifier);
        ledsB[led_no].green = (int) (1 + pass_value * amplifier);
        ledsB[led_no].blue = (int) (1 + pass_value * amplifier);
        led_no++;
      }
    }  

  }else if(_sample_size==4){
    FFT.Windowing(vReal_4, sample_size, FFT_WIN_TYP_HAMMING, FFT_FORWARD);
    FFT.Compute(vReal_4, vImag_4, sample_size, FFT_FORWARD);
    FFT.ComplexToMagnitude(vReal_4, vImag_4, sample_size);

    int led_no = 0;
    double total_pass_value=0;
    for(uint16_t i = 0; i < sample_size; i++)
    {
      pass_value_4[i] = i == 0 ? vReal_4[0] : 0.8*pass_value_4[i-1] + 0.2*vReal_4[i];
      total_pass_value += pass_value_4[i];
    }  

    double average = total_pass_value/sample_size;
    standard_deviation = 0;
    for(uint16_t i = 0; i < sample_size; i++)
    {
      standard_deviation += pow(pass_value_4[i] - average, 2);
    }
    standard_deviation = pow(standard_deviation/sample_size, 0.5);
    for(uint16_t i = 0; i < sample_size; i++)
    {
      double pass_value = pass_value_4[i];
      while(led_no<(i+1)*NUM_LEDS/sample_size){
        ledsB[led_no].red = (int) (1 + pass_value * amplifier);
        ledsB[led_no].green = (int) (1 + pass_value * amplifier);
        ledsB[led_no].blue = (int) (1 + pass_value * amplifier);
        led_no++;
      }
    }  

  }

  // extra stuff
  count++;
  if(count==CHANGE_FREQUENCY_EVAL_CYCLES){
    if(auto_find_frequency_band){
      if(last_std_dev<standard_deviation*(1-STANDARD_DEV_WIDTH)){
        if(STD_DEV_INCREASE_MOVES_UP_SAMPLING_FREQUENCY){
          _sample_size = _sample_size == max_bins ? max_bins : _sample_size+1 ;
        }else{
          _sample_size = _sample_size == 0 ? 0 : _sample_size-1 ;
        }
      }
      if(last_std_dev>standard_deviation*(1+STANDARD_DEV_WIDTH)){
        if(!STD_DEV_INCREASE_MOVES_UP_SAMPLING_FREQUENCY){
          _sample_size = _sample_size == max_bins ? max_bins : _sample_size+1 ;
        }else{
          _sample_size = _sample_size == 0 ? 0 : _sample_size-1 ;
        }
      }
    }
    last_std_dev = standard_deviation;
    amplifier = 2.0*standard_deviation;
    //if(serial) Serial.println("= next band ===========");
    sample_hertz_ratio = SAMPLE_SIZE_TO_FREQUENCY_RATION * get_sample_size(_sample_size);
    sampling_period_us = round(1000000*(1.0/samplingFrequency));
    count = 0;
  }
}

int master_play_list_index = 218;
int master_play_list_size = 234;
byte blend_amount[] = {
  0,2,4,8,8,4,2,1,1,2,4,8,8,4,2,0, //16
  0,2,4,8,8,4,2,1,1,2,4,8,8,4,2,0, //32
  0,2,4,8,8,4,2,1,1,2,4,8,8,4,2,0, //48
  8,12,16,24,36,36,24,36,36,24,36,36,24,16,12,8, //64
  8,12,16,24,36,36,24,36,36,24,36,36,24,16,12,8, //80
  8,12,16,24,36,36,24,36,36,24,36,36,24,16,12,8, //96
  8,12,16,24,36,36,24,36,36,24,36,36,24,16,12,8, //112
  8,12,16,24,36,36,24,36,36,24,36,36,24,16,12,8, //128
  8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8, //144
  8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8, //160
  8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8, //176
  36,36,36,36,36,36,36,36,36,36,36,36,36,36,36,36, //192
  120,120,120,120,120,120,120,120,120,120,120,120,120,120,120,120, //218
  25,25,25,25,25,25,25,25,25,25,25,25,25,25,25,25 //234
};


//unsigned long finishes_in_one_second = 200; this is the final value //5 mins or 1 mins i forget
unsigned long finishes_in_one_second = 100;
double minutes_for_rotation = 1;
unsigned long count_of_ticks_in_animation = 0;
void loop() {

  if(master_play_list_index < 16){
    animationAColorSwipeRed();                                               // render the first animation into leds2   
    animationBAudio();                                               // render the second animation into leds3
  }
  else if(master_play_list_index < 32){
    animationAColorSwipeGreen();                                               // render the first animation into leds2   
    animationBAudio();                                               // render the second animation into leds3
  }
  else if(master_play_list_index < 48){
    animationAColorSwipeBlue();                                               // render the first animation into leds2   
    animationBAudio();                                               // render the second animation into leds3
  }
  else if(master_play_list_index < 64){
    animationAColorSwipeRed();                                               // render the first animation into leds2   
    animationBAudio();                                               // render the second animation into leds3
  }
  else if(master_play_list_index < 80){
    animationAColorSwipeGreen();                                               // render the first animation into leds2   
    animationBAudio();                                               // render the second animation into leds3
  }
  else if(master_play_list_index < 96){
    animationAColorSwipeBlue();                                               // render the first animation into leds2   
    animationBAudio();                                               // render the second animation into leds3
  }
  else if(master_play_list_index < 112){
    animationAblendwave();                                               // render the first animation into leds2   
    animationBAudio();                                               // render the second animation into leds3
  }
  else if(master_play_list_index < 128){
    animationAblue();                                               // render the first animation into leds2   
    animationBAudio();                                               // render the second animation into leds3
  }
  else if(master_play_list_index < 144){
    animationAconfetti();                                               // render the first animation into leds2   
    animationBAudio();                                               // render the second animation into leds3
  }
  else if(master_play_list_index < 160){
    animationAdotbeat();                                               // render the first animation into leds2   
    animationBAudio();                                               // render the second animation into leds3
  }else if(master_play_list_index < 176){
    animationAeasing();
    animationBAudio();                                               // render the second animation into leds3
  }else if(master_play_list_index < 192){
    animationAmatrixray();
    animationBAudio();                                               // render the second animation into leds3
  }else if(master_play_list_index < 218){
    animationAplasma();
    animationBAudio();                                               // render the second animation into leds3
  }else if(master_play_list_index < 234){
    animationArainbowmarch();
    animationBAudio();
  }

  for (int i = 0; i < NUM_LEDS; i++) {
    leds[i] = blend( ledsA[i], ledsB[i], blend_amount[master_play_list_index] );
  }
  FastLED.show();
  count_of_ticks_in_animation++;
  if((1+count_of_ticks_in_animation)%(long(finishes_in_one_second*minutes_for_rotation*5))==0){
    master_play_list_index = (master_play_list_index + 1)%master_play_list_size;
    
    if(master_play_list_index==0){
      minutes_for_rotation = minutes_for_rotation / 2.0;
    }
  }
}

