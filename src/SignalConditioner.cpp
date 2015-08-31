#include "SignalConditioner.h"
#include "Calibrator.h"

#include <cinder/audio/MonitorNode.h>
#include <cinder/audio/Utilities.h>

namespace sw {
namespace audio {

SignalConditioner::SignalConditioner(Calibrator& calib)
	: mCalibrator(calib)
	, mNumBins(0)
{}

void SignalConditioner::connect(cinder::audio::MonitorSpectralNodeRef node)
{
	mFftNode = node;
}

void SignalConditioner::setNumBins(int n /*= 33*/)
{
	if (mNumBins == n) return;
	mNumBins = n;
}

int SignalConditioner::getNumBins() const
{
	return mNumBins;
}

void SignalConditioner::update()
{
	if (!mCalibrator.finished()) return;

	auto& mag_spectrum = mCalibrator.mFftNode->getMagSpectrum();
	if (mag_spectrum.empty()) return;

	mBand.clear();

	for (	int offset = -mNumBins, index = 0;
			offset <= mNumBins;
			++offset)
	{
		index = mCalibrator.getToneFrequencyIndex() + offset;
		if (index < 0 || index >= static_cast<int>(mag_spectrum.size())) continue;
		mBand.push_back(ci::audio::linearToDecibel(mag_spectrum[index]));
	}
}

const std::vector<float>& SignalConditioner::getBand() const
{
	return mBand;
}

}} // sw::audio
