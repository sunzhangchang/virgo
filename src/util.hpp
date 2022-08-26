_Pragma("once")

namespace virgo {
    template<class _Iter, class _N_Iter, class F>
    _Iter copy_if(_Iter st, _Iter ed, _N_Iter n_st, F && func) {
        while (st != ed) {
            if (func(*st)) {
                *n_st = *st;
                ++ n_st;
            }
            ++ st;
        }
        return n_st;
    }

#define assert(exp) (exp ? void() : ExitProcess(1))

    template<class T>
    void swap(T & a, T & b) {
        auto c = a;
        a = b;
        b = c;
    }
}
