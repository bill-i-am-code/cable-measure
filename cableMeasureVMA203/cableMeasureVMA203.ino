/* 
Arduino device to automatically measure cable length pulled. 
Bill Addison
26 February 2021
*/

#include <LiquidCrystal.h>
#include <EEPROM.h>

LiquidCrystal lcd(8, 9, 4, 5, 6, 7);

//constants to define which pins or the arduino are used for what.
const int encPin1 = 2;   //rotary encoder attached to measuring wheel CLK
const int encPin2 = 3;   //rotary encoder attached to measuring wheel DATA
const int switchPin = 0; //pin that resitor ladder and switches are connected to
const int backLightPin = 10;
const int targetLengthIndicator = 13; //output pin used to indicate the measured length is a multiple of the target length

const int backLightBrightness = 100; //set the LCD backlight brightness between 0 and 255

// variables to analyse the rotary encoder position
int pos = 0; //this will increment (or decrement) by 1 for each clockwise (counterclockwise) "click" of the rotary encoder
int state;
int lastState;

// variables to store measured and target length
int targetLength;
float measuredLength = 0;

int lcd_key = 0;
int adc_key_in = 0;
#define btnRIGHT 0
#define btnUP 1
#define btnDOWN 2
#define btnLEFT 3
#define btnSELECT 4
#define btnNONE 5
int buttonState;         // the current reading from the input pin
int lastButtonState = 5; // the previous reading from the input pin
int tmpButtonState = 5;
long lastDebounceTime = 0;
long debounceDelay = 100; // the debounce time
int actioned = 0;

//constants used to convert 'pos' variable into a length
const float C = 1.0; //circumference of measuring wheel
const int N = 10;    //number of "clicks" for one revolution of wheel

char unitOfLength[3] = " m"; //specify the unit of length (2 characters), and use the same unit of length used to specify C

// read the buttons
int read_LCD_buttons()
{
    adc_key_in = analogRead(switchPin);
    // read the value from the sensor
    // my buttons when read are centered at these valies: 0, 144, 329, 504, 741
    // we add approx 50 to those values and check to see if we are close
    if (adc_key_in > 1000)
        return btnNONE; // We make this the 1st option for speed reasons since it will be the most likely result
    // For V1.1 us this threshold
    if (adc_key_in < 50)
        return btnRIGHT;
    if (adc_key_in < 250)
        return btnUP;
    if (adc_key_in < 450)
        return btnDOWN;
    if (adc_key_in < 650)
        return btnLEFT;
    if (adc_key_in < 850)
        return btnSELECT;
    // For V1.0 comment the other threshold and use the one below:
    /*
    if (adc_key_in < 50)
        return btnRIGHT;
    if (adc_key_in < 195)
        return btnUP;
    if (adc_key_in < 380)
        return btnDOWN;
    if (adc_key_in < 555)
        return btnLEFT;
    if (adc_key_in < 790)
        return btnSELECT;
    */
    return btnNONE; // when all others fail, return this...
}

void setup()
{
    //define pinModes
    pinMode(encPin1, INPUT);
    pinMode(encPin2, INPUT);

    pinMode(targetLengthIndicator, OUTPUT);

    //read initial values required.
    lastState = digitalRead(encPin1);
    digitalWrite(targetLengthIndicator, LOW);
    targetLength = EEPROM.read(0);

    //initilise LCD display
    analogWrite(backLightPin, backLightBrightness);
    lcd.begin(16, 2);
    lcd.print("Cable Measure");
    lcd.setCursor(0, 1);
    lcd.print("V1.00 210225");
    delay(2000);
    lcd.setCursor(0, 0);
    lcd.print(" ACTUAL  TARGET ");

    Serial.begin(9600);
}

// main loop

void loop()
{

    tmpButtonState = read_LCD_buttons();
    // Serial.print(lcd_key);
    // If the switch changed, due to noise or pressing:
    if (tmpButtonState != lastButtonState)
    {
        // reset the debouncing timer
        lastDebounceTime = millis();
    }

    if ((millis() - lastDebounceTime) > debounceDelay)
    {
        // whatever the reading is at, it's been there for longer
        // than the debounce delay, so take it as the actual current state:
        buttonState = tmpButtonState;
    }

    // save the reading.  Next time through the loop,
    // it'll be the lastButtonState:
    lastButtonState = tmpButtonState;

    if (buttonState == 1 && actioned == 0)
    {
        actioned = 1;
        targetLength++;
        EEPROM.update(0, targetLength);
    }
    if (buttonState == 2 && actioned == 0)
    {
        actioned = 1;
        if (targetLength > 0)
        {
            targetLength--;
        }
        EEPROM.update(0, targetLength);
    }
    if (buttonState == 3 && actioned == 0)
    {
        actioned = 1;
        pos = 0;
    }
    if (buttonState == 5)
    {
        actioned = 0;
    }

    /*    Serial.print(tmpButtonState);
    Serial.print("\t");
    Serial.print(lastButtonState);
    Serial.print("\t");
    Serial.println(buttonState); */

    lastButtonState = tmpButtonState;

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