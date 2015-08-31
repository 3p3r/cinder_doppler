#include "Calibrator.h"

#include <cinder/audio/MonitorNode.h>
#include <cinder/audio/Utilities.h>
#include <cinder/CinderMath.h>
#include <cinder/app/App.h>

namespace sw {
namespace audio {

Calibrator::Calibrator()
	: mSweepDuration(0)
	, mStartFreq(0)
	, mStopFreq(0)
	, mSweepFrequency(0)
	, mToneFrequency(0)
	, mTimerStart(0)
	, mPeakAmplitude(0)
	, mToneFrequencyIndex(0)
{}

void Calibrator::connect(cinder::audio::MonitorSpectralNodeRef node)
{
	mFftNode = node;
}

void Calibrator::setStartFrquency(float freq /*= 18000.0f*/)
{
	if (freq <= mStopFreq || mStartFreq == 0)
		mStartFreq = freq;
}

void Calibrator::setStopFrquency(float freq /*= 19000.0f*/)
{
	if (freq >= mStartFreq || mStopFreq == 0)
		mStopFreq = freq;
}

void Calibrator::setSweepDuration(double milliseconds)
{
	if (mSweepDuration == milliseconds) return;
	mSweepDuration = milliseconds;
}

bool Calibrator::finished() const
{
	return mSweepFrequency >= mStopFreq;
}

void Calibrator::update()
{
	if (finished() && mPeakAmplitude == 0) reset();
	if (finished() || !mFftNode) return;

	auto& mag_spectrum = mFftNode->getMagSpectrum();
	if (mag_spectrum.empty()) return;

	auto time_ms = ci::app::getElapsedSeconds() * 1000.0;
	auto time_factor = ci::constrain(static_cast<float>((time_ms - mTimerStart) / mSweepDuration), 0.0f, 1.0f);

	mSweepFrequency = ci::lerp(mStartFreq, mStopFreq, time_factor);

	auto nyquist_rate = mFftNode->getSampleRate() * 0.5f;
	auto frequency_index = static_cast<size_t>((mSweepFrequency / nyquist_rate) * mFftNode->getFftSize() * 0.5f);
	auto db = ci::audio::linearToDecibel(mag_spectrum[frequency_index]);

	if (db >= mPeakAmplitude) {
		mPeakAmplitude = db;
		mToneFrequency = mSweepFrequency;
		mToneFrequencyIndex = frequency_index;
	}
}

void Calibrator::reset()
{
	mSweepFrequency = mStartFreq;
	mTimerStart = ci::app::getElapsedSeconds() * 1000.0;
}

float Calibrator::getSweepFrequency() const
{
	return mSweepFrequency;
}

float Calibrator::getToneFrequency() const
{
	return mToneFrequency;
}

int Calibrator::getToneFrequencyIndex() const
{
	return mToneFrequencyIndex;
}

}} // !sw::audio
