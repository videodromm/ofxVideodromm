#include "VDSettings.h"

using namespace videodromm;

VDSettings::VDSettings(const std::string& filename)
{
	reset();
	settingsFileName = "VD" + filename + ".xml";
	string params = ofToDataPath(settingsFileName, true);
	if (ofFile::doesFileExist(params)) {
		restore();
	}
	else {
		save();
	}
}

VDSettingsRef VDSettings::create(const std::string& filename)
{
	return std::shared_ptr<VDSettings>(new VDSettings(filename));
}

bool VDSettings::save()
{
	ofxXmlSettings xml;
	xml.addTag("settings");
	xml.pushTag("settings");

	xml.addValue("AutoLayout",          mAutoLayout);
	xml.addValue("Standalone",          mStandalone);
	xml.addValue("MainWindowWidth",     mMainWindowWidth);
	xml.addValue("MainWindowHeight",    mMainWindowHeight);
	xml.addValue("MainWindowX",         mMainWindowX);
	xml.addValue("MainWindowY",         mMainWindowY);
	xml.addValue("RenderWidth",         mRenderWidth);
	xml.addValue("RenderHeight",        mRenderHeight);
	xml.addValue("RenderX",             mRenderX);
	xml.addValue("RenderY",             mRenderY);
	xml.addValue("MIDIEnabled",         mMIDIEnabled);
	xml.addValue("OSCEnabled",          mOSCEnabled);
	xml.addValue("OSCDestinationPort",  (int)mOSCDestinationPort);
	xml.addValue("OSCDestinationHost",  mOSCDestinationHost);
	xml.addValue("OSCDestinationPort2", (int)mOSCDestinationPort2);
	xml.addValue("OSCDestinationHost2", mOSCDestinationHost2);
	xml.addValue("MIDIOpenAllInputPorts", mMIDIOpenAllInputPorts);
	xml.addValue("ShowUI",              mShowUI);
	xml.addValue("CursorVisible",       mCursorVisible);
	xml.addValue("IsOSCSender",         mIsOSCSender);
	xml.addValue("AreSocketIOEnabledAtStartup", mAreSocketIOEnabledAtStartup);
	xml.addValue("IsRouter",            mIsRouter);
	xml.addValue("IsSocketIOServer",    mIsSocketIOServer);
	xml.addValue("SocketIOProtocol",    mSocketIOProtocol);
	xml.addValue("SocketIOHost",        mSocketIOHost);
	xml.addValue("SocketIOPort",        (int)mSocketIOPort);
	xml.addValue("SocketIORoom",        mSocketIORoom);
	xml.addValue("SocketIONickname",    mSocketIONickname);
	xml.addValue("Info",                mInfo);
	xml.addValue("SplitWarpH",          mSplitWarpH);
	xml.addValue("SplitWarpV",          mSplitWarpV);

	xml.popTag();
	return xml.save(settingsFileName);
}

