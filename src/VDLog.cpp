#include "VDLog.h"

using namespace videodromm;

VDLogRef VDLog::LOGGER;

VDLogRef VDLog::create() {
	if (!LOGGER) {
		LOGGER = VDLogRef(new VDLog());
	}
	return LOGGER;
}

VDLog::VDLog()
{
	ofSetLogLevel(OF_LOG_WARNING);
#ifdef _DEBUG
	string path = "videodromm.log.txt";
	ofLogToFile(path);
#endif
}

void VDLog::log(const string& message) {
	ofLog(OF_LOG_NOTICE, message);
}

