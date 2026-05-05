#pragma once
#include "ofMain.h"
#include "VDSettings.h"
#include "VDAnimation.h"
#include "VDUniforms.h"
#include "VDMix.h"
#include "VDMediator.h"
#include "VDFboShader.h"

namespace videodromm
{
	typedef std::shared_ptr<class VDSession> VDSessionRef;
	class VDSession {
	public:
		VDSession(VDSettingsRef aVDSettings, VDAnimationRef aVDAnimation, VDUniformsRef aVDUniforms, VDMixRef aVDMix);
		~VDSession() {}
		static VDSessionRef create(VDSettingsRef aVDSettings, VDAnimationRef aVDAnimation, VDUniformsRef aVDUniforms, VDMixRef aVDMix) {
			return std::make_shared<VDSession>(aVDSettings, aVDAnimation, aVDUniforms, aVDMix);
		}
		void update();
		void draw();

		// Textures
		ofTexture*  getFboRenderedTexture(unsigned int aFboIndex);
		ofTexture*  getFboTexture(unsigned int aFboIndex);
		ofTexture*  getMixetteTexture(unsigned int aMixIndex = 0);
		ofTexture*  getPostFboTexture();
		ofTexture*  getFxFboTexture();
		ofTexture*  getAudioTexture();

		// FBO / Shader management
		unsigned int    getFboShaderListSize()  { return mVDMix->getFboShaderListSize(); }
		bool            isFboValid(unsigned int i) { return mVDMix->isFboValid(i); }
		unsigned int    createFboShaderTexture(const ofJson& aFboJson, unsigned int aFboIndex, const std::string& aFolder = "");
		bool            setFragmentShaderString(const std::string& s, const std::string& name, unsigned int index);
		int             loadFragmentShader(const std::string& path, unsigned int index);
		void            loadImageFile(const std::string& path, unsigned int index);
		void            loadVideoFile(const std::string& path, unsigned int index);
		void            loadAudioFile(const std::string& path);
		std::vector<VDShaderUniform> getFboShaderUniforms(unsigned int i) { return mVDMix->getFboShaderUniforms(i); }

		// Event handlers
		bool        handleKeyDown(int key, bool ctrlDown, bool shiftDown, bool altDown);
		bool        handleKeyUp(int key, bool ctrlDown, bool shiftDown, bool altDown);
		void        handleMouseMove(int mx, int my);
		void        handleMouseDown(int mx, int my);
		void        handleMouseDrag(int mx, int my);
		void        handleMouseUp(int mx, int my);
		void        fileDrop(const std::string& filePath, unsigned int index);

		// Persistence
		void        loadFromJsonFile(const std::string& path);
		void        save();
		void        restore();
		void        saveWarps() {} // stub

		// Warp stubs
		unsigned int getWarpCount() { return 0; }

		// Modes
		std::map<int, std::string>& getModesList() { return mModesList; }

		std::string getMode() { return mVDSettings->getMode(); }
		void        setMode(const std::string& m) { mVDSettings->setMode(m); }

	private:
		VDSettingsRef mVDSettings;
		VDAnimationRef mVDAnimation;
		VDUniformsRef  mVDUniforms;
		VDMixRef       mVDMix;

		ofFbo     mPostFbo, mFxFbo;
		ofShader  mGlslPost, mGlslFx;

		std::map<int, std::string> mModesList;
		std::string mAssetsFolder;

		void renderPostToFbo();
		void renderFxToFbo();
		void fboFromJson(const ofJson& json);
		void loadFolder(const std::string& folder);
	};
}
