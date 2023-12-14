//Version 0.9.1
// Code clean-up
//Code is mostly untested. Waiting for circuit boards to arrive
//This is a bespoke sketch for a Nixie Tube Clock that runs on an ATMega328P_PU
//   that uses two SN74HC595N's to control four K155ID1 nixie tube controllers
//See https://github.com/MordeKyle/NixieTubeClockATMEGA328P

#include <I2C_RTC.h>
#include <Wire.h>
#include <ezButton.h>

static DS1307 RTC;

//declare constants for pins going to SN74HC595N

const int latchPin = 9;
const int clockPin = 10;
const int dataPin = 8;

//declare constants for displaying correct numbers from shift register to K155ID1

const byte zeroTens = 0b00000000;
const byte oneTens = 0b10000000;
const byte twoTens = 0b01000000;
const byte threeTens = 0b11000000;
const byte fourTens = 0b00100000;
const byte fiveTens = 0b10100000;
const byte sixTens = 0b01100000;
const byte sevenTens = 0b11100000;
const byte eightTens = 0b00010000;
const byte nineTens = 0b10010000;

const byte zeroOnes = 0b00000000;
const byte oneOnes = 0b00001000;
const byte twoOnes = 0b00000100;
const byte threeOnes = 0b00001100;
const byte fourOnes = 0b00000010;
const byte fiveOnes = 0b00001010;
const byte sixOnes = 0b00000110;
const byte sevenOnes = 0b00001110;
const byte eightOnes = 0b00000001;
const byte nineOnes = 0b00001001;

//declare constants for the bytes to be sent to 
//SN74HC595N which correspond to the inputs for the K155ID1
//This is for ease of programming

const byte minutesTens[6] = {zeroTens,oneTens,twoTens,threeTens,fourTens,fiveTens};
const byte minutesOnes[10] = {zeroOnes,oneOnes,twoOnes,threeOnes,fourOnes,fiveOnes,sixOnes,sevenOnes,eightOnes,nineOnes};
const byte hoursTens[3] = {zeroTens,oneTens,twoTens};
const byte hoursOnes[10] = {zeroOnes,oneOnes,twoOnes,threeOnes,fourOnes,fiveOnes,sixOnes,sevenOnes,eightOnes,nineOnes};

//delcare variables to hold times from DS1307

int rtcMinutes;
int rtcHours;

//declare holders to be converted to int
String hTensHolder;
String hOnesHolder;
String mTensHolder;
String mOnesHolder;

//declare constant for a leading zero

const String leadingZero = "0";

//buttons and switch

const int adjustSwitch = 7;
int adjustSwitchState;
ezButton hoursButton(5);
ezButton minutesButton(6);

//function to update SN74HC595N

void updateRegister(byte mins, byte hrs)
{
  digitalWrite(latchPin, LOW); //set latch to low
  shiftOut(dataPin, clockPin, LSBFIRST, mins); //send the minutes out
  shiftOut(dataPin, clockPin, LSBFIRST, hrs); //send the hours out
  digitalWrite(latchPin, HIGH); //set latch to high
}

//function to decide if a leading zero needs to be added or not
String isLeadingZero(int input)
{
  String holder;
  if (input<10)
  {
    holder = String(leadingZero + input);
  }
  else
  {
    holder = String(input);
  }
  return holder;
}

// function to update the display

void updateDisplay(int hoursInput, int minutesInput)
{
  String zeroedHours = isLeadingZero(hoursInput); //apply leading zero if necessary
  String zeroedMinutes = isLeadingZero(minutesInput); //apply leading zero if necessary

  hTensHolder = zeroedHours[0]; //pulls out first hour digit
  hOnesHolder = zeroedHours[1]; //pulls out second hour digit
  mTensHolder = zeroedMinutes[0]; //pulls out first minute digit
  mOnesHolder = zeroedMinutes[1]; //pulls out second minute digit

  int hTens = hTensHolder.toInt(); //convert to int
  int hOnes = hOnesHolder.toInt(); //convert to int
  int mTens = mTensHolder.toInt(); //convert to int
  int mOnes = mOnesHolder.toInt(); //convert to int

  byte hours = hoursTens[hTens] + hoursOnes[hOnes]; //add hours nibbles to byte
  byte minutes = minutesTens[mTens] + minutesOnes[mOnes]; //add minutes nibbles to byte

  updateRegister(minutes, hours); //update shift registers with time
}