bool VDSettings::restore()
{
	if (!ofFile::doesFileExist(settingsFileName)) return false;

	ofxXmlSettings xml;
	if (!xml.load(settingsFileName)) return false;
	if (!xml.tagExists("settings")) return false;

	xml.pushTag("settings");

	if (xml.tagExists("AutoLayout"))            mAutoLayout     = xml.getValue("AutoLayout", mAutoLayout);
	if (xml.tagExists("Standalone"))            mStandalone     = xml.getValue("Standalone", mStandalone);
	if (xml.tagExists("MIDIEnabled"))           mMIDIEnabled    = xml.getValue("MIDIEnabled", mMIDIEnabled);
	if (xml.tagExists("OSCEnabled"))            mOSCEnabled     = xml.getValue("OSCEnabled", mOSCEnabled);
	if (xml.tagExists("IsOSCSender"))           mIsOSCSender    = xml.getValue("IsOSCSender", mIsOSCSender);
	if (xml.tagExists("OSCDestinationPort"))    mOSCDestinationPort  = xml.getValue("OSCDestinationPort", (int)mOSCDestinationPort);
	if (xml.tagExists("OSCDestinationHost"))    mOSCDestinationHost  = xml.getValue("OSCDestinationHost", mOSCDestinationHost);
	if (xml.tagExists("OSCDestinationPort2"))   mOSCDestinationPort2 = xml.getValue("OSCDestinationPort2", (int)mOSCDestinationPort2);
	if (xml.tagExists("OSCDestinationHost2"))   mOSCDestinationHost2 = xml.getValue("OSCDestinationHost2", mOSCDestinationHost2);
	if (xml.tagExists("MIDIOpenAllInputPorts")) mMIDIOpenAllInputPorts = xml.getValue("MIDIOpenAllInputPorts", mMIDIOpenAllInputPorts);
	if (xml.tagExists("ShowUI"))                mShowUI         = xml.getValue("ShowUI", mShowUI);
	if (xml.tagExists("CursorVisible"))         mCursorVisible  = xml.getValue("CursorVisible", mCursorVisible);
	if (xml.tagExists("AreSocketIOEnabledAtStartup")) mAreSocketIOEnabledAtStartup = xml.getValue("AreSocketIOEnabledAtStartup", mAreSocketIOEnabledAtStartup);
	if (xml.tagExists("IsRouter"))              mIsRouter       = xml.getValue("IsRouter", mIsRouter);
	if (xml.tagExists("IsSocketIOServer"))      mIsSocketIOServer = xml.getValue("IsSocketIOServer", mIsSocketIOServer);
	if (xml.tagExists("SocketIOProtocol"))      mSocketIOProtocol = xml.getValue("SocketIOProtocol", mSocketIOProtocol);
	if (xml.tagExists("SocketIOHost"))          mSocketIOHost   = xml.getValue("SocketIOHost", mSocketIOHost);
	if (xml.tagExists("SocketIOPort"))          mSocketIOPort   = xml.getValue("SocketIOPort", (int)mSocketIOPort);
	if (xml.tagExists("SocketIORoom"))          mSocketIORoom   = xml.getValue("SocketIORoom", mSocketIORoom);
	if (xml.tagExists("SocketIONickname"))      mSocketIONickname = xml.getValue("SocketIONickname", mSocketIONickname);
	if (xml.tagExists("Info"))                  mInfo           = xml.getValue("Info", mInfo);
	if (xml.tagExists("SplitWarpH"))            mSplitWarpH     = xml.getValue("SplitWarpH", mSplitWarpH);
	if (xml.tagExists("SplitWarpV"))            mSplitWarpV     = xml.getValue("SplitWarpV", mSplitWarpV);

	if (mAutoLayout) {
		mMainWindowWidth = 1280;
		mMainWindowHeight = 720;
		mRenderWidth = 1280;
		mRenderHeight = 720;
		mRenderX = 1024;
		mRenderY = 0;
	}
	else {
		if (xml.tagExists("MainWindowWidth"))  mMainWindowWidth  = xml.getValue("MainWindowWidth",  mMainWindowWidth);
		if (xml.tagExists("MainWindowHeight")) mMainWindowHeight = xml.getValue("MainWindowHeight", mMainWindowHeight);
		if (xml.tagExists("MainWindowX"))      mMainWindowX      = xml.getValue("MainWindowX",      mMainWindowX);
		if (xml.tagExists("MainWindowY"))      mMainWindowY      = xml.getValue("MainWindowY",      mMainWindowY);
		if (xml.tagExists("RenderWidth"))      mRenderWidth      = xml.getValue("RenderWidth",      mRenderWidth);
		if (xml.tagExists("RenderHeight"))     mRenderHeight     = xml.getValue("RenderHeight",     mRenderHeight);
		if (xml.tagExists("RenderX"))          mRenderX          = xml.getValue("RenderX",          mRenderX);
		if (xml.tagExists("RenderY"))          mRenderY          = xml.getValue("RenderY",          mRenderY);
	}

	xml.popTag();
	return true;
}

