#pragma once

#include "Key.h"

#ifdef WIN32

#include <conio.h>

namespace Game_2048 {
    inline Key get_pressed_key()
    {
        int code = _getch();
        switch (code) {
        case static_cast<int>(Key::Up) : return Key::Up;
        case static_cast<int>(Key::Left) : return Key::Left;
        case static_cast<int>(Key::Right) : return Key::Right;
        case static_cast<int>(Key::Down) : return Key::Down;
        case static_cast<int>(Key::Break) : return Key::Break;
        default: return Key::Other;
        }
    }

    inline void clear_console()
    {
        system("cls");
    }

    inline void wait_any_key()
    {
        system("pause");
    }

    template <typename T>
    bool is_future_ready(const std::future<T>& rhs)
    {
        return rhs._Is_ready();
    }

}

#endif //WIN32
