#include "VDSession.h"

using namespace videodromm;

VDSession::VDSession(VDSettingsRef aVDSettings, VDAnimationRef aVDAnimation, VDUniformsRef aVDUniforms, VDMixRef aVDMix) {
	mVDSettings  = aVDSettings;
	mVDAnimation = aVDAnimation;
	mVDUniforms  = aVDUniforms;
	mVDMix       = aVDMix;
	// Params
	mVDParams = VDParams::create();

	mModesList[0] = "Mixette";
	mModesList[1] = "Post";
	mModesList[2] = "Fx";
	mModesList[3] = "Warp";

	// Allocate FBOs
	ofFbo::Settings fboSettings;
	fboSettings.width = mVDParams->getFboWidth();
	fboSettings.height = mVDParams->getFboHeight();
	fboSettings.internalformat  = GL_RGBA;
	fboSettings.numSamples      = 0;
	//fboSettings.useDepthBuffer  = false;
	//fboSettings.useStencilBuffer = false;
	mPostFbo.allocate(fboSettings);
	mFxFbo.allocate(fboSettings);

	// Load post shader
	mAssetsFolder = mVDMix->getAssetsPath();
	std::string postVert = ofBufferFromFile(ofToDataPath(mAssetsFolder + "/passthrough.vs")).getText();
	std::string postFrag = ofBufferFromFile(ofToDataPath(mAssetsFolder + "/post.glsl")).getText();
	if (!postVert.empty() && !postFrag.empty()) {
		mGlslPost.setupShaderFromSource(GL_VERTEX_SHADER,   postVert);
		mGlslPost.setupShaderFromSource(GL_FRAGMENT_SHADER, postFrag);
		mGlslPost.linkProgram();
	}
	std::string fxFrag = ofBufferFromFile(ofToDataPath(mAssetsFolder + "/fx.glsl")).getText();
	if (!postVert.empty() && !fxFrag.empty()) {
		mGlslFx.setupShaderFromSource(GL_VERTEX_SHADER,   postVert);
		mGlslFx.setupShaderFromSource(GL_FRAGMENT_SHADER, fxFrag);
		mGlslFx.linkProgram();
	}

	// Load default folder
	loadFolder(mAssetsFolder);
}

void VDSession::update() {
	mVDAnimation->update();
	mVDMix->getMixetteTexture(0);
	renderPostToFbo();
	// if (mVDUniforms->getUniformValue(mVDUniforms->IFXVISIBLE) > 0.5f)
	// CI	if (mVDUniforms->getUniformValue(mVDUniforms->IDISPLAYMODE) == VDDisplayMode::FX || getElapsedFrames() % 100 == 0) renderFxToFbo();

		renderFxToFbo();
}

void VDSession::draw() {}

void VDSession::renderPostToFbo() {
	mPostFbo.begin();
	ofClear(0, 0, 0, 255);
	if (mGlslPost.isLoaded()) {
		mGlslPost.begin();
		ofTexture* mixTex = mVDMix->getMixetteTexture(0);
		if (mixTex) mixTex->bind(0);
		mGlslPost.setUniform1i("tex0", 0);
		mGlslPost.setUniform1f("iTime",        mVDUniforms->getUniformValue(mVDUniforms->ITIME));
		mGlslPost.setUniform2f("iResolution",  (float)mVDParams->getFboWidth(), (float)mVDParams->getFboHeight());
		mGlslPost.setUniform1f("iGlitch",      mVDUniforms->getUniformValue(mVDUniforms->IGLITCH));
		mGlslPost.setUniform1f("iChromatic",   mVDUniforms->getUniformValue(mVDUniforms->ICHROMATIC));
		mGlslPost.setUniform1f("iPixelate",    mVDUniforms->getUniformValue(mVDUniforms->IPIXELATE));
		ofDrawRectangle(0, 0, mVDParams->getFboWidth(), mVDParams->getFboHeight());
		if (mixTex) mixTex->unbind();
		mGlslPost.end();
	}
	mPostFbo.end();
}

void VDSession::renderFxToFbo() {
	mFxFbo.begin();
	ofClear(0, 0, 0, 255);
	if (mGlslFx.isLoaded()) {
		mGlslFx.begin();
		mPostFbo.getTexture().bind(0);
		mGlslFx.setUniform1i("tex0", 0);
		mGlslFx.setUniform1f("iTime", mVDUniforms->getUniformValue(mVDUniforms->ITIME));
		mGlslFx.setUniform2f("iResolution", (float)mVDParams->getFboWidth(), (float)mVDParams->getFboHeight());
		ofDrawRectangle(0, 0, mVDParams->getFboWidth(), mVDParams->getFboHeight());
		mPostFbo.getTexture().unbind();
		mGlslFx.end();
	}
	mFxFbo.end();
}

