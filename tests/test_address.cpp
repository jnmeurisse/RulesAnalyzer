#include <gtest/gtest.h>
#include <memory>
#include "model\address.h"


TEST(SrcAddressTest, address) {
	using namespace fwm;
	std::unique_ptr<SrcAddress> a{ SrcAddress::create("a3", "192.0.2.1", true) };

	EXPECT_EQ(a->interval().size(), 1);
	EXPECT_EQ(a->interval().lower(), 3221225985);
	EXPECT_EQ(a->interval().upper(), 3221225985);
	EXPECT_EQ(a->at(), Ipv4AddressType::Address);
}


TEST(SrcAddressTest, subnet) {
	using namespace fwm;
	std::unique_ptr<SrcAddress> a1{ SrcAddress::create("a1", "10.0.0.0/8", true) };
	std::unique_ptr<SrcAddress> a2{ SrcAddress::create("a2", "10.0.0.0/255.0.0.0", true) };

	EXPECT_TRUE(a1->equal(*a2));
	EXPECT_EQ(a1->interval().size(), a2->interval().size());
	EXPECT_TRUE(a1->equal(*a2));

	EXPECT_EQ(a1->at(), Ipv4AddressType::Subnet);
	EXPECT_EQ(a2->at(), Ipv4AddressType::Subnet);
}


TEST(SrcAddressTest, range) {
	using namespace fwm;
	std::unique_ptr<SrcAddress> a1{ SrcAddress::create("a1", "0.0.0.0-255.255.255.255", true) };
	std::unique_ptr<SrcAddress> a2{ SrcAddress::create("a2", "any", true) };
	std::unique_ptr<SrcAddress> a3{ SrcAddress::create("a3", "192.0.2.1-192.0.2.1", true) };

	EXPECT_TRUE(a1->equal(*a2));
	EXPECT_EQ(a1->interval().size(), a2->interval().size());
	EXPECT_TRUE(a1->equal(*a2));

	EXPECT_EQ(a3->interval().size(), 1);
	EXPECT_EQ(a3->interval().lower(), 3221225985);
	EXPECT_EQ(a3->interval().upper(), 3221225985);

	EXPECT_EQ(a1->at(), Ipv4AddressType::Range);
	EXPECT_EQ(a2->at(), Ipv4AddressType::Subnet);
	EXPECT_EQ(a3->at(), Ipv4AddressType::Range);
}


TEST(SrcAddressTest, syntax) {
	using namespace fwm;

	EXPECT_THROW(SrcAddress::create("a1", "10.0.0.0.0", true), fwm::Ipv4AddressError);
	EXPECT_THROW(SrcAddress::create("a1", "72.12.260.0", true), fwm::Ipv4AddressError);
	EXPECT_THROW(SrcAddress::create("a1", "10.0.0.0/", true), fwm::Ipv4AddressError);
	EXPECT_THROW(SrcAddress::create("a1", "10.0..0", true), fwm::Ipv4AddressError);
	EXPECT_THROW(SrcAddress::create("a1", "", true), fwm::Ipv4AddressError);
	EXPECT_THROW(SrcAddress::create("a1", "192.168.0.10-192.168.0.0", true), fwm::Ipv4AddressError);
	EXPECT_THROW(SrcAddress::create("a1", "-192.168.0.0", true), fwm::Ipv4AddressError);
	EXPECT_THROW(SrcAddress::create("a1", "-", true), fwm::Ipv4AddressError);
	EXPECT_THROW(SrcAddress::create("a1", "/", true), fwm::Ipv4AddressError);
	EXPECT_THROW(SrcAddress::create("a1", "192.168.10.0/8", true), fwm::Ipv4AddressError);
}


