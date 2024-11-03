#include <gtest/gtest.h>
#include "model/network.h"

TEST(Network6, src_address) {
	using namespace fwm;

	ModelConfig model_config;
	model_config.ip_model = IPAddressModel::IP6Model;
	model_config.strict_ip_parser = true;
	Network network(model_config);

	ASSERT_TRUE(network.get_src_address("any"));
	ASSERT_TRUE(network.get_src_address("any")->is_any());
	ASSERT_EQ(network.get_src_address("any")->to_string(), "::/0");
	
	ASSERT_NO_THROW(network.register_src_address("a1", "2001:db8:2:1::/64"));
	ASSERT_NO_THROW(network.register_src_address("a2", "2001:db8:5:1::/64"));
	ASSERT_TRUE(network.get_src_address("a1"));
	ASSERT_TRUE(network.get_src_address("a2"));
	ASSERT_EQ(network.get_src_address("a0"), nullptr);
	ASSERT_EQ(network.register_src_address("a2", "2001:db8:6:1::/64"), network.get_src_address("a2"));
}


TEST(Network6, src_address_group) {
	using namespace fwm;

	ModelConfig model_config;
	model_config.ip_model = IPAddressModel::IP6Model;
	model_config.strict_ip_parser = true;
	Network network(model_config);

	ASSERT_NO_THROW(network.register_src_address("a1", "2001:db8:2:1::/64"));
	ASSERT_NO_THROW(network.register_src_address("a2", "2001:db8:5:1::/64"));
	ASSERT_NO_THROW(network.register_src_address("a3", "2001:db8:2:1::1"));

	ASSERT_NO_THROW(network.register_src_address_group("g1.1", { "a1", "a2" }));
	ASSERT_NO_THROW(network.register_src_address_group("g1.2", { "a1", "a2" }));
	ASSERT_NO_THROW(network.register_src_address_group("g1.3", { "a1", "a2", "a3" }));

	ASSERT_NO_THROW(network.register_src_address_group("g1", {"g1.1", "g1.2", "g1.3"}));

	ASSERT_TRUE(network.get_src_address_group("g1")->contains(network.get_src_address("a2")));
	ASSERT_FALSE(network.get_src_address_group("g1")->contains(network.get_src_address("xx")));
	ASSERT_TRUE(network.get_src_address_group("g1")->is_superset(*network.get_src_address("a2")));
	ASSERT_TRUE(network.get_src_address("a2")->is_subset(*network.get_src_address_group("g1")));
	ASSERT_TRUE(network.get_src_address_group("g1")->contains(network.get_src_address("a3")));
}


TEST(Network6, dst_address) {
	using namespace fwm;

	ModelConfig model_config;
	model_config.ip_model = IPAddressModel::IP6Model;
	model_config.strict_ip_parser = true;
	Network network(model_config);

	ASSERT_TRUE(network.get_dst_address("any"));
	ASSERT_TRUE(network.get_dst_address("any")->is_any());
	ASSERT_EQ(network.get_dst_address("any")->to_string(), "::/0");

	ASSERT_NO_THROW(network.register_dst_address("a1", "2001:dc8:2:1::/64"));
	ASSERT_NO_THROW(network.register_dst_address("a2", "2001:dc8:5:1::/64"));
	ASSERT_TRUE(network.get_dst_address("a1"));
	ASSERT_TRUE(network.get_dst_address("a2"));
	ASSERT_EQ(network.get_dst_address("a0"), nullptr);
	ASSERT_EQ(network.register_dst_address("a2", "2001:dc8:6:1::/64"), network.get_dst_address("a2"));
}


TEST(Network6, dst_address_group) {
	using namespace fwm;

	ModelConfig model_config;
	model_config.ip_model = IPAddressModel::IP6Model;
	model_config.strict_ip_parser = true;
	Network network(model_config);

	ASSERT_NO_THROW(network.register_dst_address("a1", "2001:dc8:2:1::/64"));
	ASSERT_NO_THROW(network.register_dst_address("a2", "2001:dc8:5:1::/64"));
	ASSERT_NO_THROW(network.register_dst_address("a3", "2001:dc8:2:1::1"));

	ASSERT_NO_THROW(network.register_dst_address_group("g1.1", { "a1", "a2" }));
	ASSERT_NO_THROW(network.register_dst_address_group("g1.2", { "a1", "a2" }));
	ASSERT_NO_THROW(network.register_dst_address_group("g1.3", { "a1", "a2", "a3" }));

	ASSERT_NO_THROW(network.register_dst_address_group("g1", { "g1.1", "g1.2", "g1.3" }));

	ASSERT_TRUE(network.get_dst_address_group("g1")->contains(network.get_dst_address("a2")));
	ASSERT_FALSE(network.get_dst_address_group("g1")->contains(network.get_dst_address("xx")));
	ASSERT_TRUE(network.get_dst_address_group("g1")->is_superset(*network.get_dst_address("a2")));
	ASSERT_TRUE(network.get_dst_address("a2")->is_subset(*network.get_dst_address_group("g1")));
	ASSERT_TRUE(network.get_dst_address_group("g1")->contains(network.get_dst_address("a3")));
}
