#pragma once

#include "ofMain.h"
#include "ofxXmlSettings.h"

using namespace std;

namespace videodromm {

	typedef std::shared_ptr<class VDSettings> VDSettingsRef;

	class VDSettings
	{
	public:
		VDSettings(const std::string& filename);
		static VDSettingsRef create(const std::string& filename);

		enum VDAnims {
			ANIM_NONE = 0,
			ANIM_TIME = 1,
			ANIM_AUTO = 2,
			ANIM_BASS = 3,
			ANIM_MID = 4,
			ANIM_SMOOTH = 5
		};

		bool						save();
		bool						restore();
		void						reset();
		void						resetSomeParams();

		// params
		int							mMainWindowX, mMainWindowY, mMarginSmall, mMainWindowWidth, mMainWindowHeight;
		float						mCodeEditorWidth, mCodeEditorHeight;
		// MIDI
		bool						mMIDIEnabled;
		// OSC
		bool						mOSCEnabled;
		std::string					mOSCDestinationHost;
		uint16_t					mOSCDestinationPort;
		std::string					mOSCDestinationHost2;
		uint16_t					mOSCDestinationPort2;
		bool						mIsOSCSender;

		// render windows
		int							mRenderWidth;
		int							mRenderHeight;
		glm::vec2					mTexMult, mLeftRenderXY, mRightRenderXY, mPreviewRenderXY, mWarp1RenderXY, mWarp2RenderXY;
		glm::vec2					mRenderPosXY;
		glm::vec2					mPreviewFragXY;
		glm::vec2					mCamPosXY;
		int							mxLeft;
		int							mxRight;
		int							myLeft;
		int							myRight;
		bool						mAutoLayout;
		bool						mStandalone;

		// code editor
		glm::vec2					mRenderCodeEditorXY;
		bool						mLiveCode;
		bool						mShowUI;
		bool						mCursorVisible;
		bool						isUIDirty;
		bool						mMIDIOpenAllInputPorts;
		int							mCount;
		std::string					mImageFile;

		// shader uniforms
		float						iStart = 1.0f;
		float						iBarDuration = 1.0f;
		float						iChannelTime[4];
		bool						iFade;
		bool						iRepeat;
		float						iAlpha;
		bool						iShowFps;
		unsigned int				shaderEditIndex;
		// transition
		int							iTransition;
		float						iAnim;
		double						mTransitionDuration;
		double						mTransitionTime;

		// windows and params
		int							mRenderX;
		int							mRenderY;
		int							mDisplayCount;
		bool						mPreviewEnabled;
		std::string					mCurrentFilePath;
		// Textures
		bool						mRenderThumbs;
		int							currentSelectedIndex;

		int							mWindowToCreate;
		ofColor						FPSColor;
		ofColor						ColorGreen;
		ofColor						ColorRed;
		ofColor						ColorYellow;
		ofColor						ColorPurple;
		// audio
		bool						mIsPlaying;

		float						iSeed;

		// z EyePointZ
		float						defaultEyePointZ;
		float						minEyePointZ;
		float						maxEyePointZ;
		bool						tEyePointZ;
		bool						autoEyePointZ;

		float						mStateTransition;

		bool						mSendToOutput;
		bool						autoInvert;

		// spout
		int							mFboResolution;
		int							selectedChannel;
		int							selectedWarp;
		int							mWarpCount;
		bool						mOptimizeUI;
		bool						mSplitWarpH;
		bool						mSplitWarpV;
		int							mUIZoom;
		int							mCurrentPreviewFboIndex;

		// camera
		ofCamera					mCamera;
		glm::vec2					mCamEyePointXY;
		float						mCamEyePointZ;

		// web sockets
		bool						mIsRouter;
		bool						mAreSocketIOEnabledAtStartup;
		bool						mIsSocketIOServer;
		std::string					mSocketIOProtocol;
		std::string					mSocketIOHost;
		std::string					mSocketIORoom;
		std::string					mSocketIONickname;
		uint16_t					mSocketIOPort;
		std::string					mSocketIOMsg;

		std::string					mInfo;

		std::string					getMsg()       { return mMsg; }
		void						setMsg(const std::string& aMsg) { mMsg = aMsg; }
		std::string					getErrorMsg()  { return mErrorMsg; }
		void						setErrorMsg(const std::string& aMsg) { mErrorMsg = aMsg; }

	private:
		std::string					settingsFileName;
		std::string					mMsg;
		std::string					mErrorMsg;
	};
}
