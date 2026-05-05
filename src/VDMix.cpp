#include "VDMix.h"

using namespace videodromm;

VDMix::VDMix(VDSettingsRef aVDSettings, VDAnimationRef aVDAnimation, VDUniformsRef aVDUniforms) {
	mVDParams    = VDParams::create();
	mVDSettings  = aVDSettings;
	mVDAnimation = aVDAnimation;
	mVDUniforms  = aVDUniforms;

	mDefaultTexture.allocate(mVDParams->getFboWidth(), mVDParams->getFboHeight(), GL_RGBA);

	mixPath = ofToDataPath("mix.json");

	if (ofFile::doesFileExist(mixPath)) {
		restore(mixPath);
	} else {
		ofFile f(mixPath, ofFile::WriteOnly);
		f.close();
		save();
	}

	mMixetteTexture.allocate(mVDParams->getFboWidth(), mVDParams->getFboHeight(), GL_RGBA);

	// init mixette FBO
	ofFbo::Settings fboSettings;
	fboSettings.width           = mVDParams->getFboWidth();
	fboSettings.height          = mVDParams->getFboHeight();
	fboSettings.internalformat  = GL_RGBA;
	fboSettings.wrapModeHorizontal = GL_CLAMP_TO_BORDER;
	fboSettings.wrapModeVertical   = GL_CLAMP_TO_BORDER;
	mMixetteFbo.allocate(fboSettings);

	std::string mixettePath = ofToDataPath("mixette.glsl");
	if (!ofFile::doesFileExist(mixettePath)) {
		mError = mixettePath + " does not exist";
		ofLogWarning("VDMix") << mError;
		mVDSettings->setErrorMsg(mError);
	} else {
		ofBuffer buf = ofBufferFromFile(mixettePath);
		mGlslMixette.setupShaderFromSource(GL_FRAGMENT_SHADER, buf.getText());
		mGlslMixette.setupShaderFromSource(GL_VERTEX_SHADER,   mVDParams->getDefaultVertexString());
		mGlslMixette.linkProgram();
	}
}

bool VDMix::save() {
	ofJson doc;
	doc["settings"]["assetspath"] = mAssetsPath;
	ofSaveJson(mixPath, doc);
	return true;
}

void VDMix::restore(const std::string& aFilePath) {
	if (!ofFile::doesFileExist(aFilePath)) return;
	ofJson doc = ofLoadJson(aFilePath);
	if (doc.is_null()) return;
	if (doc.contains("settings")) {
		if (doc["settings"].contains("assetspath"))
			mAssetsPath = doc["settings"]["assetspath"].get<std::string>();
	}
	if (doc.contains("uniforms")) {
		const auto& uniforms = doc["uniforms"];
		for (unsigned int i = 0; i < 100; i++) {
			std::string name = mVDUniforms->getUniformName(i);
			if (!name.empty() && uniforms.contains(name)) {
				mVDUniforms->setUniformValue(i, uniforms[name].get<float>());
			}
		}
	}
}

unsigned int VDMix::getValidFboIndex(unsigned int aFboIndex) {
	if (mFboShaderList.empty()) return 0;
	return std::min(aFboIndex, (unsigned int)mFboShaderList.size() - 1);
}

unsigned int VDMix::createFboShaderTexture(const ofJson& json, unsigned int aFboIndex, const std::string& aFolder) {
	unsigned int rtn = 0;
	if (!aFolder.empty()) mAssetsPath = aFolder;
	VDFboShaderRef fboShader = VDFboShader::create(mVDUniforms, mVDAnimation, json, aFboIndex, mAssetsPath);
	if (mFboShaderList.empty() || aFboIndex == 0) {
		mFboShaderList.push_back(fboShader);
		rtn = (unsigned int)mFboShaderList.size() - 1;
	} else if (aFboIndex <= (unsigned int)mFboShaderList.size() - 1) {
		mFboShaderList[aFboIndex] = fboShader;
		rtn = aFboIndex;
	} else {
		mFboShaderList.push_back(fboShader);
		rtn = (unsigned int)mFboShaderList.size() - 1;
	}
	return rtn;
}

std::vector<VDShaderUniform> VDMix::getFboShaderUniforms(unsigned int aFboShaderIndex) {
	return mFboShaderList[aFboShaderIndex]->getUniforms();
}

float VDMix::getUniformValueByLocation(unsigned int aFboShaderIndex, unsigned int aLocationIndex) {
	return mFboShaderList[aFboShaderIndex]->getUniformValueByLocation(aLocationIndex);
}

void VDMix::setUniformValueByLocation(unsigned int aFboShaderIndex, unsigned int aLocationIndex, float aValue) {
	mFboShaderList[aFboShaderIndex]->setUniformValueByLocation(aLocationIndex, aValue);
}

unsigned int VDMix::findAvailableIndex(unsigned int aFboShaderIndex, const ofJson& json) {
	unsigned int iSecond = (unsigned int)ofGetElapsedTimef();
	if (aFboShaderIndex == 0) {
		if (iSecond != mCurrentSecond) { mCurrentSecond = iSecond; mCurrentIndex = 0; }
		else mCurrentIndex++;
		aFboShaderIndex = mCurrentIndex;
	}

	if (mFboShaderList.empty()) {
		mFboShaderList.push_back(VDFboShader::create(mVDUniforms, mVDAnimation, json, 0, mAssetsPath));
		return (unsigned int)mFboShaderList.size() - 1;
	}
	if (aFboShaderIndex < (unsigned int)mFboShaderList.size()) return aFboShaderIndex;
	if (aFboShaderIndex < MAXSHADERS) {
		mFboShaderList.push_back(VDFboShader::create(mVDUniforms, mVDAnimation, json, (unsigned int)mFboShaderList.size(), mAssetsPath));
		return (unsigned int)mFboShaderList.size() - 1;
	}
	// reuse invalid or last
	for (unsigned int i = 0; i < (unsigned int)mFboShaderList.size(); i++) {
		if (!mFboShaderList[i]->isValid()) return i;
	}
	return (unsigned int)mFboShaderList.size() - 1;
}