void setup()
{
  Serial.begin(9600);
  RTC.begin();
  Serial.println();
  Serial.println("DS 1307 RTC");
  Serial.print("Is DS1307 Running: ");
  if (RTC.isRunning())
  {
    Serial.println("Yes");
  }
  else
  {
    Serial.println("No");
  }
  pinMode(latchPin, OUTPUT);
  pinMode(clockPin, OUTPUT);
  pinMode(dataPin, OUTPUT);
  pinMode(adjustSwitch, INPUT);

  hoursButton.setDebounceTime(50);
  minutesButton.setDebounceTime(50);
}

void loop()
{
  //read the state of the time setting switch

  adjustSwitchState = digitalRead(adjustSwitch);

  //if the switch is in time keeping mode

  if (adjustSwitchState == 0)
  {
    rtcHours = RTC.getHours(); //pull hours from RTC
    rtcMinutes = RTC.getMinutes(); //pull minutes from RTC
    updateDisplay(rtcHours, rtcMinutes); //update the display
  }

  //if the time setting switch is in adjust mode

  else
  {
    hoursButton.loop();
    minutesButton.loop();
    int hoursButtonState = hoursButton.getState();
    int minutesButtonState = minutesButton.getState();

    rtcHours = RTC.getHours(); //pull hours from RTC
    rtcMinutes = RTC.getMinutes(); //pull minutes from RTC
    updateDisplay(rtcHours, rtcMinutes); //update display

    if (hoursButton.isPressed())
    {
      if (RTC.getHourMode() == CLOCK_H24)
      {
        rtcHours = RTC.getHours(); //pull hours from RTC
        if (rtcHours < 23)
        {
          rtcHours = rtcHours + 1; //add 1 hour
          RTC.setHours(rtcHours); //set hours in RTC
          delay(250); //delay for 250 milliseconds (maybe not necessary)
          rtcHours = RTC.getHours(); //pull hours from RTC
          rtcMinutes = RTC.getMinutes(); //pull minutes from RTC
          updateDisplay(rtcHours, rtcMinutes); //update display
        }
        else
        {
          rtcHours = 0; //set hours to 0
          RTC.setHours(rtcHours); //set time in RTC
          delay(250); //delay for 250 milliseconds (maybe not necessary)
          rtcHours = RTC.getHours(); //pull hours from RTC
          rtcMinutes = RTC.getMinutes(); //pull minutes from RTC
          updateDisplay(rtcHours, rtcMinutes); //update display
        }
      }
    }
    if (minutesButton.isPressed())
    {
      rtcMinutes = RTC.getMinutes(); //pull minutes from RTC
      if (rtcMinutes < 59)
      {
        rtcMinutes = rtcMinutes + 1; //add 1 minute
        RTC.setMinutes(rtcMinutes); //set minutes in RTC
        delay(250); //delay for 250 milliseconds (maybe not necessary)
        rtcHours = RTC.getHours(); //pull hours from RTC
        rtcMinutes = RTC.getMinutes(); //pull minutes from RTC
        updateDisplay(rtcHours, rtcMinutes); //update display
      }
      else
      {
        rtcMinutes = 0; //set minutes to 0
        RTC.setMinutes(rtcMinutes); //set minutes in RTC
        delay(250); //delay for 250 milliseconds (maybe not necessary)
        rtcHours = RTC.getHours(); //pull hours from RTC
        rtcMinutes = RTC.getMinutes(); //pull minutes from RTC
        updateDisplay(rtcHours, rtcMinutes); //update display
      }
    }
  }
}
