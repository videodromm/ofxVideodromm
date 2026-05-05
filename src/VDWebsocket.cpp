#include "VDWebsocket.h"

using namespace videodromm;

VDWebsocket::VDWebsocket(VDSettingsRef aVDSettings, VDAnimationRef aVDAnimation, VDUniformsRef aVDUniforms) {
	mVDSettings  = aVDSettings;
	mVDAnimation = aVDAnimation;
	mVDUniforms  = aVDUniforms;
	mIsConnected = false;
	mPingTime    = ofGetElapsedTimef();
	mWSMsg       = "";
}

VDWebsocket::~VDWebsocket() {
	mClient.close();
}

void VDWebsocket::setupWebsocket(VDMediatorObservableRef aVDMediator, const std::string& host, int port) {
	mVDMediator = aVDMediator;
	mWSHost     = host;
	mWSPort     = port;
	mClient.addListener(this);
	wsConnect();
}

void VDWebsocket::wsConnect() {
	mIsConnected = mClient.connect(mWSHost, mWSPort);
	if (mIsConnected) ofLogNotice("VDWebsocket") << "Connected to " << mWSHost << ":" << mWSPort;
	else               ofLogWarning("VDWebsocket") << "Could not connect to " << mWSHost << ":" << mWSPort;
}

void VDWebsocket::update() {
	if (mIsConnected) {
		float now = ofGetElapsedTimef();
		if (now - mPingTime > 10.0f) {
			mClient.send("ping");
			mPingTime = now;
		}
	}
}

void VDWebsocket::wsWrite(const std::string& aMsg) {
	if (mIsConnected) mClient.send(aMsg);
}
void VDWebsocket::onConnect(ofxLibwebsockets::Event & args) {
	ofLogNotice("VDWebsocket") << "on connected" << endl;
}
void VDWebsocket::onOpen(ofxLibwebsockets::Event& args) {
	mIsConnected = true;
	mWSMsg = "WS opened";
	ofLogNotice("VDWebsocket") << mWSMsg;
}
void VDWebsocket::onIdle(ofxLibwebsockets::Event & args) {
	ofLogNotice("VDWebsocket") << "on idle";
}
void VDWebsocket::onClose(ofxLibwebsockets::Event& args) {
	mIsConnected = false;
	mWSMsg = "WS closed";
	ofLogWarning("VDWebsocket") << mWSMsg;
}
//void VDWebsocket::onError(ofxLibwebsockets::Event& args) {
//	mWSMsg = "WS error: " + args.message;
//	ofLogError("VDWebsocket") << mWSMsg;
//}
void VDWebsocket::onMessage(ofxLibwebsockets::Event& args) {
	parseMessage(args.message);
}
void VDWebsocket::onBroadcast(ofxLibwebsockets::Event& args) {
	parseMessage(args.message);
}

void VDWebsocket::parseMessage(const std::string& aMsg) {
	if (aMsg == "pong") { mWSMsg = "WS pong"; return; }
	mWSMsg = "WS rcv: " + aMsg;
	try {
		ofJson json = ofJson::parse(aMsg);
		if (json.contains("params")) {
			for (auto& el : json["params"]) {
				if (el.contains("name") && el.contains("value")) {
					int   idx = el["name"].get<int>();
					float val = el["value"].is_number_float()
					            ? el["value"].get<float>()
					            : (float)el["value"].get<int>();
					mVDMediator->setUniformValue(idx, val);
				}
				// vec4 (e.g. for body joints)
				if (el.contains("vec4")) {
					std::string v4str = el["vec4"].get<std::string>();
					auto parts = ofSplitString(v4str, ",");
					if (parts.size() == 5) {
						int joint = std::atoi(parts[0].c_str());
						float x = std::atof(parts[1].c_str());
						float y = std::atof(parts[2].c_str());
						float z = std::atof(parts[3].c_str());
						float w = std::atof(parts[4].c_str());
						mVDUniforms->setVec4UniformValueByIndex(joint, glm::vec4(x, y, z, w));
					}
				}
			}
		}
		if (json.contains("folder")) {
			std::string folder = json["folder"].get<std::string>();
			std::string folderPath = ofToDataPath(folder);
			if (!ofDirectory::doesDirectoryExist(folderPath))
				ofDirectory::createDirectory(folderPath, false, true);
			mVDSettings->setMsg("WS folder: " + folderPath);
		}
	}
	catch (const nlohmann::json::parse_error& e) {
		ofLogError("VDWebsocket") << "parse error: " << e.what() << " msg=" << aMsg;
	}
}
