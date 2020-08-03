// CONSTANTS
//int const LED1 = 3;
//int const LED2 = 5;
//int const LED3 = 6;
int const POT = A4;
int const UPDATE_INT = 5;  // update the light status every x milliseconds (kinda like a frame rate)
int const PRINT_INT = 100;
int const STD_BRIGHT = 0.8;  // standard brightness, in fraction
//int const FB = 255;  // full brightness value

// FM/(milliseconds in a second) * 1/transitions_per_second * UPDATE_INT;
// float led1_rate = FB/(1000.0) * 1.0/1 * UPDATE_INT;
// float led2_rate = FB/(1000.0) * 1.0/4 * UPDATE_INT;
// float led3_rate = FB/(1000.0) * 1.0/1.05 * UPDATE_INT;

// VARIABLES
int last_time = millis();
int print_time = millis();
int t = 0;

// CLASSES
class LED {
  private:
    int pin = -1;
    int FB = 255;  // full brightness / top level
    float pulse_rate = 1;
    float lvl = 200;  // the brightness level of the led
    float rup = 0;  // up rate for current state
    float rdown = 0;  // down rate for current state
    float wave_rate = 0;
    float wave_duration = 0;  // in millis

    unsigned short state = 1;
    /*
    * States:
    * 0 - constant
    * 1 - pulsing
    * 2 - standing up (mostly constant)
    * 3 - standing down
    * 4 - flickering down
    * 5 - flickering up
    */
        
  public:
    LED(int led_pin);
    void update_led();
    void pulse();
    void flicker();
    void wave();
    void start_flicker(float rup, float rdown);
    void start_wave(float wave_rate, int duration);
};

LED::LED(int led_pin) {
  pin = led_pin;
  pinMode(pin, OUTPUT);
}

void LED::update_led() {
  if(state == 1) {  // pulsing
    lvl += pulse_rate;
    if(lvl > FB || lvl < 0) {
      pulse_rate *= -1;
      lvl += pulse_rate * 2;
    }
  }

  analogWrite(pin, lvl);
}

LED led1(3);
LED led2(5);
LED led3(6);

// SET UP
void setup() {
  Serial.begin(115200);
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