ofTexture* VDSession::getFboRenderedTexture(unsigned int idx) { return mVDMix->getFboTexture(idx); }
ofTexture* VDSession::getFboTexture(unsigned int idx)         { return mVDMix->getFboTexture(idx); }
ofTexture* VDSession::getMixetteTexture(unsigned int idx)     { return mVDMix->getMixetteTexture(idx); }
ofTexture* VDSession::getPostFboTexture()                     { return &mPostFbo.getTexture(); }
ofTexture* VDSession::getFxFboTexture()                       { return &mFxFbo.getTexture(); }
ofTexture* VDSession::getAudioTexture()                       { return &mVDAnimation->getAudioTexture(); }

bool VDSession::setFragmentShaderString(const std::string& s, const std::string& name, unsigned int index) {
	return mVDMix->setFragmentShaderString(s, name, index);
}
int VDSession::loadFragmentShader(const std::string& path, unsigned int index) {
	return mVDMix->loadFragmentShader(path, index);
}
void VDSession::loadImageFile(const std::string& path, unsigned int index) {
	mVDMix->loadImageFile(path, index);
}
void VDSession::loadVideoFile(const std::string& path, unsigned int index) {
	mVDMix->loadVideoFile(path, index);
}
void VDSession::loadAudioFile(const std::string& /*path*/) {
	ofLogWarning("VDSession") << "loadAudioFile not supported in OF addon";
}

unsigned int VDSession::createFboShaderTexture(const ofJson& aFboJson, unsigned int aFboIndex, const std::string& aFolder) {
	return mVDMix->createFboShaderTexture(aFboJson, aFboIndex, aFolder);
}

void VDSession::loadFromJsonFile(const std::string& path) {
	if (!ofFile::doesFileExist(path)) return;
	try {
		ofJson json = ofLoadJson(path);
		fboFromJson(json);
	}
	catch (const std::exception& e) {
		ofLogError("VDSession") << "loadFromJsonFile error: " << e.what();
	}
}

void VDSession::fboFromJson(const ofJson& json) {
	if (!json.is_object()) return;
	if (json.contains("fbos")) {
		for (auto& fboJson : json["fbos"]) {
			unsigned int idx = 0;
			if (fboJson.contains("index")) idx = fboJson["index"].get<unsigned int>();
			mVDMix->createFboShaderTexture(fboJson, idx, mAssetsFolder);
		}
	}
}

void VDSession::loadFolder(const std::string& folder) {
	std::string mixPath = ofToDataPath(folder + "/mix.json");
	if (ofFile::doesFileExist(mixPath)) {
		mVDMix->restore(mixPath);
		return;
	}
	// Try individual fbo json files
	for (unsigned int f = 0; f < 8; f++) {
		std::string fboPath = ofToDataPath(folder + "/fbo" + ofToString(f) + ".json");
		if (ofFile::doesFileExist(fboPath)) {
			try {
				ofJson fboJson = ofLoadJson(fboPath);
				mVDMix->createFboShaderTexture(fboJson, f, folder);
			}
			catch (const std::exception& e) {
				ofLogError("VDSession") << "loadFolder fbo error: " << e.what();
			}
		}
	}
}

void VDSession::save()    {
	//mVDMix->save();
}
void VDSession::restore() { mVDMix->restore(ofToDataPath(mAssetsFolder + "/mix.json")); }

bool VDSession::handleKeyDown(int key, bool ctrlDown, bool shiftDown, bool altDown) {
	bool handled = mVDAnimation->handleKeyDown(key, ctrlDown, shiftDown, altDown);
	return handled;
}
bool VDSession::handleKeyUp(int key, bool ctrlDown, bool shiftDown, bool altDown) {
	return false;
}

void VDSession::handleMouseMove(int mx, int my) { mVDMix->handleMouseDown(mx, my); }
void VDSession::handleMouseDown(int mx, int my) { mVDMix->handleMouseDown(mx, my); }
void VDSession::handleMouseDrag(int mx, int my) { mVDMix->handleMouseDrag(mx, my); }
void VDSession::handleMouseUp(int mx, int my)   { mVDMix->handleMouseDown(mx, my); }

void VDSession::fileDrop(const std::string& filePath, unsigned int index) {
	std::string ext = ofToLower(filePath.substr(filePath.find_last_of(".") + 1));
	if (ext == "glsl" || ext == "frag" || ext == "fs") {
		loadFragmentShader(filePath, index);
	} else if (ext == "jpg" || ext == "jpeg" || ext == "png" || ext == "gif" || ext == "bmp" || ext == "tga") {
		loadImageFile(filePath, index);
	} else if (ext == "mp4" || ext == "mov" || ext == "avi" || ext == "mkv") {
		loadVideoFile(filePath, index);
	} else if (ext == "json") {
		loadFromJsonFile(filePath);
	} else {
		ofLogWarning("VDSession") << "Unknown file type dropped: " << filePath;
	}
}
