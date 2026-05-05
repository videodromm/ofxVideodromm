#include "VDParams.h"

using namespace videodromm;

VDParamsRef VDParams::PARAMS;

VDParamsRef VDParams::create() {
	if (!PARAMS) {
		PARAMS = VDParamsRef(new VDParams());
	}
	return PARAMS;
}
