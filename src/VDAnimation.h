#pragma once

#include "ofMain.h"
#include "VDSettings.h"
#include "VDUniforms.h"

namespace videodromm
{
	typedef std::shared_ptr<class VDAnimation> VDAnimationRef;

	class VDAnimation {
	public:
		VDAnimation(VDSettingsRef aVDSettings, VDUniformsRef aVDUniforms);

		static VDAnimationRef create(VDSettingsRef aVDSettings, VDUniformsRef aVDUniforms) {
			return std::make_shared<VDAnimation>(aVDSettings, aVDUniforms);
		}

		void update();
		void resetAnim();
		void resetUniformAnim(unsigned int anim);

		const int mBlendModes = 28;
		void      blendRenderEnable(bool render) { mBlendRender = render; }

		// tap tempo
		bool  mUseTimeWithTempo;
		void  toggleUseTimeWithTempo() { mUseTimeWithTempo = !mUseTimeWithTempo; }
		void  useTimeWithTempo()       { mUseTimeWithTempo = true; }
		bool  getUseTimeWithTempo()    { return mUseTimeWithTempo; }
		float iTempoTimeBeatPerBar;
		void  tapTempo();

		// animation
		int currentScene;

		// keyboard (OF-style: int key + modifier bools instead of KeyEvent)
		bool handleKeyDown(int key, bool ctrlDown = false, bool shiftDown = false, bool altDown = false);
		bool handleKeyUp(int key);

		// audio texture (32x1 red texture)
		ofTexture& getAudioTexture();
		std::string getAudioTextureName() { return mAudioName; }

		bool mLineInInitialized;
		bool mWaveInitialized;

		// iFreqs can be set externally (e.g., from an audio callback or ofSoundGetSpectrum)
		static const int mFFTWindowSize = 32;
		float            iFreqs[mFFTWindowSize];
		int              mPosition;
		std::string      mAudioName;

		void preventLineInCrash()  { mUseLineIn = false; }
		void saveLineIn()          { mUseLineIn = true; }
		void preventWaveMonitorCrash() { mUseAudio = false; }
		void saveWaveMonitor()     { mUseAudio = true; }
		void initLineIn();

		bool getUseAudio()   { return mUseAudio; }
		bool getUseLineIn()  { return mUseLineIn; }
		void setUseLineIn(bool v)          { mUseLineIn = v; }
		void toggleUseLineIn() {
			mUseLineIn = !mUseLineIn;
			if (!mLineInInitialized && mUseLineIn) initLineIn();
		}
		void setPreferredAudioInputDevice(const std::string& d)  { mPreferredAudioInputDevice  = d; }
		void setPreferredAudioOutputDevice(const std::string& d) { mPreferredAudioOutputDevice = d; }
		void setUseWaveMonitor(bool v)     { mUseAudio = v; }
		bool getUseWaveMonitor()           { return mUseAudio; }
		void toggleUseWaveMonitor()        { mUseAudio = !mUseAudio; }

		bool isAudioBuffered()   { return mAudioBuffered; }
		void toggleAudioBuffered() { mAudioBuffered = !mAudioBuffered; }
		bool getUseRandom()      { return mUseRandom; }
		void toggleUseRandom()   { mUseRandom = !mUseRandom; }

		// uniforms delegation
		int  getUniformTypeByName(const std::string& aName)  { return mVDUniforms->getUniformTypeByName(aName); }
		bool isExistingUniform(const std::string& aName)     { return mVDUniforms->isExistingUniform(aName); }
		int  getUniformIndexForName(const std::string& aName){ return mVDUniforms->getUniformIndexForName(aName); }
		bool toggleValue(unsigned int aIndex);

		void         setAnim(unsigned int aCtrl, unsigned int aAnim) { mVDUniforms->setAnim(aCtrl, aAnim); }
		unsigned int getAnim(unsigned int aCtrl)                     { return mVDUniforms->getAnim(aCtrl); }
		unsigned int getBlendModesCount() { return mBlendModes; }
		bool         renderBlend()        { return mBlendRender; }

		unsigned int getFreqIndexSize()                              { return (unsigned int)freqIndexes.size(); }
		unsigned int getFreqIndex(unsigned int i)                    { return freqIndexes[i]; }
		void         setFreqIndex(unsigned int i, unsigned int freq) { freqIndexes[i] = freq; }

	private:
		VDSettingsRef mVDSettings;
		VDUniformsRef mVDUniforms;

		std::string  mPreferredAudioInputDevice;
		std::string  mPreferredAudioOutputDevice;
		bool         mUseAudio     = true;
		bool         mUseRandom    = false;
		bool         mUseLineIn    = false;
		bool         mAudioBuffered = false;
		bool         mBlendRender  = false;

		std::map<int, int> freqIndexes;

		// Audio texture (32x1 GL_RED)
		ofTexture    mAudioTexture;
		bool         mAudioTextureAllocated = false;

		// JSON helper
		ofJson uniformToJson(int i);

		// Timer – simulated with absolute time + offset
		double mTimerOffset;   // set to ofGetElapsedTimef() on start/reset
		double currentTime;
		double startTime;
		float  previousTime;
		float  previousTimeBeatPerBar;
		float  mLastBar = 0.0f;

		std::deque<double>            buffer;
		void calculateTempo();
		int    counter;
		double averageTime;

		std::unordered_map<int, float> mBadTV;
	};
}
