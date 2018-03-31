#include "FastLED.h"                                          // FastLED library.
#include "arduinoFFT.h"
#include <time.h>
#if FASTLED_VERSION < 3001000
#error "Requires FastLED 3.1 or later; check github for latest code."
#endif
#define CHANNEL A0
#define LED_DT 12                                             // Data pin to connect to the strip.
#define COLOR_ORDER BGR                                       // It's GRB for WS2812 and BGR for APA102.
#define LED_TYPE WS2812                                       // Using APA102, WS2812, WS2801. Don't forget to change LEDS.addLeds.
#define NUM_LEDS 175                                           // Number of LED's.
uint8_t max_bright = 128;                                     // Overall brightness definition. It can be changed on the fly.
//new stuff
int _sample_size = 0; 
bool auto_find_frequency_band = true;
byte CHANGE_FREQUENCY_EVAL_CYCLES = 100;
float STANDARD_DEV_WIDTH = 0.1;
bool STD_DEV_INCREASE_MOVES_UP_SAMPLING_FREQUENCY = true;
int SAMPLE_SIZE_TO_FREQUENCY_RATION = 60;
int sample_hertz_ratio =  SAMPLE_SIZE_TO_FREQUENCY_RATION*8;
double color_cycle_factor = pow(2.0, -10);
double amplifier = 1.0 * pow(2.0, -6);
int SIGNAL_MAX = 1;
bool enable_sigmoid = false;
double sigmoid_smooth_factor = 10; //higher means smoother
bool enable_1_over_x = false;
int red_base_line = 1;
int green_base_line = 1;
int blue_base_line = 1;
int bluetooth_signal_pin = 5;
int bluetooth_strength = 0;
const uint16_t samples_0 = 8; 
const uint16_t samples_1 = 16; 
const uint16_t samples_2 = 32; 
const uint16_t samples_3 = 64; 
const uint16_t samples_4 = 128; 
const uint16_t samples_5 = 256; 
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
double vReal_5[samples_5];
double vImag_5[samples_5];
double pass_value_5[samples_5];
unsigned long microseconds;
arduinoFFT FFT = arduinoFFT();
int sum = 0;
//new
CRGB leds[NUM_LEDS];
CRGB leds2[NUM_LEDS];
CRGB leds3[NUM_LEDS];
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
  }else{
    return 256;
  }
}
void setup() {
  delay(1000);                                                // Power-up safety delay.
  Serial.begin(57600);                                        // Initialize serial port for debugging.
  LEDS.addLeds<LED_TYPE, LED_DT, COLOR_ORDER>(leds, NUM_LEDS);  // Use this for WS2812
  FastLED.setBrightness(max_bright);
  set_max_power_in_volts_and_milliamps(5, 500);               // FastLED Power management set at 5V, 500mA.
  samplingFrequency = get_sample_size(_sample_size)*sample_hertz_ratio; //Hz, must be less than 10000 due to ADC
  sampling_period_us = round(1000000*(1.0/samplingFrequency));
}
void animationA() {                                             // running red stripe.
  for (int i = 0; i < NUM_LEDS; i++) {
    uint8_t red = (millis() / 10) + (i * 12);                    // speed, length
    if (red > 128) red = 0;
    leds2[i] = CRGB(red, 0, 0);
  }
}
void animationB() {                                               // running green stripe in opposite direction.
  for (int i = 0; i < NUM_LEDS; i++) {
    uint8_t green = (millis() / 5) - (i * 12);                    // speed, length
    if (green > 128) green = 0;
    leds3[i] = CRGB(0, green, 0);
  }
}void loop() {
  animationA();                                               // render the first animation into leds2   
  animationB();                                               // render the second animation into leds3

  uint8_t ratio = beatsin8(2);                                // Alternate between 0 and 255 every minute
  for (int i = 0; i < NUM_LEDS; i++) {                        // mix the 2 arrays together
    leds[i] = blend( leds2[i], leds3[i], ratio );
  }
  FastLED.show();
}

