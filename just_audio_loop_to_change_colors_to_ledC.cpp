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

  // FastLED.show();
  ledStrip.write(ledsB, NUM_LEDS);
  

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
