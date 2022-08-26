#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#include <shellapi.h>
#include "Virgo.hpp"

void __main(void) __asm__("__main");
void __main(void) {
// int main() {
    virgo::Virgo v{};
    MSG msg;
    v.init();
    while (GetMessage(&msg, nullptr, 0, 0)) {
        if (msg.message != WM_HOTKEY) {
            continue;
        }
        if (msg.wParam == QUIT_HOTKEY) {
            break;
        }
        if (msg.wParam == STOP_HOTKEY) {
            v.toggle_hotkeys();
        } else if (msg.wParam == PIN_HOTKEY) {
            v.toggle_pin(GetForegroundWindow());
        } else if (msg.wParam % 2 == 0) {
            v.go_to_desk(msg.wParam / 2);
        } else {
            v.move_to_desk((msg.wParam - 1) / 2);
        }
    }
    v.destroy();
    ExitProcess(0);
    // return 0;
}
