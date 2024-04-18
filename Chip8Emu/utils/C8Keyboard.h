#pragma once
#include <map>

#include "../Engine/Instructions.h"
#include "../Engine/KeyPad.h"
#include "../windows/Keyboard.h"

class C8Keyboard
{
public:
    C8Keyboard(KeyPad& kp): kp(kp) {}
    void SetKeyPressed(unsigned char keycode);
    void SetKeyReleased(unsigned char keycode);

private:
    KeyPad& kp;
    std::map<unsigned char, C8_BYTE> keycodes = {
        {'1', 0x1},{'2', 0x2},{'3', 0x3},{'4', 0xc},
        {'Q', 0x4},{'W', 0x5},{'E', 0x6},{'R', 0xd},
        {'A', 0x7},{'S', 0x8},{'D', 0x9},{'F', 0xe},
        {'Z', 0xa},{'X', 0x0},{'C', 0xb},{'V', 0xf}   
    };
};
