#pragma once
#include "ofMain.h"
#include "ofxOsc.h"
#include "VDSettings.h"
#include "VDAnimation.h"
#include "VDUniforms.h"
#include "VDMediator.h"

namespace videodromm
{
	class VDMediatorObservable;
	typedef std::shared_ptr<VDMediatorObservable> VDMediatorObservableRef;

	typedef std::shared_ptr<class VDOscReceiver> VDOscReceiverRef;
	class VDOscReceiver {
	public:
		VDOscReceiver(VDSettingsRef aVDSettings, VDAnimationRef aVDAnimation, VDUniformsRef aVDUniforms);
		static VDOscReceiverRef create(VDSettingsRef aVDSettings, VDAnimationRef aVDAnimation, VDUniformsRef aVDUniforms) {
			return std::make_shared<VDOscReceiver>(aVDSettings, aVDAnimation, aVDUniforms);
		}
		void setupOSCReceiver(VDMediatorObservableRef aVDMediator, int aOSCReceiverPort);
		void update();
		void setOSCMsg(const std::string& aMsg) { mOSCMsg = aMsg; }
		std::string getOSCMsg() { return mOSCMsg; }
	private:
		VDSettingsRef           mVDSettings;
		VDAnimationRef          mVDAnimation;
		VDUniformsRef           mVDUniforms;
		VDMediatorObservableRef mVDMediator;
		ofxOscReceiver          mOscReceiver;
		std::string             mOSCMsg;
		int                     mVelocity = 0;
		int                     mNote     = 0;
		void processMessage(const ofxOscMessage& msg);
	};
}
