#pragma once

#include <chrono>
#include <memory>

namespace cinder { namespace audio {
using MonitorSpectralNodeRef = std::shared_ptr< class MonitorSpectralNode >;
}} // ci::audio

namespace sw {
namespace audio {

class Calibrator {

	friend class SignalConditioner;

public:

	Calibrator();

	void	connect(cinder::audio::MonitorSpectralNodeRef);
	void	setStartFrquency(float freq = 18000.0f);
	void	setStopFrquency(float freq = 19000.0f);
	void	setSweepDuration(double milliseconds);
	bool	finished() const;
	void	update();
	void	reset();
	float	getSweepFrequency() const;
	float	getToneFrequency() const;
	int		getToneFrequencyIndex() const;

private:
	cinder::audio::MonitorSpectralNodeRef
			mFftNode;
	double	mSweepDuration;
	double	mTimerStart;
	float	mStartFreq,
			mStopFreq,
			mSweepFrequency;
	float	mToneFrequency;
	int		mToneFrequencyIndex;
	float	mPeakAmplitude;
};

}} // sw::audio
