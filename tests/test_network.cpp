#include <gtest/gtest.h>
#include "model\network.h"

TEST(Network, src_address) {
	using namespace fwm;

	Network network;

	ASSERT_TRUE(network.get_src_address("any"));
	ASSERT_TRUE(network.get_src_address("any")->is_any());
	ASSERT_EQ(network.get_src_address("any")->to_string(), "0.0.0.0/0");

	ASSERT_NO_THROW(network.register_src_address("a1", "10.0.4.0/30"));
	ASSERT_NO_THROW(network.register_src_address("a2", "10.0.5.0/30"));
	ASSERT_TRUE(network.get_src_address("a1"));
	ASSERT_TRUE(network.get_src_address("a2"));
	ASSERT_EQ(network.get_src_address("a0"), nullptr);
	ASSERT_EQ(network.register_src_address("a2", "10.0.2.0/30"), network.get_src_address("a2"));
}


TEST(Network, src_address_group) {
	using namespace fwm;

	Network network;

	ASSERT_NO_THROW(network.register_src_address("a1", "10.0.4.0/30"));
	ASSERT_NO_THROW(network.register_src_address("a2", "10.0.5.0/30"));
	ASSERT_NO_THROW(network.register_src_address("a3", "10.0.4.2"));

	ASSERT_NO_THROW(network.register_src_address_group("g1.1", {"a1", "a2"}));
	ASSERT_NO_THROW(network.register_src_address_group("g1.2", { "a1", "a2" }));
	ASSERT_NO_THROW(network.register_src_address_group("g1.3", { "a1", "a2", "a3" }));

	ASSERT_NO_THROW(network.register_src_address_group("g1", {"g1.1", "g1.2", "g1.3"}));

	ASSERT_TRUE(network.get_src_address_group("g1")->contains(network.get_src_address("a2")));
	ASSERT_FALSE(network.get_src_address_group("g1")->contains(network.get_src_address("xx")));
	ASSERT_TRUE(network.get_src_address_group("g1")->is_superset(*network.get_src_address("a2")));
	ASSERT_TRUE(network.get_src_address("a2")->is_subset(*network.get_src_address_group("g1")));
	ASSERT_TRUE(network.get_src_address_group("g1")->contains(network.get_src_address("a3")));
}