#pragma once
#include <vector>
#include <queue>
#include <iostream>
#include "../kissfft/kiss_fftr.h"
#include "MathUtils.h"
#include "FftWindow.h"

class FreqEngine
{
public:
	struct Params {
		float strength = 0.0f;
	};
	FreqEngine(const int fftSize, const double sampleRate);
	~FreqEngine();
	void process(float* const* data, const int n);

	// [custom]
	void setParams(const Params params);
	Params getParams();

private:
	static const int kNumChannels = 2;
	double mSampleRate = 0;
	int mFftSize, mFftCross, mFftDelta, mFreqDomainSize;
	bool mIsFirstDone = false;

	float* mMainBuf[kNumChannels];
	float* mSwapBuf[kNumChannels];
	float* mCrossBuf[kNumChannels];
	std::queue<float> mDstQueue[kNumChannels];
	int mCurPos = 0, mCurPosSwap = 0;

	//fft
	kiss_fftr_cfg mCfgFft, mCfgIfft;
	float* fftArrTime;
	kiss_fft_cpx* fftArrFreq[kNumChannels];


	// [custom]
	Params mParams;
	void procFlatten(kiss_fft_cpx* data);
};

