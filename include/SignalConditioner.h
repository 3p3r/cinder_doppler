#pragma once

#include <vector>
#include <memory>

namespace cinder { namespace audio {
using MonitorSpectralNodeRef = std::shared_ptr< class MonitorSpectralNode >;
}} // ci::audio

namespace sw {
namespace audio {

class Calibrator;

class SignalConditioner {
public:

	SignalConditioner(Calibrator&);

	void						connect(cinder::audio::MonitorSpectralNodeRef);
	const std::vector<float>&	getBand() const;
	void						setNumBins(int = 33);
	int							getNumBins() const;
	void						update();

private:
	cinder::audio::MonitorSpectralNodeRef
								mFftNode;
	std::vector<float>			mBand;
	Calibrator&					mCalibrator;
	int							mNumBins;
};

}} // sw::audio
