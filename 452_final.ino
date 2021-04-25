/*
  -------------LCD---------------
  
  The circuit:
 * 
    LCD RS pin to digital pin 7
    LCD Enable pin to digital pin 6
    LCD D4 pin to digital pin 5
    LCD D5 pin to digital pin 4
    LCD D6 pin to digital pin 3
    LCD D7 pin to digital pin 2
    LCD R/W pin to ground
    LCD VSS pin to ground
    LCD VDD pin to 5V
    A to 10K resistor to 5V:
    K to Ground
    wiper to LCD VO pin (pin 3)
-------------POT0-----------------
volume
SI to pin 11
CS to pin 10
SCK to pin 13
-------------POT1-----------------
SI to pin 11
CS to pin 9
SCK to pin 13
--------------IR------------------
G to ground
R to Vcc
Y to pin A0
*/


// include the library code:
#include <LiquidCrystal.h>
#include <SPI.h>
#include <IRremote.h>



/***********************PIN Definitions*************************/
// set pin 10 as the slave select (SS) for the digital pot0
const int CS_PIN_V = 10;
// set pin 9 as the slave select (SS) for the digital pot1
const int CS_PIN_T = 9;

/***********************MCP42XXX Commands************************/
//potentiometer select byte
const int POT0_SEL = 0x11;
const int POT1_SEL = 0x12;
const int BOTH_POT_SEL = 0x13;

//shutdown the device to put it into power-saving mode.
//In this mode, terminal A is open-circuited and the B and W terminals are shorted together.
//send new command and value to exit shutdowm mode.
const int POT0_SHUTDOWN = 0x21;
const int POT1_SHUTDOWN = 0x22;
const int BOTH_POT_SHUTDOWN = 0x23;

/***********************Customized Varialbes**********************/
//resistance value byte (0 - 255)
//The wiper is reset to the mid-scale position upon power-up, i.e. POT0_Dn = POT1_Dn = 128
int POT0_Dn = 128;
int POT1_Dn = 128;
int BOTH_POT_Dn = 128;

//Function Declaration
void DigitalPotTransfer(int cmd, int value);     //send the command and the wiper value through SPI

// initialize the library by associating any needed LCD interface pin
// with the arduino pin number it is connected to
const int rs = 7, en = 6, d4 = 5, d5 = 4, d6 = 3, d7 = 2;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

// IR receiver setup
const byte IR_RECEIVE_PIN = A0;

// variables to be controlled
float vol = 5.0, treb = 5.0, bass = 5.0;
int funB = 0;     // keep track of the function button

void setup() {
  //Serial.begin(115200);       //begin serial monitor

  IrReceiver.begin(IR_RECEIVE_PIN, ENABLE_LED_FEEDBACK);        //start IR receiver

  
  pinMode(CS_PIN_V, OUTPUT);   // set the CS_PIN as an output:
  pinMode(CS_PIN_T, OUTPUT);
  
  digitalWrite(CS_PIN_V, HIGH);    //start slave select high
  digitalWrite(CS_PIN_T, HIGH);
  
  SPI.begin();     // initialize SPI:
  
  // set up the LCD's number of columns and rows:
  lcd.begin(16, 2);

  //initial values to display
  lcd.print("Vol:");
  lcd.print(vol);
  lcd.setCursor(8,0);
  lcd.print("Bass:");
  lcd.print(bass);
  lcd.setCursor(0,1);
  lcd.print("Treb:");
  lcd.print(treb);
  }

void loop() {

  //get input from IR remote
    if (IrReceiver.decode())
   {

      //send irremove data to serial monitor for testing
      //Serial.println(IrReceiver.decodedIRData.command);

      if (IrReceiver.decodedIRData.command == 71)   //function button pressed
      {
        if (funB == 0)
        {
          funB++;
        }
        else
        {
          funB = 0;
        }

        // helps if you press the function button to long
        delay(50);
      }

      if (IrReceiver.decodedIRData.command == 9 && funB == 0 && bass < 10)    //up button and funtion 0 bass
      {
        bass += 0.5;
      }
      else if (IrReceiver.decodedIRData.command == 7 && funB == 0 && bass > 0.5)  //down button and function 0
      {
        bass -= 0.5;
      }
      else if (IrReceiver.decodedIRData.command == 9 && funB == 1 && treb < 10) //up button and function 1
      {
        treb += 0.5;
      }
      else if (IrReceiver.decodedIRData.command == 7 && funB == 1 && treb > 0.5)  //down button and function 1
      {
        treb -= 0.5;
      }
      else if (IrReceiver.decodedIRData.command == 70 && vol < 11.5)  //volume up presses
      {
        vol += 0.5;
      }
      else if (IrReceiver.decodedIRData.command == 21 && vol > 0)   //volume down pressed
      {
        vol -= 0.5;
      }


      IrReceiver.resume();     // resume listening to the IR sensor
   }

    // set volume
    DigitalPotWrite(POT0_SEL, floor(vol/11.5*255), CS_PIN_V);

    // set trebble
    DigitalPotWrite(POT0_SEL, treb * 25.5, CS_PIN_T);

    //set Bass
    DigitalPotWrite(POT1_SEL, bass * 25.5, CS_PIN_T);


  //update values to LCD
  lcd.setCursor(4,0);
  lcd.print(vol);
  lcd.setCursor(8,0);
  lcd.print("Bass:");
  lcd.setCursor(13,0);
  lcd.print(bass);
  lcd.setCursor(5,1);
  lcd.print(treb);
  lcd.setCursor(8, 1);
}
  

void DigitalPotWrite(int cmd, int val, int POT)
{
  // constrain input value within 0 - 255
  val = constrain(val, 0, 255);
  // set the CS pin to low to select the chip:
  digitalWrite(POT, LOW);
  // send the command and value via SPI:
  SPI.transfer(cmd);
  SPI.transfer(val);
  // Set the CS pin high to execute the command:
  digitalWrite(POT, HIGH);
}
