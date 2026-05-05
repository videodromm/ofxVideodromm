#pragma once
#include "ofMain.h"
#include "VDSettings.h"
#include "VDAnimation.h"
#include "VDUniforms.h"
#include "VDFboShader.h"
#include "VDParams.h"

#if defined(__APPLE__)
#include "ofxSyphon.h"
#endif

namespace videodromm
{
	typedef std::shared_ptr<class VDMix> VDMixRef;

	class VDMix {
	public:
		VDMix(VDSettingsRef aVDSettings, VDAnimationRef aVDAnimation, VDUniformsRef aVDUniforms);
		static VDMixRef create(VDSettingsRef aVDSettings, VDAnimationRef aVDAnimation, VDUniformsRef aVDUniforms) {
			return std::make_shared<VDMix>(aVDSettings, aVDAnimation, aVDUniforms);
		}

		unsigned int getFboShaderListSize()           { return (unsigned int)mFboShaderList.size(); }
		bool isFboValid(unsigned int aFboIndex) {
			if (mFboShaderList.empty()) return false;
			return mFboShaderList[getValidFboIndex(aFboIndex)]->isValid();
		}
		std::string getFboMsg(unsigned int i)    { return mFboShaderList[getValidFboIndex(i)]->getFboMsg(); }
		std::string getFboError(unsigned int i)  { return mFboShaderList[getValidFboIndex(i)]->getFboError(); }
		std::string getFboStatus(unsigned int i) { return mFboShaderList[getValidFboIndex(i)]->getFboStatus(); }
		bool isValidInputTexture(unsigned int t = 0) {
			return mFboShaderList[getValidFboIndex(mSelectedFbo)]->isValidInputTexture(t);
		}
		unsigned int getFboMs(unsigned int t = 0) {
			return mFboShaderList[getValidFboIndex(mSelectedFbo)]->getFboMs(t);
		}
		unsigned int getFboMsTotal(unsigned int i = 0) {
			return mFboShaderList[getValidFboIndex(i)]->getFboMsTotal();
		}
		std::string getAssetsPath() { return mAssetsPath; }

		unsigned int findAvailableIndex(unsigned int aFboShaderIndex, const ofJson& json);
		bool         setFragmentShaderString(const std::string& aFragmentShaderString, const std::string& aName = "", unsigned int aFboShaderIndex = 0);
		int          loadFragmentShader(const std::string& aFilePath, unsigned int aFboShaderIndex);

		std::vector<VDShaderUniform> getUniforms(unsigned int aFboIndex = 0) {
			return mFboShaderList[getValidFboIndex(aFboIndex)]->getUniforms();
		}
		void setSelectedFbo(unsigned int i = 0) { mSelectedFbo = getValidFboIndex(i); }
		unsigned int getSelectedFbo()           { return mSelectedFbo; }

		ofTexture* getFboInputTexture(unsigned int t = 0) {
			return mFboShaderList[getValidFboIndex(mSelectedFbo)]->getInputTexture(t);
		}
		std::string getInputTextureName(unsigned int t = 0) {
			return mFboShaderList[getValidFboIndex(mSelectedFbo)]->getTextureName(t);
		}
		std::string getFboTextureName(unsigned int i) {
			return mFboShaderList[getValidFboIndex(i)]->getTextureName();
		}
		int getFboInputTextureWidth(unsigned int i)  { return mFboShaderList[getValidFboIndex(i)]->getInputTextureWidth(); }
		int getFboInputTextureHeight(unsigned int i) { return mFboShaderList[getValidFboIndex(i)]->getInputTextureHeight(); }
		void setFboTextureAudioMode(unsigned int i)  { mFboShaderList[getValidFboIndex(i)]->setFboTextureAudioMode(); }
		void saveThumbnail(unsigned int i)           { mFboShaderList[getValidFboIndex(i)]->saveThumbnail(); }
		ofTexture* getFboInputTextureListItem(unsigned int aFboIndex, unsigned int aTexIndex) {
			return mFboShaderList[getValidFboIndex(aFboIndex)]->getFboInputTextureListItem(aTexIndex);
		}
		unsigned int getInputTexturesCount(unsigned int i = 0) {
			return mFboShaderList[getValidFboIndex(i)]->getInputTexturesCount();
		}
		std::string getFboShaderName(unsigned int i) {
			return mFboShaderList[getValidFboIndex(i)]->getShaderName();
		}
		void loadImageFile(const std::string& aFile, unsigned int aFboIndex = 0);
		void loadVideoFile(const std::string& aFile, unsigned int aFboIndex = 0);

		std::vector<VDShaderUniform> getFboShaderUniforms(unsigned int aFboShaderIndex);
		float        getUniformValueByLocation(unsigned int aFboShaderIndex, unsigned int aLocationIndex);
		void         setUniformValueByLocation(unsigned int aFboShaderIndex, unsigned int aLocationIndex, float aValue);

		unsigned int createFboShaderTexture(const ofJson& json, unsigned int aFboIndex = 0, const std::string& aFolder = "");
		void clearFboShaderList() { mFboShaderList.clear(); }

		ofTexture* getFboRenderedTexture(unsigned int aFboIndex) {
			if (mFboShaderList.empty()) return mDefaultTexture.isAllocated() ? &mDefaultTexture : nullptr;
			return mFboShaderList[getValidFboIndex(aFboIndex)]->getRenderedTexture();
		}
		ofTexture* getFboTexture(unsigned int aFboIndex) {
			if (mFboShaderList.empty()) return mDefaultTexture.isAllocated() ? &mDefaultTexture : nullptr;
			unsigned int idx = (aFboIndex > (unsigned int)mFboShaderList.size() - 1) ? 0 : aFboIndex;
			return mFboShaderList[idx]->getTexture();
		}

		ofTexture* getMixetteTexture(unsigned int aFboIndex);
		ofTexture* getRenderedMixetteTexture(unsigned int aFboIndex) { return &mMixetteTexture; }

		void selectSenderPanel() {
			if (mFboShaderList.empty()) return;
			for (auto& fbo : mFboShaderList) {
				if (fbo->getInputTextureMode() == VDTextureMode::SHARED) fbo->selectSenderPanel();
			}
		}

		void restore(const std::string& aFilePath);

		bool handleMouseDown(int mx, int my) {
			bool handled = false;
			for (auto& fbo : mFboShaderList) if (fbo->handleMouseDown(mx, my)) handled = true;
			return handled;
		}
		bool handleMouseDrag(int mx, int my) {
			for (auto& fbo : mFboShaderList) fbo->handleMouseDrag(mx, my);
			return true;
		}

	private:
		VDParamsRef      mVDParams;
		VDAnimationRef   mVDAnimation;
		VDSettingsRef    mVDSettings;
		VDUniformsRef    mVDUniforms;

		VDFboShaderRef   mMixFboShader;
		VDFboShaderRef   mFboShader;
		VDFboShaderList  mFboShaderList;
		unsigned int     mSelectedFbo    = 0;

		bool save();

		ofFbo         mMixetteFbo;
		ofShader      mGlslMixette;
		ofTexture     mMixetteTexture;
		ofTexture     mDefaultTexture;

		std::string   mError;
		const unsigned int MAXSHADERS = 8;
		std::string   mAssetsPath     = "";
		std::string   mixPath;
		unsigned int  mCurrentSecond  = 0;
		unsigned int  mCurrentIndex   = 0;

		unsigned int getValidFboIndex(unsigned int aFboIndex) {
			if (mFboShaderList.empty()) return 0;
			return std::min(aFboIndex, (unsigned int)mFboShaderList.size() - 1);
		}
	};
}
