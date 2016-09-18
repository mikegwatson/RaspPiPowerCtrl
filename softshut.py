#!/usr/bin/python
## INSTALL GPIO STEP 1) sudo apt-get update
## INSTALL GPIO STEP 2) sudo apt-get -y install python-rpi.gpio
## Save filename: softshut.py
## chmod +x softshut.py
## PiPowerStateSense = GPIO27-pin13(Input) --> If HIGH then ACK with GPIO22-pin15(Outpt) going HIGH and shutdown
## Create directory: /home/pi/PiSupply
## sudo nano /etc/rc.local
## add following line before it says "exit 0": sudo /home/pi/PiSupply/softshut.py & 

# Import the modules to send commands to the system and access GPIO pins 
from subprocess import call
import RPi.GPIO as gpio

# loop function - do nothing and keep script running
def loop():
    import signal
    while True:
        signal.pause()

# shutdown function called when event detect interrupt called
def shutdown(pin):
    gpio.setup(15, gpio.OUT)		# Setup Pi PWR ACK to OFF  
    gpio.output(15, True)
    call('halt', shell=False)	

# Initialize GPIO states
gpio.setmode(gpio.BOARD)			# Set GPIO pin numbering to board pin numbers

gpio.setup(15, gpio.OUT)
gpio.output(15, False)				# Set Pi PWR ACK to OFF

gpio.setup(13, gpio.IN)				# Set Switch Sense as an input
gpio.add_event_detect(13, gpio.RISING, callback=shutdown, bouncetime=200) # Set up an interrupt to look for button presses

loop()								# Run the loop function to keep script running