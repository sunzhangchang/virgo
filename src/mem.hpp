_Pragma("once")

#include <windows.h>

namespace virgo {
	void * halloc(unsigned n);
	void * hrealloc(void * a, unsigned n);
	void hfree(void * a);
}