TEST(SrcAddressTest, to_string) {
	using namespace fwm;

	std::unique_ptr<SrcAddress> a1{ SrcAddress::create("a1", "10.0.0.0/8", true) };
	std::unique_ptr<SrcAddress> a2{ SrcAddress::create("a2", "10.0.0.0/255.0.0.0", true) };
	std::unique_ptr<SrcAddress> a3{ SrcAddress::create("a3", "192.0.2.1", true) };
	std::unique_ptr<SrcAddress> a4{ SrcAddress::create("a3", "192.0.2.1-192.0.2.2", true) };
	std::unique_ptr<SrcAddress> a5{ SrcAddress::create("a3", "192.0.2.1-192.0.2.3", true) };

	EXPECT_EQ(a1->to_string(), "10.0.0.0/8");
	EXPECT_EQ(a2->to_string(), "10.0.0.0/8");
	EXPECT_EQ(a3->to_string(), "192.0.2.1");
	EXPECT_EQ(a4->to_string(), "192.0.2.1/31");
	EXPECT_EQ(a5->to_string(), "192.0.2.1+2");
}


TEST(SrcAddressTest, bdd) {
	using namespace fwm;

	std::unique_ptr<SrcAddress> a1{ SrcAddress::create("a1", "10.0.0.0/8", true) };
	std::unique_ptr<SrcAddress> a2{ SrcAddress::create("a2", "10.4.1.0/24", true) };
	std::unique_ptr<SrcAddress> a3{ SrcAddress::create("a3", "192.168.0.0/24", true) };
	std::unique_ptr<SrcAddress> a4{ SrcAddress::create("a4", "192.168.0.1-192.168.0.100", true) };
	std::unique_ptr<SrcAddress> a5{ SrcAddress::create("a5", "192.168.0.50-192.168.0.110", true) };

	EXPECT_TRUE(a2->is_subset(*a1));
	EXPECT_TRUE(a3->is_disjoint(*a1));
	EXPECT_TRUE(a4->overlaps(*a5));
	EXPECT_TRUE(a5->overlaps(*a4));

	EXPECT_EQ(a1->compare(*a2), MnodeRelationship::superset);
	EXPECT_EQ(a2->compare(*a1), MnodeRelationship::subset);
	EXPECT_EQ(a5->compare(*a4), MnodeRelationship::overlap);
	EXPECT_EQ(a4->compare(*a5), MnodeRelationship::overlap);
}


TEST(DstAddressTest, address) {
	using namespace fwm;
	std::unique_ptr<DstAddress> a{ DstAddress::create("a3", "192.0.2.1", true) };

	EXPECT_EQ(a->interval().size(), 1);
	EXPECT_EQ(a->interval().lower(), 3221225985);
	EXPECT_EQ(a->interval().upper(), 3221225985);
	EXPECT_EQ(a->at(), Ipv4AddressType::Address);
}


TEST(DstAddressTest, subnet) {
	using namespace fwm;
	std::unique_ptr<DstAddress> a1{ DstAddress::create("a1", "10.0.0.0/8", true) };
	std::unique_ptr<DstAddress> a2{ DstAddress::create("a2", "10.0.0.0/255.0.0.0", true) };

	EXPECT_TRUE(a1->equal(*a2));
	EXPECT_EQ(a1->interval().size(), a2->interval().size());
	EXPECT_TRUE(a1->equal(*a2));

	EXPECT_EQ(a1->at(), Ipv4AddressType::Subnet);
	EXPECT_EQ(a2->at(), Ipv4AddressType::Subnet);
}


TEST(DstAddressTest, range) {
	using namespace fwm;
	std::unique_ptr<DstAddress> a1{ DstAddress::create("a1", "0.0.0.0-255.255.255.255", true) };
	std::unique_ptr<DstAddress> a2{ DstAddress::create("a2", "any", true) };
	std::unique_ptr<SrcAddress> a3{ SrcAddress::create("a3", "192.0.2.1-192.0.2.1", true) };

	EXPECT_TRUE(a1->equal(*a2));
	EXPECT_EQ(a1->interval().size(), a2->interval().size());
	EXPECT_TRUE(a1->equal(*a2));

	EXPECT_EQ(a3->interval().size(), 1);
	EXPECT_EQ(a3->interval().lower(), 3221225985);
	EXPECT_EQ(a3->interval().upper(), 3221225985);

	EXPECT_EQ(a1->at(), Ipv4AddressType::Range);
	EXPECT_EQ(a2->at(), Ipv4AddressType::Subnet);
	EXPECT_EQ(a3->at(), Ipv4AddressType::Range);
}


