#pragma once

namespace sw {
namespace audio {

class SignalConditioner;

class BandwidthExtractor {
public:

	BandwidthExtractor(SignalConditioner&);

	float				getLeftBandwidth() const;
	float				getRightBandwidth() const;
	float				getLeftBandwidthAvg() const;
	float				getRightBandwidthAvg() const;
	void				update();
	void				setMinimumThreshold(float = 0.1f);

private:
	SignalConditioner&	mConditioner;
	float				mMinimumThreshold;
	float				mLeftBandwidth,
						mRightBandwidth;
	float				mLeftBandwidthAvg,
						mRightBandwidthAvg;
};

}} // sw::audio
