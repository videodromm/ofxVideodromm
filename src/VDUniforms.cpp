#include "VDUniforms.h"

using namespace videodromm;

VDUniformsRef VDUniforms::UNIFORMS;

VDUniformsRef VDUniforms::create() {
	if (!UNIFORMS) {
		UNIFORMS = VDUniformsRef(new VDUniforms());
	}
	return UNIFORMS;
}

VDUniforms::VDUniforms() {
	mRenderWidth  = 1280;
	mRenderHeight = 720;
	mErrorCode    = 0;

	for (size_t i = 0; i < 30; i++) {
		createSampler2DUniform("iChannel" + ofToString(i), 400 + i, i);
	}
	createSampler2DUniform("inputImage", 399, 0);

	createFloatUniform("TIME",             ITIME,          0.0f);
	createFloatUniform("iColorX",          ICOLORX,        0.45f);
	createFloatUniform("iColorY",          ICOLORY,        0.0f);
	createFloatUniform("iColorZ",          ICOLORZ,        1.0f);
	createFloatUniform("iAlpha",           IALPHA,         1.0f);
	createFloatUniform("iRedMultiplier",   IFRX,           1.0f, 0.0f, 3.0f);
	createFloatUniform("iGreenMultiplier", IFGX,           1.0f, 0.0f, 3.0f);
	createFloatUniform("iBlueMultiplier",  IFBX,           1.0f, 0.0f, 3.0f);
	createFloatUniform("iSobel",           ISOBEL,         0.02f, 0.02f, 2.0f);
	createFloatUniform("iFactor",          IFACTOR,        1.0f, 0.4f, 0.6f);
	createFloatUniform("iVignette",        IVIGNETTE,      0.0f);
	createFloatUniform("iRotationSpeed",   IROTATIONSPEED, 0.0f, -0.1f, 1.0f);
	createFloatUniform("iEffect",          IEFFECT,        0.0f, -0.5f, 0.5f, true);
	createFloatUniform("iSmooth",          ISMOOTH,        0.005f);
	createFloatUniform("iExposure",        IEXPOSURE,      1.0f, 0.0f, 13.0f);
	createFloatUniform("iPixelate",        IPIXELATE,      1.0f, 0.01f, 1.0f);
	createFloatUniform("iTrixels",         ITRIXELS,       0.0f, 0.0f, 1.0f);
	createFloatUniform("iChromatic",       ICHROMATIC,     0.0f, 0.000000001f);
	createFloatUniform("iCrossfade",       IXFADE,         0.27710f, 0.0f, 1.0f);
	createFloatUniform("iMouseX",         IMOUSEX,        0.27710f, 0.0f, 1280.0f);
	createFloatUniform("iMouseY",         IMOUSEY,        0.5648f,  0.0f, 800.0f);
	createFloatUniform("iMouseZ",         IMOUSEZ,        0.0f,     0.0f, 1.0f);
	createFloatUniform("iMouseW",         IMOUSEW,        0.91f,    0.0f, 1.0f);
	createFloatUniform("iRenderXYX",      IRENDERXYX,     0.0f, -1.0f, 1920.0f);
	createFloatUniform("iRenderXYY",      IRENDERXYY,     0.0f, -1.0f, 1080.0f);
	createFloatUniform("iZoom",           IZOOM,          1.0f, 0.01f, 30.05f, true);
	createFloatUniform("iSteps",          ISTEPS,         16.0f, 1.0f, 128.0f);
	createFloatUniform("iRatio",          IRATIO,         20.0f, 0.00000000001f, 40.0f);
	createFloatUniform("iSpeed",          ISPEED,         1.0f, 0.01f, 12.0f);
	createFloatUniform("iContour",        ICONTOUR,       0.0f, 0.0f, 0.5f);
	createFloatUniform("iWeight0",        IWEIGHT0,       1.0f, 0.0f, 1.0f);
	createFloatUniform("iWeight1",        IWEIGHT1,       0.0f, 0.0f, 1.0f);
	createFloatUniform("iWeight2",        IWEIGHT2,       0.0f, 0.0f, 1.0f);
	createFloatUniform("iWeight3",        IWEIGHT3,       0.0f, 0.0f, 1.0f);
	createFloatUniform("iWeight4",        IWEIGHT4,       0.0f, 0.0f, 1.0f);
	createFloatUniform("iWeight5",        IWEIGHT5,       0.0f, 0.0f, 1.0f);
	createFloatUniform("iWeight6",        IWEIGHT6,       0.0f, 0.0f, 1.0f);
	createFloatUniform("iWeight7",        IWEIGHT7,       0.0f, 0.0f, 1.0f);
	createFloatUniform("iWeight8",        IWEIGHT8,       0.0f, 0.0f, 1.0f);
	createFloatUniform("iAudioX",         IAUDIOX,        1.0f, 0.01f, 7.0f);
	createFloatUniform("iStart",          ISTART,         0.0f, 1.0f, 4200.0f);
	createFloatUniform("iDeltaTime",      IDELTATIME,     60.0f / 160.0f);
	createFloatUniform("iTempoTime",      ITEMPOTIME,     0.1f);
	createFloatUniform("iFps",            IFPS,           60.0f, 0.0f, 100.0f);
	createFloatUniform("iBpm",            IBPM,           166.0f, 0.000000001f, 400.0f);
	createFloatUniform("iVFallOff",       IVFALLOFF,      0.31f, 0.0f, 1.0f);
	createFloatUniform("iBadTv",          IBADTV,         0.0f, 0.0f, 5.0f);
	createFloatUniform("iTimeFactor",     ITIMEFACTOR,    1.0f);
	createFloatUniform("iDisplayMode",    IDISPLAYMODE,   7.0f, 0.0f, 9.0f);
	createFloatUniform("volume0",         IMAXVOLUME,     0.0f, 0.0f, 255.0f);
	createFloatUniform("iMultiplier",     IMULT,          1.0f, 0.0f, 30.0f);
	createFloatUniform("iPar0",           IPAR0,          0.01f, 0.0f, 30.0f);
	createFloatUniform("iPar1",           IPAR1,          0.20f, 0.0f, 30.0f);
	createFloatUniform("iPar2",           IPAR2,          1.0f,  0.0f, 30.0f);
	createFloatUniform("iPar3",           IPAR3,          1.385f, 0.0f, 30.0f);
	createFloatUniform("iPar4",           IPAR4,          1.817f, 0.0f, 10.0f);
	createFloatUniform("iPar5",           IPAR5,          1.0f,  0.0f, 3.0f);
	createFloatUniform("iBlendmode",      IBLENDMODE,     0.0f, 0.0f, 25.0f);
	createFloatUniform("iBeat",           IBEAT,          0.0f, 0.0f, 300.0f);
	createFloatUniform("iBar",            IBAR,           0.0f, 0.0f, 3008.0f);
	createFloatUniform("iBarBeat",        IBARBEAT,       1.0f, 1.0f, 3600.0f);
	createFloatUniform("iFboA",           IFBOA,          0);
	createFloatUniform("iFboB",           IFBOB,          1);
	createFloatUniform("iOutW",           IOUTW,          (float)mRenderWidth);
	createFloatUniform("iOutH",           IOUTH,          (float)mRenderHeight);
	createFloatUniform("iBeatsPerBar",    IBEATSPERBAR,   4);
	createFloatUniform("iPhase",          IPHASE,         1.0f);
	createFloatUniform("iTrack",          ITRACK,         0.0f, 0.0f, 90000.0f);
	createFloatUniform("iVAmount",        IVAMOUNT,       0.91f, 0.0f, 1.0f);
	createFloatUniform("iDateX",          IDATEX,         0.91f, 0.0f, 90000.0f);
	createFloatUniform("iDateY",          IDATEY,         0.91f, 0.0f, 90000.0f);
	createFloatUniform("iDateZ",          IDATEZ,         0.91f, 0.0f, 90000.0f);
	createFloatUniform("iDateW",          IDATEW,         0.91f, 0.0f, 90000.0f);
	createFloatUniform("iBarStart",       IBARSTART,      0.0f, 0.0f, 1200.0f);
	createFloatUniform("iBackgroundColorX", IBACKGROUNDCOLORX, 0.56f);
	createFloatUniform("iBackgroundColorY", IBACKGROUNDCOLORY, 0.0f);
	createFloatUniform("iBackgroundColorZ", IBACKGROUNDCOLORZ, 1.0f);
	createVec3Uniform("iBackgroundColor", IBACKGROUNDCOLOR, glm::vec3(getUniformValue(IBACKGROUNDCOLORX), getUniformValue(IBACKGROUNDCOLORY), getUniformValue(IBACKGROUNDCOLORZ)));
	createFloatUniform("iResolutionX",    IRESOLUTIONX,   (float)mRenderWidth,  320.01f, 4280.0f);
	createFloatUniform("iResolutionY",    IRESOLUTIONY,   (float)mRenderHeight, 240.01f, 2160.0f);
	createFloatUniform("iPixelX",         IPIXELX,        0.0f, -0.5f, 0.5f);
	createFloatUniform("iPixelY",         IPIXELY,        0.0f, -0.5f, 0.5f);
	createVec3Uniform("iResolution",      IRESOLUTION,    glm::vec3(getUniformValue(IRESOLUTIONX), getUniformValue(IRESOLUTIONY), 1.0f));
	createVec2Uniform("iRenderXY",        IRENDERXY,      glm::vec2(getUniformValue(IRENDERXYX),   getUniformValue(IRENDERXYY)));
	createVec3Uniform("iColor",           ICOLOR,         glm::vec3(0.45f, 0.0f, 1.0f));
	createVec4Uniform("iDate",            IDATE,          glm::vec4(2021.0f, 12.0f, 1.0f, 5.0f));
	createFloatUniform("iGlitch",         IGLITCH,        0.0f);
	createFloatUniform("iClear",          ICLEAR,         0.0f);
	createFloatUniform("iToggle",         ITOGGLE,        0.0f);
	createFloatUniform("iInvert",         IINVERT,        0.0f);
	createFloatUniform("iGreyScale",      IGREYSCALE,     0.0f);
	createFloatUniform("iDebug",          IDEBUG,         0.0f);
	createFloatUniform("iXorY",           IXORY,          0.0f);
	createFloatUniform("iFlipH",          IFLIPH,         0.0f);
	createFloatUniform("iFlipV",          IFLIPV,         0.0f);
	createFloatUniform("iFlipPostH",      IFLIPPOSTH,     0.0f);
	createFloatUniform("iFlipPostV",      IFLIPPOSTV,     0.0f);
	createFloatUniform("srcXLeft",        SRCXLEFT,       0.0f, 0.0f, 4280.0f);
	createFloatUniform("srcXRight",       SRCXRIGHT,      (float)mRenderWidth, 320.01f, 4280.0f);
	createFloatUniform("srcYLeft",        SRCYLEFT,       0.0f, 0.0f, 1024.0f);
	createFloatUniform("srcYRight",       SRCYRIGHT,      (float)mRenderHeight, 0.0f, 1024.0f);
	createFloatUniform("iFreq0",          IFREQ0,         0.0f, 0.01f, 256.0f);
	createFloatUniform("iFreq1",          IFREQ1,         0.0f, 0.01f, 256.0f);
	createFloatUniform("iFreq2",          IFREQ2,         0.0f, 0.01f, 256.0f);
	createFloatUniform("iFreq3",          IFREQ3,         0.0f, 0.01f, 256.0f);
	createFloatUniform("iSpeed0",         ISPEED0,        0.0052f);
	createFloatUniform("iSpeed1",         ISPEED1,        0.0043f);
	createFloatUniform("iSpeed2",         ISPEED2,        0.0045f);
	createFloatUniform("iSpeed3",         ISPEED3,        0.003f);
	createFloatUniform("iSpeed4",         ISPEED4,        0.0058f);
	createFloatUniform("iSpeed5",         ISPEED5,        0.0035f);
	createFloatUniform("iSpeed6",         ISPEED6,        0.0041f);
	createFloatUniform("iSpeed7",         ISPEED7,        0.0036f);
	createFloatUniform("iSpeed8",         ISPEED8,        0.005f);
	createFloatUniform("iSpeed9",         ISPEED9,        0.0035f);
	createFloatUniform("iSpeed10",        ISPEED10,       0.0052f);
	createFloatUniform("iSpeed11",        ISPEED11,       0.0045f);
	createFloatUniform("iSpeed12",        ISPEED12,       0.0052f);
	createFloatUniform("iSpeed13",        ISPEED13,       0.0042f);
	createFloatUniform("iSpeed14",        ISPEED14,       0.0051f);
	createFloatUniform("iSpeed15",        ISPEED15,       0.0065f);
	createFloatUniform("iColumn1",        ICOLUMN1,       0.0f);
	createFloatUniform("iColumn2",        ICOLUMN2,       0.0f);
	createFloatUniform("iColumn3",        ICOLUMN3,       0.0f);
	createFloatUniform("iColumn4",        ICOLUMN4,       0.0f);
	createFloatUniform("iColumn5",        ICOLUMN5,       0.0f);
	createFloatUniform("iColumn6",        ICOLUMN6,       0.0f);
	createFloatUniform("iColumn7",        ICOLUMN7,       0.0f);
	createFloatUniform("iColumn8",        ICOLUMN8,       0.0f);
	createVec2Uniform("resolution",       RESOLUTION,     glm::vec2(1280.0f, 720.0f));
	createVec2Uniform("RENDERSIZE",       RENDERSIZE,     glm::vec2(getUniformValue(IRESOLUTIONX), getUniformValue(IRESOLUTIONY)));
	createVec4Uniform("iMouse",           IMOUSE,         glm::vec4(0.27710f, 0.5648f, 0.0f, 0.0f));

	// Azure Kinect joints
	for (int j = 0; j < 32; j++) {
		const char* names[] = {
			"Pelvis","SpineNavel","SpineChest","Neck","ClavL","ShldrL","ElbowL","WristL",
			"HandL","HandTipL","ThumbL","ClavR","ShldrR","ElbowR","WristR","HandR",
			"HandTipR","ThumbR","HipL","KneeL","AnkleL","FootL","HipR","KneeR",
			"AnkleR","FootR","Head","Nose","EyeL","EarL","EyeR","EarR"
		};
		createVec4Uniform(names[j], 200 + j, glm::vec4(320.0f, 240.0f, 0.0f, 0.0f));
	}
}

