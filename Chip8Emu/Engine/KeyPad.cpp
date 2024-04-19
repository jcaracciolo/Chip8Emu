#include "KeyPad.h"

bool KeyPad::IsKeyPressed(C8_BYTE key) const
{
    return keypad.test(key);
}

void KeyPad::SetKeyPressed(C8_BYTE key)
{
    keypad.set(key);
}

void KeyPad::SetKeyReleased(C8_BYTE key)
{
    if(keypad.test(key))
    {
        keypad.set(key, false);
        isExpectingInput = false;
        lastKeyReleased = key;
    }
}

void KeyPad::SetExpectingInput()
{
    isExpectingInput = true;
}

bool KeyPad::IsExpectingInput() const
{
    return isExpectingInput;
}

C8_BYTE KeyPad::GetLastKeyPressed() const
{
    return lastKeyReleased;
}

