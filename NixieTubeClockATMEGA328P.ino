#include <RTC.h>
#include <Wire.h>

static DS1307 RTC;

//declare constants for pins going to SN74HC595N
const int latchPin = 9;
const int clockPin = 10;
const int dataPin = 8;

//declare constants for the bytes to be sent to 
//SN74HC595N which correspond to the inputs for the K155ID1
const byte minutesTens[6] = {0b00000000,0b10000000,0b01000000,0b11000000,0b00100000,0b10100000};
const byte minutesOnes[10] = {0b00000000,0b00001000,0b00000100,0b00001100,0b00000010,0b00001010,0b00000110,0b00001110,0b00000001,0b00001001};
const byte hoursTens[3] = {0b00000000,0b10000000,0b01000000};
const byte hoursOnes[10] = {0b00000000,0b00001000,0b00000100,0b00001100,0b00000010,0b00001010,0b00000110,0b00001110,0b00000001,0b00001001};

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
  shiftOut(dataPin, clockPin, LSBFIRST, hrs);
  shiftOut(dataPin, clockPin, LSBFIRST, mins);
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
  //update time from RTC
  rtcMinutes = RTC.getMinutes();
  rctHours = RTC.getHours();

  //apply leading zero if necessary
  zeroedMinutes = isLeadingZero(rtcMinutes);
  zeroedHours = isLeadingZero(rtcHours);

  //separate the ones and tens
  hTensHolder = zeroedHours[0];
  hOnesHolder = zeroedHours[1];
  mTensHolder = zeroedHours[0];
  mOnesHolder = zeroedHours[1];

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
  
}
