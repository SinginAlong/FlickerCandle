// CONSTANTS
// int const POT = A4;
int const UPDATE_INT = 5;  // update the light status every x milliseconds (kinda like a frame rate)
int const PRINT_INT = 500;
int const STD_BRIGHT = 0.8;  // standard brightness, in fraction

// VARIABLES
int last_time = millis();
int print_time = millis();
int t = 0;

// FUNCTIONS
float norm(float mean, float sigma) {
  // takes mean and sigma (standard div) and returns a random value with a normal distribution probablity
  //return value=exp(-0.5*pow((x-mean)/sigma,2.));
  return sqrt(-2*log(random(0,1000)/1000.0)) * cos(2*PI*random(0,1000)/1000.0) * sigma + mean;
}

// CLASSES
class LED {
  private:
    int pin = -1;
    int FB = 255;  // full brightness / top level
    float lvl = 255;  // the brightness level of the led
    
    float pulse_rate = 1;
    float flicker_depth = 0;
    float urate = 0;  // up rate for current state
    float drate = 0;  // down rate for current state
    int wave_depth;
    float wave_rate = 0;
    // float wave_duration = 0;  // in millis - with this actually be used?
    
    unsigned short pulse_state = 0;
    unsigned short flicker_state = 2;
    unsigned short wave_state = 2;
    unsigned short state = 1;
    /*
    * States:
    * 0 - not active
    * 1 - active
    * 2 - complete, unacknowledged
    * 3 - complete, acknowledged
    */

    // FLICKER CONSTANTS, SPF is seconds per frame (technically milliseconds per frame)
    // depth is the bottom of the dip (as opposed to the amount down from the top)
    float const flicker_rate_down_mean = UPDATE_INT * 255 / 300;  // SPF * depth / time, in lvl per 5 ms
    float const flicker_rate_down_sigma =  UPDATE_INT * 0.1;      // in lvl per 5 ms
    float const flicker_rate_up_mean = UPDATE_INT * 255 / 700;    // SPF * depth / time, in lvl per 5 ms
    float const flicker_rate_up_sigma = UPDATE_INT * 0.1;         // in lvl per 5 ms
    int const flicker_depth_mean = 20;                            // in lvl
    int const flicker_depth_sigma = 10;                           // in lvl
    
    float const wave_rate_mean = UPDATE_INT * 255 / 1320;         // SPF * depth / time, in lvl per 5 ms
    float const wave_rate_sigma = UPDATE_INT * 10;                // SPT * depth / time, in lvl per 5 ms
    int const wave_depth_mean = 100;                              // in lvl
    int const wave_depth_sigma = 30;                             // in lvl
        
  public:
    LED(int led_pin);
    void update_led();
    void pulse();
    void flicker();
    void wave();
    void start_pulse(float rate);
    void start_flicker(float rateup, float ratedown, float depth);
    void start_wave(float rate, int depth);
    bool pulse_done();
    bool flicker_done();
    bool wave_done();
    void handle_LED_events();
    void print_lvl();
    void print_flicker_state();
};

LED::LED(int led_pin) {
  pin = led_pin;
  pinMode(pin, OUTPUT);
}

void LED::start_pulse(float rate) {
  state = 1;
  pulse_rate = -1*abs(rate);
}

void LED::start_flicker(float rateup, float ratedown, float depth) {
  state = 2;
  urate = rateup;
  drate = ratedown;
  flicker_depth = depth;
  Serial.print("LED pin: ");
  Serial.print(pin);
  Serial.print(" started flicker, rateup: ");
  Serial.print(urate);
  Serial.print(", ratedown: ");
  Serial.print(drate);
  Serial.print(", depth: ");
  Serial.println(flicker_depth);
}

void LED::start_wave(float rate, int depth) {
  wave_state = 1;
  wave_rate = -1*abs(rate);
  wave_depth = depth;
  //wave_duration = duration;
}

void LED::pulse() {
  // FM/(milliseconds in a second) * 1/transitions_per_second * UPDATE_INT;
  // float led1_rate = FB/(1000.0) * 1.0/1 * UPDATE_INT;
  lvl += pulse_rate;
  if(lvl < 0) {
    pulse_rate *= -1;
    lvl += pulse_rate*2;
  }
  if(lvl > FB) {
    lvl = FB;
    pulse_state = 2;
  }
}

void LED::flicker() {
  if(lvl > flicker_depth) {
    lvl -= drate;
  } else {
    lvl += urate;
    if(lvl > FB) {
      lvl = FB;
      flicker_state = 2;  // complete, unacknowledged
    }
  }
}

void LED::wave() {
  if(lvl > wave_depth) {
    lvl -= wave_rate;
  } else {
    lvl += wave_rate;
    if(lvl > FB) {
      lvl = FB;
      wave_state = 2;
    }
  }
}

void LED::update_led() {
  if(pulse_state == 1) {
    this->pulse();
  }
  if(flicker_state == 1) {
   this->flicker();
  }
  if(wave_state == 1) {
    this->wave();
  }
  analogWrite(pin, lvl);
}

bool LED::pulse_done() {
  if(pulse_state == 2) {
    pulse_state = 3;
    return true;
  }
  return false;
}

bool LED::flicker_done() {
  if(flicker_state >= 2) {
    flicker_state = 3;
    return true;
  }
  return false;
}

bool LED::wave_done() {
  if(wave_state >= 2) {
    wave_state = 3;
    return true;
  }
  return false;
}

void LED::handle_LED_events() {
  /*if(this->pulse_done()) {
    this->start_pulse(
  } */
  if(this->flicker_done()) {
    this->start_flicker(norm(flicker_rate_up_mean, flicker_rate_up_sigma),
                        norm(flicker_rate_down_mean, flicker_rate_down_sigma),
                        norm(flicker_depth_mean, flicker_depth_sigma));
  }
  if(this->wave_done()) {
    this->start_wave(norm(wave_rate_mean, wave_rate_sigma),
                     norm(wave_depth_mean, wave_depth_sigma));
  }
}

void LED::print_lvl() {
  Serial.println(lvl);
}
void LED::print_flicker_state() {
  Serial.println(flicker_state);
}

LED led1(3);
LED led2(5);
LED led3(6);


// SET UP
void setup() {
  Serial.begin(115200);
  randomSeed(analogRead(0));
}

// MAIN
void loop() {
  led1.handle_LED_events();
  led2.handle_LED_events();
  led3.handle_LED_events();
  
  led1.update_led();
  led2.update_led();
  led3.update_led();

  t = millis();
  if(t > print_time + PRINT_INT) {
    // led1.print_lvl();
    // led2.print_lvl();
    // led3.print_lvl();
    // led1.print_flicker_state();
    print_time = t;
  }
  
  t = millis();
  if(t < (last_time + UPDATE_INT)) {
    delay(UPDATE_INT - (t-last_time));
  }
  last_time = millis();
}
