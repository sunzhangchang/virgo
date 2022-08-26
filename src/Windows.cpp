#include "Windows.hpp"

namespace virgo {
    bool is_valid_window(HWND hwnd) {
        WINDOWINFO wi;
        wi.cbSize = sizeof(wi);
        GetWindowInfo(hwnd, &wi);
        return (wi.dwStyle & WS_VISIBLE) && !(wi.dwExStyle & WS_EX_TOOLWINDOW);
    }

    void Windows::show() {
        if (this->windows.empty()) {
            return;
        }
        for (auto it = this->windows.end(); it != this->windows.begin(); -- it) {
            ShowWindow(*(it - 1), SW_SHOW);
        }
    }

    void Windows::hide() {
        for (auto & hwnd : this->windows) {
            ShowWindow(hwnd, SW_HIDE);
        }
    }

    void Windows::push(HWND hwnd) { this->windows.push_back(hwnd); }
    void Windows::erase(HWND hwnd) { this->windows.erase(this->windows.find(hwnd)); }
    void Windows::clear() { this->windows.clear(); this->focus = nullptr; }

    void Windows::set_focus(HWND hwnd) { this->focus = hwnd; }
    void Windows::set_focus_fore() { this->set_focus(GetForegroundWindow()); }
    void Windows::restore_focus() {
        if (!this->focus || !is_valid_window(this->focus)) {
            return;
        }
        ShowWindow(this->focus, SW_HIDE);
        ShowWindow(this->focus, SW_SHOW);
        SetForegroundWindow(this->focus);
    }
}
