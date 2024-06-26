﻿#pragma once
#include <bitset>
#include <stack>

#include "Instructions.h"
#include "KeyPad.h"
#include "Timer.h"

constexpr int CHIP8_HEIGHT = 32;
constexpr int CHIP8_WIDTH = 64;


class Emulator
{
public:
    Emulator();
    void LoadRom(const C8_BYTE* rom, long long size);
    bool Tick();
    const std::bitset<CHIP8_WIDTH>* getScreen() const;
    KeyPad keypad;

private:
    bool LoadSprite(C8_BYTE Vx, C8_BYTE Vy, C8_BYTE bytes);
    void clearScreen();
    std::bitset<CHIP8_WIDTH> display[CHIP8_HEIGHT] = {};
    C8_BYTE memory[4096] = {};
    C8_BYTE registers[16] = {};
    std::stack<C8_POINTER> stack = {};

    C8_POINTER pc = 0;
    C8_POINTER index = 0;
    Timer delay;
    Timer sound;
    char shouldLoadKeypad = -1;

    C8_INSTRUCTION Fetch();

    static constexpr C8_BYTE fontPosition = 0x50;
    static constexpr C8_BYTE characterSize = 5;
    static constexpr C8_BYTE font[] = {
                                 0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
                                 0x20, 0x60, 0x20, 0x20, 0x70, // 1
                                 0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
                                 0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
                                 0x90, 0x90, 0xF0, 0x10, 0x10, // 4
                                 0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
                                 0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
                                 0xF0, 0x10, 0x20, 0x40, 0x40, // 7
                                 0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
                                 0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
                                 0xF0, 0x90, 0xF0, 0x90, 0x90, // A
                                 0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
                                 0xF0, 0x80, 0x80, 0x80, 0xF0, // C
                                 0xE0, 0x90, 0x90, 0x90, 0xE0, // D
                                 0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
                                 0xF0, 0x80, 0xF0, 0x80, 0x80  // F
                             };
};
