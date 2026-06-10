#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <Keypad.h>
#include <Servo.h>
#include <SoftwareSerial.h>
#include <EEPROM.h>

// LCD CONFIG
LiquidCrystal_I2C lcd(0x27, 16, 2);

// KEYPAD CONFIG
const byte ROWS = 4;
const byte COLS = 4;

char rawKeys[ROWS][COLS] = {
{'1','2','3','A'},
{'4','5','6','B'},
{'7','8','9','C'},
{'*','0','#','D'}
};

byte rowPins[ROWS] = {2, 3, 4, 5};
byte colPins[COLS] = {6, 7, 8, 9};

Keypad keypad = Keypad(makeKeymap(rawKeys), rowPins, colPins, ROWS, COLS);

// SERVO CONFIG
Servo lockServo;
int servoPin = 10;

// GSM CONFIG
SoftwareSerial gsm(11, 12); // RX, TX

// PHONE NUMBER
String phone = "+91XXXXXXXXXX";

// VARIABLES
String generatedPassword = "";
String inputPassword = "";

const int otpFlagAddress = 0;

// KEYPAD FIX FUNCTION
char correctKey(char k) {
switch (k) {
case 'D': return '1';
case 'C': return '2';
case 'B': return '3';
case 'A': return 'A';
case '#': return '4';
case '9': return '5';
case '6': return '6';
case '3': return 'B';
case '0': return '7';
case '8': return '8';
case '5': return '9';
case '2': return 'C';
case '7': return '0';
case '4': return '#';
case '1': return 'D';
default: return k;
}
}

// SEND SMS FUNCTION
void sendSMS(String msg) {
gsm.println("AT+CMGF=1");
delay(500);

gsm.println("AT+CMGS="" + phone + """);
delay(500);

gsm.print(msg);
delay(500);

gsm.write(26);
delay(3000);
}

// OTP GENERATION
String generateOTP() {
randomSeed(analogRead(0));
int otp = random(1000, 9999);
return String(otp);
}

void setup() {

lcd.init();
lcd.backlight();
lcd.clear();

lcd.print("System Starting.");

lockServo.attach(servoPin);
lockServo.write(0);

gsm.begin(9600);

delay(3000);

gsm.println("AT");
delay(1000);

byte otpSentFlag = EEPROM.read(otpFlagAddress);

if (otpSentFlag != 1) {

```
generatedPassword = generateOTP();

sendSMS("Your Door Lock OTP is: " + generatedPassword);

EEPROM.write(otpFlagAddress, 1);

lcd.clear();
lcd.print("OTP Sent to phone");

delay(3000);
```

} else {

```
lcd.clear();
lcd.print("OTP already sent");

delay(2000);
```

}

lcd.clear();
lcd.print("Enter Password:");
}

void loop() {

char key = keypad.getKey();

if (key) {

```
char actual = correctKey(key);

// CLEAR INPUT
if (actual == '*') {

  inputPassword = "";

  lcd.clear();
  lcd.print("Enter Password:");
}

// SUBMIT PASSWORD
else if (actual == '#') {

  lcd.clear();

  if (inputPassword == generatedPassword && generatedPassword != "") {

    lcd.print("Access Granted");

    sendSMS("Door unlocked with correct OTP.");

    lockServo.write(90);

    delay(5000);

    lockServo.write(0);

  } else {

    lcd.print("Wrong Password");

    sendSMS("Alert: Wrong password attempt!");
  }

  delay(2000);

  lcd.clear();
  lcd.print("Enter Password:");

  inputPassword = "";
}

// NORMAL INPUT
else {

  inputPassword += actual;

  lcd.setCursor(0, 1);
  lcd.print(inputPassword);
}
```

}
}
