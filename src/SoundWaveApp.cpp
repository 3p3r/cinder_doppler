#include <cinder/app/AppNative.h>
#include <cinder/gl/gl.h>

#include <cinder/audio/Context.h>
#include <cinder/audio/GenNode.h>
#include <cinder/audio/MonitorNode.h>

#include "BandwidthExtractor.h"
#include "SignalConditioner.h"
#include "Calibrator.h"

class SoundWaveApp : public ci::app::AppNative
{
public:

	SoundWaveApp();

	void								setup();
	void								update();
	void								draw();
	void								keyDown(ci::app::KeyEvent event);

private:
	ci::audio::MonitorSpectralNodeRef	mMonitorSpectralNode;
	ci::audio::InputDeviceNodeRef		mInputDeviceNode;
	ci::audio::GenNodeRef				mGenNode;
	ci::Font							mFont;

	sw::audio::Calibrator				mCalibrator;
	sw::audio::SignalConditioner		mConditioner;
	sw::audio::BandwidthExtractor		mExtractor;
};

SoundWaveApp::SoundWaveApp()
	: mConditioner(mCalibrator)
	, mExtractor(mConditioner)
{}

void SoundWaveApp::setup()
{
	mFont = ci::Font("Arial", 36.0f);

	auto ctx = ci::audio::master();

	ci::audio::GenSineNode::Format gen_format;
	gen_format.setAutoEnable(true);

	mGenNode = ctx->makeNode(new ci::audio::GenSineNode(gen_format));
	mGenNode >> ctx->getOutput();

	auto monitor_format = ci::audio::MonitorSpectralNode::Format()
		.fftSize(2048)
		.windowSize(1024);
	mMonitorSpectralNode = ctx->makeNode(new ci::audio::MonitorSpectralNode(monitor_format));

	mInputDeviceNode = ctx->createInputDeviceNode();
	mInputDeviceNode >> mMonitorSpectralNode;
	mInputDeviceNode->enable();

	ctx->enable();

	mCalibrator.connect(mMonitorSpectralNode);
	mCalibrator.setStartFrquency(18000.0f);
	mCalibrator.setStopFrquency(21000.0f);
	mCalibrator.setSweepDuration(500.0);

	mConditioner.connect(mMonitorSpectralNode);
	mConditioner.setNumBins(33);

	mExtractor.setMinimumThreshold(0.1f);
}

void SoundWaveApp::update()
{
	mCalibrator.update();
	mConditioner.update();
	mExtractor.update();

	mGenNode->setFreq(mCalibrator.getToneFrequency());
}

void SoundWaveApp::draw()
{
	ci::gl::clear(ci::Color(0.2f, 0.2f, 0.2f));
	ci::gl::enableAlphaBlending();

	auto left_diff = ci::math<float>::abs(mExtractor.getLeftBandwidth() - mExtractor.getLeftBandwidthAvg());
	auto right_diff = ci::math<float>::abs(mExtractor.getRightBandwidth() - mExtractor.getRightBandwidthAvg());

	if (left_diff < FLT_EPSILON) left_diff = 0;
	if (right_diff < FLT_EPSILON) right_diff = 0;

	auto left_percentage = left_diff / mExtractor.getLeftBandwidth();
	auto right_percentage = right_diff / mExtractor.getLeftBandwidth();

	left_percentage = ci::constrain(left_percentage, 0.0f, 1.0f);
	right_percentage = ci::constrain(right_percentage, 0.0f, 1.0f);

	static auto left_percentage_last_frame = left_percentage;
	static auto right_percentage_last_frame = right_percentage;

	if (ci::math<float>::abs(left_percentage_last_frame - left_percentage) > 0.2f)
		left_percentage = left_percentage_last_frame;
	if (ci::math<float>::abs(right_percentage_last_frame - right_percentage) > 0.2f)
		right_percentage = right_percentage_last_frame;

	auto margin = 40.0f;
	auto full_height = ci::app::getWindowHeight() - 3*margin;
	auto height = full_height / 2.0f;
	auto full_width = ci::app::getWindowWidth() - 2*margin;

	ci::gl::color(ci::Color::white());
	ci::gl::drawStrokedRect(ci::Rectf(margin, margin, full_width + margin, margin + height));
	ci::gl::drawSolidRect(ci::Rectf(margin, margin, full_width * left_percentage + margin, margin + height));

	ci::gl::color(ci::Color::white());
	ci::gl::drawStrokedRect(ci::Rectf(margin, 2 * margin + height, full_width + margin, 2 * (margin + height)));
	ci::gl::drawSolidRect(ci::Rectf(margin, 2 * margin + height, full_width * right_percentage + margin, 2 * (margin + height)));

	ci::gl::drawStringCentered("OUTWARD", ci::Vec2i(ci::app::getWindowWidth() / 2, static_cast<int>(margin + height / 2)), ci::Color::black(), mFont);
	ci::gl::drawStringCentered("INWARD", ci::Vec2i(ci::app::getWindowWidth() / 2, static_cast<int>(2 * margin + 3 * height / 2)), ci::Color::black(), mFont);
}

void SoundWaveApp::keyDown(ci::app::KeyEvent event)
{
	if (event.getCode() == event.KEY_f)
		setFullScreen(!isFullScreen());
	else if (event.getCode() == event.KEY_ESCAPE)
		quit();
	else
		ci::app::App::keyDown(event);
}

CINDER_APP_NATIVE(SoundWaveApp, ci::app::RendererGl)
