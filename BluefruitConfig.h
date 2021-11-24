#define BUFSIZE                        128   // Size of the read buffer for incoming data
#define VERBOSE_MODE                   true  // If set to 'true' enables debug output

// SOFTWARE UART SETTINGS
// The following macros declare the pins that will be used for 'SW' serial.
#define BLUEFRUIT_SWUART_RXD_PIN       9
#define BLUEFRUIT_SWUART_TXD_PIN       10
#define BLUEFRUIT_UART_CTS_PIN         11
#define BLUEFRUIT_UART_RTS_PIN         8
#define BLUEFRUIT_UART_MODE_PIN        -1 

// FACTORYRESET_ENABLE       Perform a factory reset when running(1 will enable reset)
#define FACTORYRESET_ENABLE         0
#define MINIMUM_FIRMWARE_VERSION    "0.6.6"
#define MODE_LED_BEHAVIOUR          "MODE"

// Pin definitions
#define RPM       2
#define MAXDO     3
#define MAXCLK    4
#define MAXCS1    5
#define MAXCS2    6
#define DoubleBandPowerPin 7
#define SingleBandPowerPin 8
#define MotorPower A0

////////// Initiallizing All Parameters //////////

// Thermocouple
Adafruit_MAX31855 thermocouple1(MAXCLK, MAXCS1, MAXDO);
Adafruit_MAX31855 thermocouple2(MAXCLK, MAXCS2, MAXDO);

//PID Controller
double Setpoint, Input, Output;
int WindowSize = 5000;
unsigned long windowStartTime;
PID myPID(&Input, &Output, &Setpoint, 2, 0, 0, DIRECT);

// Stepper Motor
int StepsRequired;
const float steps_per_rev = 32;                             //Internal motor revolution
const float gear_reduction = 64;                            //Gear reduction
const float total_steps = steps_per_rev*gear_reduction;     //Total # of steps
Stepper steppermotor(steps_per_rev, 9, 11, 10, 12);         // Pins used are 9, 10, 11, 12(IN1, IN2, IN3, IN4, respectively)

RPM Counter
unsigned int counter = 0;
float diskslots = 24.00;
float rotation;

// Timer Count
void ISR_count(){
 counter++;
}

// Timer
void ISR_timerone(){
 Timer1.detachInterrupt();
 float rotation = (counter / diskslots) * 60.00;
 counter = 0;
 Timer1.attachInterrupt(ISR_timerone);
}

A small helper
void error(const __FlashStringHelper*err) {
 Serial.println(err);
 while (1);
}

SoftwareSerial bluefruitSS = SoftwareSerial(BLUEFRUIT_SWUART_TXD_PIN, BLUEFRUIT_SWUART_RXD_PIN);
Adafruit_BluefruitLE_UART ble(bluefruitSS, BLUEFRUIT_UART_MODE_PIN, BLUEFRUIT_UART_CTS_PIN, BLUEFRUIT_UART_RTS_PIN);

void setup(void){
  // Pin Setup, Bluetooth uses A0->A3
  pinMode(A0, OUTPUT);
  digitalWrite(A0, HIGH);
 // pinMode(A1, OUTPUT);
 // digitalWrite(A1, HIGH);
//  pinMode(A2, OUTPUT);
//  digitalWrite(A2, HIGH);
//  pinMode(A3, OUTPUT);
//  digitalWrite(A3, HIGH);
  pinMode(DoubleBandPowerPin, OUTPUT);
  pinMode(SingleBandPowerPin, OUTPUT);

  // PID Setup
 windowStartTime = millis();
 Setpoint = 180;
 myPID.SetOutputLimits(0, WindowSize);
 myPID.SetMode(AUTOMATIC);

 while (!Serial);
 delay(500);

  ///// Turn on heaters for 8 seconds ////
  Serial.begin(9600);
  digitalWrite(DoubleBandPowerPin, HIGH);
  digitalWrite(SingleBandPowerPin, HIGH);
  delay(8000);

  // Initialize timer interrupt
 Timer1.initialize(1000000);
 attachInterrupt(digitalPinToInterrupt(2), ISR_count, RISING);
 Timer1.attachInterrupt(ISR_timerone);

  // Begin Bluetooth
 Serial.begin(115200);
 if ( !ble.begin(VERBOSE_MODE) ){
   error(F("Couldn't find Bluefruit, make sure it's in Command mode & check wiring?"));
 }
 Serial.println( F("OK!") );
 if (FACTORYRESET_ENABLE){
   Serial.println(F("Performing a factory reset: "));
   if ( ! ble.factoryReset() ){
     error(F("Couldn't factory reset"));
   }
 }
 
 ble.echo(false);
 ble.verbose(false);  // debug info is a little annoying after this point!

 while (! ble.isConnected()){
   delay(500);}

 // LED Activity command is only supported from 0.6.6
 if ( ble.isVersionAtLeast(MINIMUM_FIRMWARE_VERSION)){
   ble.sendCommandCheckOK("AT+HWModeLED=" MODE_LED_BEHAVIOUR); // Change Mode LED Activity
 }
}
