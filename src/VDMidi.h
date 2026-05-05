#pragma once
#include "ofMain.h"
#include "ofxMidi.h"
#include "VDUniforms.h"
#include "VDMediator.h"

namespace videodromm
{
	class VDMediatorObservable;
	typedef std::shared_ptr<VDMediatorObservable> VDMediatorObservableRef;

	struct midiInput  { std::string portName; bool isConnected; };
	struct midiOutput { std::string portName; bool isConnected; };

	typedef std::shared_ptr<class VDMidi> VDMidiRef;
	class VDMidi : public ofxMidiListener {
	public:
		VDMidi(VDUniformsRef aVDUniforms);
		~VDMidi();
		static VDMidiRef create(VDUniformsRef aVDUniforms) {
			return std::make_shared<VDMidi>(aVDUniforms);
		}
		void setupMidi(VDMediatorObservableRef aVDMediator);
		void newMidiMessage(ofxMidiMessage& msg) override;
		void setMidiMsg(const std::string& aMsg) { mMidiMsg = aMsg; }
		std::string getMidiMsg() { return mMidiMsg; }
		void saveMidiPorts();
		int         getMidiInPortsCount()          { return (int)mMidiInputs.size(); }
		std::string getMidiInPortName(int i)        { return (i < (int)mMidiInputs.size()) ? mMidiInputs[i].portName : "No midi in ports"; }
		bool        isMidiInConnected(int i)        { return (i < (int)mMidiInputs.size()) ? mMidiInputs[i].isConnected : false; }
		int         getMidiOutPortsCount()          { return (int)mMidiOutputs.size(); }
		std::string getMidiOutPortName(int i)       { return (i < (int)mMidiOutputs.size()) ? mMidiOutputs[i].portName : "No midi out ports"; }
		bool        isMidiOutConnected(int i)       { return (i < (int)mMidiOutputs.size()) ? mMidiOutputs[i].isConnected : false; }
		void        midiOutSendNoteOn(int i, int channel, int pitch, int velocity);
		void        openMidiInPort(int i);
		void        closeMidiInPort(int i);
		void        openMidiOutPort(int i);
		void        closeMidiOutPort(int i);
	private:
		VDUniformsRef           mVDUniforms;
		VDMediatorObservableRef mVDMediator;
		std::vector<midiInput>  mMidiInputs;
		std::vector<midiOutput> mMidiOutputs;
		ofxMidiIn               mMidiIn0, mMidiIn1, mMidiIn2, mMidiIn3;
		ofxMidiOut              mMidiOut0, mMidiOut1, mMidiOut2, mMidiOut3;
		std::string             mMidiMsg;
		std::string             midiControlType;
		int   midiControl = 0, midiPitch = 0, midiVelocity = 0;
		float midiNormalizedValue = 0.0f;
		int   midiValue = 0, midiChannel = 0;
		bool  midiWeights = true;
		void midiSetup();
	};
}
