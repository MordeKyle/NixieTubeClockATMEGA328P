//declare constants for pins going to SN74HC595N
const int latchPin = 9;
const int clockPin = 10;
const int dataPin = 8;

//declare constants for the bytes to be sent to 
//SN74HC595N which correspond to the inputs for the K155ID1
const byte minutesTens[6] = {0b00000000,0b10000000,0b01000000,0b11000000,0b00100000,0b10100000};
const byte minutesOnes[10] = {0b00000000,0b00001000,0b00000100,0b00001100,0b00000010,0b00001010,0b00000110,0b00001110,0b00000001,0b00001001};
const byte hoursTens[3] = {0b00000000,0b10000000,0b01000000};
const byte hoursMinutes[10] = {0b00000000,0b00001000,0b00000100,0b00001100,0b00000010,0b00001010,0b00000110,0b00001110,0b00000001,0b00001001};

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

void setup()
{
  pinMode(latchPin, OUTPUT);
  pinMode(clockPin, OUTPUT);
  pinMode(dataPin, OUTPUT);
}

void loop()
{
  //update time from rtc
  //rtcMinutes = rtc.minutes;
  //rtcHours = rtc.hours;
  
  zeroedMinutes = isLeadingZero(rtcMinutes);
  zeroedHours = isLeadingZero(rtcHours);

  hTens = zeroedHours[0].toInt();
  hOnes = zeroedHours[1].toInt();
  mTens = zeroedHours[0].toInt();
  mOnes = zeroedHours[1].toInt();

  minutes = minutesTens[mTens] + minutesOnes[mOnes];
  hours = hoursTens[hTens] + hoursOnes[hOnes];

  updateRegister(minutes, hours);
  
}

//function to update SN74HC595N
void updateRegister(byte mins, byte hrs)
{
  digitalWrite(latchPin, LOW);
  shiftOut(dataPin, clockPin, LSBFIRST, hrs);
  shiftOut(dataPin, clockPin, LSBFIRST, mins);
  digitalWrite(latchPin, HIGH);
}

//function to decide if a leading zero needs to be added or not
void needsLeadingZero(int input)
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
