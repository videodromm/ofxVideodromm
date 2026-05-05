#include "VDAnimation.h"

using namespace videodromm;

VDAnimation::VDAnimation(VDSettingsRef aVDSettings, VDUniformsRef aVDUniforms) {
	mVDSettings = aVDSettings;
	mVDUniforms = aVDUniforms;
	mBlendRender    = false;
	mAudioBuffered  = false;
	mUseAudio       = true;
	mUseLineIn      = false;
	mLineInInitialized = false;
	mWaveInitialized   = false;
	mAudioName = "not initialized";

	for (int i = 0; i < 7; i++)   freqIndexes[i] = i * 7;
	for (int i = 0; i < mFFTWindowSize; i++) iFreqs[i] = 0.0f;

	currentScene = 0;
	previousTime = 0.0f;
	counter = 0;
	averageTime = 0.0;
	mUseTimeWithTempo = false;

	// Timer start
	mTimerOffset = ofGetElapsedTimef();
	startTime = currentTime = 0.0;

	mVDUniforms->setVec3UniformValueByIndex(mVDUniforms->IRESOLUTION,
		glm::vec3(mVDUniforms->getUniformValue(mVDUniforms->IRESOLUTIONX),
		          mVDUniforms->getUniformValue(mVDUniforms->IRESOLUTIONY), 1.0f));
}

ofJson VDAnimation::uniformToJson(int i) {
	ofJson u;
	int uniformType = mVDUniforms->getUniformType(i);
	u["type"]  = uniformType;
	u["name"]  = mVDUniforms->getUniformName(i);
	u["index"] = i;
	switch (uniformType) {
	case 0: // float
		u["value"] = mVDUniforms->getDefaultUniformValue(i);
		u["min"]   = mVDUniforms->getMinUniformValue(i);
		u["max"]   = mVDUniforms->getMaxUniformValue(i);
		break;
	case 1: // sampler2d
		u["textureindex"] = mVDUniforms->getUniformTextureIndex(i);
		break;
	default:
		u["value"] = mVDUniforms->getDefaultUniformValue(i);
		break;
	}
	ofJson json;
	json.push_back(u);
	return json;
}

bool VDAnimation::handleKeyDown(int key, bool ctrlDown, bool shiftDown, bool altDown) {
	return false;
}

bool VDAnimation::handleKeyUp(int key) {
	return false;
}

void VDAnimation::initLineIn() {
	ofLogWarning("VDAnimation") << "initLineIn: ofSoundStream line-in not implemented; set iFreqs[] externally.";
	mLineInInitialized = false;
}

ofTexture& VDAnimation::getAudioTexture() {
	unsigned char signal[mFFTWindowSize];

	mVDUniforms->setUniformValue(mVDUniforms->IMAXVOLUME, 0.0f);

	for (size_t i = 0; i < mFFTWindowSize; i++) {
		float f = iFreqs[i];
		if (f > mVDUniforms->getUniformValue(mVDUniforms->IMAXVOLUME)) {
			mVDUniforms->setUniformValue(mVDUniforms->IMAXVOLUME, f);
		}
		if (i == getFreqIndex(0)) mVDUniforms->setUniformValue(mVDUniforms->IFREQ0, f);
		if (i == getFreqIndex(1)) mVDUniforms->setUniformValue(mVDUniforms->IFREQ1, f);
		if (i == getFreqIndex(2)) mVDUniforms->setUniformValue(mVDUniforms->IFREQ2, f);
		if (i == getFreqIndex(3)) mVDUniforms->setUniformValue(mVDUniforms->IFREQ3, f);
		signal[i] = static_cast<unsigned char>((int)f);
	}

	if (!mAudioTextureAllocated) {
		mAudioTexture.allocate(mFFTWindowSize, 1, GL_R8);
		mAudioTextureAllocated = true;
		mAudioName = "audio";
	}
	mAudioTexture.loadData(signal, mFFTWindowSize, 1, GL_RED);
	return mAudioTexture;
}

void VDAnimation::resetAnim() {
	for (unsigned int anim = 1; anim < 29; anim++) {
		mVDUniforms->setAnim(anim, mVDSettings->ANIM_NONE);
		mVDUniforms->setUniformValue(anim, mVDUniforms->getDefaultUniformValue(anim));
	}
}

void VDAnimation::resetUniformAnim(unsigned int anim) {
	mVDUniforms->setUniformValue(anim, mVDUniforms->getDefaultUniformValue(anim));
	mVDUniforms->setAnim(anim, mVDSettings->ANIM_NONE);
}

