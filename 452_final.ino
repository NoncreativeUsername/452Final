/*
  -------------LCD---------------
  
  The circuit:
 * LCD RS pin to digital pin 7
 * LCD Enable pin to digital pin 6
 * LCD D4 pin to digital pin 5
 * LCD D5 pin to digital pin 4
 * LCD D6 pin to digital pin 3
 * LCD D7 pin to digital pin 2
 * LCD R/W pin to ground
 * LCD VSS pin to ground
 * LCD VCC pin to 5V
 * 10K resistor:
 * ends to +5V and ground
 * wiper to LCD VO pin (pin 3)


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
// set pin 10 as the slave select (SS) for the digital pot
// for using Arduino UNO
const int CS_PIN = 10;
const int CS_PIN1 = 9;

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
//IRrecv irrecv(RECV_PIN);
//decode_results results;

// variables to be controlled
float vol = 5.0, treb = 5.0, bass = 5.0;
int funB = 0;     // keep track of the function button

void setup() {
  Serial.begin(115200);       //begin serial monitor

  IrReceiver.begin(IR_RECEIVE_PIN, ENABLE_LED_FEEDBACK);        //start IR receiver

  
  pinMode(CS_PIN, OUTPUT);   // set the CS_PIN as an output:
  pinMode(CS_PIN1, OUTPUT);
  digitalWrite(CS_PIN, HIGH);
  digitalWrite(CS_PIN1, HIGH);
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
    
      Serial.println(IrReceiver.decodedIRData.command);

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
        delay(30);
      }

      if (IrReceiver.decodedIRData.command == 9 && funB == 0 && bass < 11.1)    //up button and funtion 0 bass
      {
        bass += 0.1;
      }
      else if (IrReceiver.decodedIRData.command == 7 && funB == 0 && bass > 0.1)  //down button and function 0
      {
        bass -= 0.1;
      }
      else if (IrReceiver.decodedIRData.command == 9 && funB == 1 && treb < 11.1) //up button and function 1
      {
        treb += 0.1;
      }
      else if (IrReceiver.decodedIRData.command == 7 && funB == 1 && treb > 0.1)  //down button and function 1
      {
        treb -= 0.1;
      }

      switch (IrReceiver.decodedIRData.command)
      {
        case 21:      //volume down pressed
          if (vol > 0.1)
          {
            vol -= 0.1;
          }
          break;
        case 70:      //volume up pressed
          if (vol<11.1)
          {
            vol+= 0.1;
          }
          break;
            
      }

      IrReceiver.resume();     // resume listening to the IR sensor
   }

    // set volume
    DigitalPotWrite(POT0_SEL, vol* 22.5);

    // set trebble
    DigitalPotWrite1(POT0_SEL, treb * 22.5);

    //set Bass
    DigitalPotWrite1(POT1_SEL, bass * 22.5);


  //update values to LCD
  lcd.setCursor(4,0);
  lcd.print(vol);
  lcd.setCursor(8,0);
  lcd.print("Bass:");
  lcd.setCursor(13,0);
  lcd.print(bass);
  lcd.setCursor(5,1);
  lcd.print(treb);
  }

void DigitalPotWrite(int cmd, int val)
{
  // constrain input value within 0 - 255
  val = constrain(val, 0, 255);
  // set the CS pin to low to select the chip:
  digitalWrite(CS_PIN, LOW);
  // send the command and value via SPI:
  SPI.transfer(cmd);
  SPI.transfer(val);
  // Set the CS pin high to execute the command:
  digitalWrite(CS_PIN, HIGH);
}


void DigitalPotWrite1(int cmd, int val)
{
  // constrain input value within 0 - 255
  val = constrain(val, 0, 255);
  // set the CS pin to low to select the chip:
  digitalWrite(CS_PIN1, LOW);
  // send the command and value via SPI:
  SPI.transfer(cmd);
  SPI.transfer(val);
  // Set the CS pin high to execute the command:
  digitalWrite(CS_PIN1, HIGH);
}