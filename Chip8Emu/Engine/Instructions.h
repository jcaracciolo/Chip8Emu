#pragma once

#include <intsafe.h>
#include <iostream>

#define GET_INS_MASK(x) (((x) & 0xFFFF0000) >> 4*4)
#define GET_UPPER_BYTE(x) (((x) & 0xFF00) >> 2*4)
#define GET_LOWER_BYTE(x) ((x) & 0x00FF)

#define GET_MID_BYTE(x) (((x) & 0x00F0) >>4)
#define GET_LOWEST_BYTE(x) ((x) & 0x000F)

// Actually is UINT12 but doesn't matter
typedef UINT16 C8_POINTER;

#define C8_INS_SIZE 16
typedef UINT16 C8_INSTRUCTION;

typedef unsigned char C8_BYTE;

enum InstructionSet: unsigned int
{
    CLS         = 0xFFFF00E0, // 00E0 - CLS
    RET         = 0xFFFF00EE, // 00EE - RET
    SYS_addr    = 0xF0000000, // 0nnn - SYS addr
    JP_addr     = 0xF0001000, // 1nnn - JP addr
    CALL_addr   = 0xF0002000, // 2nnn - CALL addr
    SE_Vx_byte  = 0xF0003000, // 3xkk - SE Vx, byte
    SNE_Vx_byte = 0xF0004000, // 4xkk - SNE Vx, byte
    SE_Vx_Vy    = 0xF0005000, // 5xy0 - SE Vx, Vy
    LD_Vx_byte  = 0xF0006000, // 6xkk - LD Vx, byte
    ADD_Vx_byte = 0xF0007000, // 7xkk - ADD Vx, byte
    LD_Vx_Vy    = 0xF00F8000, // 8xy0 - LD Vx, Vy
    OR_Vx_Vy    = 0xF00F8001, // 8xy1 - OR Vx, Vy
    AND_Vx_Vy   = 0xF00F8002, // 8xy2 - AND Vx, Vy
    XOR_Vx_Vy   = 0xF00F8003, // 8xy3 - XOR Vx, Vy
    ADD_Vx_Vy   = 0xF00F8004, // 8xy4 - ADD Vx, Vy
    SUB_Vx_Vy   = 0xF00F8005, // 8xy5 - SUB Vx, Vy
    SHR_Vx_Vy   = 0xF00F8006, // 8xy6 - SHR Vx {, Vy}
    SUBN_Vx_Vy  = 0xF00F8007, // 8xy7 - SUBN Vx, Vy
    SHL_Vx_Vy   = 0xF00F800E, // 8xyE - SHL Vx {, Vy}
    SNE_Vx_Vy   = 0xF00F9000, // 9xy0 - SNE Vx, Vy
    LD_I_addr   = 0xF000A000, // Annn - LD I, addr
    JP_V0_addr  = 0xF000B000, // Bnnn - JP V0, addr
    RND_Vx_byte = 0xF000C000, // Cxkk - RND Vx, byte
    DRW_Vx_Vy_nibble    = 0xF000D000, // Dxyn - DRW Vx, Vy, nibble
    SKP_Vx      = 0xF0FFE09E, // Ex9E - SKP Vx
    SKNP_Vx     = 0xF0FFE0A1, // ExA1 - SKNP Vx
    LD_Vx_DT    = 0xF0FFF007, // Fx07 - LD Vx, DT
    LD_Vx_K     = 0xF0FFF00A, // Fx0A - LD Vx, K
    LD_DT_Vx    = 0xF0FFF015, // Fx15 - LD DT, Vx
    LD_ST_Vx    = 0xF0FFF018, // Fx18 - LD ST, Vx
    ADD_I_Vx    = 0xF0FFF01E, // Fx1E - ADD I, Vx
    LD_F_Vx     = 0XF0FFF029, // Fx29 - LD F, Vx
    LD_B_Vx     = 0xF0FFF033, // Fx33 - LD B, Vx
    LD_I_Vx     = 0xF0FFF055, // Fx55 - LD [I], Vx
    LD_Vx_I     = 0xF0FFF065, // Fx65 - LD Vx, [I]
};

class Instructions
{
public:
    struct InstructionInfo {
        InstructionSet ins;
        const char* name;
    };
    
    static std::string GetName(C8_INSTRUCTION instr);
    static std::string GetNames();
    static void PrintIntsNames(const C8_BYTE* memory);
    static void PrintName(InstructionInfo instruction, C8_INSTRUCTION readFrom);
    static InstructionInfo GetInfo(C8_INSTRUCTION instr);
    static void PrintIntsNames(const C8_BYTE* const memory, int size);

private:
    static constexpr InstructionInfo AllInstructions[] = { 
        {CLS, "CLS"},      
        {RET, "RET"}, 
        {SYS_addr, "SYS_addr"},
        {JP_addr, "JP_addr"}, 
        {CALL_addr, "CALL_addr"}, 
        {SE_Vx_byte, "SE_Vx_byte"},
        {SNE_Vx_byte, "SNE_Vx_byte"},
        {SE_Vx_Vy, "SE_Vx_Vy"},
        {LD_Vx_byte, "LD_Vx_byte"},
        {ADD_Vx_byte, "ADD_Vx_byte"},
        {LD_Vx_Vy, "LD_Vx_Vy"},
        {OR_Vx_Vy, "OR_Vx_Vy"},
        {AND_Vx_Vy, "AND_Vx_Vy"}, 
        {XOR_Vx_Vy, "XOR_Vx_Vy"}, 
        {ADD_Vx_Vy, "ADD_Vx_Vy"}, 
        {SUB_Vx_Vy, "SUB_Vx_Vy"}, 
        {SHR_Vx_Vy, "SHR_Vx_Vy"}, 
        {SUBN_Vx_Vy, "SUBN_Vx_Vy"},
        {SHL_Vx_Vy, "SHL_Vx_Vy"}, 
        {SNE_Vx_Vy, "SNE_Vx_Vy"}, 
        {LD_I_addr, "LD_I_addr"}, 
        {JP_V0_addr, "JP_V0_addr"},
        {RND_Vx_byte, "RND_Vx_byte"},
        {DRW_Vx_Vy_nibble, "DRW_Vx_Vy_nibble"},
        {SKP_Vx, "SKP_Vx"},
        {SKNP_Vx, "SKNP_Vx"}, 
        {LD_Vx_DT, "LD_Vx_DT"},
        {LD_Vx_K, "LD_Vx_K"}, 
        {LD_DT_Vx, "LD_DT_Vx"},
        {LD_ST_Vx, "LD_ST_Vx"},
        {ADD_I_Vx, "ADD_I_Vx"},
        {LD_F_Vx, "LD_F_Vx"}, 
        {LD_B_Vx, "LD_B_Vx"}, 
        {LD_I_Vx, "LD_I_Vx"},   
        {LD_Vx_I, "LD_Vx_I"},
    };
};

