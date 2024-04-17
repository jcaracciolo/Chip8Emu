#pragma once
#include <map>

#include "../Engine/Instructions.h"
#include "../windows/Keyboard.h"

class C8Keyboard
{
public:
    C8Keyboard(Keyboard& kb): kb(kb) {}
    bool GetKeyState(C8_BYTE key);
private:
    Keyboard& kb;
    std::map<C8_BYTE, unsigned char> keycodes = {
        {0x1, '1'},{0x2, '2'},{0x3, '3'},{0xc, '4'},
        {0x4, 'Q'},{0x5, 'W'},{0x6, 'E'},{0xd, 'R'},
        {0x7, 'A'},{0x8, 'S'},{0x9, 'D'},{0xe, 'F'},
        {0xa, 'Z'},{0x0, 'X'},{0xb, 'C'},{0xf, 'V'}   
    };
};
