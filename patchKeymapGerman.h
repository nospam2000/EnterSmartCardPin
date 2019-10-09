// Copyright (c) 2019 by michael@5dot1.de
// License: Apache Version 2.0


// translate the string for a Germany keymap. Not fully implemented right now
void patchKeymapGerman(void)
{
  for(int i = 0; i < bufSize; i++)
  {
    switch(stringBuf[i])
    {
      // TODO: add all keys which are different between US and German keyboard
      case 0:
        goto endPatch;
        break;

      case 'y':
        stringBuf[i] = 'z';
        break;

      case 'z':
        stringBuf[i] = 'y';
        break;

      case 'Y':
        stringBuf[i] = 'Z';
        break;

      case 'Z':
        stringBuf[i] = 'Y';
        break;        
    }
  }

endPatch:
  return;
}
