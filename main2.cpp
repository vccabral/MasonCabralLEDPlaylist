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
uint8_t max_bright = 128;                                     // Overall brightness definition. It can be changed on the fly.
//new stuff
int _sample_size = 0; 
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





// define new animations here that affect A

// color swipe A
void animationAColorSwipeRed() {                                             // running red stripe.
  for (int i = 0; i < NUM_LEDS; i++) {
    uint8_t red = (millis() / 10) + (i * 12);                    // speed, length
    if (red > 128) red = 0;
    ledsA[i] = CRGB(red, 0, 0);
  }
}
void animationAColorSwipeGreen() {                                               // running green stripe in opposite direction.
  for (int i = 0; i < NUM_LEDS; i++) {
    uint8_t green = (millis() / 5) - (i * 12);                    // speed, length
    if (green > 128) green = 0;
    ledsA[i] = CRGB(0, green, 0);
  }
}
void animationAColorSwipeBlue() {                                               // running green stripe in opposite direction.
  for (int i = 0; i < NUM_LEDS; i++) {
    uint8_t blue = (millis() / 5) - (i * 12);                    // speed, length
    if (blue > 128) blue = 0;
    ledsA[i] = CRGB(0, 0, blue);
  }
}



// define new animations here that affect B
// color swipe B

// original audio analysis
void animationBAudio() {                                               // running green stripe in opposite direction.
  bluetooth_strength = 0;
  int bluetooth_samples = 10;
  for(byte i = 0;i<bluetooth_samples;i++){
    bluetooth_strength += analogRead(bluetooth_signal_pin);
  }
  float red_factor = sin(bluetooth_strength*color_cycle_factor);
  float green_factor = sin(bluetooth_strength*color_cycle_factor+3.14/2.0);
  float blue_factor = sin(bluetooth_strength*color_cycle_factor+3.14);

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
    }else{
      vReal_5[i] = analogRead(CHANNEL);
      vImag_5[i] = 0;
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
      if(enable_1_over_x){
        pass_value = -(1/(pass_value-1))+1; 
      }
      if(enable_sigmoid){
        pass_value = 1.0/(1.0+pow(2.71828, -pass_value*sigmoid_smooth_factor)); 
      }

      while(led_no<(i+1)*NUM_LEDS/sample_size){
        ledsB[led_no].red = min((int) (red_base_line + red_factor * pass_value * amplifier), SIGNAL_MAX);
        ledsB[led_no].green = min((int) (green_base_line + green_factor * pass_value * amplifier), SIGNAL_MAX);
        ledsB[led_no].blue = min((int) (blue_base_line + blue_factor * pass_value * amplifier), SIGNAL_MAX);
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
        ledsB[led_no].red = (int) (red_base_line + red_factor * pass_value * amplifier);
        ledsB[led_no].green = (int) (green_base_line + green_factor * pass_value * amplifier);
        ledsB[led_no].blue = (int) (blue_base_line + blue_factor * pass_value * amplifier);
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
        ledsB[led_no].red = (int) (red_base_line + red_factor * pass_value * amplifier);
        ledsB[led_no].green = (int) (green_base_line + green_factor * pass_value * amplifier);
        ledsB[led_no].blue = (int) (blue_base_line + blue_factor * pass_value * amplifier);
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
        ledsB[led_no].red = (int) (red_base_line + red_factor * pass_value * amplifier);
        ledsB[led_no].green = (int) (green_base_line + green_factor * pass_value * amplifier);
        ledsB[led_no].blue = (int) (blue_base_line + blue_factor * pass_value * amplifier);
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
        ledsB[led_no].red = (int) (red_base_line + red_factor * pass_value * amplifier);
        ledsB[led_no].green = (int) (green_base_line + green_factor * pass_value * amplifier);
        ledsB[led_no].blue = (int) (blue_base_line + blue_factor * pass_value * amplifier);
        led_no++;
      }
    }  

  }else{
    FFT.Windowing(vReal_5, sample_size, FFT_WIN_TYP_HAMMING, FFT_FORWARD);
    FFT.Compute(vReal_5, vImag_5, sample_size, FFT_FORWARD);
    FFT.ComplexToMagnitude(vReal_5, vImag_5, sample_size);

    int led_no = 0;
    double total_pass_value=0;
    for(uint16_t i = 0; i < sample_size; i++)
    {
      pass_value_5[i] = i == 0 ? vReal_5[0] : 0.8*pass_value_5[i-1] + 0.2*vReal_5[i];
      total_pass_value += pass_value_5[i];
    }  

    double average = total_pass_value/sample_size;
    standard_deviation = 0;
    for(uint16_t i = 0; i < sample_size; i++)
    {
      standard_deviation += pow(pass_value_5[i] - average, 2);
    }
    standard_deviation = pow(standard_deviation/sample_size, 0.5);
    for(uint16_t i = 0; i < sample_size; i++)
    {
      double pass_value = pass_value_5[i];
      while(led_no<(i+1)*NUM_LEDS/sample_size){
        ledsB[led_no].red = (int) (red_base_line + red_factor * pass_value * amplifier);
        ledsB[led_no].green = (int) (green_base_line + green_factor * pass_value * amplifier);
        ledsB[led_no].blue = (int) (blue_base_line + blue_factor * pass_value * amplifier);
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
          _sample_size = _sample_size == 5 ? 5 : _sample_size+1 ;
        }else{
          _sample_size = _sample_size == 0 ? 0 : _sample_size-1 ;
        }
      }
      if(last_std_dev>standard_deviation*(1+STANDARD_DEV_WIDTH)){
        if(!STD_DEV_INCREASE_MOVES_UP_SAMPLING_FREQUENCY){
          _sample_size = _sample_size == 5 ? 5 : _sample_size+1 ;
        }else{
          _sample_size = _sample_size == 0 ? 0 : _sample_size-1 ;
        }
      }
    }
    last_std_dev = standard_deviation;
    amplifier = 10 * standard_deviation;
    //if(serial) Serial.println("= next band ===========");
    sample_hertz_ratio = SAMPLE_SIZE_TO_FREQUENCY_RATION * get_sample_size(_sample_size);
    sampling_period_us = round(1000000*(1.0/samplingFrequency));
    count = 0;
  }
}



