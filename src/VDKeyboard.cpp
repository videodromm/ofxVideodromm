#include "VDKeyboard.h"

using namespace videodromm;

VDKeyboard::VDKeyboard(VDSettingsRef aVDSettings, VDAnimationRef aVDAnimation, VDUniformsRef aVDUniforms) {
	mVDSettings  = aVDSettings;
	mVDAnimation = aVDAnimation;
	mVDUniforms  = aVDUniforms;
}

void VDKeyboard::setupKeyboard(VDMediatorObservableRef aVDMediator) {
	mVDMediator = aVDMediator;
}

bool VDKeyboard::handleKeyDown(int key, bool ctrlDown, bool shiftDown, bool altDown) {
	bool handled = true;
	float delta = 0.0f;
	if (shiftDown) delta =  0.05f;
	if (ctrlDown)  delta = -0.05f;

	switch (key) {
	case 'm':
		if (shiftDown) mVDMediator->setUniformValue(mVDUniforms->IBLENDMODE, std::max(mVDUniforms->getUniformValue(mVDUniforms->IBLENDMODE) + 1.0f, 0.0f));
		else if (ctrlDown) mVDMediator->setUniformValue(mVDUniforms->IBLENDMODE, std::max(mVDUniforms->getUniformValue(mVDUniforms->IBLENDMODE) - 1.0f, 0.0f));
		break;
	case 'x':
		mVDMediator->setUniformValue(mVDUniforms->ITRIXELS, std::max(mVDUniforms->getUniformValue(mVDUniforms->ITRIXELS) + delta, 0.0f));
		break;
	case 'r':
		if (altDown) mVDMediator->setUniformValue(mVDUniforms->IBACKGROUNDCOLORX, std::max(mVDUniforms->getUniformValue(mVDUniforms->IBACKGROUNDCOLORX) + delta, 0.0f));
		else mVDMediator->setUniformValue(mVDUniforms->ICOLORX, std::max(mVDUniforms->getUniformValue(mVDUniforms->ICOLORX) + delta, 0.0f));
		break;
	case 'g':
		if (altDown) mVDMediator->setUniformValue(mVDUniforms->IBACKGROUNDCOLORY, std::max(mVDUniforms->getUniformValue(mVDUniforms->IBACKGROUNDCOLORY) + delta, 0.0f));
		else mVDMediator->setUniformValue(mVDUniforms->ICOLORY, std::max(mVDUniforms->getUniformValue(mVDUniforms->ICOLORY) + delta, 0.0f));
		break;
	case 'b':
		if (altDown) mVDMediator->setUniformValue(mVDUniforms->IBACKGROUNDCOLORZ, std::max(mVDUniforms->getUniformValue(mVDUniforms->IBACKGROUNDCOLORZ) + delta, 0.0f));
		else mVDMediator->setUniformValue(mVDUniforms->ICOLORZ, std::max(mVDUniforms->getUniformValue(mVDUniforms->ICOLORZ) + delta, 0.0f));
		break;
	case 'a':
		mVDMediator->setUniformValue(mVDUniforms->IALPHA, std::max(mVDUniforms->getUniformValue(mVDUniforms->IALPHA) + delta, 0.0f));
		break;
	case 'u': {
		float newVal = std::max(mVDUniforms->getUniformValue(mVDUniforms->ICHROMATIC) + delta, 0.0f);
		if (newVal > 1.0f) newVal = 0.0f;
		mVDMediator->setUniformValue(mVDUniforms->ICHROMATIC, newVal);
		break;
	}
	case 'p':
		mVDMediator->setUniformValue(mVDUniforms->IPIXELATE, std::max(mVDUniforms->getUniformValue(mVDUniforms->IPIXELATE) + delta, 0.0f));
		break;
	case 'y':
		mVDMediator->setUniformValue(mVDUniforms->IGLITCH, 1.0f);
		break;
	case 'i':
		mVDMediator->setUniformValue(mVDUniforms->IINVERT, 1.0f);
		break;
	case 'o':
		mVDMediator->setUniformValue(mVDUniforms->ITOGGLE, 1.0f);
		break;
	case 'z':
		mVDMediator->setUniformValue(mVDUniforms->IZOOM, std::max(mVDUniforms->getUniformValue(mVDUniforms->IZOOM) + delta, 0.0f));
		break;
	case OF_KEY_PAGE_DOWN:
	case OF_KEY_RIGHT:
		if (mVDUniforms->getUniformValue(mVDUniforms->IXFADE) < 1.0f)
			mVDMediator->setUniformValue(mVDUniforms->IXFADE, mVDUniforms->getUniformValue(mVDUniforms->IXFADE) + 0.01f);
		break;
	case OF_KEY_PAGE_UP:
	case OF_KEY_LEFT:
		if (mVDUniforms->getUniformValue(mVDUniforms->IXFADE) > 0.0f)
			mVDMediator->setUniformValue(mVDUniforms->IXFADE, mVDUniforms->getUniformValue(mVDUniforms->IXFADE) - 0.01f);
		break;
	case OF_KEY_F1: mVDMediator->setUniformValue(mVDUniforms->IDISPLAYMODE, VDDisplayMode::POST);     break;
	case OF_KEY_F2: mVDMediator->setUniformValue(mVDUniforms->IDISPLAYMODE, VDDisplayMode::MIXETTE);  break;
	case OF_KEY_F3: mVDMediator->setUniformValue(mVDUniforms->IDISPLAYMODE, VDDisplayMode::FX);       break;
	case OF_KEY_F4: mVDMediator->setUniformValue(mVDUniforms->IDISPLAYMODE, VDDisplayMode::FBO0);     break;
	case OF_KEY_F5: mVDMediator->setUniformValue(mVDUniforms->IDISPLAYMODE, VDDisplayMode::FBO1);     break;
	case OF_KEY_F6: mVDMediator->setUniformValue(mVDUniforms->IDISPLAYMODE, VDDisplayMode::FBO2);     break;
	case OF_KEY_F7: mVDMediator->setUniformValue(mVDUniforms->IDISPLAYMODE, VDDisplayMode::FBO3);     break;
	case OF_KEY_F8: mVDMediator->setUniformValue(mVDUniforms->IDISPLAYMODE, VDDisplayMode::FBO4);     break;
	case OF_KEY_F9: mVDMediator->setUniformValue(mVDUniforms->IDISPLAYMODE, VDDisplayMode::FBO5);     break;
	case OF_KEY_F10: mVDMediator->setUniformValue(mVDUniforms->IDISPLAYMODE, VDDisplayMode::WARP);    break;
	default:
		handled = false;
		break;
	}
	return handled;
}

bool VDKeyboard::handleKeyUp(int key, bool ctrlDown, bool shiftDown, bool altDown) {
	bool handled = true;
	switch (key) {
	case 'y': mVDMediator->setUniformValue(mVDUniforms->IGLITCH,   0.0f); break;
	case 't': mVDMediator->setUniformValue(mVDUniforms->ITRIXELS,  0.0f); break;
	case 'i': mVDMediator->setUniformValue(mVDUniforms->IINVERT,   0.0f); break;
	case 'u': mVDMediator->setUniformValue(mVDUniforms->ICHROMATIC,0.0f); break;
	case 'p': mVDMediator->setUniformValue(mVDUniforms->IPIXELATE, 1.0f); break;
	case 'o': mVDMediator->setUniformValue(mVDUniforms->ITOGGLE,   0.0f); break;
	case 'z': mVDMediator->setUniformValue(mVDUniforms->IZOOM,     1.0f); break;
	default:  handled = false; break;
	}
	return handled;
}
