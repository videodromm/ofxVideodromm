#include "VDSessionFacade.h"
#include "VDKeyboard.h"

using namespace videodromm;

VDSessionFacadeRef VDSessionFacade::createVDSession(VDSettingsRef aVDSettings, VDAnimationRef aVDAnimation, VDUniformsRef aVDUniforms, VDMixRef aVDMix) {
	auto mediator = VDMediatorObservable::createVDMediatorObservable(aVDSettings, aVDAnimation, aVDUniforms, aVDMix);
	auto keyboard = VDKeyboard::create(aVDSettings, aVDAnimation, aVDUniforms);
	keyboard->setupKeyboard(mediator);
	mediator->setKeyboard(keyboard);
	auto session = VDSession::create(aVDSettings, aVDAnimation, aVDUniforms, aVDMix);
	auto facade  = std::make_shared<VDSessionFacade>(aVDSettings, aVDAnimation, aVDUniforms, aVDMix, mediator, session);
	facade->setupSession();
	return facade;
}

VDSessionFacade::VDSessionFacade(VDSettingsRef aVDSettings, VDAnimationRef aVDAnimation, VDUniformsRef aVDUniforms, VDMixRef aVDMix, VDMediatorObservableRef aVDMediator, VDSessionRef aVDSession) {
	mVDSettings  = aVDSettings;
	mVDAnimation = aVDAnimation;
	mVDUniforms  = aVDUniforms;
	mVDMix       = aVDMix;
	mVDMediator  = aVDMediator;
	mVDSession   = aVDSession;
}

void VDSessionFacade::setupSession() {
	mSessionPath = ofToDataPath(mSessionFileName);
	if (ofFile::doesFileExist(mSessionPath)) {
		restore();
	} else {
		ofFile f(mSessionPath, ofFile::WriteOnly);
		f.close();
		save();
	}
}

void VDSessionFacade::loadFromJsonFile(const std::string& path) {
	if (!ofFile::doesFileExist(path)) return;
	try {
		ofJson doc = ofLoadJson(path);
		if (doc.contains("preferredMidiInput")) {
			mVDMediator->setPreferredMidiInputDevice(doc["preferredMidiInput"].get<std::string>());
		}
		if (doc.contains("fbos")) {
			mVDSession->loadFromJsonFile(path);
		}
	}
	catch (const std::exception& e) {
		ofLogError("VDSessionFacade") << "loadFromJsonFile error: " << e.what();
	}
}

void VDSessionFacade::restore() {
	loadFromJsonFile(mSessionPath);
}

void VDSessionFacade::save() {
	ofJson doc;
	doc["preferredMidiInput"] = mVDMediator->getPreferredMidiInputDevice();
	ofSaveJson(mSessionPath, doc);
	mVDSession->save();
}

void VDSessionFacade::update() {
	mVDMediator->update();
	mVDSession->update();
}

void VDSessionFacade::draw() {
	mVDSession->draw();
}

bool VDSessionFacade::handleKeyDown(int key, bool ctrlDown, bool shiftDown, bool altDown) {
	bool handled = mVDMediator->handleKeyDown(key, ctrlDown, shiftDown, altDown);
	if (!handled) handled = mVDSession->handleKeyDown(key, ctrlDown, shiftDown, altDown);
	return handled;
}
bool VDSessionFacade::handleKeyUp(int key, bool ctrlDown, bool shiftDown, bool altDown) {
	bool handled = mVDMediator->handleKeyUp(key, ctrlDown, shiftDown, altDown);
	if (!handled) handled = mVDSession->handleKeyUp(key, ctrlDown, shiftDown, altDown);
	return handled;
}

void VDSessionFacade::handleMouseMove(int mx, int my) { mVDSession->handleMouseMove(mx, my); }
void VDSessionFacade::handleMouseDown(int mx, int my) { mVDSession->handleMouseDown(mx, my); }
void VDSessionFacade::handleMouseDrag(int mx, int my) { mVDSession->handleMouseDrag(mx, my); }
void VDSessionFacade::handleMouseUp  (int mx, int my) { mVDSession->handleMouseUp  (mx, my); }

void VDSessionFacade::fileDrop(const std::string& filePath, unsigned int index) {
	mVDSession->fileDrop(filePath, index);
}