// go here to change play lists

int master_play_list_index = 0;
int master_play_list_size = 96;
byte blend_amount[] = {
  0,2,4,8,8,4,2,1,1,2,4,8,8,4,2,0,
  0,2,4,8,8,4,2,1,1,2,4,8,8,4,2,0,
  0,2,4,8,8,4,2,1,1,2,4,8,8,4,2,0,
  8,12,16,24,36,36,24,36,36,24,36,36,24,16,12,8,
  8,12,16,24,36,36,24,36,36,24,36,36,24,16,12,8,
  8,12,16,24,36,36,24,36,36,24,36,36,24,16,12,8
};
//unsigned long finishes_in_one_second = 200; this is the final value //5 mins or 1 mins i forget
unsigned long finishes_in_one_second = 5;
unsigned long minutes_for_rotation = 1;
unsigned long count_of_ticks_in_animation = 0;
void loop() {
  count_of_ticks_in_animation++;
  if((1+count_of_ticks_in_animation)%(finishes_in_one_second*minutes_for_rotation*5)==0){
    master_play_list_index = (master_play_list_index + 1)%master_play_list_size;
  }

  if(master_play_list_index < 16){
    animationAColorSwipeRed();                                               // render the first animation into leds2   
    animationBAudio();                                               // render the second animation into leds3
  }
  if(master_play_list_index >= 16 && master_play_list_index < 32){
    animationAColorSwipeGreen();                                               // render the first animation into leds2   
    animationBAudio();                                               // render the second animation into leds3
  }
  if(master_play_list_index >= 32 && master_play_list_index < 48){
    animationAColorSwipeBlue();                                               // render the first animation into leds2   
    animationBAudio();                                               // render the second animation into leds3
  }
  if(master_play_list_index >= 48 && master_play_list_index < 64){
    animationAColorSwipeBlue();                                               // render the first animation into leds2   
    animationBAudio();                                               // render the second animation into leds3
  }
  if(master_play_list_index >= 64 && master_play_list_index < 80){
    animationAColorSwipeBlue();                                               // render the first animation into leds2   
    animationBAudio();                                               // render the second animation into leds3
  }
  if(master_play_list_index >= 80 && master_play_list_index < 96){
    animationAColorSwipeBlue();                                               // render the first animation into leds2   
    animationBAudio();                                               // render the second animation into leds3
  }

  for (int i = 0; i < NUM_LEDS; i++) {
    leds[i] = blend( ledsA[i], ledsB[i], blend_amount[master_play_list_index] );
  }
  FastLED.show();
}

