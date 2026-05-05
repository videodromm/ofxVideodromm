#pragma once
#include "ofMain.h"
#include "VDSettings.h"
#include "VDAnimation.h"
#include "VDUniforms.h"
#include "VDMix.h"

#define OSC_DEFAULT_PORT 2346
#define WS_DEFAULT_HOST  "127.0.0.1"
#define WS_DEFAULT_PORT  8088

namespace videodromm
{
	// Forward declarations
	class VDKeyboard;
	class VDOscReceiver;
	class VDMidi;
	class VDWebsocket;
	typedef std::shared_ptr<VDKeyboard>   VDKeyboardRef;
	typedef std::shared_ptr<VDOscReceiver> VDOscReceiverRef;
	typedef std::shared_ptr<VDMidi>        VDMidiRef;
	typedef std::shared_ptr<VDWebsocket>   VDWebsocketRef;

	// Abstract observer interface
	class VDUniformObserver : public std::enable_shared_from_this<VDUniformObserver> {
	public:
		virtual ~VDUniformObserver() {}
		virtual std::shared_ptr<VDUniformObserver> setUniformValue(int aIndex, float aValue) = 0;
	};
	typedef std::shared_ptr<VDUniformObserver> VDUniformObserverRef;

	typedef std::shared_ptr<class VDMediatorObservable> VDMediatorObservableRef;
	class VDMediatorObservable : public std::enable_shared_from_this<VDMediatorObservable> {
	public:
		VDMediatorObservable(VDSettingsRef aVDSettings, VDAnimationRef aVDAnimation, VDUniformsRef aVDUniforms, VDMixRef aVDMix);
		~VDMediatorObservable() {}
		static VDMediatorObservableRef createVDMediatorObservable(VDSettingsRef aVDSettings, VDAnimationRef aVDAnimation, VDUniformsRef aVDUniforms, VDMixRef aVDMix);

		void update();
		void addObserver(VDUniformObserverRef observer)    { mObservers.push_back(observer); }
		void removeObserver(VDUniformObserverRef observer) { mObservers.erase(std::remove(mObservers.begin(), mObservers.end(), observer), mObservers.end()); }
		VDMediatorObservableRef setUniformValue(int aIndex, float aValue);

		void setupOSCReceiver(int aPort = OSC_DEFAULT_PORT);
		void setupWebsocket(const std::string& host = WS_DEFAULT_HOST, int port = WS_DEFAULT_PORT);
		void setupMidi();
		void setKeyboard(VDKeyboardRef kb) { mVDKeyboard = kb; }

		bool handleKeyDown(int key, bool ctrlDown, bool shiftDown, bool altDown);
		bool handleKeyUp(int key, bool ctrlDown, bool shiftDown, bool altDown);

		std::string getOSCMsg()        { return mOSCMsg; }
		std::string getMidiMsg()       { return mMidiMsg; }
		std::string getWSMsg()         { return mWSMsg; }
		bool        isWSConnected()    { return mWSConnected; }
		int         getOSCReceiverPort() { return mOSCReceiverPort; }
		int         getWSPort()          { return mWSPort; }
		std::string getWSHost()          { return mWSHost; }

		void        setPreferredMidiInputDevice(const std::string& s) { mPreferredMidiInputDevice = s; }
		std::string getPreferredMidiInputDevice()                      { return mPreferredMidiInputDevice; }

		void loadOSCReceiverFromJsonFile(const std::string& jsonFile);
		void saveOSCReceiverToJson();
		void loadWSFromJsonFile(const std::string& jsonFile);
		void saveWSToJson();

		// MIDI pass-through
		int         getMidiInPortsCount()            { return mMidiInCount; }
		std::string getMidiInPortName(int i)          { return (i < (int)mMidiInPortNames.size()) ? mMidiInPortNames[i] : ""; }
		bool        isMidiInConnected(int i)          { return (i < (int)mMidiInConnected.size()) ? mMidiInConnected[i] : false; }
		int         getMidiOutPortsCount()            { return mMidiOutCount; }
		std::string getMidiOutPortName(int i)         { return (i < (int)mMidiOutPortNames.size()) ? mMidiOutPortNames[i] : ""; }
		bool        isMidiOutConnected(int i)         { return (i < (int)mMidiOutConnected.size()) ? mMidiOutConnected[i] : false; }

	private:
		VDSettingsRef           mVDSettings;
		VDAnimationRef          mVDAnimation;
		VDUniformsRef           mVDUniforms;
		VDMixRef                mVDMix;
		VDKeyboardRef           mVDKeyboard;
		VDOscReceiverRef        mVDOscReceiver;
		VDMidiRef               mVDMidi;
		VDWebsocketRef          mVDWebsocket;
		std::vector<VDUniformObserverRef> mObservers;

		int         mOSCReceiverPort = OSC_DEFAULT_PORT;
		std::string mWSHost          = WS_DEFAULT_HOST;
		int         mWSPort          = WS_DEFAULT_PORT;
		bool        mWSConnected     = false;
		std::string mOSCMsg, mMidiMsg, mWSMsg;
		std::string mPreferredMidiInputDevice;
		std::string mOSCJsonFileName = "osc.json";
		std::string mWSJsonFileName  = "ws.json";

		// MIDI cached stats
		int  mMidiInCount = 0, mMidiOutCount = 0;
		std::vector<std::string> mMidiInPortNames, mMidiOutPortNames;
		std::vector<bool>        mMidiInConnected, mMidiOutConnected;

		bool validateJson(const ofJson& tree, const std::string& key, int minVal, int maxVal, int& outVal);
	};
}
