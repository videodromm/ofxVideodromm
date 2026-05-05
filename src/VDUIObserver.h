#pragma once
#include "ofMain.h"
#include "VDSettings.h"
#include "VDUniforms.h"
#include "VDMediator.h"

namespace videodromm
{
	class VDUIObserver : public VDUniformObserver {
	public:
		static VDUniformObserverRef connect(VDSettingsRef aVDSettings, VDUniformsRef aVDUniforms) {
			VDUIObserver* o = new VDUIObserver(aVDSettings, aVDUniforms);
			return VDUniformObserverRef(o);
		}
		VDUniformObserverRef setUniformValue(int aIndex, float aValue) override {
			mVDUniforms->setUniformValue(aIndex, aValue);
			return shared_from_this();
		}
		bool mIsConnected = false;
		~VDUIObserver() {}
	private:
		VDUIObserver(VDSettingsRef aVDSettings, VDUniformsRef aVDUniforms) {
			mVDSettings = aVDSettings;
			mVDUniforms = aVDUniforms;
		}
		VDSettingsRef mVDSettings;
		VDUniformsRef mVDUniforms;
	};
}
