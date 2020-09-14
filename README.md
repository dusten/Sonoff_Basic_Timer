# Sonoff_Basic_Timer
---
# Why?
  
  - Lets start with that the *Sonoff Basic* devices are cheap. At the current time
    they can be purchased for about $4 each. They include wifi, 120v to 5v power,
    a 10A relay, one button, one LED and the ability to run on mains power.
    
  - For what they offer you get a lot, but you need to be connected to the internet
    100% of the time. A primary requirement for me was to host the server myself and if
    I wanted a cloud service there was an option to connect to one.
    
  - I was looking for a way to have many, 10 or more, timers for different things.

  - Was there other option besides Blynk, yes but they seemed a lot more involved
    to get up and running.

# What?

  - This code should be a quick replacement on the *Sonoff Basic*.

  - You should be able to add your WiFi, Blynk Server, and Auth token, flash the
    code to the *Sonoff Basic* and be ready to connect to your Blynk Server.

  - Is this code feature compelete, ***NO***. Lots of little things need to be done.
    Sunrise/Sunset timers, Alert Mode, Manual Mode, IFTTT integration, and more...?

  - What would you like to see added? How are you using it?

# How it should work

 - Button on *Sonoff* turns on and off the relay, status LED is YELLOW.

 - In App Button turns on and off the relay, status LED is ORANGE.

 - You can turn on with the button and off with the app, or on with the app and
   off with the button.  Status LED reflects where you turned it on.

- Timer set will turn it on and off, status LED BLUE.

- Manually turn on the Relay and have the time turn it off at some point in the future.

# Sources

  - https://community.blynk.cc/t/error-isr-not-in-iram/37426/19 - Interrupt Issue for button
  - https://community.blynk.cc/t/automatic-yard-sprinkler-system/46281 - LED Status idea 
  - https://community.blynk.cc/t/time-input-widget-and-eventor/14868/17 - Timer Code
  - https://everythingsmarthome.co.uk/howto/how-to-flash-custom-firmware-to-a-sonoff-basic/ - How to Flash Firmware
  - https://medium.com/@jeffreyroshan/flashing-a-custom-firmware-to-sonoff-wifi-switch-with-arduino-ide-402e5a2f77b - How to Flash Firmware
  
