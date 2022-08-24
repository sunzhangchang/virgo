#pragma once

#include "vector.hpp"
#include <windows.h>

namespace virgo {
    static unsigned is_valid_window(HWND hwnd) {
        WINDOWINFO wi;
        wi.cbSize = sizeof(wi);
        GetWindowInfo(hwnd, &wi);
        return (wi.dwStyle & WS_VISIBLE) && !(wi.dwExStyle & WS_EX_TOOLWINDOW);
    }

    class Windows {
    public:
        vector<HWND> windows;
        HWND focus;

        void modify(unsigned state) {
            for (auto & win : this->windows) {
                ShowWindow(win, state);
            }
        }

        void show() { this->modify(SW_SHOW); }
        void hide() { this->modify(SW_HIDE); }

        void push(HWND hwnd) { this->windows.push_back(hwnd); }
        void erase(HWND hwnd) {
            this->windows.erase(this->windows.find(hwnd));
            // this->windows.erase(find(this->windows.begin(), this->windows.end(), hwnd));
        }

        void set_focus(HWND hwnd) { this->focus = hwnd; }
        void set_focus_fore() { this->set_focus(GetForegroundWindow()); }
        void restore_focus() {
            if (!this->focus || !is_valid_window(this->focus)) {
                return;
            }
            SetForegroundWindow(this->focus);
        }
    };
}