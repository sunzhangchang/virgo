#pragma once

#include <windows.h>

namespace virgo {
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
}