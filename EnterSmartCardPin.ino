
/*
Copyright (c) 2019 by michael@5dot1.de
License: Apache Version 2.0

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

*/

#include <Arduino.h>
#include "Keyboard.h"

const int buttonPin = 2; // input pin for pushbutton
const int bufSize = 2048UL + 32UL;
const int32_t baudRate = 115200UL;
const uint32_t keyLifeTime = (12UL * 60UL * 60UL * 1000UL); // maximum lifetime of the key in ms. After this time, the key will be deleted. Starts when setting the key.
const uint32_t keyNotUsedLifeTime = (90UL * 60UL * 1000UL); // after this time of not using the key it will be deleted
const uint32_t lineTimeout = 4UL * (bufSize * 11UL * 1000UL) / baudRate;
const uint32_t charTimeout = 100UL + ((15UL * 11UL * 1000UL) / baudRate);
char stringBuf[bufSize];
int previousButtonState = HIGH;   // for checking the state of a pushButton
uint32_t keySetTime;
uint32_t keyLastUsedTime;

void clearStringBuf(void) {
    memset(stringBuf, 0, bufSize);
}

void setup() {
  clearStringBuf();
  pinMode(buttonPin, INPUT_PULLUP);
  Serial.begin(115200);
  Keyboard.begin();
}

int readStringWithTimeouts()
{
  int i = -1;
  if(Serial.available())
  {
    int c = Serial.read();
    if(c == '^') // beginning of new key
    {
      clearStringBuf();
      uint32_t lineBegin = millis();
      for(i = 0 ; i < (bufSize - 1); i++)
      {
        uint32_t charBegin = millis();
        for(;;)
        {
          if(Serial.available())
          {
            c = Serial.read();
            stringBuf[i] = c;
            if(c == 0)
              goto endReadLine;
            break;
          }
          else
          {
            uint32_t now = millis();
            if(((now - lineBegin) > lineTimeout)
              || ((now - charBegin) > charTimeout))
            {
              goto endReadLine;
            }
            delay(1);
          }
        }
      }
  
  endReadLine:
      stringBuf[i] = 0;
    }
  }
  
  return i;
}

void loop() {
  uint32_t now = millis();

  // handle 'button pressed'
  int buttonState = digitalRead(buttonPin);
  if (buttonState != previousButtonState) {
    if(buttonState == LOW) {
      if(stringBuf[0])
        Keyboard.print(stringBuf);
      keyLastUsedTime = now;
      delay(200); // avoid bouncing
    }
    previousButtonState = buttonState;
  }

  // handle programming of new PIN
  int curBuflen = readStringWithTimeouts();
  if(curBuflen > 0) {
    keySetTime = now;
    keyLastUsedTime = keySetTime;
    //patchKeymapGerman(); // TODO: is not implemented
    Serial.print(F("NewKeyAccepted len="));
    Serial.println(curBuflen);
  }

  // handle timeouts: clear saved password after timeout
  if(stringBuf[0]
    &&   ((now - keySetTime) > keyLifeTime) || ((now - keyLastUsedTime) > keyNotUsedLifeTime))
  {
    clearStringBuf();
    Serial.println(F("KeyAgeExpired"));
  }

  // use RX LED to display state: LED=on means key is stored
  if(stringBuf[0]) {
    RXLED1;
  } else {
    RXLED0;
  }
}
