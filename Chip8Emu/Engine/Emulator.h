#pragma once
#include <bitset>
#include <stack>
#include <vector>

#include "Instructions.h"

constexpr int CHIP8_HEIGHT = 32;
constexpr int CHIP8_WIDTH = 64;

class Emulator
{
public:

    void LoadRom(const C8_BYTE* rom, int size);
    bool Tick();
    const std::bitset<CHIP8_WIDTH>* getScreen() const;
    

private:
    bool LoadSprite(C8_BYTE Vx, C8_BYTE Vy, C8_BYTE bytes);
    std::bitset<CHIP8_WIDTH> display[CHIP8_HEIGHT] = {};
    C8_BYTE memory[4096]= {};
    C8_BYTE registers[16] = {};
    std::stack<C8_POINTER> stack= {};
    
    C8_POINTER pc = 0;
    C8_POINTER index = 0;

    
    C8_INSTRUCTION Fetch();
    
    
    C8_BYTE* At(C8_POINTER location);
    C8_POINTER Pop();
    void PUSH(C8_POINTER address);

    
};
