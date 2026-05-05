meta:
	ADDON_NAME = ofxVideodromm
	ADDON_DESCRIPTION = openFrameworks addon for Videodromm projects
	ADDON_AUTHOR = Bruce Lane
	ADDON_TAGS = "Videodromm"
	ADDON_URL = https://github.com/videodromm/ofxVideodromm

common:
	# Dependencies (must be present in OF addons folder)
	ADDON_DEPENDENCIES = ofxOsc ofxMidi ofxLibwebsockets ofxXmlSettings
	# ofxSpout2 on Windows only; ofxSyphon on macOS — add conditionally in project
	# ADDON_DEPENDENCIES += ofxSpout2   (Windows)
	# ADDON_DEPENDENCIES += ofxSyphon   (macOS)

	ADDON_LIBS =
	ADDON_INCLUDES =
	ADDON_CFLAGS =
