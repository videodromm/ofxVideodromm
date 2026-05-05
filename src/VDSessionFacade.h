#pragma once
#include "ofMain.h"
#include "VDSettings.h"
#include "VDAnimation.h"
#include "VDUniforms.h"
#include "VDMix.h"
#include "VDMediator.h"
#include "VDSession.h"

namespace videodromm
{
	typedef std::shared_ptr<class VDSessionFacade> VDSessionFacadeRef;
	class VDSessionFacade {
	public:
		VDSessionFacade(VDSettingsRef aVDSettings, VDAnimationRef aVDAnimation, VDUniformsRef aVDUniforms, VDMixRef aVDMix, VDMediatorObservableRef aVDMediator, VDSessionRef aVDSession);
		~VDSessionFacade() {}
		static VDSessionFacadeRef createVDSession(VDSettingsRef aVDSettings, VDAnimationRef aVDAnimation, VDUniformsRef aVDUniforms, VDMixRef aVDMix);
		void update();
		void draw();

		// Textures
		ofTexture* getFboRenderedTexture(unsigned int idx)    { return mVDSession->getFboRenderedTexture(idx); }
		ofTexture* getFboTexture(unsigned int idx)            { return mVDSession->getFboTexture(idx); }
		ofTexture* getMixetteTexture(unsigned int idx = 0)   { return mVDSession->getMixetteTexture(idx); }
		ofTexture* getPostFboTexture()                        { return mVDSession->getPostFboTexture(); }
		ofTexture* getFxFboTexture()                          { return mVDSession->getFxFboTexture(); }
		ofTexture* getAudioTexture()                          { return mVDSession->getAudioTexture(); }

		// FBO / shader
		unsigned int getFboShaderListSize()                            { return mVDSession->getFboShaderListSize(); }
		bool         isFboValid(unsigned int i)                         { return mVDSession->isFboValid(i); }
		bool         setFragmentShaderString(const std::string& s, const std::string& name, unsigned int index) { return mVDSession->setFragmentShaderString(s, name, index); }
		int          loadFragmentShader(const std::string& path, unsigned int index) { return mVDSession->loadFragmentShader(path, index); }
		void         loadImageFile(const std::string& path, unsigned int index)      { mVDSession->loadImageFile(path, index); }
		void         loadVideoFile(const std::string& path, unsigned int index)      { mVDSession->loadVideoFile(path, index); }
		std::vector<VDShaderUniform> getFboShaderUniforms(unsigned int i)            { return mVDSession->getFboShaderUniforms(i); }

		// Events
		bool handleKeyDown(int key, bool ctrlDown, bool shiftDown, bool altDown);
		bool handleKeyUp(int key, bool ctrlDown, bool shiftDown, bool altDown);
		void handleMouseMove(int mx, int my);
		void handleMouseDown(int mx, int my);
		void handleMouseDrag(int mx, int my);
		void handleMouseUp(int mx, int my);
		void fileDrop(const std::string& filePath, unsigned int index);

		// Persistence
		void save();
		void restore();
		void saveWarps() { mVDSession->saveWarps(); }

		// Observers
		void addObserver(VDUniformObserverRef obs)    { mVDMediator->addObserver(obs); }
		void removeObserver(VDUniformObserverRef obs) { mVDMediator->removeObserver(obs); }

		// OSC / WS / MIDI setup
		void setupOSCReceiver(int port = OSC_DEFAULT_PORT)                                { mVDMediator->setupOSCReceiver(port); }
		void setupWebsocket(const std::string& host = WS_DEFAULT_HOST, int port = WS_DEFAULT_PORT) { mVDMediator->setupWebsocket(host, port); }
		void setupMidi()                                                                   { mVDMediator->setupMidi(); }

		// Info
		std::string getOSCMsg()    { return mVDMediator->getOSCMsg(); }
		std::string getMidiMsg()   { return mVDMediator->getMidiMsg(); }
		std::string getWSMsg()     { return mVDMediator->getWSMsg(); }

		VDSettingsRef           getSettings()  { return mVDSettings; }
		VDAnimationRef          getAnimation() { return mVDAnimation; }
		VDUniformsRef           getUniforms()  { return mVDUniforms; }
		VDMixRef                getMix()       { return mVDMix; }
		VDMediatorObservableRef getMediator()  { return mVDMediator; }

	private:
		VDSettingsRef           mVDSettings;
		VDAnimationRef          mVDAnimation;
		VDUniformsRef           mVDUniforms;
		VDMixRef                mVDMix;
		VDMediatorObservableRef mVDMediator;
		VDSessionRef            mVDSession;
		std::string             mSessionPath;
		std::string             mSessionFileName = "session.json";

		void setupSession();
		void loadFromJsonFile(const std::string& path);
	};
}
