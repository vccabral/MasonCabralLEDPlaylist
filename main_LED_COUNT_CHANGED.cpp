#include "arduinoFFT.h"
#include <time.h>
#include <PololuLedStrip.h>

#define CHANNEL A0
// large increases buckets
int _sample_size = 0; 

// stuff on where the frequency band goes
bool auto_find_frequency_band = true;
byte CHANGE_FREQUENCY_EVAL_CYCLES = 100;
float STANDARD_DEV_WIDTH = 0.1;
bool STD_DEV_INCREASE_MOVES_UP_SAMPLING_FREQUENCY = true;
int SAMPLE_SIZE_TO_FREQUENCY_RATION = 60;

// large lowers the delay from 'frame updates'
int sample_hertz_ratio =  SAMPLE_SIZE_TO_FREQUENCY_RATION*8;

// how fast the bluetooth signal moves through the color band 
double color_cycle_factor = pow(2.0, -10);

// how much the bands get increased before adding to the base
double amplifier = 1.0 * pow(2.0, -6);

//globlal signal maximum
int SIGNAL_MAX = 1;

//whether to use a smooth sigmoid for 
// not used right now
bool enable_sigmoid = false;
double sigmoid_smooth_factor = 10; //higher means smoother
bool enable_1_over_x = false;

// base color of each item
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

PololuLedStrip<12> ledStrip;

// Create a buffer for holding the colors (3 bytes per color).
#define NUM_LEDS 175

rgb_color colors[NUM_LEDS];

// rgb_color colors[NUM_LEDS];


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

void setup()
{
  samplingFrequency = get_sample_size(_sample_size)*sample_hertz_ratio; //Hz, must be less than 10000 due to ADC
  sampling_period_us = round(1000000*(1.0/samplingFrequency));
}

// rgb_color hsvToRgb(uint16_t h, uint8_t s, uint8_t v)
// {
//     uint8_t f = (h % 60) * 255 / 60;
//     uint8_t p = (255 - s) * (uint16_t)v / 255;
//     uint8_t q = (255 - f * (uint16_t)s / 255) * (uint16_t)v / 255;
//     uint8_t t = (255 - (255 - f) * (uint16_t)s / 255) * (uint16_t)v / 255;
//     uint8_t r = 0, g = 0, b = 0;
//     switch((h / 60) % 6){
//         case 0: r = v; g = t; b = p; break;
//         case 1: r = q; g = v; b = p; break;
//         case 2: r = p; g = v; b = t; break;
//         case 3: r = p; g = q; b = v; break;
//         case 4: r = t; g = p; b = v; break;
//         case 5: r = v; g = p; b = q; break;
//     }
//     return rgb_color(r/100, g/100, b/100);
// }

void loop()
{
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
        colors[led_no].red = min((int) (red_base_line + red_factor * pass_value * amplifier), SIGNAL_MAX);
        colors[led_no].green = min((int) (green_base_line + green_factor * pass_value * amplifier), SIGNAL_MAX);
        colors[led_no].blue = min((int) (blue_base_line + blue_factor * pass_value * amplifier), SIGNAL_MAX);
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
        colors[led_no].red = (int) (red_base_line + red_factor * pass_value * amplifier);
        colors[led_no].green = (int) (green_base_line + green_factor * pass_value * amplifier);
        colors[led_no].blue = (int) (blue_base_line + blue_factor * pass_value * amplifier);
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
        colors[led_no].red = (int) (red_base_line + red_factor * pass_value * amplifier);
        colors[led_no].green = (int) (green_base_line + green_factor * pass_value * amplifier);
        colors[led_no].blue = (int) (blue_base_line + blue_factor * pass_value * amplifier);
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
        colors[led_no].red = (int) (red_base_line + red_factor * pass_value * amplifier);
        colors[led_no].green = (int) (green_base_line + green_factor * pass_value * amplifier);
        colors[led_no].blue = (int) (blue_base_line + blue_factor * pass_value * amplifier);
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
        colors[led_no].red = (int) (red_base_line + red_factor * pass_value * amplifier);
        colors[led_no].green = (int) (green_base_line + green_factor * pass_value * amplifier);
        colors[led_no].blue = (int) (blue_base_line + blue_factor * pass_value * amplifier);
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
        colors[led_no].red = (int) (red_base_line + red_factor * pass_value * amplifier);
        colors[led_no].green = (int) (green_base_line + green_factor * pass_value * amplifier);
        colors[led_no].blue = (int) (blue_base_line + blue_factor * pass_value * amplifier);
        led_no++;
      }
    }  

  }

  // FastLED.show();
  ledStrip.write(colors, NUM_LEDS);
  

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

