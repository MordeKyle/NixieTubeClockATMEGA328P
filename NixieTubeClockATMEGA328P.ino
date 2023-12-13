#include <I2C_RTC.h>
#include <Wire.h>
#include <AcksenButton.h>

static DS1307 RTC;

//declare constants for pins going to SN74HC595N
const int latchPin = 9;
const int clockPin = 10;
const int dataPin = 8;

//declare constants for displaying correct numbers from shift register to K155ID1
const byte zeroTens = 0b00000000;
const byte oneTens = 0b00010000;
const byte twoTens = 0b00100000;
const byte threeTens = 0b00110000;
const byte fourTens = 0b01000000;
const byte fiveTens = 0b01010000;
const byte sixTens = 0b01100000;
const byte sevenTens = 0b01110000;
const byte eightTens = 0b10000000;
const byte nineTens = 0b10010000;

const byte zeroOnes = 0b00000000;
const byte oneOnes = 0b00000001;
const byte twoOnes = 0b00000010;
const byte threeOnes = 0b00000011;
const byte fourOnes = 0b00000100;
const byte fiveOnes = 0b00000101;
const byte sixOnes = 0b00000110;
const byte sevenOnes = 0b00000111;
const byte eightOnes = 0b00001000;
const byte nineOnes = 0b00001001;

//declare constants for the bytes to be sent to 
//SN74HC595N which correspond to the inputs for the K155ID1
//This is for ease of programming
const byte minutesTens[6] = {zeroTens,oneTens,twoTens,threeTens,fourTens,fiveTens};
const byte minutesOnes[10] = {zeroOnes,oneOnes,twoOnes,threeOnes,fourOnes,fiveOnes,sixOnes,sevenOnes,eightOnes,nineOnes};
const byte hoursTens[3] = {zeroTens,oneTens,twoTens};
const byte hoursOnes[10] = {zeroOnes,oneOnes,twoOnes,threeOnes,fourOnes,fiveOnes,sixOnes,sevenOnes,eightOnes,nineOnes};

//declare variables to send into shift register function
byte hours;
byte minutes;

//delcare variables to hold times from DS1307
int rtcMinutes;
int rtcHours;

//declare constant for a leading zero
const String leadingZero = "0";

//declare variable to hold return from isLeadingZero
String zeroedMinutes;
String zeroedHours;

//declare variables to hold single digits of time
int hTens;
int hOnes;
int mTens;
int mOnes;

//declare variable to hold single digit of time in string form for conversion
String hTensHolder;
String hOnesHolder;
String mTensHolder;
String mOnesHolder;

//function to update SN74HC595N
void updateRegister(byte mins, byte hrs)
{
  digitalWrite(latchPin, LOW);
  shiftOut(dataPin, clockPin, LSBFIRST, mins);
  shiftOut(dataPin, clockPin, LSBFIRST, hrs);
  digitalWrite(latchPin, HIGH);
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
}

void loop()
{
  //TODO add toggle switch that decides if clock is in timekeeping mode
  //or in time setting mode

  //TODO the following will fall under timekeeping mode
  //update time from RTC
  rtcMinutes = RTC.getMinutes();
  rtcHours = RTC.getHours();

  //apply leading zero if necessary
  zeroedMinutes = isLeadingZero(rtcMinutes);
  zeroedHours = isLeadingZero(rtcHours);

  //separate the ones and tens
  hTensHolder = zeroedHours[0];
  hOnesHolder = zeroedHours[1];
  mTensHolder = zeroedMinutes[0];
  mOnesHolder = zeroedMinutes[1];

  //convert to int
  hTens = hTensHolder.toInt();
  hOnes = hOnesHolder.toInt();
  mTens = mTensHolder.toInt();
  mOnes = mOnesHolder.toInt();
  
  //add the ones and tens bits into bytes
  minutes = minutesTens[mTens] + minutesOnes[mOnes];
  hours = hoursTens[hTens] + hoursOnes[hOnes];

  //send the bytes to the shift registers
  updateRegister(minutes, hours);
  //TODO end of timekeeping mode

  //TODO the following will fall under time setting mode
  //wait for button press to adjust hours
  //test for hours format
  if (RTC.getHourMode() == CLOCK_H24)
  {
    rtcHours = RTC.getHours();
    if (rtcHours < 23)
    {
      rtcHours = rtcHours + 1;
      RTC.setHours(rtcHours);
    }
    else
    {
      rtcHours = 0;
      RTC.setHours(rtcHours);
    }
  }
  //wait for button press to adjust minutes
  //TODO end of time setting mode
  
}
