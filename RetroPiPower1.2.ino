/*
  Retro Pie Power Control

  Pin 7:  (Output) debug / Extra LED showing uC is working
  
  Pin 8:  (Input) monitor Raspberry Pi shutdown ACK

  Pin 9:  (Input) monitor state of switch to determine if system should be powered on or off

  Pin 10: (Output) tell the Raspberry Pi when it's time to power down

  Pin 11: (Output) LED status to tell whether the system is powering up, on, or powering down

  Pin 12: (Output) Control the state of Relay 1 (Raspberry Pi Power)

  Pin 13: (Output) Control the state of Relay 2 (Power Strip Power) 
 */

int ShieldLEDHeaderPin = 7;                                 //Extra LED showing uC is working
int RPiShutdownACK = 8;                                     //Rpi pin 13(GPIO27) to send ACK to arduino on shutdown
int switchPin = 9;                                          //switch connected to digital pin 9
int PowerPiPin = 10;                                        //connected to Raspberry Pi GPIO 22
int powerStateLEDPin = 11;                                  //LED connected to digital pin 9
int Relay1Pin = 12;                                         //connected to Relay 1 (Raspberry Pi Power)
int Relay2Pin = 13;                                         //connected to Relay 2 (Power Strip Power) 
int stateButton = digitalRead(switchPin);                   //read the state of the switch
unsigned long startTime;
boolean changeOfPowerState;
boolean singleShot;                                         // set SingleShot to off
int lastState = 2;                                          // set last state to neither 0(off) or 1(on)

void setup() {
  pinMode(RPiShutdownACK, INPUT);                           //set the RPiShutdownACK pin as INPUT
  pinMode(switchPin, INPUT);                                //set the switch pin as INPUT
  pinMode(ShieldLEDHeaderPin, OUTPUT);                      //set the ShieldLEDHeaderPin pin as OUTPUT
  pinMode(PowerPiPin, OUTPUT);                              //set the PowerPi pin as OUTPUT
  pinMode(powerStateLEDPin, OUTPUT);                        //set the LED pin as OUTPUT
  pinMode(Relay1Pin, OUTPUT);                               //set the Relay1 pin as OUTPUT
  pinMode(Relay2Pin, OUTPUT);                               //set the Relay2 pin as OUTPUT
  digitalWrite(PowerPiPin, LOW);                            //initialize POWERPIPIN (Pi signal to power down) to off
  digitalWrite(ShieldLEDHeaderPin, LOW);                    //initialize ShieldLEDHeaderPin pin to off
  digitalWrite(Relay1Pin, LOW);                             //initialize RELAY1 (Raspberry Pi) to off
  digitalWrite(Relay2Pin, LOW);                             //initialize RELAY2 (Power Strip) to off
  singleShot = false;
  changeOfPowerState = false;
  stateButton = digitalRead(switchPin);                     //read the state of the switch 
}

void CheckPowerStateTimer() {                               //tells us if we're in the middle of powering up or down 
   if (((millis() - startTime)/1000) < 20) {                //If the timer < 20 seconds 
     changeOfPowerState = true;                             
   }
   else
   {
     changeOfPowerState = false;
     singleShot = false;                            
   }
}


void LEDPowerUp() {                                             //fade LED on and off
  for (int fadeValue = 0; fadeValue <= 200; fadeValue += 5) {   // fade in from min to max in increments of 5 points
    analogWrite(powerStateLEDPin, fadeValue);                   // sets the value (range from 0 to 200):
    delay(35);                                                  // wait for 35 milliseconds to see the dimming effect
  }   
  for (int fadeValue = 200; fadeValue >= 0; fadeValue -= 5) {   // fade out from max to min in increments of 5 points
    analogWrite(powerStateLEDPin, fadeValue);                   // sets the value (range from 0 to 255):
    delay(35);                                                  // wait for 35 milliseconds to see the dimming effect
  }
  delay(250);  
}


void LEDOn() {                                             //turn LED solid
   digitalWrite(powerStateLEDPin, HIGH);                   //write HIGH to led pin to turn it on
}


void LEDPowerDown() {                                      //urgently blink LED
   digitalWrite(powerStateLEDPin, HIGH);                   //write HIGH to led pin to turn it on
   delay(200);
   digitalWrite(powerStateLEDPin, LOW);                    //write Low to led pin to turn it off
   delay(200);
}


void ShieldLEDHeaderPinBlink() {                           //urgently blink LED
   digitalWrite(ShieldLEDHeaderPin, HIGH);                 //write HIGH to led pin to turn it on
   delay(200);
   digitalWrite(ShieldLEDHeaderPin, LOW);                  //write Low to led pin to turn it off
   delay(200);
}


void LEDOff() {                                            //turn on LED off
   digitalWrite(powerStateLEDPin, LOW);                    //write LOW to led pin to turn it off
}


void loop() {
  if (changeOfPowerState == false) {                       //if we're not in the middle of powering up or down then
    stateButton = digitalRead(switchPin);                  //read the state of the switch 
  }
  if ((stateButton == 1) && ((lastState == 0) || (lastState == 2))) {                                   //if switch on  
    if (singleShot == false) {                              //and it's the first time through
      startTime = millis();                                 //start the timer  
      singleShot = true;
    }
    CheckPowerStateTimer();                                 //check to see if time has elapsed
    if ((changeOfPowerState == true) && ((lastState == 0) or (lastState == 2))){   //If changing power state    
      LEDPowerUp();                                         //then slowly fade the LED on and off 
      digitalWrite(Relay2Pin, HIGH);                        //write HIGH to enable RELAY 2 (Power Strip)
      delay(1500);                                          //wait 1.5 seconds to allow power to stabilize before powering Pi
      digitalWrite(Relay1Pin, HIGH);                        //write HIGH to enable RELAY 1 (Raspberry Pi)  
    }    
    CheckPowerStateTimer();                                 //check to see if time has elapsed
    if ((changeOfPowerState == false) && ((lastState == 0) or (lastState == 2))){
      LEDOn();                                              //else turn on the LED to make it solid
      lastState = 1;                                        //save state as previously powered on
      singleShot = false;     
    }
  } 
  if ((stateButton == 0) && (lastState == 1)) {             //if switch off
    if (singleShot == false) {
      startTime = millis();                                 //start the timer 
      singleShot = true;
    }   
    if (lastState == 1) {
      digitalWrite(Relay2Pin, LOW);                         //write LOW to power off RELAY 2 (Power Strip)
    }
    CheckPowerStateTimer();                                 //check to see if time has elapsed 
    if ((changeOfPowerState == true) && (lastState == 1)){  //If changing power state 
      digitalWrite(PowerPiPin, HIGH);                       //Tell the Raspberry Pi to prepare for shutdown  
      LEDPowerDown();                                       //then blink LED quickly   
    }
    CheckPowerStateTimer();                                 //check to see if time has elapsed 
    if ((changeOfPowerState == false) && (lastState == 1)){   
      LEDOff();                                             //then blink LED quickly
      digitalWrite(Relay1Pin, LOW);                         //write LOW to power off RELAY 1 (Raspberry Pi)
      lastState = 0;                                        //save state as previously powered off
      singleShot = false;    
    }
  }
  if (digitalRead(RPiShutdownACK)) {   
    digitalWrite(ShieldLEDHeaderPin, HIGH); 
  }
}

