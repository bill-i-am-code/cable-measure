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

int switchstate1;
int switchstate2;
int switchstate3;

const float pi = 3.14;
const float R = 3.25;
const int N = 40;


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
}
void loop(){
    state = digitalRead(encPin1);
    if (state != lastState){     
        if (digitalRead(encPin2) != state) { 
            pos ++;
        } 
     
        else {
            pos --;
        }
    } 
    lcd.setCursor(0, 1);
    char buff[8];
    sprintf(buff, "%4d", pos);
    lcd.print(buff);
    lastState = state;

    switchstate1 = digitalRead(switchPin1);
    if (switchstate1 == HIGH){
        lcd.setCursor(15, 1);
        lcd.print('1');
        delay(100);
    }
    switchstate2 = digitalRead(switchPin2);
    if (switchstate2 == HIGH){
        lcd.setCursor(15, 1);
        lcd.print('2');
        delay(100);
    }
    switchstate3 = digitalRead(switchPin3);
    if (switchstate3 == HIGH){
        lcd.setCursor(15, 1);
        lcd.print('3');
        delay(100);
    }




    lcd.setCursor(15, 1);
        lcd.print(' ');

}