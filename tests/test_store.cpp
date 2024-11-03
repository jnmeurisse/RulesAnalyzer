#include <gtest/gtest.h>
#include "model/ipaddress.h"
#include "ostore/objectstore.h"
#include "ostore/ostoreconfig.h"


TEST(ObjectsStore, test1) {
	using namespace fos;

	OstoreConfig config;
	config.model_config.ip_model = IPAddressModel::IP4Model;

	ObjectStore os{config};
	os.initialize();

//	os.load_address_groups("")
}
