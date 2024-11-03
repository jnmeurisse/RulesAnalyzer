#include <gtest/gtest.h>
#include "model/domains.h"


int main(int argc, char* arg[])
{
	fwm::Domains& domains = fwm::Domains::get();
	domains.init_bdd(400000, 10000);

	testing::InitGoogleTest();
	return RUN_ALL_TESTS();
}