bool VDAnimation::toggleValue(unsigned int aIndex) {
	bool rtn = mVDUniforms->getUniformValue(aIndex) != 0.0f;
	rtn = !rtn;
	mVDUniforms->setUniformValue(aIndex, rtn ? 1.0f : 0.0f);
	return rtn;
}

void VDAnimation::update() {
	// update channel times
	float elapsed = ofGetElapsedTimef();
	mVDSettings->iChannelTime[0] = elapsed;
	mVDSettings->iChannelTime[1] = elapsed - 1.0f;
	mVDSettings->iChannelTime[2] = elapsed - 2.0f;
	mVDSettings->iChannelTime[3] = elapsed - 3.0f;

	// iTime
	mVDUniforms->setUniformValue(mVDUniforms->ITIME,
		(elapsed - mVDUniforms->getUniformValue(mVDUniforms->ISTART))
		* mVDUniforms->getUniformValue(mVDUniforms->ISPEED)
		* mVDUniforms->getUniformValue(mVDUniforms->ITIMEFACTOR));

	float current = mVDUniforms->getUniformValue(mVDUniforms->IBARBEAT);
	if (current == 426.0f || current == 428.0f || current == 442.0f) mLastBar = 0.0f;
	if (mLastBar != mVDUniforms->getUniformValue(mVDUniforms->IBAR)) {
		mLastBar = mVDUniforms->getUniformValue(mVDUniforms->IBAR);
		if (mLastBar < 419.0f && mLastBar > 424.0f) mVDSettings->iStart = mVDUniforms->getUniformValue(mVDUniforms->ITIME);
	}

	// resolution vectors
	mVDUniforms->setVec3UniformValueByIndex(mVDUniforms->IRESOLUTION,
		glm::vec3(mVDUniforms->getUniformValue(mVDUniforms->IRESOLUTIONX),
		          mVDUniforms->getUniformValue(mVDUniforms->IRESOLUTIONY), 1.0f));
	mVDUniforms->setVec2UniformValueByIndex(mVDUniforms->RESOLUTION,
		glm::vec2(mVDUniforms->getUniformValue(mVDUniforms->IRESOLUTIONX),
		          mVDUniforms->getUniformValue(mVDUniforms->IRESOLUTIONY)));
	mVDUniforms->setVec2UniformValueByIndex(mVDUniforms->RENDERSIZE,
		glm::vec2(mVDUniforms->getUniformValue(mVDUniforms->IRESOLUTIONX),
		          mVDUniforms->getUniformValue(mVDUniforms->IRESOLUTIONY)));
	mVDUniforms->setVec2UniformValueByIndex(mVDUniforms->IRENDERXY,
		glm::vec2(mVDUniforms->getUniformValue(mVDUniforms->IRENDERXYX),
		          mVDUniforms->getUniformValue(mVDUniforms->IRENDERXYY)));

	// iDate
	time_t now = time(nullptr);
	struct tm* t = gmtime(&now);
	mVDUniforms->setVec4UniformValueByIndex(mVDUniforms->IDATE,
		glm::vec4(float(t->tm_year + 1900), float(t->tm_mon + 1), float(t->tm_mday),
		          float(t->tm_hour * 3600 + t->tm_min * 60 + t->tm_sec)));
	mVDUniforms->setUniformValue(mVDUniforms->IDATEX, float(t->tm_hour * 3600 + t->tm_min * 60 + t->tm_sec));
	mVDUniforms->setUniformValue(mVDUniforms->IDATEY, float(t->tm_hour + 2));
	mVDUniforms->setUniformValue(mVDUniforms->IDATEZ, float(t->tm_sec));
	mVDUniforms->setUniformValue(mVDUniforms->IDATEW, float((t->tm_hour + 2) * 3600 + t->tm_min * 60 + t->tm_sec));

	// tempo animation
	currentTime = ofGetElapsedTimef() - mTimerOffset;
	int timeUS   = (int)((currentTime - startTime) * 1000000.0);
	int elapsedUS = (int)(mVDUniforms->getUniformValue(mVDUniforms->IDELTATIME) * 1000000.0);

	if (elapsedUS > 0) {
		double modulo = (timeUS % elapsedUS) / 1000000.0;
		mVDUniforms->setUniformValue(mVDUniforms->ITEMPOTIME, (float)std::abs(modulo));
		previousTime = mVDUniforms->getUniformValue(mVDUniforms->ITEMPOTIME);

		float targetValue = 1.0f;
		for (unsigned int anim = 1; anim < 29; anim++) {
			switch (mVDUniforms->getUniformAnim(anim)) {
			case 1: // ANIM_TIME
				mVDUniforms->setUniformValue(anim, (modulo < 0.1f)
					? mVDUniforms->getMaxUniformValue(anim)
					: mVDUniforms->getMinUniformValue(anim));
				break;
			case 2: // ANIM_AUTO
				mVDUniforms->setUniformValue(anim,
					ofMap((float)mVDUniforms->getUniformValue(mVDUniforms->ITEMPOTIME),
					      0.00001f, mVDUniforms->getUniformValue(mVDUniforms->IDELTATIME),
					      mVDUniforms->getMinUniformValue(anim), mVDUniforms->getMaxUniformValue(anim)));
				break;
			case 3: // ANIM_BASS
				mVDUniforms->setUniformValue(anim,
					(mVDUniforms->getDefaultUniformValue(anim) + 0.01f) * mVDUniforms->getUniformValue(mVDUniforms->IFREQ0) / 25.0f);
				break;
			case 4: // ANIM_MID
				mVDUniforms->setUniformValue(anim,
					(mVDUniforms->getDefaultUniformValue(anim) + 0.01f) * mVDUniforms->getUniformValue(mVDUniforms->IFREQ1) / 5.0f);
				break;
			case 5: // ANIM_SMOOTH
				targetValue = mVDUniforms->getTargetUniformValue(anim);
				if (std::abs(targetValue - mVDUniforms->getUniformValue(anim)) <= 0.006f) {
					resetUniformAnim(anim);
				} else {
					float smooth = mVDUniforms->getUniformValue(mVDUniforms->ISMOOTH) / 27.0f;
					if (mVDUniforms->getUniformValue(anim) > targetValue)
						mVDUniforms->setUniformValue(anim, mVDUniforms->getUniformValue(anim) - smooth);
					else
						mVDUniforms->setUniformValue(anim, mVDUniforms->getUniformValue(anim) + smooth);
				}
				break;
			default:
				break;
			}
		}

		// foreground color
		mVDUniforms->setVec3UniformValueByIndex(mVDUniforms->ICOLOR,
			glm::vec3(mVDUniforms->getUniformValue(mVDUniforms->ICOLORX),
			          mVDUniforms->getUniformValue(mVDUniforms->ICOLORY),
			          mVDUniforms->getUniformValue(mVDUniforms->ICOLORZ)));
		// background color
		mVDUniforms->setVec3UniformValueByIndex(mVDUniforms->IBACKGROUNDCOLOR,
			glm::vec3(mVDUniforms->getUniformValue(mVDUniforms->IBACKGROUNDCOLORX),
			          mVDUniforms->getUniformValue(mVDUniforms->IBACKGROUNDCOLORY),
			          mVDUniforms->getUniformValue(mVDUniforms->IBACKGROUNDCOLORZ)));
		// mouse vec4
		mVDUniforms->setVec4UniformValueByIndex(mVDUniforms->IMOUSE,
			glm::vec4(mVDUniforms->getUniformValue(mVDUniforms->IMOUSEX),
			          mVDUniforms->getUniformValue(mVDUniforms->IMOUSEY),
			          mVDUniforms->getUniformValue(mVDUniforms->IMOUSEZ),
			          mVDUniforms->getUniformValue(mVDUniforms->IMOUSEW)));

		if (mVDSettings->autoInvert) {
			mVDUniforms->setUniformValue(mVDUniforms->IINVERT, (modulo < 0.1) ? 1.0f : 0.0f);
		}
	}
}

void VDAnimation::tapTempo() {
	currentTime = ofGetElapsedTimef() - mTimerOffset;
	startTime = currentTime;
	// reset timer
	mTimerOffset = ofGetElapsedTimef();
	currentTime = 0.0;

	if (counter > 2 && (buffer.back() * 1.5 < currentTime || buffer.back() * 0.5 > currentTime)) {
		buffer.clear();
		counter = 0;
		averageTime = 0.0;
	}
	if (counter >= 1) {
		buffer.push_back(startTime); // push inter-tap interval (startTime before reset)
		calculateTempo();
	}
	counter++;
}

void VDAnimation::calculateTempo() {
	double tAverage = 0;
	for (size_t i = 0; i < buffer.size(); i++) tAverage += buffer[i];
	averageTime = tAverage / buffer.size();
	mVDUniforms->setUniformValue(mVDUniforms->IDELTATIME, (float)averageTime);
	mVDUniforms->setUniformValue(mVDUniforms->IBPM, (float)(60.0 / averageTime));
}