void VDUniforms::setIBarBeat() {
	shaderUniforms[IBARBEAT].floatValue =
		shaderUniforms[IBAR].floatValue * shaderUniforms[IBEATSPERBAR].floatValue +
		shaderUniforms[IBEAT].floatValue;
}

void VDUniforms::loadUniforms(const std::string& path) {
	ofJson json = ofLoadJson(path);
	if (json.is_null()) return;
	if (!json.contains("uniforms")) return;
	const auto& uniforms = json["uniforms"];
	for (const auto& child : uniforms) {
		if (!child.contains("uniform")) continue;
		const auto& u = child["uniform"];
		int uniformType = u.contains("type") ? u["type"].get<int>() : 0;
		switch (uniformType) {
		case GL_FLOAT:
		case GL_INT:
		case GL_BOOL:     floatFromJson(child);    break;
		case GL_SAMPLER_2D: sampler2dFromJson(child); break;
		case GL_FLOAT_VEC2: vec2FromJson(child);   break;
		case GL_FLOAT_VEC3: vec3FromJson(child);   break;
		case GL_FLOAT_VEC4: vec4FromJson(child);   break;
		}
	}
}

void VDUniforms::floatFromJson(const ofJson& json) {
	if (!json.contains("uniform")) return;
	const auto& u = json["uniform"];
	std::string jName  = u.contains("name")  ? u["name"].get<std::string>() : "unknown";
	int jCtrl          = u.contains("index") ? u["index"].get<int>()        : 249;
	float jValue       = u.contains("value") ? u["value"].get<float>()      : 0.01f;
	float jMin         = u.contains("min")   ? u["min"].get<float>()        : 0.0f;
	float jMax         = u.contains("max")   ? u["max"].get<float>()        : 1.0f;
	createFloatUniform(jName, jCtrl, jValue, jMin, jMax);
}