TEST(DstAddressTest, syntax) {
	using namespace fwm;

	EXPECT_THROW(DstAddress::create("a1", "10.0.0.0.0", true), fwm::Ipv4AddressError);
	EXPECT_THROW(DstAddress::create("a1", "72.12.260.0", true), fwm::Ipv4AddressError);
	EXPECT_THROW(DstAddress::create("a1", "10.0.0.0/", true), fwm::Ipv4AddressError);
	EXPECT_THROW(DstAddress::create("a1", "10.0..0", true), fwm::Ipv4AddressError);
	EXPECT_THROW(DstAddress::create("a1", "", true), fwm::Ipv4AddressError);
	EXPECT_THROW(DstAddress::create("a1", "192.168.0.10-192.168.0.0", true), fwm::Ipv4AddressError);
	EXPECT_THROW(DstAddress::create("a1", "-192.168.0.0", true), fwm::Ipv4AddressError);
	EXPECT_THROW(DstAddress::create("a1", "-", true), fwm::Ipv4AddressError);
	EXPECT_THROW(DstAddress::create("a1", "/", true), fwm::Ipv4AddressError);
	EXPECT_THROW(DstAddress::create("a1", "192.168.10.0/8", true), fwm::Ipv4AddressError);
}


TEST(DstAddressTest, to_string) {
	using namespace fwm;

	std::unique_ptr<DstAddress> a1{ DstAddress::create("a1", "10.0.0.0/8", true) };
	std::unique_ptr<DstAddress> a2{ DstAddress::create("a2", "10.0.0.0/255.0.0.0", true) };
	std::unique_ptr<DstAddress> a3{ DstAddress::create("a3", "192.0.2.1", true) };
	std::unique_ptr<DstAddress> a4{ DstAddress::create("a3", "192.0.2.1-192.0.2.2", true) };
	std::unique_ptr<DstAddress> a5{ DstAddress::create("a3", "192.0.2.1-192.0.2.3", true) };

	EXPECT_EQ(a1->to_string(), "10.0.0.0/8");
	EXPECT_EQ(a2->to_string(), "10.0.0.0/8");
	EXPECT_EQ(a3->to_string(), "192.0.2.1");
	EXPECT_EQ(a4->to_string(), "192.0.2.1/31");
	EXPECT_EQ(a5->to_string(), "192.0.2.1+2");
}


TEST(DstAddressTest, bdd) {
	using namespace fwm;

	std::unique_ptr<DstAddress> a1{ DstAddress::create("a1", "10.0.0.0/8", true) };
	std::unique_ptr<DstAddress> a2{ DstAddress::create("a2", "10.4.1.0/24", true) };
	std::unique_ptr<DstAddress> a3{ DstAddress::create("a3", "192.168.0.0/24", true) };
	std::unique_ptr<DstAddress> a4{ DstAddress::create("a4", "192.168.0.1-192.168.0.100", true) };
	std::unique_ptr<DstAddress> a5{ DstAddress::create("a5", "192.168.0.50-192.168.0.110", true) };

	EXPECT_TRUE(a2->is_subset(*a1));
	EXPECT_TRUE(a3->is_disjoint(*a1));
	EXPECT_TRUE(a4->overlaps(*a5));
	EXPECT_TRUE(a5->overlaps(*a4));

	EXPECT_EQ(a1->compare(*a2), MnodeRelationship::superset);
	EXPECT_EQ(a2->compare(*a1), MnodeRelationship::subset);
	EXPECT_EQ(a5->compare(*a4), MnodeRelationship::overlap);
	EXPECT_EQ(a4->compare(*a5), MnodeRelationship::overlap);
}
