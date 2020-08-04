// CONSTANTS
// int const POT = A4;
int const UPDATE_INT = 5;  // update the light status every x milliseconds (kinda like a frame rate)
int const PRINT_INT = 100;
int const STD_BRIGHT = 0.8;  // standard brightness, in fraction

// FM/(milliseconds in a second) * 1/transitions_per_second * UPDATE_INT;
// float led1_rate = FB/(1000.0) * 1.0/1 * UPDATE_INT;
// float led2_rate = FB/(1000.0) * 1.0/4 * UPDATE_INT;
// float led3_rate = FB/(1000.0) * 1.0/1.05 * UPDATE_INT;

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
    float pulse_rate = 1;
    bool pulse_done = false;
    float lvl = 200;  // the brightness level of the led
    float flicker_depth = 0;
    float urate = 0;  // up rate for current state
    float drate = 0;  // down rate for current state
    bool flicker_done = false;
    float wave_depth;
    float wave_rate = 0;
    float wave_duration = 0;  // in millis
    bool wave_done = false;

    unsigned short state = 1;
    /*
    * States:
    * 0 - constant
    * 1 - pulsing
    * 2 - flickering
    * 3 - waving
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

void LED::pulse() {
  lvl += pulse_rate;
  if(lvl < 0) {
    pulse_rate *= -1;
    lvl += pulse_rate*2;
  }
  if(lvl > FB) {
    lvl = FB;
    pulse_done = true;
    state = 0;
  }
}

void LED::start_flicker(float rateup, float ratedown, float depth) {
  state = 2;
  urate = rateup;
  drate = ratedown;
  flicker_depth = depth;
}

void LED::flicker() {
  if(lvl > flicker_depth) {
    lvl -= drate;
  } else {
    lvl += urate;
    if(lvl > FB) {
      lvl = FB;
      flicker_done = true;
      state = 0;
    }
  }
}

void LED::update_led() {
  if(state == 0) {
    // do nothing, leave the lvl at current, constant
  }
  if(state == 1) {  // pulsing
    this->pulse();
  }
  if(state == 2) { // flickering
   this->flicker();
  }
  
  analogWrite(pin, lvl);
}

bool LED::is_event() {
  if(pulse_done || flicker_done || wave_done) {
    return true;
  }
  return false;
}

// not sure the best way to handle events (make event class, requires deleting)

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