void VDUniforms::sampler2dFromJson(const ofJson& json) {
	if (!json.contains("uniform")) return;
	const auto& u = json["uniform"];
	std::string jName = u.contains("name")  ? u["name"].get<std::string>()   : "unknown";
	int jCtrl         = u.contains("index") ? u["index"].get<int>()          : 250;
	int jTex          = u.contains("textureindex") ? u["textureindex"].get<int>() : 0;
	createSampler2DUniform(jName, jCtrl, jTex);
}

void VDUniforms::vec2FromJson(const ofJson& json) {
	if (!json.contains("uniform")) return;
	const auto& u = json["uniform"];
	std::string jName = u.contains("name")  ? u["name"].get<std::string>() : "unknown";
	int jCtrl         = u.contains("index") ? u["index"].get<int>()        : 251;
	createVec2Uniform(jName, jCtrl);
}

void VDUniforms::vec3FromJson(const ofJson& json) {
	if (!json.contains("uniform")) return;
	const auto& u = json["uniform"];
	std::string jName = u.contains("name")  ? u["name"].get<std::string>() : "unknown";
	int jCtrl         = u.contains("index") ? u["index"].get<int>()        : 252;
	createVec3Uniform(jName, jCtrl);
}

void VDUniforms::vec4FromJson(const ofJson& json) {
	if (!json.contains("uniform")) return;
	const auto& u = json["uniform"];
	std::string jName = u.contains("name")  ? u["name"].get<std::string>() : "unknown";
	int jCtrl         = u.contains("index") ? u["index"].get<int>()        : 253;
	createVec4Uniform(jName, jCtrl);
}

