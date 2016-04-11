#pragma once

#include "ofMain.h"

using namespace std;

namespace VideoDromm
{
	// stores the pointer to the VDLog instance
	typedef std::shared_ptr<class VDLog> VDLogRef;

	class VDLog {
	public:		
		VDLog();

		static VDLogRef	create()
		{
			return shared_ptr<VDLog>(new VDLog());
		}
		void log(string message);
	private:

	};


}