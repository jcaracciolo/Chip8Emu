#pragma once
#include <bitset>

#include "Instructions.h"

class KeyPad
{
public:
    bool IsKeyPressed(C8_BYTE key) const;
    void SetKeyPressed(C8_BYTE key);
    void SetKeyReleased(C8_BYTE key);
    void SetExpectingInput();
    bool IsExpectingInput() const;
    C8_BYTE GetLastKeyPressed() const;
    
private:
    std::bitset<0xF+1> keypad;
    bool isExpectingInput = false;
    C8_BYTE lastKeyReleased = 0;
};
