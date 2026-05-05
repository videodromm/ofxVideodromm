#include "VDFboShader.h"

using namespace videodromm;

VDFboShader::VDFboShader(VDUniformsRef aVDUniforms, VDAnimationRef aVDAnimation,
                         const ofJson& json, unsigned int aFboIndex,
                         const std::string& aAssetsPath)
	: mVDUniforms(aVDUniforms), mVDAnimation(aVDAnimation)
{
	ofLogVerbose("VDFboShader") << "constructor";
	mVDParams    = VDParams::create();
	mAssetsPath  = aAssetsPath;
	mFboIndex    = aFboIndex;
	mIsHydraTex  = false;

	// load shader include from data folder
	ofFile shaderInc(ofToDataPath("shadertoy.vd"));
	if (shaderInc.exists()) {
		ofBuffer buf = ofBufferFromFile(shaderInc.getAbsolutePath());
		shaderInclude = buf.getText();
	}

	mShaderName = mShaderFileName = "inputImage.fs";
	mShaderFragmentString = mVDParams->getDefaultShaderFragmentString();

	setFboTextureAudioMode();

	if (json.contains("shader")) {
		const auto& sh = json["shader"];
		mShaderName = mShaderFileName = sh.contains("shadername") ? sh["shadername"].get<std::string>() : "inputImage.fs";
		mShaderFragmentString         = sh.contains("shadertext")  ? sh["shadertext"].get<std::string>()  : "";
	}
	if (json.contains("texture")) {
		createInputTexture(json["texture"]);
	}

	// init FBO
	ofFbo::Settings fboSettings;
	fboSettings.width  = mVDParams->getFboWidth();
	fboSettings.height = mVDParams->getFboHeight();
	fboSettings.internalformat = GL_RGBA;
	mFbo.allocate(fboSettings);
	mRenderedTexture.allocate(mVDParams->getFboWidth(), mVDParams->getFboHeight(), GL_RGBA);
	isReady   = false;
	mFboMsg   = "";
	mFboError = "";

	if (!mShaderFragmentString.empty()) {
		mValid = setFragmentShaderString(mShaderFragmentString, mShaderName);
	} else {
		mValid = loadFragmentShaderFromFile(mShaderName);
	}

	if (!mValid) {
		mFboError = "VDFboShader constructor failed";
		ofLogError("VDFboShader") << mFboError;
	}
}

unsigned int VDFboShader::createInputTexture(const ofJson& json) {
	mCurrentFilename = mTextureName = json.contains("texturename") ? json["texturename"].get<std::string>() : "0.jpg";
	mTypestr         = json.contains("texturetype")  ? json["texturetype"].get<std::string>()  : "UNKNOWN";
	mTextureMode     = json.contains("texturemode")  ? json["texturemode"].get<int>()          : VDTextureMode::UNKNOWN;
	mTextureCount    = json.contains("texturecount") ? json["texturecount"].get<int>()          : 1;
	mPreloadTextures = json.contains("preloadtextures") ? json["preloadtextures"].get<bool>()  : false;
	msTotal          = 0;

	switch (mTextureMode) {
	case VDTextureMode::AUDIO:
		setFboTextureAudioMode();
		break;
	case VDTextureMode::SEQUENCE:
		mInputTextureList[0].isValid = false;
		loadNextTexture(1);
		break;
	case VDTextureMode::PARTS:
		mInputTextureList[0].isValid = false;
		for (int i = 0; i < mTextureCount; i++) loadNextTexture(i);
		break;
	case VDTextureMode::SHARED: {
#ifdef _WIN32
		// receive first frame
		ofTexture tmp;
		mSpoutIn.receive(tmp);
		if (tmp.isAllocated()) {
			mInputTextureList[0].texture = tmp;
			mInputTextureList[0].name    = "spout";
		}
#endif
		mInputTextureList[0].ms      = 0;
		mInputTextureList[0].isValid = true;
		break;
	}
	default:
		colonIndex = mTextureName.find(":");
		std::string resolvedPath;
		if (colonIndex != std::string::npos) {
			resolvedPath = mTextureName; // full path
		} else {
			resolvedPath = ofToDataPath(mAssetsPath.empty() ? mTextureName : mAssetsPath + "/" + mTextureName);
		}
		mExt = "";
		dotIndex = resolvedPath.find_last_of(".");
		if (dotIndex != std::string::npos) mExt = resolvedPath.substr(dotIndex + 1);

		if (mExt == "jpg" || mExt == "png") {
			mFboMsg      = "jpg or png";
			mTextureMode = VDTextureMode::IMAGE;
			mTypestr     = "image";
			loadImageFile(resolvedPath, 0);
		} else if (mExt == "mp4" || mExt == "mov") {
			if (ofFile::doesFileExist(resolvedPath)) {
				mIsVideoLoaded = mVideo.load(resolvedPath);
				if (mIsVideoLoaded) {
					mVideoDuration = mVideo.getDuration();
					mVideoPos      = mVideo.getPosition();
					mVideo.setLoopState(OF_LOOP_NORMAL);
					mVideo.play();
					mTypestr     = "video";
					mTextureMode = VDTextureMode::MOVIE;
				} else {
					mFboMsg = "video load failed: " + mTextureName;
					setFboTextureAudioMode();
				}
			} else {
				mFboMsg = "video does not exist: " + mTextureName;
				setFboTextureAudioMode();
			}
		} else {
			mFboMsg = "audio, not image or video: " + mTextureName;
			setFboTextureAudioMode();
		}
		break;
	}

	size_t slashIdx = mTextureName.find_last_of("\\/");
	mFboStatus = (slashIdx != std::string::npos) ? mTextureName.substr(slashIdx + 1) : mTextureName;
	return (unsigned int)mInputTextureList.size() - 1;
}

