#pragma once
#include "ofMain.h"
#include "VDUniforms.h"
#include "VDAnimation.h"
#include "VDParams.h"

#ifdef _WIN32
//#include "ofxSpout2.h"
#endif

#include <atomic>
#include <chrono>
#include <vector>

namespace videodromm
{
	typedef std::shared_ptr<class VDFboShader>  VDFboShaderRef;
	typedef std::vector<VDFboShaderRef>          VDFboShaderList;
	typedef std::chrono::high_resolution_clock   Clock;

	struct VDTextureStruct {
		std::string  name;
		unsigned int ms      = 0;
		ofTexture    texture;
		bool         isValid = false;
	};

	class VDFboShader {
	public:
		VDFboShader(VDUniformsRef aVDUniforms, VDAnimationRef aVDAnimation, const ofJson& json,
		            unsigned int aFboIndex, const std::string& aAssetsPath);
		~VDFboShader() = default;

		static VDFboShaderRef create(VDUniformsRef aVDUniforms, VDAnimationRef aVDAnimation,
		                             const ofJson& json, unsigned int aFboIndex,
		                             const std::string& aAssetsPath)
		{
			return std::make_shared<VDFboShader>(aVDUniforms, aVDAnimation, json, aFboIndex, aAssetsPath);
		}

		ofTexture*                   getTexture();
		ofTexture*                   getRenderedTexture()  { return &mRenderedTexture; }
		void                         saveThumbnail(bool overwrite = true);
		bool                         isValid()             { return mValid; }
		std::string                  getShaderName()       { return mShaderName; }
		std::vector<VDShaderUniform> getUniforms()         { return mActiveUniforms; }

		bool setFragmentShaderString(const std::string& aFragmentShaderString,
		                             const std::string& aName = "");
		bool loadFragmentShaderFromFile(const std::string& aFileOrPath, bool isAudio = false);

		float getUniformValueByLocation(unsigned int i)           { return mUniformValueByLocation[i]; }
		void  setUniformValueByLocation(unsigned int i, float v)  { mUniformValueByLocation[i] = v; }

		void setFboTextureAudioMode() {
			mCurrentFilename = mTextureName = mVDAnimation->getAudioTextureName();
			mTextureMode = VDTextureMode::AUDIO;
			mUseAudioTexture = true;
			mInputTextureList[0].name    = mVDAnimation->getAudioTextureName();
			mInputTextureList[0].ms      = 0;
			mInputTextureList[0].isValid = true;
		}

		ofTexture* getFboInputTextureListItem(unsigned int aTexIndex) {
			return &mInputTextureList[getValidTexIndex(aTexIndex)].texture;
		}
		std::string  getTextureName(unsigned int aTexIndex = 0) {
			return mInputTextureList[aTexIndex].name;
		}
		unsigned int getFboMs(unsigned int aTexIndex = 0)  { return mInputTextureList[aTexIndex].ms; }
		unsigned int getFboMsTotal()                       { return msTotal; }

		void loadImageFile(const std::string& aFile, unsigned int aCurrentIndex = 0);
		unsigned int getInputTexturesCount()               { return (unsigned int)mInputTextureList.size(); }

		std::string getFboMsg()    { return mFboMsg; }
		std::string getFboError()  { return mFboError; }
		std::string getFboStatus() { return mFboStatus; }

		bool isHydraTex()         { return mIsHydraTex; }
		ofTexture* getInputTexture(unsigned int aTexIndex = 0) {
			return &mInputTextureList[getValidTexIndex(aTexIndex)].texture;
		}
		bool isValidInputTexture(unsigned int aTexIndex) {
			return mInputTextureList[getValidTexIndex(aTexIndex)].isValid;
		}
		int getInputTextureWidth()  { return mInputTextureList[0].texture.getWidth(); }
		int getInputTextureHeight() { return mInputTextureList[0].texture.getHeight(); }
		int getInputTextureMode()   { return mTextureMode; }

		void selectSenderPanel() {
#ifdef _WIN32
			// ofxSpout2 sender panel (if supported)
#endif
		}

		bool handleMouseDown(int mx_, int my_) { mx = (float)mx_; my = (float)my_; return false; }
		bool handleMouseDrag(int mx_, int my_) { return true; }

	private:
		VDParamsRef              mVDParams;
		const VDAnimationRef     mVDAnimation;
		VDUniformsRef            mVDUniforms;

		std::string    mCurrentFilename;
		std::map<unsigned int, VDTextureStruct> mInputTextureList;
		unsigned int   mCacheImageIndex         = 0;
		bool           mUseAudioTexture         = false;

#ifdef _WIN32
		//ofxSpout2::Receiver mSpoutIn;
#endif

		unsigned int   createInputTexture(const ofJson& json);
		bool           mLoadTopDown             = false;
		int            mCurrentImageSequenceIndex = 0;
		std::string    mTextureName             = "";
		std::string    mTypestr                 = "";
		std::string    mExt                     = "jpg";
		int            mTextureMode             = VDTextureMode::UNKNOWN;
		int            mTextureCount            = 1;
		bool           mPreloadTextures         = false;
		unsigned int   msTotal                  = 0;
		size_t         dotIndex                 = std::string::npos;
		size_t         colonIndex               = std::string::npos;
		bool           mIsHydraTex              = false;
		std::string    mFragFilePath            = "";
		bool           fileExists               = false;

		// shader
		ofShader       mShader;
		std::vector<VDShaderUniform>           mActiveUniforms;
		std::map<unsigned int, float>          mUniformValueByLocation;
		std::string    mShaderName              = "";
		std::string    mName                    = "";
		std::string    mShaderFileName          = "";
		bool           loadFragmentStringFromFile();
		std::string    mShaderFragmentString;
		std::string    mFileNameWithExtension;
		std::string    shaderInclude;
		bool           mValid                   = false;

		// FBO
		ofFbo          mFbo;
		bool           isReady                  = false;
		ofTexture      mRenderedTexture;

		void loadNextTexture(unsigned int aCurrentIndex);
		ofTexture* getFboTexture();

		std::string    mFboMsg;
		std::string    mFboError                = "";
		std::string    mFboStatus               = "";
		std::string    mAssetsPath              = "";
		unsigned int   mFboIndex                = 0;

		// video
		ofVideoPlayer  mVideo;
		float          mVideoPos                = 0.0f;
		float          mVideoDuration           = 0.0f;
		bool           mIsVideoLoaded           = false;

		float mx = 0.0f;
		float my = 0.0f;

		unsigned int getValidTexIndex(unsigned int aTexIndex) {
			if (mInputTextureList.empty()) return 0;
			return std::min(aTexIndex, (unsigned int)mInputTextureList.size() - 1);
		}

		void applyAllUniforms();
	};
}
