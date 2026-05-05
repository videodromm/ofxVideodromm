#include "VDMidi.h"

using namespace videodromm;

VDMidi::VDMidi(VDUniformsRef aVDUniforms) {
	mVDUniforms = aVDUniforms;
	mMidiMsg = "";
}
VDMidi::~VDMidi() {
	mMidiIn0.closePort(); mMidiIn1.closePort(); mMidiIn2.closePort(); mMidiIn3.closePort();
	mMidiOut0.closePort(); mMidiOut1.closePort(); mMidiOut2.closePort(); mMidiOut3.closePort();
}

void VDMidi::setupMidi(VDMediatorObservableRef aVDMediator) {
	mVDMediator = aVDMediator;
	midiSetup();
}

void VDMidi::midiSetup() {
	std::stringstream ss;
	int numPorts = mMidiIn0.getNumInPorts();
	if (numPorts > 0) {
		for (int i = 0; i < numPorts; i++) {
			midiInput mIn;
			mIn.portName = mMidiIn0.getInPortName(i);
			mIn.isConnected = false;
			bool already = false;
			for (auto& inp : mMidiInputs) if (inp.portName == mIn.portName) { already = true; break; }
			if (!already) {
				mMidiInputs.push_back(mIn);
				std::size_t nameIdx = mMidiInputs.back().portName.find(mVDMediator->getPreferredMidiInputDevice());
				if (nameIdx != std::string::npos) {
					openMidiInPort(i);
					mMidiInputs.back().isConnected = true;
					ss << "Opening MIDI in " << i << " " << mMidiInputs.back().portName;
				} else {
					ss << "Available MIDI in " << i << " " << mIn.portName;
				}
			}
		}
	} else {
		ss << "no midi in ports";
	}
	int numOut = mMidiOut0.getNumOutPorts();
	if (numOut > 0) {
		for (int i = 0; i < numOut; i++) {
			midiOutput mOut;
			mOut.portName = mMidiOut0.getOutPortName(i);
			mOut.isConnected = false;
			bool already = false;
			for (auto& out : mMidiOutputs) if (out.portName == mOut.portName) { already = true; break; }
			if (!already) {
				mMidiOutputs.push_back(mOut);
				ss << " Available MIDI out " << i << " " << mOut.portName;
			}
		}
	} else {
		ss << " no midi out ports";
	}
	saveMidiPorts();
	midiControlType = "none";
	midiControl = midiPitch = midiVelocity = midiValue = midiChannel = 0;
	midiNormalizedValue = 0.0f;
	mMidiMsg = ss.str() + "\n";
}

void VDMidi::saveMidiPorts() {
	ofJson json;
	json["midiin"] = ofJson::array();
	for (auto& p : mMidiInputs)  json["midiin"].push_back(p.portName);
	json["midiout"] = ofJson::array();
	for (auto& p : mMidiOutputs) json["midiout"].push_back(p.portName);
	ofSaveJson(ofToDataPath("midi.json"), json);
}

void VDMidi::openMidiInPort(int i) {
	if (i < mMidiIn0.getNumInPorts()) {
		if (i == 0) { mMidiIn0.openPort(i); mMidiIn0.addListener(this); }
		else if (i == 1) { mMidiIn1.openPort(i); mMidiIn1.addListener(this); }
		else if (i == 2) { mMidiIn2.openPort(i); mMidiIn2.addListener(this); }
		else if (i == 3) { mMidiIn3.openPort(i); mMidiIn3.addListener(this); }
		if (i < (int)mMidiInputs.size()) mMidiInputs[i].isConnected = true;
	}
}
void VDMidi::closeMidiInPort(int i) {
	if (i == 0) mMidiIn0.closePort();
	else if (i == 1) mMidiIn1.closePort();
	else if (i == 2) mMidiIn2.closePort();
	else if (i == 3) mMidiIn3.closePort();
	if (i < (int)mMidiInputs.size()) mMidiInputs[i].isConnected = false;
}
void VDMidi::midiOutSendNoteOn(int i, int channel, int pitch, int velocity) {
	if (i == 0 && (int)mMidiOutputs.size() > 0 && mMidiOutputs[0].isConnected) mMidiOut0.sendNoteOn(channel, pitch, velocity);
	else if (i == 1 && (int)mMidiOutputs.size() > 1 && mMidiOutputs[1].isConnected) mMidiOut1.sendNoteOn(channel, pitch, velocity);
	else if (i == 2 && (int)mMidiOutputs.size() > 2 && mMidiOutputs[2].isConnected) mMidiOut2.sendNoteOn(channel, pitch, velocity);
}
void VDMidi::openMidiOutPort(int i) {
	std::stringstream ss;
	if (i < (int)mMidiOutputs.size()) {
		bool ok = false;
		if (i == 0) ok = mMidiOut0.openPort(i);
		else if (i == 1) ok = mMidiOut1.openPort(i);
		else if (i == 2) ok = mMidiOut2.openPort(i);
		mMidiOutputs[i].isConnected = ok;
		ss << (ok ? "Opened" : "Can't open") << " MIDI out " << i;
	}
	mMidiMsg = ss.str() + "\n";
}
void VDMidi::closeMidiOutPort(int i) {
	if (i == 0) mMidiOut0.closePort();
	else if (i == 1) mMidiOut1.closePort();
	else if (i == 2) mMidiOut2.closePort();
	if (i < (int)mMidiOutputs.size()) mMidiOutputs[i].isConnected = false;
}

