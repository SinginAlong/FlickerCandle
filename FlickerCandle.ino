// CONSTANTS
// int const POT = A4;
int const UPDATE_INT = 5;  // update the light status every x milliseconds (kinda like a frame rate)
int const PRINT_INT = 100;
int const STD_BRIGHT = 0.8;  // standard brightness, in fraction

// VARIABLES
int last_time = millis();
int print_time = millis();
int t = 0;

// FUNCTIONS
float norm(float mean, float sigma) {
  // takes mean and sigma (standard div) and returns a random value with a normal distribution probablity
  //return value=exp(-0.5*pow((x-mean)/sigma,2.));
  return sqrt(-2*log(random(0,1))) * cos(2*PI*random(0,1)) * sigma + mean;
}

// CLASSES
class LED {
  private:
    int pin = -1;
    int FB = 255;  // full brightness / top level
    float lvl = 200;  // the brightness level of the led
    
    float pulse_rate = 1;
    float flicker_depth = 0;
    float urate = 0;  // up rate for current state
    float drate = 0;  // down rate for current state
    float wave_depth;
    float wave_rate = 0;
    float wave_duration = 0;  // in millis - with this actually be used?
    
    unsigned short pulse_state = 0;
    unsigned short flicker_state = 0;
    unsigned short wave_state = 0;
    unsigned short state = 1;
    /*
    * States:
    * 0 - not active
    * 1 - active
    * 2 - complete, unacknowledged
    * 3 - complete, acknowledged
    */
        
  public:
    LED(int led_pin);
    void update_led();
    void pulse();
    void flicker();
    void wave();
    void start_pulse(float rate);
    void start_flicker(float rateup, float ratedown, float depth);
    void start_wave(float wave_rate, int duration);
    bool is_event();  // returns if there was an event
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
}

void LED::start_wave(float rate, int duration) {
  wave_state = 1;
  wave_rate = -1*abs(rate);
  wave_duration = duration;
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

bool LED::is_event() {
  if(pulse_state == 2 || flicker_state == 2 || wave_state == 2) {
    return true;
  }
  return false;
}

// not sure the best way to handle events

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
  led1.update_led();
  led2.update_led();
  led3.update_led();
  
  t = millis();
  if(last_time < t + UPDATE_INT) {
    delay(UPDATE_INT - (t-last_time));
  }
  last_time = millis();
}
