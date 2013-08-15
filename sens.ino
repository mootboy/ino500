#include <Wire.h>
#include <Adafruit_BMP085.h>
#include "Adafruit_MAX31855.h"
#include <LiquidCrystal.h>

// LM35 pin:
int sensorPin = 0;
// 31855 pins:
int thermoDO = 3;
int thermoCS = 4;
int thermoCLK = 5;
// initialize the library with the numbers of the interface pins
LiquidCrystal lcd(12, 11, 10, 9, 8, 7);

Adafruit_BMP085 bmp;
Adafruit_MAX31855 thermocouple(thermoCLK, thermoCS, thermoDO);

void setup() {
  // set up the LCD's number of columns and rows: 
  lcd.begin(16, 2);
  Serial.begin(9600);  //Start the serial connection with the computer
  //to view the result open the serial monitor
  if (!bmp.begin()) {
    Serial.println("Could not find a valid BMP085 sensor, check wiring!");
    while (1) {
    }
  }
}

/* reverse:  reverse string s in place */
void reverse(char s[]) {
  int  i, j;
  char c;

  for (i = 0, j = strlen(s)-1; i<j; i++, j--) {
    c    = s[i];
    s[i] = s[j];
    s[j] = c;
  }
}

void format(double val, char buf[]) {
  int i, sig, sign;

  sig = (int)val;
  unsigned int frac;

  if ((sign = sig) < 0) {
    sig = -sig;
  }

  frac = (val * 10000 - sig * 10000);

  i = 0;
  do { buf[i++] = frac % 10 + '0';
  } while ((frac /= 10) > 0);

  buf[i++] = '.';
  
  do { buf[i++] = sig % 10 + '0';
  } while ((sig /= 10) > 0);

  if (sign < 0) { buf[i++] = '-'; }
  buf[i] = '\0';
  reverse(buf);
}

void loop()                     // run over and over again
{
  // converting that reading to voltage, for 3.3v arduino use 3.3
  float voltage = (analogRead(sensorPin) * 3.3) / 1024.0;

  //converting from 10 mv per degree with 500 mV offset:
  float  lm35_temp    = (voltage + 0.062) * 100 ;
  float  bmp_temp     = bmp.readTemperature();
  float  bmp_pressure = bmp.readPressure() / 100; // in hPa
  double term_temp    = thermocouple.readCelsius();

  // Build string for display on LCD:
  char lt[8] = "0000000"; 
  format(lm35_temp, lt);
  
  char bt[8] = "0000000"; 
  format(bmp_temp, bt);
  
  char bp[8] = "0000000"; 
  format(bmp_pressure, bp);
  
  char tt[8] = "0000000"; 
  format(term_temp, tt);
  
  char line1[16] = "               ";
  char line2[16] = "               ";

  sprintf(line1, "K:%4.5s P:%4.5s", tt, bp);
  sprintf(line2, "RT:%4.4s HT:%4.4s", bt, lt);

  Serial.println(line2);
  Serial.println(line1);

  lcd.print(line2);
  lcd.print(line1);

  delay(1000); //waiting a second
}
//vim: filetype=c
