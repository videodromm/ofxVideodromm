#pragma once
#include "ofMain.h"
#include "ofxLibwebsockets.h"
#include "VDSettings.h"
#include "VDAnimation.h"
#include "VDUniforms.h"
#include "VDMediator.h"

namespace videodromm
{
	class VDMediatorObservable;
	typedef std::shared_ptr<VDMediatorObservable> VDMediatorObservableRef;

	typedef std::shared_ptr<class VDWebsocket> VDWebsocketRef;
	class VDWebsocket  {
	public:
		VDWebsocket(VDSettingsRef aVDSettings, VDAnimationRef aVDAnimation, VDUniformsRef aVDUniforms);
		~VDWebsocket();
		static VDWebsocketRef create(VDSettingsRef aVDSettings, VDAnimationRef aVDAnimation, VDUniformsRef aVDUniforms) {
			return std::make_shared<VDWebsocket>(aVDSettings, aVDAnimation, aVDUniforms);
		}
		void setupWebsocket(VDMediatorObservableRef aVDMediator, const std::string& host, int port);
		void update();
		void wsWrite(const std::string& aMsg);
		bool isConnected() { return mIsConnected; }
		std::string getWSMsg() { return mWSMsg; }
		// ofxLibwebsockets::WebSocketListener
		//void onOpen   (ofxLibwebsockets::Event& args) override;
		//void onClose  (ofxLibwebsockets::Event& args) override;
		//void onError  (ofxLibwebsockets::Event& args) override;
		//void onMessage(ofxLibwebsockets::Event& args) override;
		//void onBroadcast(ofxLibwebsockets::Event& args) override;
		// websocket methods
		void onConnect(ofxLibwebsockets::Event & args);
		void onOpen(ofxLibwebsockets::Event & args);
		void onClose(ofxLibwebsockets::Event & args);
		void onIdle(ofxLibwebsockets::Event & args);
		void onMessage(ofxLibwebsockets::Event & args);
		void onBroadcast(ofxLibwebsockets::Event & args);
	private:
		VDSettingsRef           mVDSettings;
		VDAnimationRef          mVDAnimation;
		VDUniformsRef           mVDUniforms;
		VDMediatorObservableRef mVDMediator;
		ofxLibwebsockets::Client mClient;
		std::string             mWSHost;
		int                     mWSPort;
		bool                    mIsConnected;
		float                   mPingTime;
		std::string             mWSMsg;
		void parseMessage(const std::string& aMsg);
		void wsConnect();
	};
}
