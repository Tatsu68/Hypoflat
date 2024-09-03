#pragma once

#include "MathUtils.h"
inline void fwHann(float* buf, const int n) {
	for (int i = 0; i < n; i++) {
		auto ss = sinf(PI * ((float)i / (float)n));
		buf[i] *= ss * ss;
	}
}

inline void fwHann(float* buf, const int n, const bool fin, const bool fout) {
    int a =n/2, b= n/2;
    if (fin) a = 0;
    if (fout) b = n;
	for (int i = a; i < b; i++) {
		auto ss = sinf(PI * ((float)i / (float)n));
		buf[i] *= ss*ss;
	}
}