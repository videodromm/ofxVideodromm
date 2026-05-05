#include "VDMediator.h"
#include "VDKeyboard.h"
#include "VDOscReceiver.h"
#include "VDMidi.h"
#include "VDWebsocket.h"

using namespace videodromm;

VDMediatorObservable::VDMediatorObservable(VDSettingsRef aVDSettings, VDAnimationRef aVDAnimation, VDUniformsRef aVDUniforms, VDMixRef aVDMix) {
	mVDSettings  = aVDSettings;
	mVDAnimation = aVDAnimation;
	mVDUniforms  = aVDUniforms;
	mVDMix       = aVDMix;
	mPreferredMidiInputDevice = "";
}

VDMediatorObservableRef VDMediatorObservable::createVDMediatorObservable(VDSettingsRef aVDSettings, VDAnimationRef aVDAnimation, VDUniformsRef aVDUniforms, VDMixRef aVDMix) {
	return std::make_shared<VDMediatorObservable>(aVDSettings, aVDAnimation, aVDUniforms, aVDMix);
}

VDMediatorObservableRef VDMediatorObservable::setUniformValue(int aIndex, float aValue) {
	mVDUniforms->setUniformValue(aIndex, aValue);
	for (auto& obs : mObservers) {
		obs->setUniformValue(aIndex, aValue);
	}
	// Notify via websocket
	if (mVDWebsocket && mWSConnected) {
		std::string msg = "{\"params\":[{\"name\":" + ofToString(aIndex) + ",\"value\":" + ofToString(aValue) + "}]}";
		mVDWebsocket->wsWrite(msg);
	}
	return shared_from_this();
}

void VDMediatorObservable::update() {
	if (mVDOscReceiver) mVDOscReceiver->update();
	if (mVDWebsocket)  mVDWebsocket->update();
}

void VDMediatorObservable::setupOSCReceiver(int aPort) {
	loadOSCReceiverFromJsonFile(ofToDataPath(mOSCJsonFileName));
	if (aPort != OSC_DEFAULT_PORT) mOSCReceiverPort = aPort;
	mVDOscReceiver = VDOscReceiver::create(mVDSettings, mVDAnimation, mVDUniforms);
	mVDOscReceiver->setupOSCReceiver(shared_from_this(), mOSCReceiverPort);
}

void VDMediatorObservable::setupMidi() {
	mVDMidi = VDMidi::create(mVDUniforms);
	mVDMidi->setupMidi(shared_from_this());
	// Cache MIDI info
	mMidiInCount = mVDMidi->getMidiInPortsCount();
	for (int i = 0; i < mMidiInCount; i++) {
		mMidiInPortNames.push_back(mVDMidi->getMidiInPortName(i));
		mMidiInConnected.push_back(mVDMidi->isMidiInConnected(i));
	}
	mMidiOutCount = mVDMidi->getMidiOutPortsCount();
	for (int i = 0; i < mMidiOutCount; i++) {
		mMidiOutPortNames.push_back(mVDMidi->getMidiOutPortName(i));
		mMidiOutConnected.push_back(mVDMidi->isMidiOutConnected(i));
	}
}

void VDMediatorObservable::setupWebsocket(const std::string& host, int port) {
	loadWSFromJsonFile(ofToDataPath(mWSJsonFileName));
	if (host != WS_DEFAULT_HOST) mWSHost = host;
	if (port != WS_DEFAULT_PORT) mWSPort = port;
	mVDWebsocket = VDWebsocket::create(mVDSettings, mVDAnimation, mVDUniforms);
	mVDWebsocket->setupWebsocket(shared_from_this(), mWSHost, mWSPort);
	mWSConnected = mVDWebsocket->isConnected();
}

bool VDMediatorObservable::handleKeyDown(int key, bool ctrlDown, bool shiftDown, bool altDown) {
	if (mVDKeyboard) return mVDKeyboard->handleKeyDown(key, ctrlDown, shiftDown, altDown);
	return false;
}
bool VDMediatorObservable::handleKeyUp(int key, bool ctrlDown, bool shiftDown, bool altDown) {
	if (mVDKeyboard) return mVDKeyboard->handleKeyUp(key, ctrlDown, shiftDown, altDown);
	return false;
}

void VDMediatorObservable::loadOSCReceiverFromJsonFile(const std::string& path) {
	if (!ofFile::doesFileExist(path)) {
		mOSCReceiverPort = OSC_DEFAULT_PORT;
		saveOSCReceiverToJson();
		return;
	}
	try {
		ofJson doc = ofLoadJson(path);
		if (doc.contains("receiver")) {
			int port = OSC_DEFAULT_PORT;
			if (validateJson(doc["receiver"], "port", 1024, 65535, port)) mOSCReceiverPort = port;
		}
	}
	catch (const std::exception& e) {
		ofLogError("VDMediator") << "loadOSCReceiverFromJsonFile error: " << e.what();
	}
}

void VDMediatorObservable::saveOSCReceiverToJson() {
	ofJson doc;
	doc["receiver"]["port"] = mOSCReceiverPort;
	ofSaveJson(ofToDataPath(mOSCJsonFileName), doc);
}

void VDMediatorObservable::loadWSFromJsonFile(const std::string& path) {
	if (!ofFile::doesFileExist(path)) {
		saveWSToJson();
		return;
	}
	try {
		ofJson doc = ofLoadJson(path);
		if (doc.contains("ws")) {
			if (doc["ws"].contains("host")) mWSHost = doc["ws"]["host"].get<std::string>();
			int port = WS_DEFAULT_PORT;
			if (validateJson(doc["ws"], "port", 1024, 65535, port)) mWSPort = port;
		}
	}
	catch (const std::exception& e) {
		ofLogError("VDMediator") << "loadWSFromJsonFile error: " << e.what();
	}
}

void VDMediatorObservable::saveWSToJson() {
	ofJson doc;
	doc["ws"]["host"] = mWSHost;
	doc["ws"]["port"] = mWSPort;
	ofSaveJson(ofToDataPath(mWSJsonFileName), doc);
}

bool VDMediatorObservable::validateJson(const ofJson& tree, const std::string& key, int minVal, int maxVal, int& outVal) {
	if (!tree.contains(key)) return false;
	int v = tree[key].get<int>();
	if (v >= minVal && v <= maxVal) { outVal = v; return true; }
	return false;
}
