#pragma once
#include "ofMain.h"
#include "ofxOsc.h"
#include "VDSettings.h"
#include "VDUniforms.h"
#include "VDMediator.h"

namespace videodromm
{
	class VDOscObserver : public VDUniformObserver {
	public:
		static VDUniformObserverRef connect(const std::string& host, unsigned int port) {
			VDOscObserver* o = new VDOscObserver(host, port);
			VDUniformObserverRef obs(o);
			return obs;
		}
		VDUniformObserverRef setUniformValue(int aIndex, float aValue) override {
			std::string oscMsg, oscMsg2, oscMsg3;
			bool sendIndex    = true;
			bool sendValueOnly = false;
			switch (aIndex) {
			case 11: oscMsg = "/composition/dashboard/link1"; sendIndex = false; break;
			case 12: oscMsg = "/composition/dashboard/link2"; sendIndex = false; break;
			case 13: oscMsg = "/composition/dashboard/link3"; sendIndex = false; break;
			case 14: oscMsg = "/composition/dashboard/link4"; sendIndex = false; break;
			case 18: oscMsg = "/composition/crossfader/phase"; sendIndex = false; sendValueOnly = true; break;
			case 44: break; // IFPS - skip
			case 88: sendIndex = false; break;
			case 251: oscMsg = "/composition/layers/1/clips/1/connect"; sendIndex = false; break;
			case 252: oscMsg = "/composition/layers/2/clips/2/connect"; sendIndex = false; break;
			case 253: oscMsg = "/composition/layers/1/clips/3/connect"; sendIndex = false; break;
			case 254: oscMsg = "/composition/layers/2/clips/4/connect"; oscMsg2 = "/composition/layers/3/clips/4/connect"; sendIndex = false; break;
			case 255: oscMsg = "/composition/layers/1/clips/5/connect"; sendIndex = false; break;
			case 256: oscMsg = "/composition/layers/2/clips/6/connect"; sendIndex = false; break;
			case 257: oscMsg = "/composition/layers/1/clips/7/connect"; sendIndex = false; break;
			case 258: oscMsg = "/composition/layers/2/clips/8/connect"; sendIndex = false; break;
			default:  oscMsg = "/params"; break;
			}
			auto sendMsg = [&](const std::string& addr) {
				ofxOscMessage m;
				m.setAddress(addr);
				if (sendIndex) m.addIntArg(aIndex);
				m.addFloatArg(aValue);
				mSender.sendMessage(m, false);
			};
			if (oscMsg.length() > 1) sendMsg(oscMsg);
			if (oscMsg2.length() > 1) { ofxOscMessage m2; m2.setAddress(oscMsg2); m2.addFloatArg(aValue); mSender.sendMessage(m2, false); }
			if (oscMsg3.length() > 1) { ofxOscMessage m3; m3.setAddress(oscMsg3); m3.addFloatArg(aValue); mSender.sendMessage(m3, false); }
			return shared_from_this();
		}
		bool mIsConnected = false;
		~VDOscObserver() {}
	private:
		VDOscObserver(const std::string& host, unsigned int port) {
			mSender.setup(host, port);
			mIsConnected = true;
		}
		ofxOscSender mSender;
	};
}
