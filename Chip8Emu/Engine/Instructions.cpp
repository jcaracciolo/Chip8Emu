#include "Instructions.h"

#include <iomanip>

Instructions::InstructionInfo Instructions::GetInfo(C8_INSTRUCTION instr)
{
    for(const auto iset: AllInstructions)
    {
        C8_INSTRUCTION mask = GET_INS_MASK(iset.ins);
        C8_INSTRUCTION instructionValue = static_cast<C8_INSTRUCTION>(iset.ins) & mask;

        if(instructionValue == (instr & mask))
        {
            return iset;
        }
    }

    throw "INSTR NOT FOUND";
}

void Instructions::PrintIntsNames(const C8_BYTE* const memory, int size)
{
    for (int i = 0; i < size-1;)
    {
        C8_INSTRUCTION firstNibble = memory[i++];
        C8_INSTRUCTION secondNibble = memory[i++];

        firstNibble  = firstNibble << (C8_INS_SIZE/2); // NOLINT(clang-diagnostic-shift-count-overflow)
    
        C8_INSTRUCTION instruction = firstNibble | secondNibble;
        PrintName(GetInfo(instruction), instruction);
    }
}

void Instructions::PrintName(InstructionInfo instruction, C8_INSTRUCTION readFrom)
{
    C8_INSTRUCTION mask = GET_INS_MASK(instruction.ins);
    C8_INSTRUCTION instructionValue = static_cast<C8_INSTRUCTION>(instruction.ins) & mask;
    std::cout << instruction.name << " - " << std::hex << std::setfill('0')  << std::setw(4) << instruction.ins << " - " << std::setw(4) << mask << " - " << std::setw(4) << instructionValue << " - " << std::setw(4) << readFrom << '\n';
}
