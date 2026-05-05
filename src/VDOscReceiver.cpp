#include "VDOscReceiver.h"

using namespace videodromm;

VDOscReceiver::VDOscReceiver(VDSettingsRef aVDSettings, VDAnimationRef aVDAnimation, VDUniformsRef aVDUniforms) {
	mVDSettings  = aVDSettings;
	mVDAnimation = aVDAnimation;
	mVDUniforms  = aVDUniforms;
	mOSCMsg = "";
}

void VDOscReceiver::setupOSCReceiver(VDMediatorObservableRef aVDMediator, int aOSCReceiverPort) {
	mVDMediator = aVDMediator;
	mOscReceiver.setup(aOSCReceiverPort);
	ofLogNotice("VDOscReceiver") << "OSC receiver on port " << aOSCReceiverPort;
}

void VDOscReceiver::update() {
	while (mOscReceiver.hasWaitingMessages()) {
		ofxOscMessage m;
		mOscReceiver.getNextMessage(m);
		processMessage(m);
	}
}

void VDOscReceiver::processMessage(const ofxOscMessage& msg) {
	bool found = false;
	std::string ctrl;
	int index = -1;
	int i = 0;
	float f = 1.0f;
	std::stringstream ss;
	std::string addr = msg.getAddress();

	// /cc  (midi cc via osc)
	ctrl = "/cc";
	if (addr.find(ctrl) != std::string::npos) {
		found = true;
		i = msg.getArgAsInt(0);
		f = msg.getArgAsFloat(1);
		mVDMediator->setUniformValue(i, f);
	}

	if (!found) {
		ctrl = "grab";
		if (addr.find(ctrl) != std::string::npos) {
			found = true;
			f = msg.getArgAsFloat(0);
			int lastSlash = addr.find_last_of("/");
			int iu = std::atoi(addr.substr(lastSlash + 1).c_str());
			if (iu == mVDUniforms->IMULT) f *= 4.0f;
			if (iu == mVDUniforms->IZOOM) f *= mVDUniforms->getUniformValue(mVDUniforms->IMULT);
			mVDMediator->setUniformValue(iu, f);
		}
	}

	if (!found) {
		ctrl = "accxyz";
		if (addr.find(ctrl) != std::string::npos) { found = true; }
	}

	if (!found) {
		ctrl = "/Note1";
		if (addr.find(ctrl) != std::string::npos) {
			found = true;
			mNote = msg.getArgAsInt(0);
			if (mVelocity > 0 && mNote > 109) {
				if (mVDUniforms->getUniformValue(mVDUniforms->ITRACK) == (float)mNote)
					mVDMediator->setUniformValue(mVDUniforms->IBARSTART, mVDUniforms->getUniformValue(mVDUniforms->IBAR));
				else
					mVDMediator->setUniformValue(mVDUniforms->ITRACK, (float)mNote);
			}
		}
	}

	if (!found) {
		ctrl = "/Velocity";
		if (addr.find(ctrl) != std::string::npos) {
			found = true;
			mVelocity = msg.getArgAsInt(0);
		}
	}

	if (!found) {
		ctrl = "/live/tempo";
		if (addr.find(ctrl) != std::string::npos) {
			found = true;
			mVDMediator->setUniformValue(mVDUniforms->IBPM, msg.getArgAsFloat(0));
		}
	}

	if (!found) {
		ctrl = "/play";
		if (addr.find(ctrl) != std::string::npos) { found = true; }
	}

	if (!found) {
		ctrl = "/ticks";
		if (addr.find(ctrl) != std::string::npos) {
			found = true;
			f = msg.getArgAsFloat(0);
			mVDAnimation->useTimeWithTempo();
			mVDMediator->setUniformValue(mVDUniforms->ITIME, f);
		}
	}

	if (!found) {
		ctrl = "/tempo";
		if (addr.find(ctrl) != std::string::npos) {
			found = true;
			f = msg.getArgAsFloat(0);
			mVDAnimation->useTimeWithTempo();
			mVDMediator->setUniformValue(mVDUniforms->IBPM, f);
		}
	}

	if (!found) {
		ctrl = "/Spectrum";
		if (addr.find(ctrl) != std::string::npos) {
			found = true;
			mVDMediator->setUniformValue(mVDUniforms->IMAXVOLUME, 0.0f);
			for (int a = 0; a < msg.getNumArgs(); a++) {
				f = msg.getArgAsFloat(a) * 200.0f * mVDUniforms->getUniformValue(mVDUniforms->IAUDIOX);
				if (f > mVDUniforms->getUniformValue(mVDUniforms->IMAXVOLUME))
					mVDMediator->setUniformValue(mVDUniforms->IMAXVOLUME, f);
				mVDAnimation->iFreqs[a] = f;
				if (a == mVDAnimation->getFreqIndex(0)) mVDMediator->setUniformValue(mVDUniforms->IFREQ0, f);
				if (a == mVDAnimation->getFreqIndex(1)) mVDMediator->setUniformValue(mVDUniforms->IFREQ1, f);
				if (a == mVDAnimation->getFreqIndex(2)) mVDMediator->setUniformValue(mVDUniforms->IFREQ2, f);
				if (a == mVDAnimation->getFreqIndex(3)) mVDMediator->setUniformValue(mVDUniforms->IFREQ3, f);
			}
		}
	}

	if (!found) {
		ctrl = "/beat";
		if (addr.find(ctrl) != std::string::npos) {
			found = true;
			mVDMediator->setUniformValue(mVDUniforms->IBEAT, (float)msg.getArgAsInt(0) - 1);
		}
	}

	if (!found) {
		ctrl = "/bar";
		if (addr.find(ctrl) != std::string::npos) {
			found = true;
			mVDMediator->setUniformValue(mVDUniforms->IBAR, (float)msg.getArgAsInt(0));
		}
	}

	if (!found) {
		ctrl = "link";
		if (addr.find(ctrl) != std::string::npos) {
			found = true;
			mVDMediator->setUniformValue(mVDUniforms->IBPM, msg.getArgAsFloat(0));
		}
	}

	// Azure Kinect body joints
	if (!found) {
		ctrl = "kb";
		if (addr.find(ctrl) != std::string::npos) {
			try {
				int lastSlash = addr.find_last_of("/");
				found = true;
				f = msg.getArgAsFloat(0);
				float posY = msg.getArgAsFloat(1);
				float posZ = msg.getArgAsFloat(2);
				i = msg.getArgAsInt(3);
				int joint = std::stoi(addr.substr(lastSlash + 1));
				mVDUniforms->setVec4UniformValueByIndex(joint, glm::vec4(f, posY, posZ, i));
				if (joint == 227) {
					mVDMediator->setUniformValue(mVDUniforms->IFREQ0, f);
					mVDMediator->setUniformValue(mVDUniforms->IFREQ1, posY);
					mVDMediator->setUniformValue(mVDUniforms->IFREQ2, posZ);
					mVDMediator->setUniformValue(mVDUniforms->IFREQ3, i);
					mNote = joint;
					mVelocity = (int)posY;
				}
			}
			catch (const std::exception& e) {
				mVDSettings->setErrorMsg("kb not integer: " + addr);
			}
		}
	}

	// TouchOSC pages
	if (!found) {
		try {
			int page = std::stoi(addr.substr(1, 1));
			if (addr.length() > 2) {
				int lastSlash = addr.find_last_of("/");
				if (!found) {
					ctrl = "multifader1";
					if (addr.find(ctrl) != std::string::npos) {
						found = true; f = msg.getArgAsFloat(0);
						i = std::stoi(addr.substr(lastSlash + 1)) + 8;
						mVDMediator->setUniformValue(i, f);
					}
				}
				if (!found) {
					ctrl = "multifader2";
					if (addr.find(ctrl) != std::string::npos) {
						found = true; f = msg.getArgAsFloat(0);
						i = std::stoi(addr.substr(lastSlash + 1)) + 32;
						mVDMediator->setUniformValue(i, f);
					}
				}
				if (!found) {
					ctrl = "multifader";
					if (addr.find(ctrl) != std::string::npos) {
						found = true; f = msg.getArgAsFloat(0);
						i = std::stoi(addr.substr(lastSlash + 1)) + 56;
						mVDMediator->setUniformValue(i, f);
					}
				}
				if (!found) {
					ctrl = "fader";
					size_t idx = addr.find(ctrl);
					if (idx != std::string::npos) {
						found = true; f = msg.getArgAsFloat(0);
						i = std::stoi(addr.substr(idx + ctrl.length()));
						mVDMediator->setUniformValue(i, f);
					}
				}
				if (!found) {
					ctrl = "rotary";
					size_t idx = addr.find(ctrl);
					if (idx != std::string::npos) {
						found = true; f = msg.getArgAsFloat(0);
						i = std::stoi(addr.substr(idx + ctrl.length())) + 10;
						mVDMediator->setUniformValue(i, f);
					}
				}
				if (!found) {
					ctrl = "toggle";
					size_t idx = addr.find(ctrl);
					if (idx != std::string::npos) {
						found = true; f = msg.getArgAsFloat(0);
						i = std::stoi(addr.substr(idx + ctrl.length())) + 80;
						mVDAnimation->toggleValue(i);
					}
				}
				if (!found) {
					ctrl = "push";
					size_t idx = addr.find(ctrl);
					if (idx != std::string::npos) {
						found = true; f = msg.getArgAsFloat(0);
						i = std::stoi(addr.substr(idx + ctrl.length())) + 80;
						mVDAnimation->toggleValue(i);
					}
				}
			}
		}
		catch (const std::exception&) {
			// addr doesn't start with page integer, ok
		}
	}

	if (!found) {
		mVDSettings->setMsg("osc not handled: " + addr);
	}
	if (addr != "/play" && addr != "/Spectrum" && addr != "/bar" && addr != "/beat" && addr != "/tempo" && addr != "/ticks") {
		ss << addr << " f:" << f << " i:" << i << " note:" << mNote << " velocity:" << mVelocity;
		mOSCMsg = ss.str();
	}
}
