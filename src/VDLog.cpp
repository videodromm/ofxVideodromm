#include "VDLog.h"

using namespace VideoDromm;
	
VDLog::VDLog()
{
	ofSetLogLevel(OF_LOG_ERROR);
	string path = "videodromm.txt";
	ofLogToFile(path);
}
void VDLog::log(string message) { 
	ofLog(OF_LOG_NOTICE, message);
}

