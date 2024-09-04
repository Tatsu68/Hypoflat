
#pragma once

#include <cmath>
#include <complex>
#include <algorithm>
#include <vector>
#include "../kissfft/kiss_fftr.h"

#define PI 3.14159265359
#define TAU 6.28318530718

inline const bool apprEq0(float x)
{
    return abs(x) < (1.0f / (2 << 30));
}

inline const float smoothstep(const float a, const float b, const float x) {
    const float t = std::clamp((x - a) / (b - a), 0.0f, 1.0f);
    return t * t * (3.0f - 2.0f * t);
}

inline const float clamp01(const float x) {
    float r = x;
    if (r < 0.0f) r = 0.0f;
    if (r > 1.0f) r = 1.0f;
    return r;
}

inline const float getComplexMag(const std::complex<float> comp) {

    float rl = comp.real();
    float im = comp.imag();
    return sqrt(rl * rl + im * im);
}

inline const float getComplexMag(float r, float i) {

    return sqrt(r * r + i * i);
}

inline const float valMapLinToLog(float x, float r = 10.0f) {
    return log2(x * exp2(r) + 1.0f) / r;
}

inline const float valMapLogToLin(float x, float r = 10.0f) {
    return (exp2(r * x) - 1.0f) / (exp2(r) - 1.0f);
}

inline const float smtToScale(float smt) {
    return exp2(smt / 12.0f);
}

inline const kiss_fft_cpx rotate2D(kiss_fft_cpx cpx, kiss_fft_scalar rad) {
    auto x = cpx.r;
    auto y = cpx.i;
    auto cs = cos(rad);
    auto sn = sin(rad);
    return kiss_fft_cpx{x * cs - y * sn, x * sn + y * cs};
}

inline const kiss_fft_cpx remap(float x, kiss_fft_cpx a, kiss_fft_cpx b) {
    kiss_fft_cpx result{ b.r * x + a.r * (1.0f - x), b.r * x + a.r * (1.0f - x) };
    return result;
}

inline const bool isAllZero(float* data, int count) {
    for (int i = 0; i < count; i++) {
        if (!apprEq0(data[i]))
            return false;
    }
    return true;
}
inline const bool isAllZero(kiss_fft_cpx* data, int count) {
    for (int i = 0; i < count; i++) {
        if (!(apprEq0(data[i].r) && apprEq0(data[i].i)))
            return false;
    }
    return true;
}

inline const kiss_fft_cpx resizeCpx(const kiss_fft_cpx& x, float m) {
    kiss_fft_cpx c = x;
    float mag = getComplexMag(c.r, c.i);

    if (!apprEq0(mag)) {
        c.r *= m;
        c.i *= m;
    }
    return c;
}


//Gaussian 
inline std::vector <float> generateGaussianKernel(int kernelSize, float sigma) {
    std::vector< float> kernel(kernelSize);
    float sum = 0.0;
    int halfSize = kernelSize / 2;

    for (int i = 0; i < kernelSize; ++i) {
        int x = i - halfSize;
        kernel[i] = std::exp(-0.5 * (x * x) / (sigma * sigma)) / (sigma * std::sqrt(2 * PI));
        sum += kernel[i];
    }

    for (int i = 0; i < kernelSize; ++i) {
        kernel[i] /= sum;
    }

    return kernel;
}

inline void gaussian(float* data, const int dataSize, const int kernelSize, const float sigma) {
    auto kernel = generateGaussianKernel(kernelSize, sigma);
    int halfSize = kernelSize / 2;
    std::vector<float> result(dataSize, 0.0f);

    for (int i = 0; i < dataSize; ++i) {
        float sum = 0.0f;
        for (int j = 0; j < kernelSize; ++j) {
            int index = i + j - halfSize;
            if (index >= 0 && index < dataSize) {
                sum += data[index] * kernel[j];
            }
        }
        result[i] = sum;
    }
    memcpy(data, result.data(), sizeof(float) * dataSize);

}
