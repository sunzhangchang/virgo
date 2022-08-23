#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <shellapi.h>
#include <vector>

#ifndef MOD_NOREPEAT
#define MOD_NOREPEAT 0x4000
#endif

#define NUM_DESKTOPS 4
#define NUM_PINNED 8

#define PIN_HOTKEY NUM_DESKTOPS * 2 + 2
#define QUIT_HOTKEY NUM_DESKTOPS * 2 + 1

static unsigned is_valid_window(HWND hwnd) {
    WINDOWINFO wi;
    wi.cbSize = sizeof(wi);
    GetWindowInfo(hwnd, &wi);
    return (wi.dwStyle & WS_VISIBLE) && !(wi.dwExStyle & WS_EX_TOOLWINDOW);
}

class Windows {
public:
    std::vector<HWND> windows;
    HWND focus;

    void modify(unsigned state) {
        for (auto & win : this->windows) {
            ShowWindow(win, state);
        }
    }

    void show() { this->modify(SW_SHOW); }
    void hide() { this->modify(SW_HIDE); }

    void push(HWND hwnd) { this->windows.emplace_back(hwnd); }
    void erase(HWND hwnd) {
        for (auto it = this->windows.begin(); it != this->windows.end(); ++ it) {
            if (*it == hwnd) {
                this->windows.erase(it);
                return;
            }
        }
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

class Trayicon {
public:
    NOTIFYICONDATA nid;
    HBITMAP hBitmap;
    HFONT hFont;
    HWND hwnd;
    HDC mdc;
    unsigned bitmapWidth;

    HICON draw(char *text, unsigned len) {
        ICONINFO iconInfo;
        HBITMAP hOldBitmap;
        HFONT hOldFont;
        hOldBitmap = (HBITMAP)SelectObject(this->mdc, this->hBitmap);
        hOldFont = (HFONT)SelectObject(this->mdc, this->hFont);
        TextOut(this->mdc, this->bitmapWidth / 4, 0, text, len);
        SelectObject(this->mdc, hOldBitmap);
        SelectObject(this->mdc, hOldFont);
        iconInfo.fIcon = TRUE;
        iconInfo.xHotspot = iconInfo.yHotspot = 0;
        iconInfo.hbmMask = iconInfo.hbmColor = this->hBitmap;
        return CreateIconIndirect(&iconInfo);
    }

    void init() {
        HDC hdc;
        this->hwnd = CreateWindowA("STATIC", "virgo", 0, 0, 0, 0, 0, NULL, NULL, NULL, NULL);
        this->bitmapWidth = GetSystemMetrics(SM_CXSMICON);
        this->nid.cbSize = sizeof(this->nid);
        this->nid.hWnd = this->hwnd;
        this->nid.uID = 100;
        this->nid.uFlags = NIF_ICON;
        hdc = GetDC(this->hwnd);
        this->hBitmap = CreateCompatibleBitmap(hdc, this->bitmapWidth, this->bitmapWidth);
        this->mdc = CreateCompatibleDC(hdc);
        ReleaseDC(this->hwnd, hdc);
        SetBkColor(this->mdc, RGB(0x00, 0x00, 0x00));
        SetTextColor(this->mdc, RGB(0x00, 0xFF, 0x00));
        this->hFont = CreateFont(-MulDiv(11, GetDeviceCaps(this->mdc, LOGPIXELSY), 72), 0,
            0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, TEXT("Arial"));
        this->nid.hIcon = this->draw((char *)"1", 1);
        Shell_NotifyIcon(NIM_ADD, &this->nid);
    }

    void deinit() {
        Shell_NotifyIcon(NIM_DELETE, &this->nid);
        DestroyIcon(this->nid.hIcon);
        DeleteObject(this->hBitmap);
        DeleteObject(this->hFont);
        DeleteDC(this->mdc);
        DestroyWindow(this->hwnd);
    }

    void set(unsigned number) {
        char snumber[2];
        if (number > 9) {
            return;
        }
        snumber[0] = number + '0';
        snumber[1] = 0;
        DestroyIcon(this->nid.hIcon);
        this->nid.hIcon = this->draw(snumber, 1);
        Shell_NotifyIcon(NIM_MODIFY, &this->nid);
    }
};

static void register_hotkey(unsigned id, unsigned mod, unsigned vk) {
    if (!RegisterHotKey(NULL, id, mod, vk)) {
        MessageBox(NULL, "could not register hotkey", "error",
            MB_ICONEXCLAMATION);
        ExitProcess(1);
    }
}

template<class _Iter, class _N_Iter, class F>
static _Iter copy_if(_Iter st, _Iter ed, _N_Iter n_st, F && func) {
    while (st != ed) {
        if (func(*st)) {
            *n_st = *st;
            ++ n_st;
        }
        ++ st;
    }
    return n_st;
}

template<class T, class F>
static std::vector<T> filter(std::vector<T> v, F && func) {
    v.resize(copy_if(v.cbegin(), v.cend(), v.begin(), std::forward<F>(func)) - v.cbegin());
    return std::move(v);
}

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
    auto v = (Virgo *)lParam;
    unsigned i;
    if (!is_valid_window(hwnd)) {
        return true;
    }
    for (i = 0; i < NUM_DESKTOPS; i++) {
        const auto & desk = v->desktops[i];
        for (const auto & e : desk.windows) {
            if (e == hwnd) {
                return true;
            }
        }
    }
    if (v->is_pinned(hwnd)) {
        return true;
    }
    v->desktops[v->current].push(hwnd);
    return true;
}

void Virgo::update() {
    unsigned i;
    HWND hwnd;
    for (i = 0; i < NUM_DESKTOPS; i++) {
        auto & windows = this->desktops[i].windows;
        windows = filter(windows, [](const HWND & hwnd) -> bool {
            return !!GetWindowThreadProcessId(hwnd, NULL);
        });
    }
    for (i = 0; i < NUM_PINNED; i++) {
        hwnd = this->pinned[i];
        if (!GetWindowThreadProcessId(hwnd, NULL)) {
            this->pinned[i] = 0;
        }
    }
    auto & windows = this->desktops[this->current].windows;
    windows = filter(windows, [](const HWND & hwnd) -> bool {
        return !!IsWindowVisible(hwnd);
    });

    EnumWindows((WNDENUMPROC)&enum_func, (LPARAM)this);
}

// void __main(void) __asm__("__main");
// void __main(void) {
int main() {
    Virgo v = {0};
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
    // ExitProcess(0);
    return 0;
}
