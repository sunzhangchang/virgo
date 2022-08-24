#pragma once

#include "vector.hpp"
#include "Trayicon.hpp"
#include "Windows.hpp"
#include <windows.h>

namespace virgo {
    static void register_hotkey(unsigned id, unsigned mod, unsigned vk) {
        if (!RegisterHotKey(NULL, id, mod, vk)) {
            MessageBox(NULL, "could not register hotkey", "error",
                MB_ICONEXCLAMATION);
            ExitProcess(1);
        }
    }

#ifndef MOD_NOREPEAT
#define MOD_NOREPEAT 0x4000
#endif

#define NUM_DESKTOPS 4
#define NUM_PINNED 8

#define PIN_HOTKEY NUM_DESKTOPS * 2 + 2
#define QUIT_HOTKEY NUM_DESKTOPS * 2 + 1

    class Virgo {
    public:
        unsigned current;
        unsigned handle_hotkeys;
        Windows desktops[NUM_DESKTOPS];
        Trayicon trayicon;
        HWND pinned[NUM_PINNED];

        void init() {
            unsigned i;
            this->handle_hotkeys = 1;
            for (i = 0; i < NUM_DESKTOPS; i++) {
                register_hotkey(i * 2, MOD_ALT | MOD_NOREPEAT, i + 1 + '0');
                register_hotkey(i * 2 + 1, MOD_CONTROL | MOD_NOREPEAT, i + 1 + '0');
            }
            register_hotkey(i * 2, MOD_ALT | MOD_CONTROL | MOD_SHIFT | MOD_NOREPEAT,
                'Q');
            register_hotkey(i * 2 + 1, MOD_ALT | MOD_CONTROL | MOD_SHIFT | MOD_NOREPEAT,
                'S');
            register_hotkey(i * 2 + 2, MOD_ALT | MOD_CONTROL | MOD_SHIFT | MOD_NOREPEAT,
                'P');
            this->trayicon.init();
        }

        void deinit() {
            for (unsigned i = 0; i < NUM_DESKTOPS; ++ i) {
                this->desktops[i].show();
                this->desktops[i].windows.clear();
            }
            this->trayicon.deinit();
        }

        void toggle_hotkeys() {
            unsigned i;
            this->handle_hotkeys = !this->handle_hotkeys;
            if (this->handle_hotkeys) {
                for (i = 0; i < NUM_DESKTOPS; ++ i) {
                    register_hotkey(i * 2, MOD_ALT | MOD_NOREPEAT, i + 1 + '0');
                    register_hotkey(i * 2 + 1, MOD_CONTROL | MOD_NOREPEAT, i + 1 + '0');
                }
            } else {
                for (i = 0; i < NUM_DESKTOPS; ++ i) {
                    UnregisterHotKey(NULL, i * 2);
                    UnregisterHotKey(NULL, i * 2 + 1);
                }
            }
        }

        void toggle_pin(HWND hwnd) {
            unsigned i;
            unsigned empty = NUM_PINNED;
            for (i = 0; i < NUM_PINNED; ++i) {
                if (this->pinned[i] == hwnd) {
                    this->pinned[i] = 0;
                    this->desktops[this->current].push(hwnd);
                    return;
                }
                if (!this->pinned[i]) {
                    empty = i;
                }
            }
            if (empty == NUM_PINNED) {
                MessageBox(NULL, "reached pinned windows limit", "error",
                    MB_ICONEXCLAMATION);
                return;
            }
            this->pinned[empty] = hwnd;
            this->desktops[this->current].erase(hwnd);
        }

        void go_to_desk(unsigned desk) {
            if (this->current == desk) {
                return;
            }
            this->update();
            this->desktops[this->current].set_focus_fore();
            this->desktops[this->current].hide();
            this->desktops[desk].show();
            this->desktops[desk].restore_focus();
            this->current = desk;
            this->trayicon.set(this->current + 1);
        }

        void move_to_desk(unsigned desk) {
            HWND hwnd;
            if (this->current == desk) {
                return;
            }
            this->update();
            hwnd = GetForegroundWindow();
            if (this->is_pinned(hwnd)) {
                return;
            }
            if (!hwnd || !is_valid_window(hwnd)) {
                return;
            }
            this->desktops[this->current].erase(hwnd);
            this->desktops[desk].push(hwnd);
            ShowWindow(hwnd, SW_HIDE);
            this->desktops[desk].set_focus(hwnd);
            this->update();
        }

        void update();

        bool is_pinned(HWND hwnd) {
            for (unsigned i = 0; i < NUM_PINNED; ++ i) {
                if (this->pinned[i] == hwnd) {
                    return true;
                }
            }
            return false;
        }
    };

    BOOL enum_func(HWND hwnd, LPARAM lParam) {
        Virgo * v = (Virgo *)lParam;
        unsigned i;
        if (!is_valid_window(hwnd)) {
            return true;
        }
        for (i = 0; i < NUM_DESKTOPS; i++) {
            auto & tmp = v->desktops[i].windows;
            // if (find(x.begin(), x.end(), hwnd) != x.end()) {
            //     return true;
            // }
            if (tmp.find(hwnd) != tmp.end()) {
                return true;
            }
        }
        if (v->is_pinned(hwnd)) {
            return true;
        }
        v->desktops[v->current].push(hwnd);
        return true;
    }

    void Virgo::update() {
        for (unsigned i = 0; i < NUM_DESKTOPS; ++ i) {
            auto & windows = this->desktops[i].windows;
            windows = windows.filter([](const HWND & hwnd) -> bool {
                return !!GetWindowThreadProcessId(hwnd, NULL);
            });
        }
        for (unsigned i = 0; i < NUM_PINNED; i++) {
            auto & hwnd = this->pinned[i];
            if (!GetWindowThreadProcessId(hwnd, NULL)) {
                this->pinned[i] = 0;
            }
        }
        auto & windows = this->desktops[this->current].windows;
        windows = windows.filter([](const HWND & hwnd) -> bool {
            return !!IsWindowVisible(hwnd);
        });
        EnumWindows((WNDENUMPROC)&enum_func, (LPARAM)this);
    }
}