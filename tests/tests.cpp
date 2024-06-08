#include <gtest\gtest.h>
#include "model\domains.h"


void main(int argc, char* arg[])
{
	fwm::Domains domains;
	domains.init(400000, 10000);

	testing::InitGoogleTest();
	RUN_ALL_TESTS();
}
