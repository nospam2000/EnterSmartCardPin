# EnterSmartCardPin
Emulate entering a smartcard pin on a keyboard. The PIN is stored transiently for some hours on an Arduino Pro Micro/Leonardo.

Implement a PIN-store device which allows to temporary store a PIN and type it in via the keyboard whenever a button is pressed.
The idea is to temporarily store the PIN at the beginning of the day and then just press the single button to enter it.

As hardware you need an Arduino Pro Micro or Arduino Leonardo with an ATmega32U4 MCU. You can get them cheap at aliexpress.com for
around 3$ (including shipping and handling). You need to put a button between the pin labeled with "2" (or "D2") and GND. You can also
choose another pin and adapt the constant 'buttonPin'.

The PIN has to be entered on the connected PC and is transferred to the RAM of the Arduino.
See the powershell script "SetHardwarePinKeyboard.ps1" to read a PIN from the keyboard and send it to the Arduino. You have
to adapt the COM port (or change the number of the COM port in Device Manager to COM9).
You can also use your own program to set a PIN just send it via the COM port to the Arduino (115200 baud, 8N1) and prefix it
with the '^' character. Don't make any pause during sending because the end criteria is when no character is received for 100ms.

The PIN will be cleared 12h after entering it, see constant 'keyLifeTime'
The PIN will be cleared 90min after not using it, see constant 'keyNotUsedLifeTime'

The translation of the entered string to the sent keyboard keystrokes assumes an american keyboard layout, that means when you have a 
German keyboard you need to swap 'y' and 'z' and all the keys which are on different positions on the keyboard. For numeric PINs
this is no problem and this is the main purpose of this project. Only the characters with a codepoint <128 can be used.

Any linefeed character will be translated to the enter key.

The length of the PIN is limited to 2048+31 characters ;-)