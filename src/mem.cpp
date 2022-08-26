#include "mem.hpp"

namespace virgo {
    void * halloc(unsigned n) { return HeapAlloc(GetProcessHeap(), 0, n); }
    void * hrealloc(void * a, unsigned n) { return HeapReAlloc(GetProcessHeap(), 0, a, n); }
    void hfree(void * a) { (a != nullptr) ? (HeapFree(GetProcessHeap(), 0, a), void()) : void(); a = nullptr; }
}
