#define WIN32_LEAN_AND_MEAN
#undef STD

#include <windows.h>
#include <shellapi.h>
#include "vector.hpp"
#include "util.hpp"
#include "Virgo.hpp"

void __main(void) __asm__("__main");
void __main(void) {
// int main() {
    virgo::Virgo v = {0};
    MSG msg;
    v.init();
    while (GetMessage(&msg, NULL, 0, 0)) {
        if (msg.message != WM_HOTKEY) {
            continue;
        }
        if (msg.wParam == NUM_DESKTOPS * 2) {
            break;
        }
        if (msg.wParam == QUIT_HOTKEY) {
            v.toggle_hotkeys();
        } else if (msg.wParam == PIN_HOTKEY) {
            v.toggle_pin(GetForegroundWindow());
        } else if (msg.wParam % 2 == 0) {
            v.go_to_desk(msg.wParam / 2);
        } else {
            v.move_to_desk((msg.wParam - 1) / 2);
        }
    }
    v.deinit();
    ExitProcess(0);
    // return 0;
}
