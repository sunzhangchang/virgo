#include "Virgo.hpp"

namespace virgo {
    void register_hotkey(unsigned id, unsigned mod, unsigned vk) {
        if (!RegisterHotKey(nullptr, id, mod, vk)) {
            MessageBox(nullptr, "could not register hotkey", "error", MB_ICONEXCLAMATION);
            ExitProcess(1);
        }
    }

    Windows & Virgo::desk(unsigned i) & {
        return this->desktops[i];
    }
    const Windows & Virgo::desk(unsigned i) const & {
        return this->desktops[i];
    }

    Windows & Virgo::current_desk() & {
        return this->desktops[this->current];
    }

    const Windows & Virgo::current_desk() const & {
        return this->desktops[this->current];
    }

    void Virgo::init() {
        this->handle_hotkeys = true;
        for (unsigned i = 0; i < NUM_DESKTOPS; ++ i) {
            register_hotkey(i * 2, MOD_ALT | MOD_NOREPEAT, i + 1 + '0');
            register_hotkey(i * 2 + 1, MOD_CONTROL | MOD_NOREPEAT, i + 1 + '0');
        }
        register_hotkey(QUIT_HOTKEY, MOD_ALT | MOD_CONTROL | MOD_SHIFT | MOD_NOREPEAT, 'Q');
        register_hotkey(STOP_HOTKEY, MOD_ALT | MOD_CONTROL | MOD_SHIFT | MOD_NOREPEAT, 'S');
        register_hotkey(PIN_HOTKEY, MOD_ALT | MOD_CONTROL | MOD_SHIFT | MOD_NOREPEAT, 'P');
        this->trayicon.init();
    }

    void Virgo::destroy() {
        for (auto & desktop : this->desktops) {
            desktop.show();
            desktop.windows.clear();
        }
        this->trayicon.destroy();
    }

    void Virgo::toggle_hotkeys() {
        this->handle_hotkeys = !this->handle_hotkeys;
        if (this->handle_hotkeys) {
            for (unsigned i = 0; i < NUM_DESKTOPS; ++ i) {
                register_hotkey(i * 2, MOD_ALT | MOD_NOREPEAT, i + 1 + '0');
                register_hotkey(i * 2 + 1, MOD_CONTROL | MOD_NOREPEAT, i + 1 + '0');
            }
        }
        else {
            for (unsigned i = 0; i < NUM_DESKTOPS; ++ i) {
                UnregisterHotKey(nullptr, i * 2);
                UnregisterHotKey(nullptr, i * 2 + 1);
            }
        }
    }

    BOOL enum_func(HWND hwnd, LPARAM lParam) {
        Virgo* v = (Virgo*)lParam;
        if (!is_valid_window(hwnd)) {
            return true;
        }
        for (unsigned i = 0; i < NUM_DESKTOPS; ++ i) {
            if (v->desk(i).windows.has(hwnd)) {
                return true;
            }
        }
        if (v->is_pinned(hwnd)) {
            return true;
        }
        v->current_desk().push(hwnd);
        return true;
    }

    void Virgo::update() {
        this->current_desk().clear();
        for (unsigned i = 0; i < NUM_DESKTOPS; ++ i) {
            auto & windows = this->desk(i).windows;
            windows = windows.filter([](const HWND& hwnd) -> bool {
                return !!GetWindowThreadProcessId(hwnd, nullptr);
            });
        }
        for (auto & hwnd : this->pinned) {
            if (!GetWindowThreadProcessId(hwnd, nullptr)) {
                hwnd = nullptr;
            }
        }
        // auto & windows = this->current_desk().windows;
        // windows = windows.filter([](const HWND& hwnd) -> bool {
        //     return !!IsWindowVisible(hwnd);
        // });
        EnumWindows((WNDENUMPROC)&enum_func, (LPARAM)this);
    }

    void Virgo::toggle_pin(HWND hwnd) {
        auto pos = this->pinned.find(hwnd);
        if (pos != this->pinned.end()) {
            *pos = nullptr;
            this->current_desk().push(hwnd);
            return;
        }
        pos = this->pinned.find(nullptr);
        if (pos == this->pinned.end()) {
            this->pinned.push_back(hwnd);
        }
        else {
            *pos = hwnd;
        }
        this->current_desk().erase(hwnd);
    }

    void Virgo::go_to_desk(unsigned i) {
        if (this->current == i) {
            return;
        }
        this->update();
        this->current_desk().set_focus_fore();
        this->current_desk().hide();
        this->desk(i).show();
        this->desk(i).restore_focus();
        this->current = i;
        this->trayicon.set(this->current + 1);
    }

    void Virgo::move_to_desk(unsigned i) {
        if (this->current == i) {
            return;
        }
        this->update();
        auto hwnd = GetForegroundWindow();
        if (this->is_pinned(hwnd)) {
            return;
        }
        if ((!hwnd) || (!is_valid_window(hwnd))) {
            return;
        }
        auto & curwindows = this->current_desk().windows;
        auto pos = curwindows.find(hwnd) - curwindows.begin();
        this->current_desk().erase(hwnd);
        if (!curwindows.empty()) {
            SetForegroundWindow(curwindows[pos]);
            this->current_desk().set_focus_fore();
        }
        this->desk(i).push(hwnd);
        ShowWindow(hwnd, SW_HIDE);
        this->desk(i).set_focus(hwnd);
        this->update();
    }

    bool Virgo::is_pinned(HWND hwnd) {
        return this->pinned.has(hwnd);
    }
}
