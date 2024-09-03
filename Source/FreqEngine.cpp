#include "FreqEngine.h"

FreqEngine::FreqEngine(const int fftSize, const double sampleRate)
{
	mSampleRate = sampleRate;
	mFftSize = fftSize;
	mFftCross = fftSize / 2;
	mFftDelta = fftSize - mFftCross;
	mFreqDomainSize = fftSize / 2 + 1;
	mCfgFft = kiss_fftr_alloc(mFftSize, 0, NULL, NULL);
	mCfgIfft = kiss_fftr_alloc(mFftSize, 1, NULL, NULL);
	// init bufs
	for (int i = 0; i < kNumChannels; i++)
	{
		mMainBuf[i].resize(mFftSize);
		mSwapBuf[i].resize(mFftSize);
		memset(mMainBuf[i].data(), 0, sizeof(float) * mFftSize);
		memset(mSwapBuf[i].data(), 0, sizeof(float) * mFftSize);
		fftArrFreq[i].resize(mFftSize);
	}
}
FreqEngine::~FreqEngine() {

	kiss_fftr_free(mCfgFft);
	kiss_fftr_free(mCfgIfft);
}

void FreqEngine::process(float* const* data, const int n)
{
	int remain = n;
	int cur = 0;
	while (remain > 0) {
		auto t = mFftSize - mCurPos;
		if (remain < t) t = remain;

		for (int i = 0; i < kNumChannels; i++) {
			memcpy(mMainBuf[i].data()+ mCurPos, data[i]+cur, sizeof(float) * t);
			memcpy(data[i] + cur, mSwapBuf[i].data()+ mCurPosSwap, sizeof(float) * t);
		}
		mCurPos += t;
		mCurPosSwap += t;
		if (mCurPos == mFftSize) {
			// process, with Hann windowing
			for (int i = 0; i < kNumChannels; i++) {
				fwHann(mMainBuf[i].data(), mFftSize, mIsFirstDone, true);
				//kiss_fftr(mCfgFft, mMainBuf[i].data(), fftArrFreq[i].data());

			}
			for (int i = 0; i < kNumChannels; i++) {
				// [custom behaviour]

				//procFlatten(fftArrFreq[i].data());

				// [custom end]
			}
			for (int i = 0; i < kNumChannels; i++) {
				//kiss_fftri(mCfgIfft, fftArrFreq[i].data(), mMainBuf[i].data());
				for (int j = 0; j < mFftSize; j++) mMainBuf[i][j] *= mFftSize; // kiss fft scale

				// copy
				memcpy(mSwapBuf[i].data(), mMainBuf[i].data(), sizeof(float) * mFftDelta);
				for (int j = 0; j < mFftCross; j++) {
					mSwapBuf[i][j] += mSwapBuf[i][mFftDelta + j];

				}

			}
			mIsFirstDone = true;
			mCurPos = mFftCross;
			mCurPosSwap = 0;
		}
		
		cur += t;
		remain -= t;
	}
}


// [custom]

void FreqEngine::setStrength(float s)
{
	mStrength = s;
}

void FreqEngine::procFlatten(kiss_fft_cpx* data)
{
	std::vector<float> dyn;
	dyn.resize(mFreqDomainSize);
	for (int i = 0; i < mFreqDomainSize; i++) {
		dyn[i] = exp2f(getComplexMag(data[i].r, data[i].i));
	}

	// calculate values!
	int ker = 16;
	float sig = 
		4.0f 
		* (1.0f - mStrength) + 
		1.0f
		* mStrength;

	// avg
	float avg = 0;
	int avd = 0;
	for (int i = 0; i < mFreqDomainSize; i++) {
		if (!(dyn[i] > -30.0f)) {
			avg += dyn[i];
			avd++;
		}
	}
	if (avd > 0) avg /= (float)avd;
	for (int i = 0; i < mFreqDomainSize; i++) {
		if (dyn[i] > -30.0f) {
			dyn[i] = avg;
		}
	}
	gaussian(dyn.data(), mFreqDomainSize, ker, sig);
	for (int i = 0; i < mFreqDomainSize; i++) {
		1.0;
	}
}
