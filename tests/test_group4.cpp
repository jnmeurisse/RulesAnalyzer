#include <gtest/gtest.h>
#include <memory>
#include "model\address.h"


TEST(SrcAddressGroup4, address) {
	using namespace fwm;

	std::unique_ptr<const SrcAddress> a1{ SrcAddress::create("a1", "10.0.3.0-10.0.3.1", IPAddressModel::IP4Model, true) };
	std::unique_ptr<const SrcAddress> a2{ SrcAddress::create("a2", "10.0.3.2", IPAddressModel::IP4Model, true) };
	std::unique_ptr<const SrcAddress> a3{ SrcAddress::create("a3", "10.0.3.3-10.0.3.15", IPAddressModel::IP4Model, true) };
	std::unique_ptr<const SrcAddress> a4{ SrcAddress::create("a4", "10.0.3.20", IPAddressModel::IP4Model, true) };

	SrcAddressGroupPtr g1 = std::make_unique<SrcAddressGroup>("g1");
	ASSERT_TRUE(g1->is_none());
	ASSERT_TRUE(g1->empty());
	ASSERT_FALSE(g1->is_any());
	ASSERT_EQ(g1->name(), "g1");

	g1->add_member(a1.get());
	g1->add_member(a2.get());
	g1->add_member(a3.get());
	g1->add_member(a4.get());
	ASSERT_FALSE(g1->is_none());
	ASSERT_FALSE(g1->is_any());
	ASSERT_TRUE(g1->contains(a3.get()));

	g1->add_member(a4.get());
	ASSERT_EQ(g1->items().size(), 4);
	
	SrcAddressGroupPtr g2 = std::make_unique<SrcAddressGroup>("g2");
	ASSERT_TRUE(g2->is_none());
	ASSERT_FALSE(g2->is_any());
	ASSERT_EQ(g2->name(), "g2");
	std::unique_ptr<const SrcAddress> a5{ SrcAddress::create("a5", "10.0.3.0/30", IPAddressModel::IP4Model, true) };
	g2->add_member(a5.get());

	SrcAddressGroupPtr g3 = std::make_unique<SrcAddressGroup>("g3");
	ASSERT_EQ(g3->name(), "g3");
	std::unique_ptr<const SrcAddress> g3_a0{ SrcAddress::create("g3_a0", "10.0.3.0", IPAddressModel::IP4Model, true) };
	std::unique_ptr<const SrcAddress> g3_a1{ SrcAddress::create("g3_a1", "10.0.3.1", IPAddressModel::IP4Model, true) };
	std::unique_ptr<const SrcAddress> g3_a2{ SrcAddress::create("g3_a2", "10.0.3.2", IPAddressModel::IP4Model, true) };
	std::unique_ptr<const SrcAddress> g3_a3{ SrcAddress::create("g3_a3", "10.0.3.3", IPAddressModel::IP4Model, true) };

	g3->add_member(g3_a0.get());
	g3->add_member(g3_a1.get());
	g3->add_member(g3_a2.get());
	g3->add_member(g3_a3.get());
	ASSERT_TRUE(g3->is_subset(*g1));

	ASSERT_TRUE(g2->equal(*g3));
	ASSERT_TRUE(g1->is_superset(*g3));
}


TEST(SrcAddressGroup4, sub_group) {
	using namespace fwm;

	std::unique_ptr<const SrcAddress> a1{ SrcAddress::create("a1", "10.0.3.0-10.0.3.1", IPAddressModel::IP4Model, true) };
	std::unique_ptr<const SrcAddress> a2{ SrcAddress::create("a2", "10.0.3.2", IPAddressModel::IP4Model, true) };
	std::unique_ptr<const SrcAddress> a3{ SrcAddress::create("a3", "10.0.3.3", IPAddressModel::IP4Model, true) };
	std::unique_ptr<const SrcAddress> a5{ SrcAddress::create("a5", "10.0.3.0/30", IPAddressModel::IP4Model, true) };

	SrcAddressGroupPtr g1 = std::make_unique<SrcAddressGroup>("g1");
	SrcAddressGroupPtr g1_1 = std::make_unique<SrcAddressGroup>("g1_1");
	g1_1->add_member(a1.get());

	SrcAddressGroupPtr g1_2_1 = std::make_unique<SrcAddressGroup>("g1_2_1");
	g1_2_1->add_member(a2.get());

	SrcAddressGroupPtr g1_2 = std::make_unique<SrcAddressGroup>("g1_2");
	g1_2->add_member(g1_2_1.get());
	g1_2->add_member(a3.get());

	g1->add_member(g1_1.get());
	g1->add_member(g1_2.get());

	ASSERT_EQ(g1->items().size(), 3);
	ASSERT_TRUE(g1->items().front()->equal(*a1));
	ASSERT_TRUE(g1->contains(a2.get()));
	ASSERT_EQ(g1->to_string(), "g1_1,g1_2");

	SrcAddressGroupPtr g2 = std::make_unique<SrcAddressGroup>("g2");
	g2->add_member(a5.get());

	ASSERT_TRUE(g1->equal(*g2));
	ASSERT_TRUE(g2->equal(*g1));
}


