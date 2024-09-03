#pragma once
#include <vector>
#include "../kissfft/kiss_fftr.h"
#include "MathUtils.h"
#include "FftWindow.h"

class FreqEngine
{
public:
	FreqEngine(const int fftSize, const double sampleRate);
	~FreqEngine();
	void process(float* const* data, const int n);

	// [custom]
	void setStrength(float s);

private:
	static const int kNumChannels = 2;
	double mSampleRate = 0;
	int mFftSize, mFftCross, mFftDelta, mFreqDomainSize;
	bool mIsFirstDone = false;

	std::vector<float> mMainBuf[kNumChannels];
	std::vector<float> mSwapBuf[kNumChannels];
	int mCurPos = 0, mCurPosSwap = 0;

	//fft
	kiss_fftr_cfg mCfgFft, mCfgIfft;
	float* fftArrTime;
	std::vector<kiss_fft_cpx> fftArrFreq[kNumChannels];


	// [custom]
	float mStrength = 0;
	void procFlatten(kiss_fft_cpx* data);
};

