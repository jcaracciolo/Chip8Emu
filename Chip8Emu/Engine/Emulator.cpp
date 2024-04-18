#include "Emulator.h"
#include <complex>

Emulator::Emulator()
{
    std::memcpy(&memory[fontPosition], font, std::size(font));
}

void Emulator::LoadRom(const C8_BYTE* rom, long long size)
{
    std::memcpy(&memory[0x200], rom, size);
    pc = 0x200;
}

bool Emulator::Tick()
{
    // Do some checks for LD_Vx_K
    if (keypad.IsExpectingInput())
    {
        return false;
    }

    if (shouldLoadKeypad >= 0)
    {
        registers[shouldLoadKeypad] = keypad.GetLastKeyPressed();
        shouldLoadKeypad = -1;
    }

    C8_INSTRUCTION instruction = Fetch();
    auto info = Instructions::GetInfo(instruction);
    Instructions::PrintName(info, instruction);

    auto reverseMask = ~GET_INS_MASK(info.ins);
    C8_INSTRUCTION value = instruction & reverseMask;
    C8_BYTE lower = GET_LOWER_BYTE(value);
    C8_BYTE upper = GET_UPPER_BYTE(value);
    C8_BYTE lowest = GET_LOWEST_BYTE(value);
    C8_BYTE mid = GET_MID_BYTE(value);

    switch (info.ins)
    {
    case CLS:
        display->reset();
        break;
    case RET:
        pc = stack.top();
        break;
    case SYS_addr:
    case JP_addr:
        pc = value;
        break;
    case CALL_addr:
        stack.push(pc);
        pc = value;
        break;
    case SE_Vx_byte:
        if (registers[upper] == lower)
            pc += 2;
        break;
    case SNE_Vx_byte:
        if (registers[upper] != lower)
            pc += 2;
        break;
    case SE_Vx_Vy:
        if (registers[upper] == registers[mid])
            pc += 2;
        break;
    case LD_Vx_byte:
        registers[upper] = lower;
        break;
    case ADD_Vx_byte:
        registers[0xF] = static_cast<C8_BYTE>(registers[upper] + lower) < registers[upper];
        registers[upper] += lower;
        break;
    case LD_Vx_Vy:
        registers[upper] = registers[mid];
        break;
    case OR_Vx_Vy:
        registers[upper] = registers[upper] | registers[mid];
        break;
    case AND_Vx_Vy:
        registers[upper] = registers[upper] & registers[mid];
        break;
    case XOR_Vx_Vy:
        registers[upper] = registers[upper] ^ registers[mid];
        break;
    case ADD_Vx_Vy:
        registers[0xF] = static_cast<C8_BYTE>(registers[upper] + registers[mid]) < registers[upper];
        registers[upper] += registers[mid];
        break;
    case SUB_Vx_Vy:
        registers[0xF] = registers[upper] > registers[mid];
        registers[upper] = registers[upper] - registers[mid];
        break;
    case SHR_Vx_Vy:
        // TODO this should be configurable
        registers[upper] = registers[mid];
        registers[0xF] = bool(registers[upper] & 1);
        registers[upper] >>= 1;
        break;
    case SUBN_Vx_Vy:
        registers[0xF] = registers[mid] > registers[upper];
        registers[upper] = registers[mid] - registers[upper];
        break;
    case SHL_Vx_Vy:
        // TODO this should be configurable
        registers[upper] = registers[mid];
        registers[0xF] = bool(registers[upper] & 0x80);
        registers[upper] <<= 1;
        break;
    case SNE_Vx_Vy:
        if (registers[upper] != registers[mid])
            pc += 2;
        break;
    case LD_I_addr:
        index = value;
        break;
    case JP_V0_addr:
        pc = value + registers[0x0];
        break;
    case RND_Vx_byte:
        registers[upper] = static_cast<C8_BYTE>(rand() % 255) & lower; // NOLINT(concurrency-mt-unsafe)
        break;
    case DRW_Vx_Vy_nibble:
        registers[0xF] = LoadSprite(upper, mid, lowest);
        break;
    case SKP_Vx:
        if (keypad.IsKeyPressed(registers[upper]))
            pc += 2;
        break;
    case SKNP_Vx:
        if (!keypad.IsKeyPressed(registers[upper]))
            pc += 2;
        break;
    case LD_Vx_DT:
        registers[upper] = delay.Get();
        break;
    case LD_Vx_K:
        keypad.SetExpectingInput();
        shouldLoadKeypad = static_cast<char>(upper);
        break;
    case LD_DT_Vx:
        delay.Set(registers[upper]);
        break;
    case LD_ST_Vx:
        sound.Set(registers[upper]);
        break;
    case ADD_I_Vx:
        index += registers[upper];
        break;
    case LD_F_Vx:
        index = static_cast<C8_BYTE>(fontPosition + characterSize * registers[upper]);
        break;
    case LD_B_Vx:
        memory[index] = static_cast<C8_BYTE>(registers[upper] / 100);
        memory[index + 1] = static_cast<C8_BYTE>(registers[upper] % 100 / 10);
        memory[index + 2] = static_cast<C8_BYTE>(registers[upper] % 10);
        break;
    case LD_I_Vx:
        for (int i = 0; i <= upper; ++i)
        {
            memory[index + i] = registers[i];
        }
        break;
    case LD_Vx_I:
        for (int i = 0; i <= upper; ++i)
        {
            registers[i] = memory[index + i];
        }
        break;
    }

    return info.ins == CLS || info.ins == DRW_Vx_Vy_nibble;
}

const std::bitset<CHIP8_WIDTH>* Emulator::getScreen() const
{
    return display;
}

bool Emulator::LoadSprite(C8_BYTE Vx, C8_BYTE Vy, C8_BYTE bytes)
{
    auto origX = registers[Vx];
    auto origY = registers[Vy];
    C8_POINTER sprite = index;
    bool collision = 0;
    for (C8_BYTE i = 0; i < bytes; ++i)
    {
        auto y = (origY + i) % CHIP8_HEIGHT;
        auto byte = memory[sprite + i];
        auto row = &display[y];
        auto mask = 1;

        for (int j = 7; j >= 0; j--)
        {
            auto x = (origX + j) % CHIP8_WIDTH;
            auto flipPixel = byte & mask;
            // If a pixel is erased, set collision to true
            collision |= flipPixel && row->test(x);
            if (flipPixel)
            {
                row->flip(x);
            }
            mask <<= 1;
        }
    }

    return collision;
}

C8_INSTRUCTION Emulator::Fetch()
{
    C8_INSTRUCTION firstNibble = memory[pc];
    C8_INSTRUCTION secondNibble = memory[pc + 1];

    firstNibble = firstNibble << (C8_INS_SIZE / 2); // NOLINT(clang-diagnostic-shift-count-overflow)

    pc += 2;
    return firstNibble | secondNibble;
}

C8_BYTE* Emulator::At(C8_POINTER location)
{
    return &memory[location];
}

C8_POINTER Emulator::Pop()
{
    return stack.top();
}

void Emulator::PUSH(C8_POINTER address)
{
    stack.push(address);
}
