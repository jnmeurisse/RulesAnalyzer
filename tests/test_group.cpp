#include <gtest/gtest.h>
#include <memory>
#include "model\address.h"


TEST(SrcAddressGroup, address) {
	using namespace fwm;

	std::unique_ptr<const SrcAddress> a1{ SrcAddress::create("", "10.0.3.0-10.0.3.1", true) };
	std::unique_ptr<const SrcAddress> a2{ SrcAddress::create("", "10.0.3.2", true) };
	std::unique_ptr<const SrcAddress> a3{ SrcAddress::create("", "10.0.3.3-10.0.3.15", true) };
	std::unique_ptr<const SrcAddress> a4{ SrcAddress::create("", "10.0.3.20", true) };

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
	std::unique_ptr<const SrcAddress> a5{ SrcAddress::create("", "10.0.3.0/30", true) };
	g2->add_member(a5.get());

	SrcAddressGroupPtr g3 = std::make_unique<SrcAddressGroup>("g3");
	ASSERT_EQ(g3->name(), "g3");
	std::unique_ptr<const SrcAddress> g3_a0{ SrcAddress::create("", "10.0.3.0", true) };
	std::unique_ptr<const SrcAddress> g3_a1{ SrcAddress::create("", "10.0.3.1", true) };
	std::unique_ptr<const SrcAddress> g3_a2{ SrcAddress::create("", "10.0.3.2", true) };
	std::unique_ptr<const SrcAddress> g3_a3{ SrcAddress::create("", "10.0.3.3", true) };

	g3->add_member(g3_a0.get());
	g3->add_member(g3_a1.get());
	g3->add_member(g3_a2.get());
	g3->add_member(g3_a3.get());
	ASSERT_TRUE(g3->is_subset(*g1));

	ASSERT_TRUE(g2->equal(*g3));
	ASSERT_TRUE(g1->is_superset(*g3));
}


TEST(SrcAddressGroup, sub_group) {
	using namespace fwm;

	std::unique_ptr<const SrcAddress> a1{ SrcAddress::create("r1", "10.0.3.0-10.0.3.1", true) };
	std::unique_ptr<const SrcAddress> a2{ SrcAddress::create("r2", "10.0.3.2", true) };
	std::unique_ptr<const SrcAddress> a3{ SrcAddress::create("r3", "10.0.3.3", true) };
	std::unique_ptr<const SrcAddress> a5{ SrcAddress::create("", "10.0.3.0/30", true) };

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


TEST(DstAddressGroup, address) {
	using namespace fwm;

	std::unique_ptr<const DstAddress> a1{ DstAddress::create("", "10.0.3.0-10.0.3.1", true) };
	std::unique_ptr<const DstAddress> a2{ DstAddress::create("", "10.0.3.2", true) };
	std::unique_ptr<const DstAddress> a3{ DstAddress::create("", "10.0.3.3-10.0.3.15", true) };
	std::unique_ptr<const DstAddress> a4{ DstAddress::create("", "10.0.3.20", true) };

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
	std::unique_ptr<const DstAddress> a5{ DstAddress::create("", "10.0.3.0/30", true) };
	g2->add_member(a5.get());

	DstAddressGroupPtr g3 = std::make_unique<DstAddressGroup>("g3");
	ASSERT_EQ(g3->name(), "g3");
	std::unique_ptr<const DstAddress> g3_a0{ DstAddress::create("", "10.0.3.0", true) };
	std::unique_ptr<const DstAddress> g3_a1{ DstAddress::create("", "10.0.3.1", true) };
	std::unique_ptr<const DstAddress> g3_a2{ DstAddress::create("", "10.0.3.2", true) };
	std::unique_ptr<const DstAddress> g3_a3{ DstAddress::create("", "10.0.3.3", true) };

	g3->add_member(g3_a0.get());
	g3->add_member(g3_a1.get());
	g3->add_member(g3_a2.get());
	g3->add_member(g3_a3.get());
	ASSERT_TRUE(g3->is_subset(*g1));

	ASSERT_TRUE(g2->equal(*g3));
	ASSERT_TRUE(g1->is_superset(*g3));
}


TEST(DstAddressGroup, sub_group) {
	using namespace fwm;

	std::unique_ptr<const DstAddress> a1{ DstAddress::create("r1", "10.0.3.0-10.0.3.1", true) };
	std::unique_ptr<const DstAddress> a2{ DstAddress::create("r2", "10.0.3.2", true) };
	std::unique_ptr<const DstAddress> a3{ DstAddress::create("r3", "10.0.3.3", true) };
	std::unique_ptr<const DstAddress> a5{ DstAddress::create("", "10.0.3.0/30", true) };

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
