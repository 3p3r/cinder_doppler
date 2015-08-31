#include "BandwidthExtractor.h"
#include "SignalConditioner.h"

namespace sw {
namespace audio {

BandwidthExtractor::BandwidthExtractor(SignalConditioner& conditioner)
	: mConditioner(conditioner)
	, mLeftBandwidth(0)
	, mRightBandwidth(0)
	, mMinimumThreshold(0)
	, mRightBandwidthAvg(0)
	, mLeftBandwidthAvg(0)
{}

float BandwidthExtractor::getLeftBandwidth() const
{
	return mLeftBandwidth;
}

float BandwidthExtractor::getRightBandwidth() const
{
	return mRightBandwidth;
}

float BandwidthExtractor::getLeftBandwidthAvg() const
{
	return mLeftBandwidthAvg;
}

float BandwidthExtractor::getRightBandwidthAvg() const
{
	return mRightBandwidthAvg;
}

void BandwidthExtractor::update()
{
	auto& band			= mConditioner.getBand();
	auto num_bins		= mConditioner.getNumBins();
	auto expected_bins	= 2 * num_bins + 1;

	if (expected_bins != band.size()) return;
	
	auto tone_vol	= band[num_bins];

	auto left_band_last_frame = mLeftBandwidth;
	auto right_band_last_frame = mRightBandwidth;

	mLeftBandwidth = 0;
	mRightBandwidth = 0;

	for (	auto index = num_bins - 1;
			index >= 0;
			--index)
	{
		auto vol_ratio = band[index] / tone_vol;
		if (vol_ratio > mMinimumThreshold)
			mLeftBandwidth = static_cast<float>(index);
	}

	for (	auto index = num_bins + 1;
			index < expected_bins;
			++index)
	{
		auto vol_ratio = band[index] / tone_vol;
		if (vol_ratio > mMinimumThreshold)
			mRightBandwidth = static_cast<float>(index);
	}

	mLeftBandwidthAvg = mLeftBandwidthAvg * 0.9f + left_band_last_frame * 0.1f;
	mRightBandwidthAvg = mRightBandwidthAvg * 0.9f + right_band_last_frame * 0.1f;
}

void BandwidthExtractor::setMinimumThreshold(float n /*= 0.1f*/)
{
	if (mMinimumThreshold == n) return;
	mMinimumThreshold = n;
}

}} // sw::audio