bool VDFboShader::loadFragmentShaderFromFile(const std::string& aFileOrPath, bool isAudio) {
	mValid = false;
	if (aFileOrPath.empty()) { mFboError = "VDFboShader empty filename"; return false; }

	// resolve path
	if (ofFile::doesFileExist(aFileOrPath)) {
		mFragFilePath = aFileOrPath;
	} else {
		std::string inAssets = ofToDataPath((!mAssetsPath.empty() ? mAssetsPath + "/" : "") + aFileOrPath);
		if (ofFile::doesFileExist(inAssets)) {
			mFragFilePath = inAssets;
		} else {
			std::string inRoot = ofToDataPath(aFileOrPath);
			if (ofFile::doesFileExist(inRoot)) {
				mFragFilePath = inRoot;
			} else {
				mFboError = "VDFboShader file not found: " + aFileOrPath;
				return false;
			}
		}
	}
	mValid = loadFragmentStringFromFile();
	if (isAudio && mValid) setFboTextureAudioMode();
	return mValid;
}

bool VDFboShader::loadFragmentStringFromFile() {
	mValid = false;
	size_t slashIdx = mFragFilePath.find_last_of("\\/");
	mFileNameWithExtension = (slashIdx != std::string::npos) ? mFragFilePath.substr(slashIdx + 1) : mFragFilePath;
	ofBuffer buf = ofBufferFromFile(mFragFilePath);
	if (buf.size() == 0) { mFboError = "empty shader file: " + mFragFilePath; return false; }
	mValid = setFragmentShaderString(buf.getText(), mFileNameWithExtension);
	return mValid;
}

bool VDFboShader::setFragmentShaderString(const std::string& aFragmentShaderString, const std::string& aName) {
	std::string fragSrc = aFragmentShaderString;
	mFboError  = "";
	mName      = aName;
	mIsHydraTex = false;
	isReady    = false;

	if (mName.empty()) {
		mName = ofToString((int)ofGetElapsedTimef());
	} else {
		size_t dot   = mName.find_last_of(".");
		size_t slash = mName.find_last_of("\\/");
		if (dot != std::string::npos && dot > (slash == std::string::npos ? 0 : slash)) {
			mName = mName.substr((slash == std::string::npos ? 0 : slash + 1), dot - (slash == std::string::npos ? 0 : slash + 1));
		}
	}
	mShaderName = mName + ".fs";

	// inject shadertoy include if no uniforms declared
	if (fragSrc.find("uniform ") == std::string::npos && !shaderInclude.empty()) {
		fragSrc = "/* " + mName + " */\n" + shaderInclude + fragSrc;
	} else {
		fragSrc = "/* " + mName + " */\n" + fragSrc;
	}

	mShader.setupShaderFromSource(GL_FRAGMENT_SHADER, fragSrc);
	mShader.setupShaderFromSource(GL_VERTEX_SHADER,   mVDParams->getDefaultVertexString());
	bool linked = mShader.linkProgram();

	if (linked) {
		mShaderFragmentString = fragSrc;
		mFboMsg   = mName;
		mValid    = true;
		// build active uniforms list via OpenGL query
		mActiveUniforms.clear();
		GLuint prog = mShader.getProgram();
		GLint numUniforms = 0;
		glGetProgramiv(prog, GL_ACTIVE_UNIFORMS, &numUniforms);
		for (int i = 0; i < numUniforms; i++) {
			char uname[256]; GLsizei len; GLint sz; GLenum type;
			glGetActiveUniform(prog, i, 255, &len, &sz, &type, uname);
			VDShaderUniform u;
			u.name     = std::string(uname, len);
			u.type     = (int)type;
			u.location = glGetUniformLocation(prog, uname);
			mActiveUniforms.push_back(u);
		}
	} else {
		mFboStatus = mName + ": link error";
		ofLogError("VDFboShader") << "setFragmentShaderString link error: " << mName;
		mValid = false;
	}
	return mValid;
}