void VDMidi::newMidiMessage(ofxMidiMessage& msg) {
	std::stringstream ss;
	midiChannel = msg.channel;
	switch (msg.status) {
	case MIDI_CONTROL_CHANGE:
		midiControlType = "/cc";
		midiControl = msg.control;
		midiValue   = msg.value;
		midiNormalizedValue = ofMap(midiValue, 0.0f, 127.0f, 0.0f, 1.0f);
		ss << " U:" << mVDUniforms->getUniformName(midiControl) << " cc Ch:" << midiChannel << " CC:" << midiControl << " Val:" << midiValue << " NVal:" << midiNormalizedValue;
		if (midiWeights && midiControl > 0 && midiControl < 9) midiControl += 30;
		if (midiNormalizedValue > 0.1f) {
			if (midiControl == 60) mVDMediator->setUniformValue(mVDUniforms->IBLENDMODE, 0.0f);
			if (midiControl == 61) {
				if (mVDUniforms->getUniformValue(mVDUniforms->IBLENDMODE) == 0)
					mVDMediator->setUniformValue(mVDUniforms->IBLENDMODE, mVDUniforms->getMaxUniformValue(mVDUniforms->IBLENDMODE));
				else
					mVDMediator->setUniformValue(mVDUniforms->IBLENDMODE, std::max(mVDUniforms->getUniformValue(mVDUniforms->IBLENDMODE) - 1.0f, 0.0f));
			}
			if (midiControl == 62) {
				if (mVDUniforms->getMaxUniformValue(mVDUniforms->IBLENDMODE) == mVDUniforms->getUniformValue(mVDUniforms->IBLENDMODE))
					mVDMediator->setUniformValue(mVDUniforms->IBLENDMODE, 0.0f);
				else
					mVDMediator->setUniformValue(mVDUniforms->IBLENDMODE, std::max(mVDUniforms->getUniformValue(mVDUniforms->IBLENDMODE) + 1.0f, 0.0f));
			}
		}
		mVDMediator->setUniformValue(midiControl, midiNormalizedValue);
		break;
	case MIDI_NOTE_ON:
		midiControlType = "/on";
		midiPitch = msg.pitch;
		if (midiPitch < 7) mVDUniforms->setUniformValue(mVDUniforms->IWEIGHT0 + midiPitch, 1.0f);
		if (midiPitch > 7 && midiPitch < 16) {
			int uIdx = midiPitch + 243;
			if (mVDUniforms->getUniformValue(uIdx) == 0.0f) mVDMediator->setUniformValue(uIdx, 1.0f);
		}
		if (midiPitch > 15 && midiPitch < 24) mVDMediator->setUniformValue(midiPitch + 65, 1.0f);
		if (midiPitch == 7) midiWeights = false;
		ss << " noteon Ch:" << midiChannel << " Pitch:" << midiPitch;
		break;
	case MIDI_NOTE_OFF:
		midiPitch = msg.pitch;
		if (midiPitch > 15 && midiPitch < 24) mVDMediator->setUniformValue(midiPitch + 65, 0.0f);
		if (midiPitch == 7) midiWeights = true;
		if (midiPitch < 7) mVDUniforms->setUniformValue(mVDUniforms->IWEIGHT0 + midiPitch, 0.0f);
		ss << " noteoff Ch:" << midiChannel << " Pitch:" << midiPitch;
		midiControlType = "/off";
		break;
	case MIDI_PITCH_BEND:
		midiControlType = "/pb";
		midiControl = msg.control;
		midiValue   = msg.value;
		midiNormalizedValue = ofMap(midiValue, 0.0f, 127.0f, 0.0f, 1.0f);
		ss << " pb Ch:" << midiChannel << " CC:" << midiControl << " Val:" << midiValue;
		mVDMediator->setUniformValue(mVDUniforms->IMOUSEX, (float)midiValue);
		break;
	default:
		ss << " unknown status:" << msg.status;
		break;
	}
	ss << std::endl;
	mMidiMsg = ss.str();
}
