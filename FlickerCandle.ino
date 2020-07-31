// CONSTANTS
int const LED1 = 3;
int const LED2 = 5;
int const LED3 = 6;
int const POT = A4;
int const UPDATE_INT = 5;  // update the light status every x milliseconds (kinda like a frame rate)
int const PRINT_INT = 100;
int const STD_BRIGHT = 0.8;  // standard brightness, in fraction
int const FB = 255;  // full brightness value

// FM/(milliseconds in a second) * 1/transitions_per_second * UPDATE_INT;
float led1_rate = FB/(1000.0) * 1.0/1 * UPDATE_INT;
float led2_rate = FB/(1000.0) * 1.0/4 * UPDATE_INT;
float led3_rate = FB/(1000.0) * 1.0/1.05 * UPDATE_INT;

// VARIABLES
float led1_val = 0;
float led2_val = 0;
float led3_val = 0;
int last_time = millis();
int print_time = millis();
int t = 0;

/*
 * States:
 * 0 - constant
 * 1 - standing up (mostly constant)
 * 2 - standing down
 * 3 - flickering down
 * 4 - flickering up
 */
unsigned short led1_state = 0;
unsigned short led2_state = 0;
unsigned short led3_state = 0;


// FUNCTIONS
void pulse(float &rate, float &value, int FB = FB) {
  value += rate;
  if(value > FB || value < 0) {
    rate *= -1;
    value += rate * 2;
  }
}

void flicker(float &rate, float $value, int rdown, int rup) {
  // rdown is rate down
  // rup is rate up
  
}

// SET UP
void setup() {
  pinMode(LED1, OUTPUT);
  pinMode(LED2, OUTPUT);
  pinMode(LED3, OUTPUT);
  // pinMode(pot, INPUT);

  Serial.begin(115200);
}

// MAIN
void loop() {

  pulse(led1_rate, led1_val);
  pulse(led2_rate, led2_val);
  pulse(led3_rate, led3_val);
  
  analogWrite(LED1, led1_val);
  analogWrite(LED2, led2_val);
  analogWrite(LED3, led3_val);

  if(print_time + PRINT_INT < millis()) {
    Serial.print("led3_val: ");
    Serial.print(led3_val);
    Serial.print(", led3_rate: ");
    Serial.println(led3_rate);
    print_time = millis();
  }

  t = millis();
  if(last_time < t + UPDATE_INT) {
    delay(UPDATE_INT - (t-last_time));
  }
  last_time = millis();
}
