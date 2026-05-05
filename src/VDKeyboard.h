#pragma once
#include "ofMain.h"
#include "VDSettings.h"
#include "VDAnimation.h"
#include "VDUniforms.h"
#include "VDMediator.h"

namespace videodromm
{
	class VDMediatorObservable;
	typedef std::shared_ptr<VDMediatorObservable> VDMediatorObservableRef;

	typedef std::shared_ptr<class VDKeyboard> VDKeyboardRef;
	class VDKeyboard {
	public:
		VDKeyboard(VDSettingsRef aVDSettings, VDAnimationRef aVDAnimation, VDUniformsRef aVDUniforms);
		static VDKeyboardRef create(VDSettingsRef aVDSettings, VDAnimationRef aVDAnimation, VDUniformsRef aVDUniforms) {
			return std::make_shared<VDKeyboard>(aVDSettings, aVDAnimation, aVDUniforms);
		}
		void setupKeyboard(VDMediatorObservableRef aVDMediator);
		bool handleKeyDown(int key, bool ctrlDown, bool shiftDown, bool altDown);
		bool handleKeyUp(int key, bool ctrlDown, bool shiftDown, bool altDown);
	private:
		VDSettingsRef           mVDSettings;
		VDAnimationRef          mVDAnimation;
		VDUniformsRef           mVDUniforms;
		VDMediatorObservableRef mVDMediator;
	};
}
