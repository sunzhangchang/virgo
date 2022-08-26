_Pragma("once")

#include <windows.h>

namespace virgo {
    class Trayicon {
        NOTIFYICONDATA nid;
        HBITMAP hBitmap;
        HFONT hFont;
        HWND hwnd;
        HDC mdc;
        unsigned bitmapWidth;

    public:
        Trayicon() : nid(), hBitmap(), hFont(), hwnd(), mdc(), bitmapWidth() {}

        HICON draw(char * text, unsigned len);

        void init();

        void destroy();

        void set(unsigned number);
    };
}
