#include <SPI.h>
#include <Stepper.h>
#include <SoftwareSerial.h>
#include <Arduino.h>
#include <PID_v1.h>
#include "TimerOne.h"
#include "Adafruit_MAX31855.h"
#include "Adafruit_BLE.h"
#include "Adafruit_BluefruitLE_UART.h"
#include "BluefruitConfig.h"

void loop() {

   double DoubleBand = thermocouple1.readCelsius();
   if (isnan(DoubleBand)) {
     Serial.println("Something wrong with thermocouple!");
   } else {
     Serial.print("Double Band = ");
     Serial.println(DoubleBand);
   }

   double SingleBand = thermocouple2.readCelsius();
   if (isnan(SingleBand)) {
     Serial.println("Something wrong with thermocouple!");
   } else {
     Serial.print("Single Band = ");
     Serial.println(SingleBand);
   }

   
//    Simple, non-PID version that is usable
   
//    if(DoubleBand = 0.0){
//      digitalWrite(DoubleBandPowerPin, LOW);}
//    else if(DoubleBand <= 160.0) {
//     digitalWrite(DoubleBandPowerPin, HIGH);} 
//    else if(DoubleBand > 160.0){
//      digitalWrite(DoubleBandPowerPin, LOW);}
//    else {
//      digitalWrite(DoubleBandPowerPin, LOW);}
//    if(SingleBand = 0.0){
//      digitalWrite(SingleBandPowerPin, LOW);}
//    else if(SingleBand <= 170.0) {
//     digitalWrite(SingleBandPowerPin, HIGH);} 
//    else if(SingleBand > 170.0){
//      digitalWrite(SingleBandPowerPin, LOW);}
//    else {
//      digitalWrite(SingleBandPowerPin, LOW);}

//    if((DoubleBandPowerPin && SingleBandPowerPin) == HIGH){
//      digitalWrite(MotorPower, HIGH);
//    }
//    else { digitalWrite(MotorPower, LOW);}


// }
     
  /// Read all sensors /////
 String temp1write = "temp1";
 String temp1 = temp1write + thermocouple1.readCelsius();
 ble.print("AT+BLEUARTTX=");
 ble.println(temp1);
 String temp2write = "temp2";
 String temp2 = temp2write + thermocouple2.readCelsius();
 ble.print("AT+BLEUARTTX=");
 ble.println(temp2);
 String AugurRPMWrite = "auguerSpeed";
 String AugurRPM = AugurRPMWrite + rotation;
 ble.print("AT+BLEUARTTX=");
 ble.println(AugurRPM);
 String diameterWrite = "diameter";
 String diameter = diameterWrite + diameter;
 ble.print("AT+BLEUARTTX=");
 ble.println(diameter);
 String pumpSpeedWrite = "pumpSpeed";
 String pumpSpeed = pumpSpeedWrite + pumpSpeed;
 ble.print("AT+BLEUARTTX=");
 ble.println(pumpSpeed);


  /// Output temperature based off reading /////
 double input1 = thermocouple1.readCelsius();
 double input2 = thermocouple2.readCelsius();
 myPID.Compute();
 unsigned long now = millis();
 if (now - windowStartTime > WindowSize){
   windowStartTime += WindowSize; }
 if (Output > now - windowStartTime){
   digitalWrite(PowerPin1 && PowerPin2, HIGH); }
 else {digitalWrite(PowerPin1 && PowerPin2, LOW); }
   
  /// Set puller motor speed /////
 StepsRequired = total_steps;
 steppermotor.setSpeed(350);
 steppermotor.step(StepsRequired);
}
