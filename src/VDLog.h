#pragma once

#include "ofMain.h"

using namespace std;

namespace videodromm
{
	// stores the pointer to the VDLog instance
	typedef std::shared_ptr<class VDLog> VDLogRef;

	class VDLog {
	public:
		static VDLogRef create();
		void log(const string& message);
	private:
		static VDLogRef LOGGER;
		VDLog();
	};
}