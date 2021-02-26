/* 
Arduino device to automatically measure cable length pulled. 
Bill Addison
26 February 2021
*/


#include <LiquidCrystal.h>
#include <EEPROM.h>

LiquidCrystal lcd (12, 11, 2, 3, 4, 5);

//constants to define which pins or the arduino are used for what.
const int encPin1 = 9; //rotary encoder attached to measuring wheel CLK
const int encPin2 = 10; //rotary encoder attached to measuring wheel DATA
const int switchPin1 = 6; //Switch 1 - decrease the target length
const int switchPin2 = 7; //Switch 2 - increase the target length
const int switchPin3 = 8; //Switch 3 - reset the measured length to 0
const int targetLengthIndicator = 13; //output pin used to indicate the measured length is a multiple of the target length


// variables to analyse the rotary encoder position
int pos = 0; //this will increment (or decrement) by 1 for each clockwise (counterclockwise) "click" of the rotary encoder
int state;
int lastState;

// variables to store measured and target length
int targetLength;
int measuredLength = 0;

//variable to analyse and debounce the three switch inputs
int switchState1;
int switchState2;
int switchState3;
int lastSwitchState1 = LOW;
int lastSwitchState2 = LOW;
int lastSwitchState3 = LOW;
unsigned long lastDebounceTime1 = 0;  
unsigned long lastDebounceTime2 = 0;  
unsigned long lastDebounceTime3 = 0;  
unsigned long debounceDelay = 50;

//constants used to convert 'pos' variable into a length
const int C = 1; //circumference of measuring wheel
const int N = 1; //number of "clicks" for one revolution of wheel


void setup(){
    //define pinModes
    pinMode (encPin1 ,INPUT);
    pinMode (encPin2 ,INPUT);
    pinMode (switchPin1 ,INPUT);
    pinMode (switchPin2 ,INPUT);
    pinMode (switchPin3 ,INPUT);
    pinMode (targetLengthIndicator, OUTPUT);

    //read initial values required.
    lastState = digitalRead(encPin1); 
    digitalWrite(targetLengthIndicator, LOW);
    targetLength =  EEPROM.read(0);

    //initilise LCD display
    lcd.begin(16, 2);
    lcd.print("Cable Measure");
    // lcd.setCursor(0,1);
    // lcd.print("V0 210225");
    Serial.begin(9600);
}

// main loop

void loop(){

    //read rotary encoder and increment or decrement  position accordingly
    state = digitalRead(encPin1);
    if (state != lastState){     
        if (digitalRead(encPin2) != state) { 
            pos ++;
        } 
        else {
            pos --;
        }
    } 
    lastState = state;

    //calculate measured length

    measuredLength = pos*C/N;

    //read from switch 1 and action

    int s1 = digitalRead(switchPin1);
    if (s1 != lastSwitchState1){
        lastDebounceTime1 = millis();
    }
    if ((millis() - lastDebounceTime1) > debounceDelay) {
        if (s1 != switchState1) {
            switchState1 = s1;
            if(switchState1 == HIGH && targetLength > 0){
                targetLength--;
                EEPROM.write(0,targetLength);
            }
        }
    } 
    lastSwitchState1 = s1;

    //read from switch 2 and action

    int s2 = digitalRead(switchPin2);
    if (s2 != lastSwitchState2){
        lastDebounceTime2 = millis();
    }
    if ((millis() - lastDebounceTime2) > debounceDelay) {
        if (s2 != switchState2) {
            switchState2 = s2;
            if(switchState2 == HIGH){
                targetLength++;
                EEPROM.write(0,targetLength);
            }
        }
    } 
    lastSwitchState2 = s2;

    //read from switch 3 action
    
    int s3 = digitalRead(switchPin3);
    if (s3 != lastSwitchState3){
        lastDebounceTime3 = millis();
    }
    if ((millis() - lastDebounceTime3) > debounceDelay) {
        if (s3 != switchState3) {
            switchState3 = s3;
            if(switchState3 == HIGH){
                pos = 0;
            }
        }
    } 
    lastSwitchState3 = s3;

    //check if target length met

    if (targetLength > 0 and measuredLength%targetLength ==0)
    {
        digitalWrite(targetLengthIndicator, HIGH);
    }
    else{
        digitalWrite(targetLengthIndicator, LOW);
    }
    

    //print results to LCD

    lcd.setCursor(0, 1);
    char buff[8];
    sprintf(buff, "%4d", measuredLength);
    lcd.print(buff);
    lcd.setCursor(9, 1);
    char buff2[8];
    sprintf(buff2, "%4d", targetLength);
    lcd.print(buff2);

    //serial comms for testing

    Serial.print(s1);
    Serial.print("\t");
    Serial.print(measuredLength);
    Serial.print("\t");
    Serial.print(targetLength);
    Serial.println();
    
 

}