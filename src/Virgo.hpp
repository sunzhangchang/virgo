_Pragma("once")

#include "vector.hpp"
#include "Trayicon.hpp"
#include "Windows.hpp"
#include <windows.h>

namespace virgo {
    void register_hotkey(unsigned id, unsigned mod, unsigned vk);

#ifndef MOD_NOREPEAT
#define MOD_NOREPEAT 0x4000
#endif

#define NUM_DESKTOPS 4

#define QUIT_HOTKEY NUM_DESKTOPS * 2
#define STOP_HOTKEY NUM_DESKTOPS * 2 + 1
#define PIN_HOTKEY NUM_DESKTOPS * 2 + 2

    BOOL enum_func(HWND hwnd, LPARAM lParam);

    class Virgo {
    private:
        unsigned current;
        bool handle_hotkeys;
        Windows desktops[NUM_DESKTOPS];
        Trayicon trayicon;
        vector<HWND> pinned;

    public:
        Virgo(): current(0), handle_hotkeys(false), desktops{}, trayicon(), pinned() {}

        void init();

        void destroy();

        void toggle_hotkeys();

        void toggle_pin(HWND hwnd);

        Windows & desk(unsigned i) &;
        const Windows & desk(unsigned i) const &;

        Windows & current_desk() &;
        const Windows & current_desk() const &;

        void go_to_desk(unsigned desk);
        void move_to_desk(unsigned desk);
        void update();

        bool is_pinned(HWND hwnd);
    };
}
