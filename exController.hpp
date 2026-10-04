// ============================================================================
//  NightFyre Frameworks
//  exController - Controller Interface
//  
//  Provides an interface for handling Xbox controller input using XInput.
// ============================================================================

#pragma once

#include <Windows.h>
#include <Xinput.h>
#include <cstdint>

#pragma comment(lib, "Xinput.lib")

namespace NF
{
    class exController
    {
    public:

        enum class Button : std::uint16_t
        {
            None = 0,

            DPadUp = XINPUT_GAMEPAD_DPAD_UP,
            DPadDown = XINPUT_GAMEPAD_DPAD_DOWN,
            DPadLeft = XINPUT_GAMEPAD_DPAD_LEFT,
            DPadRight = XINPUT_GAMEPAD_DPAD_RIGHT,

            Start = XINPUT_GAMEPAD_START,
            Back = XINPUT_GAMEPAD_BACK,

            LeftThumb = XINPUT_GAMEPAD_LEFT_THUMB,
            RightThumb = XINPUT_GAMEPAD_RIGHT_THUMB,

            LeftShoulder = XINPUT_GAMEPAD_LEFT_SHOULDER,
            RightShoulder = XINPUT_GAMEPAD_RIGHT_SHOULDER,

            A = XINPUT_GAMEPAD_A,
            B = XINPUT_GAMEPAD_B,
            X = XINPUT_GAMEPAD_X,
            Y = XINPUT_GAMEPAD_Y
        };

        struct Stick
        {
            float x = 0.0f;
            float y = 0.0f;
        };

        struct State
        {
            bool connected = false;

            std::uint16_t buttons = 0;

            float leftTrigger = 0.0f;
            float rightTrigger = 0.0f;

            Stick leftStick;
            Stick rightStick;
        };

    public:

        explicit exController(std::uint32_t index = 0) : m_index(index) { }

        bool Update()
        {
            m_previous = m_current;

            XINPUT_STATE state{};
            const DWORD result = XInputGetState(m_index, &state);

            if (result != ERROR_SUCCESS)
            {
                m_current = {};
                return false;
            }

            const XINPUT_GAMEPAD& pad = state.Gamepad;

            m_current.connected = true;
            m_current.buttons = pad.wButtons;

            m_current.leftTrigger = NormalizeTrigger(pad.bLeftTrigger);

            m_current.rightTrigger = NormalizeTrigger(pad.bRightTrigger);

            m_current.leftStick = NormalizeStick(pad.sThumbLX, pad.sThumbLY, XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE );

            m_current.rightStick = NormalizeStick(pad.sThumbRX, pad.sThumbRY, XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE);

            return true;
        }

        bool IsConnected() const
        {
            return m_current.connected;
        }

        bool IsDown(Button button) const
        {
            const auto mask = static_cast<std::uint16_t>(button);

            return (m_current.buttons & mask) != 0;
        }

        bool IsPressed(Button button) const
        {
            const auto mask = static_cast<std::uint16_t>(button);

            return (m_current.buttons & mask) != 0 && (m_previous.buttons & mask) == 0;
        }

        bool IsReleased(Button button) const
        {
            const auto mask = static_cast<std::uint16_t>(button);

            return (m_current.buttons & mask) == 0 && (m_previous.buttons & mask) != 0;
        }

        const State& GetState() const
        {
            return m_current;
        }

        const State& GetPreviousState() const
        {
            return m_previous;
        }

        std::uint32_t GetIndex() const
        {
            return m_index;
        }

        void SetIndex(std::uint32_t index)
        {
            m_index = index;

            m_current = {};
            m_previous = {};
        }

    private:

        static float NormalizeTrigger(BYTE value)
        {
            if (value <= XINPUT_GAMEPAD_TRIGGER_THRESHOLD)
                return 0.0f;

            return static_cast<float>( value - XINPUT_GAMEPAD_TRIGGER_THRESHOLD  ) / static_cast<float>( 255 - XINPUT_GAMEPAD_TRIGGER_THRESHOLD );
        }

        static Stick NormalizeStick(SHORT x, SHORT y, SHORT deadzone)
        {
            Stick result{};

            const auto NormalizeAxis =
                [deadzone](SHORT value) -> float
            {
                const int v = static_cast<int>(value);

                if (v > deadzone)
                    return static_cast<float>(v - deadzone) / static_cast<float>(32767 - deadzone);
               

                if (v < -deadzone)
                    return static_cast<float>(v + deadzone) / static_cast<float>(32768 - deadzone);
                
                return 0.0f;
            };

            result.x = NormalizeAxis(x);
            result.y = NormalizeAxis(y);

            return result;
        }

    private:

        std::uint32_t m_index = 0;

        State m_current{};
        State m_previous{};
    };
}
