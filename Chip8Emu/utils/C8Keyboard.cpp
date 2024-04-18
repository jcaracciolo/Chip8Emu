#include "C8Keyboard.h"

void C8Keyboard::SetKeyPressed(unsigned char keycode)
{
    kp.SetKeyPressed(keycodes[keycode]);
}

void C8Keyboard::SetKeyReleased(unsigned char keycode)
{
    kp.SetKeyReleased(keycodes[keycode]);
}
