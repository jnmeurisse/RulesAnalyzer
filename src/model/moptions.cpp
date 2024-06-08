#include "moptions.h"


namespace fwm {
	const ModelOptions& ModelOptions::empty()
	{
		static ModelOptions empty_set;

		return empty_set;
	}
}