bool VDUniforms::setUniformValue(unsigned int aIndex, float aValue) {
	bool rtn = false;
	if (aIndex == 0) {
		shaderUniforms[aIndex].floatValue = aValue;
		return true;
	}
	if (aIndex == IBPM) {
		if (aValue > 0.0f) {
			shaderUniforms[aIndex].floatValue = aValue;
			setUniformValue(IDELTATIME, 60.0f / aValue);
		}
		return true;
	}
	if (shaderUniforms[aIndex].floatValue != aValue) {
		if (shaderUniforms[aIndex].overrideValue) {
			shaderUniforms[aIndex].floatValue = aValue;
			rtn = true;
		} else if ((aValue >= shaderUniforms[aIndex].minValue && aValue <= shaderUniforms[aIndex].maxValue) || shaderUniforms[aIndex].anim > 0) {
			switch (aIndex) {
			case IBAR:
				shaderUniforms[IBAR].floatValue = aValue;
				setIBarBeat();
				break;
			case IBEAT:
				shaderUniforms[IBEAT].floatValue = aValue;
				setIBarBeat();
				break;
			case ITRACK:
				shaderUniforms[ITRACK].floatValue   = aValue;
				shaderUniforms[ITIME].floatValue    = ofGetElapsedTimef();
				shaderUniforms[ISTART].floatValue   = shaderUniforms[ITIME].floatValue;
				shaderUniforms[IBARSTART].floatValue = shaderUniforms[IBAR].floatValue;
				shaderUniforms[IBAR].floatValue     = 0.0f;
				setIBarBeat();
				shaderUniforms[IZOOM].floatValue     = shaderUniforms[IZOOM].defaultValue;
				shaderUniforms[IPIXELATE].floatValue = shaderUniforms[IPIXELATE].defaultValue;
				break;
			case ITOGGLE:
				shaderUniforms[ITOGGLE].floatValue = aValue;
				shaderUniforms[ITIME].floatValue   = ofGetElapsedTimef();
				shaderUniforms[ISTART].floatValue  = shaderUniforms[ITIME].floatValue;
				break;
			default:
				shaderUniforms[aIndex].floatValue = aValue;
				break;
			}
			rtn = true;
		} else {
			mErrorCode = aIndex;
			if (shaderUniforms[aIndex].maxValue == 0.0f) {
				mErrorCode = 1;
			}
		}
	}
	return rtn;
}