bool VDMix::setFragmentShaderString(const std::string& aFragmentShaderString, const std::string& aName, unsigned int aFboShaderIndex) {
	ofJson json;
	json["shader"]["shadername"] = aName;
	json["shader"]["shadertype"] = "fs";
	json["shader"]["shadertext"] = aFragmentShaderString;
	json["texture"]["texturename"] = "audio";
	json["texture"]["texturetype"] = "audio";
	json["texture"]["texturemode"] = VDTextureMode::AUDIO;
	int rtn = findAvailableIndex(aFboShaderIndex, json);
	mFboShaderList[rtn]->setFragmentShaderString(aFragmentShaderString, aName);
	return rtn >= 0;
}

int VDMix::loadFragmentShader(const std::string& aFilePath, unsigned int aFboShaderIndex) {
	ofJson json;
	json["shader"]["shadername"] = "todo.txt";
	json["shader"]["shadertype"] = "fs";
	json["shader"]["shadertext"] = "todo";
	json["texture"]["texturename"] = "audio";
	json["texture"]["texturetype"] = "audio";
	json["texture"]["texturemode"] = VDTextureMode::AUDIO;
	int rtn = findAvailableIndex(aFboShaderIndex, json);
	mFboShaderList[rtn]->loadFragmentShaderFromFile(aFilePath, true);
	mVDSettings->setMsg("loaded " + mFboShaderList[rtn]->getShaderName());
	return rtn;
}

ofTexture* VDMix::getMixetteTexture(unsigned int aFboIndex) {
	if (!mGlslMixette.isLoaded()) return &mMixetteTexture;

	mMixetteFbo.begin();
	ofClear(0, 0, 0, 255);

	mGlslMixette.begin();
	mGlslMixette.setUniform3f("iResolution",
		mVDUniforms->getUniformValue(mVDUniforms->IRESOLUTIONX),
		mVDUniforms->getUniformValue(mVDUniforms->IRESOLUTIONY), 1.0f);
	mGlslMixette.setUniform1i("iBlendmode", (int)mVDUniforms->getUniformValue(mVDUniforms->IBLENDMODE));

	int i = 0;
	for (auto& fbo : mFboShaderList) {
		if (fbo->isValid()) {
			if (mVDUniforms->getUniformValue(mVDUniforms->IWEIGHT0 + i) > 0.01f) {
				ofTexture* t = fbo->getTexture();
				if (t) t->bind(100 + i);
			}
			mGlslMixette.setUniform1i("iChannel" + ofToString(i), 100 + i);
			mGlslMixette.setUniform1f("iWeight" + ofToString(i), mVDUniforms->getUniformValue(mVDUniforms->IWEIGHT0 + i));
		}
		i++;
	}

	ofDrawRectangle(0, 0, mVDUniforms->getUniformValue(mVDUniforms->IRESOLUTIONX), mVDUniforms->getUniformValue(mVDUniforms->IRESOLUTIONY));
	mGlslMixette.end();

	// unbind
	i = 0;
	for (auto& fbo : mFboShaderList) {
		if (fbo->isValid()) {
			ofTexture* t = fbo->getTexture();
			if (t) t->unbind();
		}
		i++;
	}
	mMixetteFbo.end();

	mMixetteTexture = mMixetteFbo.getTexture();
	return &mMixetteTexture;
}

void VDMix::loadImageFile(const std::string& aFile, unsigned int aFboIndex) {
	if (!ofFile::doesFileExist(aFile)) return;
	std::string ext = "";
	size_t dotIdx = aFile.find_last_of(".");
	if (dotIdx != std::string::npos) ext = aFile.substr(dotIdx + 1);
	if (ext == "jpg" || ext == "png") {
		ofJson json;
		json["texture"]["texturename"] = aFile;
		json["texture"]["texturetype"] = "image";
		json["texture"]["texturemode"] = VDTextureMode::IMAGE;
		json["texture"]["texturecount"] = 1;
		json["shader"]["shadername"]   = "inputImage.fs";
		json["shader"]["shadertype"]   = "fs";
		createFboShaderTexture(json, aFboIndex);
	}
}

void VDMix::loadVideoFile(const std::string& aFile, unsigned int aFboIndex) {
	if (!ofFile::doesFileExist(aFile)) return;
	std::string ext = "";
	size_t dotIdx = aFile.find_last_of(".");
	if (dotIdx != std::string::npos) ext = aFile.substr(dotIdx + 1);
	if (ext == "mp4" || ext == "mov") {
		ofJson json;
		json["texture"]["texturename"] = aFile;
		json["texture"]["texturetype"] = "video";
		json["texture"]["texturemode"] = VDTextureMode::MOVIE;
		json["texture"]["texturecount"] = 1;
		json["shader"]["shadername"]   = "inputVideo.fs";
		json["shader"]["shadertype"]   = "fs";
		createFboShaderTexture(json, aFboIndex);
	}
}