void VDSettings::resetSomeParams()
{
	mCamPosXY = glm::vec2(0.0f);
	mCount = 1;

	defaultEyePointZ = -400.f;
	minEyePointZ = -800.0f;
	maxEyePointZ = 0.0f;
	tEyePointZ = autoEyePointZ = false;

	iAlpha = 1.0f;
	iFade = mSendToOutput = iRepeat = false;

	iTransition = 0;
	iAnim = 0.0f;
	mTransitionDuration = 2.0f;
	mTransitionTime = 1.0f;

	autoInvert = false;
	shaderEditIndex = 0;
}

void VDSettings::reset()
{
	mMIDIOpenAllInputPorts = mAutoLayout = mShowUI = mCursorVisible = true;
	mRenderThumbs = mStandalone = mSplitWarpH = mSplitWarpV = false;
	mInfo = "";

	mMainWindowX = mRenderX = 0;
	mMainWindowY = mRenderY = 0;
	mxLeft = 0;
	mxRight = 1280;
	myLeft = 0;
	myRight = 720;
	mMainWindowWidth = 1280;
	mMainWindowHeight = 720;
	mRenderWidth = 1280;
	mRenderHeight = 720;

	mTexMult         = glm::vec2(1.0f);
	mLeftRenderXY    = mRightRenderXY = mPreviewRenderXY = mWarp1RenderXY = mWarp2RenderXY = glm::vec2(0.0f);
	mRenderPosXY     = glm::vec2(0.0f, 320.0f);
	mPreviewFragXY   = glm::vec2(0.0f);
	mRenderCodeEditorXY.x = 0;
	mRenderCodeEditorXY.y = 0;
	mCodeEditorWidth  = 800;
	mCodeEditorHeight = 600;
	mCurrentFilePath  = "currentMix.frag";
	mMarginSmall      = 2;
	mPreviewEnabled   = true;

	mIsPlaying = false;
	iSeed = 0.1f;

	for (int i = 0; i < 4; i++) iChannelTime[i] = (float)i;

	iShowFps = true;
	currentSelectedIndex = 0;
	selectedWarp = 0;
	selectedChannel = 0;
	mFboResolution = 2048;
	mWarpCount = 3;

	FPSColor    = ofColor(0,   255, 0,   255);
	ColorGreen  = ofColor(0,   255, 0,   255);
	ColorRed    = ofColor(255, 0,   0,   255);
	ColorYellow = ofColor(255, 255, 0,   255);
	ColorPurple = ofColor(128, 0,   255, 255);

	isUIDirty  = true;
	mLiveCode  = false;
	mStateTransition = 1.0f;
	mOptimizeUI = false;

	mCamEyePointXY = glm::vec2(0.f);
	mCamEyePointZ  = -400.f;
	mCamera.setPosition(mCamEyePointXY.x, mCamEyePointXY.y, mCamEyePointZ);

	mUIZoom = 1;
	mMIDIEnabled = true;
	mOSCEnabled  = false;
	mOSCDestinationHost  = "127.0.0.1";
	mOSCDestinationPort  = 7001;
	mOSCDestinationHost2 = "127.0.0.1";
	mOSCDestinationPort2 = 7002;

	mMsg = "";
	mErrorMsg = "";
	mSocketIOMsg = "";
	mIsOSCSender = false;

	mIsRouter = false;
	mAreSocketIOEnabledAtStartup = true;
	mIsSocketIOServer = false;
	mSocketIOProtocol = "http://";
	mSocketIOHost     = "127.0.0.1";
	mSocketIOPort     = 8088;
	mSocketIORoom     = "roomtest";
	mSocketIONickname = "bruce";

	resetSomeParams();
}
