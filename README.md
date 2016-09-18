# RaspPiPowerCtrl
Arduino based Raspberry Pi arcade power controller, which senses the state of an on/off power switch.  

When the power switch is turned on the LED will slowly fade in and out and Sainsmart relay 2 clicks on turning on a power strip and then relay 1 clicks on enabling the Raspberry Pi to power up.  During the power up phase the state of the power switch is ignored.  

When the power switch is turned off the LED flashed quickly, a power down signal is sent to the Raspberry Pi and Sainsmart relay 2 clicks off turning off the power strip.  After around 20 seconds the relay that controls the Raspberry Pi clicks off powering down the Raspberry Pi.  During the power off phase the state of the power switch is ignored.   

Upload .ino script to an Arduino Uno or similar Arduino.  Also, install the softshut.py script on the Raspberry Pi to enable it to shutdown when it receives the power down signal.

**NOTE** I tried using the sainsmart relays directly with the Arduino, but noticed it had some current issues when switching the relays(the Arduino would reset), as a work around I added some switching transistors to limit current draw.  In retrospect the transistors may not have been necessary.  The final solution that worked for me with the current draw problem was include separate power supplied for the Arduino/relays and another supply for the Raspberry Pi.  I found the current draw to be too great to put them all on a single power supply.  Potentially thicker gage power supply wire may solve the problem without the need for additional transistors. 