void VDFboShader::loadImageFile(const std::string& aFile, unsigned int aCurrentIndex) {
	if (mInputTextureList[aCurrentIndex].isValid && aCurrentIndex != 0) {
		mFboMsg = mInputTextureList[aCurrentIndex].name + " cached";
		return;
	}
	if (!ofFile::doesFileExist(aFile)) {
		mFboMsg = "audio, file does not exist: " + aFile;
		setFboTextureAudioMode();
		return;
	}
	auto start = Clock::now();
	size_t slashIdx = aFile.find_last_of("\\/");
	mCurrentFilename = (slashIdx != std::string::npos) ? aFile.substr(slashIdx + 1) : aFile;
	ofImage img;
	if (img.load(aFile)) {
		mInputTextureList[aCurrentIndex].texture = img.getTexture();
		mInputTextureList[aCurrentIndex].name    = mCurrentFilename;
		mInputTextureList[aCurrentIndex].isValid = true;
		auto end  = Clock::now();
		auto dur  = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
		mInputTextureList[aCurrentIndex].ms = (unsigned int)dur.count();
		msTotal += mInputTextureList[aCurrentIndex].ms;
		mFboMsg = mCurrentFilename + " " + ofToString(mInputTextureList[aCurrentIndex].ms) + "ms";
	} else {
		mFboMsg = "audio, load failed: " + aFile;
		setFboTextureAudioMode();
	}
}

void VDFboShader::loadNextTexture(unsigned int aCurrentIndex) {
	if (mCurrentImageSequenceIndex == (int)aCurrentIndex) return;
	mCurrentImageSequenceIndex = aCurrentIndex;
	std::string root = ofToDataPath(mTextureName + "/");
	std::vector<std::string> candidates = {
		root + mTextureName + " (" + ofToString(aCurrentIndex) + ").jpg",
		root + mTextureName + "-(" + ofToString(aCurrentIndex) + ").jpg",
		root + mTextureName + " (" + ofToString(aCurrentIndex) + ").png",
		root + mTextureName + "-(" + ofToString(aCurrentIndex) + ").png"
	};
	for (auto& p : candidates) {
		if (ofFile::doesFileExist(p)) { loadImageFile(p, aCurrentIndex); return; }
	}
}

ofTexture* VDFboShader::getFboTexture() {
	if (!mValid) return &mRenderedTexture;

	// update texture input
	switch (mTextureMode) {
	case VDTextureMode::AUDIO:
		mInputTextureList[0].texture = mVDAnimation->getAudioTexture();
		mInputTextureList[0].isValid = true;
		break;
	case VDTextureMode::SEQUENCE:
		loadNextTexture((int)mVDUniforms->getUniformValue(mVDUniforms->IBARBEAT));
		if (mPreloadTextures && mVDUniforms->getUniformValue(mVDUniforms->IBEAT) > 1) {
			if (mCacheImageIndex < (unsigned int)mTextureCount) {
				mCacheImageIndex++;
				mFboStatus = " " + ofToString(mCacheImageIndex) + "/" + ofToString(mTextureCount);
				loadNextTexture(mCacheImageIndex);
			}
		} else {
			mFboStatus = ofToString(mCacheImageIndex) + "/" + ofToString(mTextureCount) + " " + ofToString(msTotal) + "ms";
		}
		break;
	case VDTextureMode::SHARED:
#ifdef _WIN32
		{
			ofTexture tmp;
			mSpoutIn.receive(tmp);
			if (tmp.isAllocated()) {
				mInputTextureList[0].texture = tmp;
				mInputTextureList[0].name    = "spout";
				mInputTextureList[0].isValid = true;
			}
		}
#endif
		break;
	case VDTextureMode::MOVIE:
		if (mIsVideoLoaded) {
			mVideo.update();
			mVideoPos = mVideo.getPosition();
			if (!mVideo.isPlaying()) { mVideo.setPosition(0); mVideo.play(); }
		}
		break;
	default:
		break;
	}

	// render into FBO
	mFbo.begin();
	if (mVDUniforms->getUniformValue(mVDUniforms->ICLEAR) > 0.5f) {
		ofClear(0, 0, 0, 255);
	}

	// bind textures
	switch (mTextureMode) {
	case VDTextureMode::PARTS:
		for (size_t i = 0; i < mInputTextureList.size(); i++) {
			if (mInputTextureList[i].isValid && mInputTextureList[i].texture.isAllocated())
				mInputTextureList[i].texture.bind((int)i);
		}
		break;
	case VDTextureMode::SEQUENCE: {
		unsigned int bi = (unsigned int)mVDUniforms->getUniformValue(mVDUniforms->IBARBEAT);
		if (mInputTextureList[bi].isValid && mInputTextureList[bi].texture.isAllocated())
			mInputTextureList[bi].texture.bind(0);
		break;
	}
	default:
		if (mInputTextureList[0].isValid && mInputTextureList[0].texture.isAllocated())
			mInputTextureList[0].texture.bind(0);
		break;
	}

	mShader.begin();
	applyAllUniforms();
	ofDrawRectangle(0, 0, mVDParams->getFboWidth(), mVDParams->getFboHeight());

	// movie: draw video on top
	if (mTextureMode == VDTextureMode::MOVIE && mIsVideoLoaded) {
		mVideo.draw(0, 0, mVDParams->getFboWidth(), mVDParams->getFboHeight());
	}

	mShader.end();

	// unbind
	switch (mTextureMode) {
	case VDTextureMode::PARTS:
		for (size_t i = 0; i < mInputTextureList.size(); i++) {
			if (mInputTextureList[i].isValid && mInputTextureList[i].texture.isAllocated())
				mInputTextureList[i].texture.unbind((int)i);
		}
		break;
	default:
		if (mInputTextureList[0].isValid && mInputTextureList[0].texture.isAllocated())
			mInputTextureList[0].texture.unbind(0);
		break;
	}

	mFbo.end();
	mRenderedTexture = mFbo.getTexture();

	if (!isReady) {
		saveThumbnail(false);
		isReady = true;
	}
	return &mRenderedTexture;
}

