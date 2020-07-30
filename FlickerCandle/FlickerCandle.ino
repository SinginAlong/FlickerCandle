// CONSTANTS
int const LED1 = A1;
int const LED2 = A3;
int const LED3 = 3;
int const POT = A4;
int const UPDATE_INT = 5;  // update the light status every x milliseconds (kinda like a frame rate)
int const STD_BRIGHT = 0.8;  // standard brightness, in fraction
int const FB = 255;  // full brightness value

float LED_PULSE_RATE = FB/(1000.0/5.0);

// VARIABLES
float led1_val = 0;
float led2_val = 0;
float led3_val = 0;
int last_time = millis();


// FUNCTIONS

// SET UP
void setup() {
  pinMode(LED1, OUTPUT);
  pinMode(LED2, OUTPUT);
  pinMode(LED3, OUTPUT);
  // pinMode(pot, INPUT);

  Serial.begin(115200);

  Serial.print("LED Pulse Rate set to: ");
  Serial.println(LED_PULSE_RATE);
}

// MAIN
void loop() {

  led3_val += LED_PULSE_RATE;
  if(led3_val > FB || led3_val < 0) {
    LED_PULSE_RATE *= -1;
    led3_val += LED_PULSE_RATE*2;
  }

  analogWrite(LED1, led1_val);
  analogWrite(LED2, led2_val);
  analogWrite(LED3, led3_val);

  if(last_time < millis() + UPDATE_INT) {
    delay(UPDATE_INT - (millis()-last_time));
  }
  last_time = millis();
}
