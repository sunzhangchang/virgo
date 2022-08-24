#pragma once

namespace virgo {
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

#ifdef STD
#include <vector>
    using std::vector;

    template<class T, class F>
    std::vector<T> filter(std::vector<T> v, F && func) {
        v.resize(copy_if(v.begin(), v.end(), v.begin(), func) - v.begin());
        v.shrink_to_fit();
        return v;
    }

#endif

    template<class _Iter, class T>
    _Iter find(_Iter bg, _Iter ed, T v) {
        while (bg != ed) {
            if (*bg == v) {
                return bg;
            }
            ++ bg;
        }
        return bg;
    }

#ifndef STD
#define assert(exp) (exp ? void() : ExitProcess(1))
#else
#include <cassert>
#endif
}
