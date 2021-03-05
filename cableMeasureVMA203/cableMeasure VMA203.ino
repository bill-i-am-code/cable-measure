/* 
Arduino device to automatically measure cable length pulled. 
Bill Addison
26 February 2021
*/

#include <LiquidCrystal.h>
#include <EEPROM.h>
#include <Arduino.h>
#include <stdlib.h>

LiquidCrystal lcd(12, 11, 2, 3, 4, 5);

//constants to define which pins or the arduino are used for what.
const int encPin1 = 9;                //rotary encoder attached to measuring wheel CLK
const int encPin2 = 10;               //rotary encoder attached to measuring wheel DATA
const int switchPin1 = 6;             //Switch 1 - decrease the target length
const int switchPin2 = 7;             //Switch 2 - increase the target length
const int switchPin3 = 8;             //Switch 3 - reset the measured length to 0
const int targetLengthIndicator = 13; //output pin used to indicate the measured length is a multiple of the target length

// variables to analyse the rotary encoder position
int pos = 0; //this will increment (or decrement) by 1 for each clockwise (counterclockwise) "click" of the rotary encoder
int state;
int lastState;

// variables to store measured and target length
int targetLength;
float measuredLength = 0;



//constants used to convert 'pos' variable into a length
const float C = 1.0; //circumference of measuring wheel
const int N = 10;    //number of "clicks" for one revolution of wheel

char unitOfLength[3] = " m"; //specify the unit of length (2 characters), and use the same unit of length used to specify C

void setup()
{
    //define pinModes
    pinMode(encPin1, INPUT);
    pinMode(encPin2, INPUT);
    pinMode(switchPin1, INPUT);
    pinMode(switchPin2, INPUT);
    pinMode(switchPin3, INPUT);
    pinMode(targetLengthIndicator, OUTPUT);

    //read initial values required.
    lastState = digitalRead(encPin1);
    digitalWrite(targetLengthIndicator, LOW);
    targetLength = EEPROM.read(0);

    //initilise LCD display
    lcd.begin(16, 2);
    lcd.print("Cable Measure");
    lcd.setCursor(0, 1);
    lcd.print("V1.00 210225");
    delay(2000);
    lcd.setCursor(0, 0);
    lcd.print(" ACTUAL  TARGET ");
}

// main loop

void loop()
{

    //read rotary encoder and increment or decrement  position accordingly
    state = digitalRead(encPin1);
    if (state != lastState)
    {
        if (digitalRead(encPin2) != state)
        {
            pos++;
        }
        else
        {
            pos--;
        }
    }
    lastState = state;

    //calculate measured length

    measuredLength = pos * C / N;

    
    //check if target length met and indicate accordingly 

    if (targetLength > 0 and int(measuredLength * 10) % (targetLength * 10) == 0) //multiply by 10 and convert to integer - compare to 1 decimal place.
    {
        digitalWrite(targetLengthIndicator, HIGH);
    }
    else
    {
        digitalWrite(targetLengthIndicator, LOW);
    }

    //format and print results to LCD

    char measuredLength_str[7];
    char line1[17];
    dtostrf(measuredLength, 4, 1, measuredLength_str); //convert measuredLength to a string, with 1 decimal place.
    sprintf(line1, "%6s%2s%6u%2s", measuredLength_str, unitOfLength, targetLength, unitOfLength); 
    lcd.setCursor(0, 1);
    lcd.print(line1);
}