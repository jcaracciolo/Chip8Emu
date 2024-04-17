#include "C8Keyboard.h"

bool C8Keyboard::GetKeyState(C8_BYTE key)
{
    if(key > 0xF)
    {
        return false;
    }

    auto a =kb.KeyIsPressed(keycodes[key]);
    return a; 
}