void VDFboShader::applyAllUniforms() {
	int channelIndex = 0;
	for (const auto& u : mActiveUniforms) {
		const std::string& uName = u.name;
		switch (u.type) {
		case GL_FLOAT:
			if (uName == "TIME" || uName == "time") {
				mShader.setUniform1f(uName, mVDUniforms->getUniformValue(mVDUniforms->ITIME));
			} else if (mVDUniforms->isExistingUniform(uName)) {
				mShader.setUniform1f(uName, mVDUniforms->getUniformValueByName(uName));
			} else {
				mShader.setUniform1f(uName, mUniformValueByLocation[u.location]);
			}
			break;
		case GL_INT:
		case GL_BOOL:
			if (mVDUniforms->isExistingUniform(uName)) {
				mShader.setUniform1i(uName, (int)mVDUniforms->getUniformValueByName(uName));
			}
			break;
		case GL_SAMPLER_2D:
			if (uName.find("iChannel") != std::string::npos) {
				mShader.setUniform1i(uName, channelIndex++);
			} else if (uName.find("tex") != std::string::npos) {
				mIsHydraTex = true;
				mShader.setUniform1i(uName, 254 + channelIndex++);
			} else {
				mShader.setUniform1i(uName, 0); // inputImage
			}
			break;
		case GL_FLOAT_VEC2:
			if (uName == "RENDERSIZE" || uName == "resolution") {
				mShader.setUniform2f(uName, (float)mVDParams->getFboWidth(), (float)mVDParams->getFboHeight());
			} else {
				glm::vec2 v = mVDUniforms->getVec2UniformValueByName(uName);
				mShader.setUniform2f(uName, v.x, v.y);
			}
			break;
		case GL_FLOAT_VEC3: {
			glm::vec3 v = mVDUniforms->getVec3UniformValueByName(uName);
			mShader.setUniform3f(uName, v.x, v.y, v.z);
			break;
		}
		case GL_FLOAT_VEC4:
			if (uName == "iDate") {
				mShader.setUniform4f(uName,
					mVDUniforms->getUniformValue(mVDUniforms->IDATEX),
					mVDUniforms->getUniformValue(mVDUniforms->IDATEY),
					mVDUniforms->getUniformValue(mVDUniforms->IDATEZ),
					mVDUniforms->getUniformValue(mVDUniforms->IDATEW));
			} else {
				glm::vec4 v = mVDUniforms->getVec4UniformValueByName(uName);
				mShader.setUniform4f(uName, v.x, v.y, v.z, v.w);
			}
			break;
		default:
			break;
		}
	}
}

ofTexture* VDFboShader::getTexture() {
	if (mValid) {
		if (!isReady) { getFboTexture(); isReady = true; }
		getFboTexture();
	}
	return &mRenderedTexture;
}

void VDFboShader::saveThumbnail(bool overwrite) {
	std::string thumbPath = ofToDataPath("thumbs/" + mName + ".jpg");
	if (!ofFile::doesFileExist(thumbPath) || overwrite) {
		ofImage img;
		mRenderedTexture.readToPixels(img.getPixels());
		if (img.getWidth() > 0) img.save(thumbPath);
	}
}
