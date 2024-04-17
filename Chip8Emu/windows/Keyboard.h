#pragma once
#include <bitset>
#include <optional>
#include <queue>

class Keyboard
{
    friend class Window;
public:
    class Event
    {
    public:
        enum class Type
        {
            Press,
            Release,
        };

        Event(Type type, unsigned char code) noexcept: type(type), code(code) {}

        bool IsPress() const noexcept
        {
            return type == Type::Press;
        }

        bool IsReleased() const noexcept
        {
            return type == Type::Release;
        }

        unsigned char GetCode() const noexcept
        {
            return code;
        }

    private:
        Type type;
        unsigned char code;
        
    };

    Keyboard();
    Keyboard(const Keyboard& other) = delete;
    Keyboard(Keyboard&& other) noexcept = delete;
    Keyboard& operator=(const Keyboard& other) = delete;
    Keyboard& operator=(Keyboard&& other) noexcept = delete;

    //keys
    bool KeyIsPressed(unsigned char keycode) const noexcept;
    std::optional<Event> ReadKey() noexcept;
    bool KeyIsEmpty() const noexcept;
    void FlushKey() noexcept;

    //char
    std::optional<char> ReadChar() noexcept;
    bool CharIsEmpty() noexcept;
    void FlushChar() noexcept;

    void Flush() noexcept;

    //autorepeat control
    void EnableAutoRepeat() noexcept;
    void DisableAutoRepeat() noexcept;
    bool AutoRepeatIsEnabled() noexcept;

private:
    void onKeyPressed(unsigned char keycode) noexcept;
    void onKeyReleased(unsigned char keycode) noexcept;
    void OnChar(char character) noexcept;
    void ClearState() noexcept;
    template<typename T> static void TrimBuffer(std::queue<T>& buffer) noexcept;

    static constexpr unsigned int nKeys = 256u;
    static constexpr unsigned int bufferSize = 16u;
    bool autoRepeatEnabled = false;
    std::bitset<nKeys> keyStates;
    std::queue<Event> keyBuffer;
    std::queue<char> charBuffer;
    
};


