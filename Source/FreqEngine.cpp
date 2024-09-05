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
		mMainBuf[i] = new float[mFftSize];
		mSwapBuf[i] = new float[mFftSize];
		mCrossBuf[i] = new float[mFftCross];
		memset(mMainBuf[i], 0, sizeof(float) * mFftSize);
		memset(mSwapBuf[i], 0, sizeof(float) * mFftSize);
		memset(mCrossBuf[i], 0, sizeof(float) * mFftCross);
		fftArrFreq[i]= new kiss_fft_cpx[mFreqDomainSize];
		// push some data
		for (int j = 0; j < mFftSize; j++) mDstQueue[i].push(0.0f);
	}
}
FreqEngine::~FreqEngine() {

	kiss_fftr_free(mCfgFft);
	kiss_fftr_free(mCfgIfft);
	for (int i = 0; i < kNumChannels; i++)
	{
		delete[] mMainBuf[i];
		delete[] mSwapBuf[i];
		delete[] mCrossBuf[i];
		delete[] fftArrFreq[i];
	}
}

void FreqEngine::process(float* const* data, const int n)
{
	int remain = n;
	int cur = 0;
	while (remain > 0) {
		auto t = mFftSize - mCurPos;
		if (remain < t) t = remain;

		for (int i = 0; i < kNumChannels; i++) {
			
			memcpy(mMainBuf[i]+ mCurPos, data[i]+cur, sizeof(float) * t);
			memcpy(data[i] + cur, mSwapBuf[i]+ mCurPosSwap, sizeof(float) * t);
		}
		mCurPos += t;
		mCurPosSwap += t;
		if (mCurPos == mFftSize) {
			// process, with Hann windowing
			for (int i = 0; i < kNumChannels; i++) {
				memcpy(mCrossBuf[i], mMainBuf[i]+ mFftDelta, sizeof(float) * mFftCross);
				fwHann(mMainBuf[i], mFftSize, mIsFirstDone, true);

			}
			// --- --- ---

			// [pre time pass]
			float tmpAmp[kNumChannels];
			for (int i = 0; i < kNumChannels; i++){
				tmpAmp[i] = 0.0f;
				for (int j = 0; j < mFftSize; j++) {
					tmpAmp[i] += mMainBuf[i][j] * mMainBuf[i][j];
				}
				tmpAmp[i] /= mFftSize;
				tmpAmp[i] = sqrtf(tmpAmp[i]);
			}
			// [end]
			for (int i = 0; i < kNumChannels; i++) {
				kiss_fftr(mCfgFft, mMainBuf[i], fftArrFreq[i]);
				// [custom pass 1]
				procFlatten(fftArrFreq[i]);
				// [end]

			}
			// [mid global pass]
			
			// [end]
			
			// fft rev
			for (int i = 0; i < kNumChannels; i++) {
				kiss_fftri(mCfgIfft, fftArrFreq[i], mMainBuf[i]);
				for (int j = 0; j < mFftSize; j++) {
					mMainBuf[i][j] /= mFftSize; 
				} // kiss fft scale
			}
			// [post time pass]
			for (int i = 0; i < kNumChannels; i++) {

				auto tmpOrig = tmpAmp[i];
				auto tmpNew = 0.0f;
				if (!apprEq0(tmpOrig)) {
					for (int j = 0; j < mFftSize; j++) {
						tmpNew += mMainBuf[i][j] * mMainBuf[i][j];
					}
					tmpNew /= mFftSize;
					tmpNew = sqrtf(tmpNew);
					tmpNew /= tmpOrig;
					if (apprEq0(tmpNew) || std::isnan(tmpNew)) continue;
					if (mParams.agc < 1.) {
						tmpNew = exp2f(log2f(tmpNew) * mParams.agc);
					}
					for (int j = 0; j < mFftSize; j++) {
						mMainBuf[i][j] /= tmpNew;
					}
				}
			}

			// --- --- ---
			for (int i = 0; i < kNumChannels; i++) {
				// copy
				memcpy(mSwapBuf[i], mMainBuf[i], sizeof(float) * mFftDelta);
				for (int j = 0; j < mFftCross; j++) {
					mSwapBuf[i][j] = mSwapBuf[i][j] + mSwapBuf[i][mFftDelta + j];
				}
				memcpy(mSwapBuf[i] + mFftDelta, mMainBuf[i] + mFftDelta, sizeof(float)* mFftCross);
				memcpy(mMainBuf[i], mCrossBuf[i], sizeof(float) * mFftCross);
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

void FreqEngine::setParams(const Params &params)
{
	mParams = params;
}

FreqEngine::Params FreqEngine::getParams()
{
	return mParams;
}

void FreqEngine::procFlatten(kiss_fft_cpx* data)
{
	std::vector<float> dyn;
	dyn.resize(mFreqDomainSize);
	for (int i = 0; i < mFreqDomainSize; i++) {
		dyn[i] = log2f(getComplexMag(data[i].r, data[i].i));
	}
	float strength = mParams.strength;
	float pink = mParams.pink;
	float soften = mParams.soften;
	float strabs = abs(strength);
	// calculate values!
	int ker = mFftSize / 4;
	float sig = 
		4.0f 
		* (1.0f - strabs) +
		0.0f
		* strabs;
	sig += 4. * soften;
	sig = exp2f(sig);
	// avg
	float avg = 0;
	int avd = 0;
	for (int i = 0; i < mFreqDomainSize; i++) {
		if (dyn[i] > -30.0f) {
			avg += dyn[i];
			avd++;
		}
	}
	if (avd > 0) avg /= (float)avd;
	for (int i = 0; i < mFreqDomainSize; i++) {
		if (dyn[i] > -30.0f) {
			dyn[i] = avg - dyn[i];
		}
		else {
			dyn[i] = 0;
		}
		
	}
	gaussian(dyn.data(), mFreqDomainSize, ker, sig);
	for (int i = 0; i < mFreqDomainSize; i++) {
		float tiltVal = (float)mSampleRate * ((float)(i <= 0? 1:i) /(float) mFftSize);
		tiltVal = 1000. / tiltVal;
		tiltVal = log2f(tiltVal) / 2. * pink;
		dyn[i] += tiltVal;
		dyn[i] *= powf(strabs
			, 0.5f) * (strength < 0 ? -1.f : 1.f);
		data[i] = resizeCpx(data[i], exp2f(dyn[i]));
	}
}
