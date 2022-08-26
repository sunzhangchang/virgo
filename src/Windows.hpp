_Pragma("once")

#include "vector.hpp"
#include <windows.h>

namespace virgo {
    bool is_valid_window(HWND hwnd);

    class Windows {
    public:
        vector<HWND> windows;
        HWND focus;

        Windows() : windows(), focus() {}

        void show();
        void hide();

        void push(HWND hwnd);
        void erase(HWND hwnd);
        void clear();

        void set_focus(HWND hwnd);
        void set_focus_fore();
        void restore_focus();
    };
}
