//#include <SoftwareSerial.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h> // Library for LCD
#include "GoPowerInverter.h"


//SoftwareSerial mySerial(12,13); //RX, TX

LiquidCrystal_I2C lcd = LiquidCrystal_I2C(0x27, 20, 4); // Change to (0x27,20,4) for 20x4 LCD.


enum errCodes {
  success,
  unknown,
  error,
  undefined
};

struct invResponse {
  float returnValue;
  enum errCodes errorCode;
};


const int psPin=5;
const int buttonPin=3;
const int ledPin=12;
volatile bool readyState=false;

invResponse invResp;

//GoPowerInverter myGPI(12,13); //TX.RX

void setup() {
  lcd.init();
  lcd.backlight();
  
  pinMode(buttonPin, INPUT);
  pinMode(ledPin, OUTPUT);
  pinMode(psPin, OUTPUT);
  digitalWrite(psPin, HIGH);
  
  digitalWrite(ledPin, HIGH);

  //myGPI.begin(19200);
  
  
  Serial.begin(19200);
  lcd.setCursor(0,0);
  lcd.print("Starting...");


  while(digitalRead(buttonPin)==HIGH) //Wait for user to let go of button
  {
    delay(100);
  }
  blinkLED(4);  

  
  printLabels();

  attachInterrupt(digitalPinToInterrupt(buttonPin),buttonPushed,FALLING);
}


void buttonPushed() {
  if(readyState)
    digitalWrite(psPin, LOW);
}


void blinkLED(int numTimes) {
  int delayTime=150;
  for(int i=0;i<numTimes;i++) {
    digitalWrite(ledPin, LOW);
    delay(delayTime);
    digitalWrite(ledPin, HIGH);
    delay(delayTime);
  }
}

/*
void loop() {
  float voltage = myGPI.getVBAT();
  Serial.print("VBAT: ");
  Serial.println(voltage);
  delay(1000);
  myGPI.powerOff();
}
*/

float vinv;
float vbat;
float frq;
float iinv;
float pinv;


void loop() {
  if(millis() > 2000)
    readyState=true;
  
  queryInvValue(&invResp, "VBAT");
  vbat = invResp.returnValue;
  
  queryInvValue(&invResp, "VINV");
  vinv = invResp.returnValue;

  if(invResp.returnValue<10) {
    sendInvCommand(&invResp, "POWER 1");
  }
  
  queryInvValue(&invResp, "FRQ");
  frq = invResp.returnValue;

  queryInvValue(&invResp, "IINV");
  iinv = invResp.returnValue;

  queryInvValue(&invResp, "PINV");
  pinv = invResp.returnValue;

  delay(1000);

  printToLCD();

}

void printLabels()
{
  lcd.clear();
  //vinv
  lcd.setCursor(0,0);
  lcd.print("VINV:");

  //vbat
  lcd.setCursor(0, 1);
  lcd.print("VBAT:");

  //frq
  lcd.setCursor(0, 2);
  lcd.print("FREQ:");

  //iinv
  lcd.setCursor(0, 3);
  lcd.print("I:");

  //pinv
  lcd.setCursor(8, 3);
  lcd.print("P:");

}

void printToLCD()
{

  //vinv
  lcd.setCursor(6, 0);
  lcd.print(vinv);

  //vbat
  lcd.setCursor(6, 1);
  lcd.print(vbat);

  //frq
  lcd.setCursor(6, 2);
  lcd.print(frq);

  //iinv
  lcd.setCursor(3, 3);
  lcd.print(iinv);

  //pinv
  lcd.setCursor(10, 3);
  lcd.print(pinv);

}



void SerialPrintError(invResponse *resp)
{
  switch(resp->errorCode)
  {
    case success:
      //Serial.println("Success!");
      break;
    case unknown:
      //Serial.println("Unknown!");
      break;
    case error:
      //Serial.println("Error!");
      break;
  }
}

void sendInvCommand(invResponse *resp, String Command)
{
  String respVal = "-1";
  String errCode = "00";
  Serial.println(Command);
  
  errCode = Serial.readStringUntil('\n'); //Flush out error code waiting in buffer
  errCode.trim();
  switch(errCode.charAt(0))
  {
    case '=':
      resp->errorCode=success;
      break;
    case '?':
      resp->errorCode=unknown;
      break;
    case '!':
      resp->errorCode=error;
      break;
    default:
      resp->errorCode = undefined;
      break;
  }
}

void queryInvValue(invResponse *resp, String Verb)
{
  String respVal = "-1";
  String errCode = "00";
  Serial.print(Verb);Serial.println("?");
  
  respVal = Serial.readStringUntil('\n');
  respVal.trim();
  resp->returnValue = respVal.toFloat();
  
  errCode = Serial.readStringUntil('\n'); //Flush out error code waiting in buffer
  errCode.trim();
  switch(errCode.charAt(0))
  {
    case '=':
      resp->errorCode=success;
      break;
    case '?':
      resp->errorCode=unknown;
      break;
    case '!':
      resp->errorCode=error;
      break;
    default:
      resp->errorCode = undefined;
      break;
  }
}
