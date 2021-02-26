#include <LiquidCrystal.h>
LiquidCrystal lcd (12, 11, 2, 3, 4, 5);

int encPin1 = 9;
int encPin2 = 10;
int switchPin1 = 6;
int switchPin2 = 7;
int switchPin3 = 8;

int pos = 0; 
int state;
int lastState;

int targetLength = 0;
int measuredLength =0;

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

const float C = 1; //circumference of measuring wheel
const int N = 1; //number of "clicks" for one revolution of wheel


void setup(){
    pinMode (encPin1 ,INPUT);
    pinMode (encPin2 ,INPUT);
    pinMode (switchPin1 ,INPUT);
    pinMode (switchPin2 ,INPUT);
    pinMode (switchPin3 ,INPUT);
    lastState = digitalRead(encPin1); 

    lcd.begin(16, 2);
    lcd.print("Cable Measure");
    // lcd.setCursor(0,1);
    // lcd.print("V0 210225");
    Serial.begin(9600);
}

// main loop

void loop(){

    //read rotary encoder

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

    //read from switch 1

    int s1 = digitalRead(switchPin1);
    if (s1 != lastSwitchState1){
        lastDebounceTime1 = millis();
    }
    if ((millis() - lastDebounceTime1) > debounceDelay) {
        if (s1 != switchState1) {
            switchState1 = s1;
            if(switchState1 == HIGH){
                targetLength--;
            }
        }
    } 
    lastSwitchState1 = s1;

    //read from switch 2

    int s2 = digitalRead(switchPin2);
    if (s2 != lastSwitchState2){
        lastDebounceTime2 = millis();
    }
    if ((millis() - lastDebounceTime2) > debounceDelay) {
        if (s2 != switchState2) {
            switchState2 = s2;
            if(switchState2 == HIGH){
                targetLength++;
            }
        }
    } 
    lastSwitchState2 = s2;

    //read from switch 3
    
    int s3 = digitalRead(switchPin3);
    if (s3 != lastSwitchState3){
        lastDebounceTime3 = millis();
    }
    if ((millis() - lastDebounceTime3) > debounceDelay) {
        if (s3 != switchState3) {
            switchState3 = s3;
            if(switchState3 == HIGH){
                targetLength = 0;
            }
        }
    } 
    lastSwitchState3 = s3;

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
    Serial.print(switchState1);
    Serial.print("\t");
    Serial.print(targetLength);
    Serial.println();

}