TEST(DstAddressGroup4, address) {
	using namespace fwm;

	std::unique_ptr<const DstAddress> a1{ DstAddress::create("a1", "10.0.3.0-10.0.3.1", IPAddressModel::IP4Model, true) };
	std::unique_ptr<const DstAddress> a2{ DstAddress::create("a2", "10.0.3.2", IPAddressModel::IP4Model, true) };
	std::unique_ptr<const DstAddress> a3{ DstAddress::create("a3", "10.0.3.3-10.0.3.15", IPAddressModel::IP4Model, true) };
	std::unique_ptr<const DstAddress> a4{ DstAddress::create("a4", "10.0.3.20", IPAddressModel::IP4Model, true) };

	DstAddressGroupPtr g1 = std::make_unique<DstAddressGroup>("g1");
	ASSERT_TRUE(g1->is_none());
	ASSERT_TRUE(g1->empty());
	ASSERT_FALSE(g1->is_any());
	ASSERT_EQ(g1->name(), "g1");

	g1->add_member(a1.get());
	g1->add_member(a2.get());
	g1->add_member(a3.get());
	g1->add_member(a4.get());
	ASSERT_FALSE(g1->is_none());
	ASSERT_FALSE(g1->is_any());
	ASSERT_TRUE(g1->contains(a3.get()));

	g1->add_member(a4.get());
	ASSERT_EQ(g1->items().size(), 4);

	DstAddressGroupPtr g2 = std::make_unique<DstAddressGroup>("g2");
	ASSERT_TRUE(g2->is_none());
	ASSERT_FALSE(g2->is_any());
	ASSERT_EQ(g2->name(), "g2");
	std::unique_ptr<const DstAddress> a5{ DstAddress::create("a5", "10.0.3.0/30", IPAddressModel::IP4Model, true) };
	g2->add_member(a5.get());

	DstAddressGroupPtr g3 = std::make_unique<DstAddressGroup>("g3");
	ASSERT_EQ(g3->name(), "g3");
	std::unique_ptr<const DstAddress> g3_a0{ DstAddress::create("g3_a0", "10.0.3.0", IPAddressModel::IP4Model, true) };
	std::unique_ptr<const DstAddress> g3_a1{ DstAddress::create("g3_a1", "10.0.3.1", IPAddressModel::IP4Model, true) };
	std::unique_ptr<const DstAddress> g3_a2{ DstAddress::create("g3_a2", "10.0.3.2", IPAddressModel::IP4Model, true) };
	std::unique_ptr<const DstAddress> g3_a3{ DstAddress::create("g3_a3", "10.0.3.3", IPAddressModel::IP4Model, true) };

	g3->add_member(g3_a0.get());
	g3->add_member(g3_a1.get());
	g3->add_member(g3_a2.get());
	g3->add_member(g3_a3.get());
	ASSERT_TRUE(g3->is_subset(*g1));

	ASSERT_TRUE(g2->equal(*g3));
	ASSERT_TRUE(g1->is_superset(*g3));
}


TEST(DstAddressGroup4, sub_group) {
	using namespace fwm;

	std::unique_ptr<const DstAddress> a1{ DstAddress::create("a1", "10.0.3.0-10.0.3.1", IPAddressModel::IP4Model, true) };
	std::unique_ptr<const DstAddress> a2{ DstAddress::create("a2", "10.0.3.2", IPAddressModel::IP4Model, true) };
	std::unique_ptr<const DstAddress> a3{ DstAddress::create("a3", "10.0.3.3", IPAddressModel::IP4Model, true) };
	std::unique_ptr<const DstAddress> a5{ DstAddress::create("a5", "10.0.3.0/30", IPAddressModel::IP4Model, true) };

	DstAddressGroupPtr g1 = std::make_unique<DstAddressGroup>("g1");
	DstAddressGroupPtr g1_1 = std::make_unique<DstAddressGroup>("g1_1");
	g1_1->add_member(a1.get());

	DstAddressGroupPtr g1_2_1 = std::make_unique<DstAddressGroup>("g1_2_1");
	g1_2_1->add_member(a2.get());

	DstAddressGroupPtr g1_2 = std::make_unique<DstAddressGroup>("g1_2");
	g1_2->add_member(g1_2_1.get());
	g1_2->add_member(a3.get());

	g1->add_member(g1_1.get());
	g1->add_member(g1_2.get());

	ASSERT_EQ(g1->items().size(), 3);
	ASSERT_TRUE(g1->items().front()->equal(*a1));
	ASSERT_TRUE(g1->contains(a2.get()));
	ASSERT_EQ(g1->to_string(), "g1_1,g1_2");

	DstAddressGroupPtr g2 = std::make_unique<DstAddressGroup>("g2");
	g2->add_member(a5.get());

	ASSERT_TRUE(g1->equal(*g2));
	ASSERT_TRUE(g2->equal(*g1));
}