bool VDUniforms::isExistingUniform(const std::string& aName) {
	return stringToIndex(aName) >= 0;
}

int VDUniforms::stringToIndex(const std::string& key) {
	if (key == "iTime" || key == "uTime" || key == "time" || key == "TIME")  return ITIME;
	if (key == "iColorX")          return ICOLORX;
	if (key == "iColorY")          return ICOLORY;
	if (key == "iColorZ")          return ICOLORZ;
	if (key == "iAlpha")           return IALPHA;
	if (key == "iRedMultiplier")   return IFRX;
	if (key == "iGreenMultiplier") return IFGX;
	if (key == "iBlueMultiplier")  return IFBX;
	if (key == "iSobel")           return ISOBEL;
	if (key == "iFactor")          return IFACTOR;
	if (key == "iVignette")        return IVIGNETTE;
	if (key == "iRotationSpeed")   return IROTATIONSPEED;
	if (key == "iEffect")          return IEFFECT;
	if (key == "iSmooth")          return ISMOOTH;
	if (key == "iExposure")        return IEXPOSURE;
	if (key == "iPixelate")        return IPIXELATE;
	if (key == "iTrixels")         return ITRIXELS;
	if (key == "iChromatic")       return ICHROMATIC;
	if (key == "iCrossfade")       return IXFADE;
	if (key == "iTempoTime")       return ITEMPOTIME;
	if (key == "iFps")             return IFPS;
	if (key == "iBpm")             return IBPM;
	if (key == "speed")            return ISPEED;
	if (key == "iSpeed")           return ISPEED;
	if (key == "iRenderXYX")      return IRENDERXYX;
	if (key == "iRenderXYY")      return IRENDERXYY;
	if (key == "iZoom")            return IZOOM;
	if (key == "iSteps")           return ISTEPS;
	if (key == "iRatio")           return IRATIO;
	if (key == "iWeight0")         return IWEIGHT0;
	if (key == "iWeight1")         return IWEIGHT1;
	if (key == "iWeight2")         return IWEIGHT2;
	if (key == "iWeight3")         return IWEIGHT3;
	if (key == "iWeight4")         return IWEIGHT4;
	if (key == "iWeight5")         return IWEIGHT5;
	if (key == "iWeight6")         return IWEIGHT6;
	if (key == "iWeight7")         return IWEIGHT7;
	if (key == "iWeight8")         return IWEIGHT8;
	if (key == "iAudioX")          return IAUDIOX;
	if (key == "iStart")           return ISTART;
	if (key == "iDeltaTime")       return IDELTATIME;
	if (key == "iBadTv")           return IBADTV;
	if (key == "iTimeFactor")      return ITIMEFACTOR;
	if (key == "iDisplayMode")     return IDISPLAYMODE;
	if (key == "volume0")          return IMAXVOLUME;
	if (key == "iMultiplier")      return IMULT;
	if (key == "iPar0")            return IPAR0;
	if (key == "iPar1")            return IPAR1;
	if (key == "iPar2")            return IPAR2;
	if (key == "iPar3")            return IPAR3;
	if (key == "iPar4")            return IPAR4;
	if (key == "iPar5")            return IPAR5;
	if (key == "iBlendmode")       return IBLENDMODE;
	if (key == "iBeat")            return IBEAT;
	if (key == "iBar")             return IBAR;
	if (key == "iBarBeat")         return IBARBEAT;
	if (key == "iFboA")            return IFBOA;
	if (key == "iFboB")            return IFBOB;
	if (key == "iBeatsPerBar")     return IBEATSPERBAR;
	if (key == "iPhase")           return IPHASE;
	if (key == "iTrack")           return ITRACK;
	if (key == "iVAmount")         return IVAMOUNT;
	if (key == "iDateX")           return IDATEX;
	if (key == "iDateY")           return IDATEY;
	if (key == "iDateZ")           return IDATEZ;
	if (key == "iDateW")           return IDATEW;
	if (key == "iBarStart")        return IBARSTART;
	if (key == "iGlitch")          return IGLITCH;
	if (key == "iClear")           return ICLEAR;
	if (key == "iToggle")          return ITOGGLE;
	if (key == "iInvert")          return IINVERT;
	if (key == "iGreyScale")       return IGREYSCALE;
	if (key == "iDebug")           return IDEBUG;
	if (key == "iXorY")            return IXORY;
	if (key == "iFlipH")           return IFLIPH;
	if (key == "iFlipV")           return IFLIPV;
	if (key == "iFlipPostH")       return IFLIPPOSTH;
	if (key == "iFlipPostV")       return IFLIPPOSTV;
	if (key == "iFreq0")           return IFREQ0;
	if (key == "iFreq1")           return IFREQ1;
	if (key == "iFreq2")           return IFREQ2;
	if (key == "iFreq3")           return IFREQ3;
	if (key == "srcXLeft")         return SRCXLEFT;
	if (key == "srcXRight")        return SRCXRIGHT;
	if (key == "srcYLeft")         return SRCYLEFT;
	if (key == "srcYRight")        return SRCYRIGHT;
	if (key == "iMouseX")         return IMOUSEX;
	if (key == "iMouseY")         return IMOUSEY;
	if (key == "iMouseZ")         return IMOUSEZ;
	if (key == "iMouseW")         return IMOUSEW;
	if (key == "iBackgroundColorX") return IBACKGROUNDCOLORX;
	if (key == "iBackgroundColorY") return IBACKGROUNDCOLORY;
	if (key == "iBackgroundColorZ") return IBACKGROUNDCOLORZ;
	if (key == "iResolutionX")     return IRESOLUTIONX;
	if (key == "iResolutionY")     return IRESOLUTIONY;
	if (key == "iPixelX")          return IPIXELX;
	if (key == "iPixelY")          return IPIXELY;
	if (key == "iContour")         return ICONTOUR;
	if (key == "iVFallOff")        return IVFALLOFF;
	if (key == "iVignette")        return IVIGNETTE;
	// speed array
	for (int s = 0; s <= 15; s++) {
		if (key == "iSpeed" + ofToString(s)) return ISPEED0 + s;
	}
	// column array
	for (int c = 1; c <= 8; c++) {
		if (key == "iColumn" + ofToString(c)) return ICOLUMN1 + c - 1;
	}
	// iChannel textures
	for (int ch = 0; ch < 30; ch++) {
		if (key == "iChannel" + ofToString(ch)) return 400 + ch;
	}
	if (key == "inputImage") return 399;
	// search the map directly as fallback
	for (auto& kv : shaderUniforms) {
		if (kv.second.name == key) return kv.first;
	}
	ofLogWarning("VDUniforms") << "stringToIndex: unknown key: " << key;
	return -1;
}
