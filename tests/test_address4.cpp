#include <gtest/gtest.h>
#include <memory>

#include "buddy/bdd.h"
#include "buddy/bvec.h"
#include "model/ipaddress.h"
#include "model/address.h"


TEST(SrcAddress4Test, address) {
	using namespace fwm;
	std::unique_ptr<const SrcAddress> a{ SrcAddress::create("a3", "192.0.2.1", IPAddressModel::IP4Model, true) };

	EXPECT_TRUE(a->value().range().is_singleton());
	EXPECT_EQ(a->at(), IPAddressType::Address);
	EXPECT_EQ(a->version(), 4);
	EXPECT_EQ(a->value().range().lbound().bitnum(), 32);
	EXPECT_EQ(a->value().range().lbound().bitnum(), a->value().range().nbits());
	EXPECT_EQ(bvec_val(a->value().range().lbound()), 3221225985);
	EXPECT_EQ(bvec_val(a->value().range().ubound()), 3221225985);
}


TEST(SrcAddress4Test, subnet) {
	using namespace fwm;
	std::unique_ptr<const SrcAddress> a1{ SrcAddress::create("a1", "10.0.0.0/8", IPAddressModel::IP4Model, true) };
	std::unique_ptr<const SrcAddress> a2{ SrcAddress::create("a2", "10.0.0.0/255.0.0.0", IPAddressModel::IP4Model, true) };

	EXPECT_TRUE(a1->equal(*a2));
	EXPECT_TRUE(a2->equal(*a1));

	EXPECT_EQ(a1->at(), IPAddressType::Subnet);
	EXPECT_EQ(a2->at(), IPAddressType::Subnet);

	EXPECT_EQ(a1->to_string(), a2->to_string());
}


TEST(SrcAddress4Test, range) {
	using namespace fwm;
	std::unique_ptr<const SrcAddress> a1{ SrcAddress::create("a1", "0.0.0.0-255.255.255.255", IPAddressModel::IP4Model, true) };
	std::unique_ptr<const SrcAddress> a2{ SrcAddress::create("a2", "0.0.0.0/0", IPAddressModel::IP4Model, true) };

	EXPECT_TRUE(a1->equal(*a2));
	EXPECT_TRUE(a2->equal(*a1));
	EXPECT_EQ(bvec_val(a1->value().range().lbound()), 0);
	EXPECT_EQ(bvec_val(a1->value().range().ubound()), UINT32_MAX);
	EXPECT_EQ(bvec_val(a2->value().range().lbound()), 0);
	EXPECT_EQ(bvec_val(a2->value().range().ubound()), UINT32_MAX);

	std::unique_ptr<const SrcAddress> a3{ SrcAddress::create("a3", "192.0.2.1-192.0.2.1", IPAddressModel::IP4Model, true) };
	EXPECT_TRUE(a3->value().range().is_singleton());
	EXPECT_EQ(bvec_val(a3->value().range().lbound()), 3221225985);
	EXPECT_EQ(bvec_val(a3->value().range().lbound()), 3221225985);

	EXPECT_EQ(a1->at(), IPAddressType::Subnet);
	EXPECT_EQ(a2->at(), IPAddressType::Subnet);
	EXPECT_EQ(a3->at(), IPAddressType::Address);
}


TEST(SrcAddress4Test, syntax) {
	using namespace fwm;

	EXPECT_THROW(SrcAddress::create("a1", "10.0.0.0.0", IPAddressModel::IP4Model, true), fwm::IPAddressError);
	EXPECT_THROW(SrcAddress::create("a1", "72.12.260.0", IPAddressModel::IP4Model, true), fwm::IPAddressError);
	EXPECT_THROW(SrcAddress::create("a1", "10.0.0.0/", IPAddressModel::IP4Model, true), fwm::IPAddressError);
	EXPECT_THROW(SrcAddress::create("a1", "10.0..0", IPAddressModel::IP4Model, true), fwm::IPAddressError);
	EXPECT_THROW(SrcAddress::create("a1", "", IPAddressModel::IP4Model, true), fwm::IPAddressError);
	EXPECT_THROW(SrcAddress::create("a1", "192.168.0.10-192.168.0.0", IPAddressModel::IP4Model, true), fwm::IPAddressError);
	EXPECT_THROW(SrcAddress::create("a1", "-192.168.0.0", IPAddressModel::IP4Model, true), fwm::IPAddressError);
	EXPECT_THROW(SrcAddress::create("a1", "-", IPAddressModel::IP4Model, true), fwm::IPAddressError);
	EXPECT_THROW(SrcAddress::create("a1", "/", IPAddressModel::IP4Model, true), fwm::IPAddressError);
	EXPECT_THROW(SrcAddress::create("a1", "192.168.10.0/8", IPAddressModel::IP4Model, true), fwm::IPAddressError);
}


TEST(SrcAddress4Test, to_string) {
	using namespace fwm;

	std::unique_ptr<const SrcAddress> a1{ SrcAddress::create("a1", "10.0.0.0/8", IPAddressModel::IP4Model, true) };
	std::unique_ptr<const SrcAddress> a2{ SrcAddress::create("a2", "10.0.0.0/255.0.0.0", IPAddressModel::IP4Model, true) };
	std::unique_ptr<const SrcAddress> a3{ SrcAddress::create("a3", "192.0.2.1", IPAddressModel::IP4Model, true) };
	std::unique_ptr<const SrcAddress> a4{ SrcAddress::create("a4", "192.0.2.1-192.0.2.2", IPAddressModel::IP4Model, true) };
	std::unique_ptr<const SrcAddress> a5{ SrcAddress::create("a5", "192.0.2.1-192.0.2.3", IPAddressModel::IP4Model, true) };
	std::unique_ptr<const SrcAddress> a6{ SrcAddress::create("a6", "0.0.0.0/0", IPAddressModel::IP4Model, true) };
	std::unique_ptr<const SrcAddress> a7{ SrcAddress::any4(IPAddressModel::IP4Model) };

	EXPECT_EQ(a1->to_string(), "10.0.0.0/8");
	EXPECT_EQ(a2->to_string(), "10.0.0.0/8");
	EXPECT_EQ(a3->to_string(), "192.0.2.1");
	EXPECT_EQ(a4->to_string(), "192.0.2.1/31");
	EXPECT_EQ(a5->to_string(), "192.0.2.1+2");
	EXPECT_EQ(a6->to_string(), "0.0.0.0/0");
	EXPECT_EQ(a7->to_string(), "0.0.0.0/0");
}


TEST(SrcAddress4Test, bdd) {
	using namespace fwm;

	std::unique_ptr<const SrcAddress> a1{ SrcAddress::create("a1", "10.0.0.0/8", IPAddressModel::IP4Model, true) };
	std::unique_ptr<const SrcAddress> a2{ SrcAddress::create("a2", "10.4.1.0/24", IPAddressModel::IP4Model, true) };
	std::unique_ptr<const SrcAddress> a3{ SrcAddress::create("a3", "192.168.0.0/24", IPAddressModel::IP4Model, true) };
	std::unique_ptr<const SrcAddress> a4{ SrcAddress::create("a4", "192.168.0.1-192.168.0.100", IPAddressModel::IP4Model, true) };
	std::unique_ptr<const SrcAddress> a5{ SrcAddress::create("a5", "192.168.0.50-192.168.0.110", IPAddressModel::IP4Model, true) };

	EXPECT_TRUE(a2->is_subset(*a1));
	EXPECT_TRUE(a3->is_disjoint(*a1));
	EXPECT_TRUE(a4->overlaps(*a5));
	EXPECT_TRUE(a5->overlaps(*a4));

	EXPECT_EQ(a1->compare(*a2), MnodeRelationship::superset);
	EXPECT_EQ(a2->compare(*a1), MnodeRelationship::subset);
	EXPECT_EQ(a5->compare(*a4), MnodeRelationship::overlap);
	EXPECT_EQ(a4->compare(*a5), MnodeRelationship::overlap);
}


TEST(DstAddress4Test, address) {
	using namespace fwm;
	std::unique_ptr<const DstAddress> a{ DstAddress::create("a1", "192.0.2.1", IPAddressModel::IP4Model, true) };

	EXPECT_TRUE(a->value().range().is_singleton());
	EXPECT_EQ(a->at(), IPAddressType::Address);
	EXPECT_EQ(a->version(), 4);
	EXPECT_EQ(a->value().range().lbound().bitnum(), 32);
	EXPECT_EQ(a->value().range().lbound().bitnum(), a->value().range().nbits());
	EXPECT_EQ(bvec_val(a->value().range().lbound()), 3221225985);
	EXPECT_EQ(bvec_val(a->value().range().ubound()), 3221225985);
}


TEST(DstAddress4Test, subnet) {
	using namespace fwm;
	std::unique_ptr<const DstAddress> a1{ DstAddress::create("a1", "10.0.0.0/8", IPAddressModel::IP4Model, true) };
	std::unique_ptr<const DstAddress> a2{ DstAddress::create("a2", "10.0.0.0/255.0.0.0", IPAddressModel::IP4Model, true) };

	EXPECT_TRUE(a1->equal(*a2));
	EXPECT_TRUE(a2->equal(*a1));

	EXPECT_EQ(a1->at(), IPAddressType::Subnet);
	EXPECT_EQ(a2->at(), IPAddressType::Subnet);

	EXPECT_EQ(a1->to_string(), a2->to_string());
}


TEST(DstAddress4Test, range) {
	using namespace fwm;
	std::unique_ptr<const DstAddress> a1{ DstAddress::create("a1", "0.0.0.0-255.255.255.255", IPAddressModel::IP4Model, true) };
	std::unique_ptr<const DstAddress> a2{ DstAddress::create("a2", "0.0.0.0/0", IPAddressModel::IP4Model, true) };

	EXPECT_TRUE(a1->equal(*a2));
	EXPECT_TRUE(a2->equal(*a1));
	EXPECT_EQ(bvec_val(a1->value().range().lbound()), 0);
	EXPECT_EQ(bvec_val(a1->value().range().ubound()), UINT32_MAX);
	EXPECT_EQ(bvec_val(a2->value().range().lbound()), 0);
	EXPECT_EQ(bvec_val(a2->value().range().ubound()), UINT32_MAX);

	std::unique_ptr<const SrcAddress> a3{ SrcAddress::create("a3", "192.0.2.1-192.0.2.1", IPAddressModel::IP4Model, true) };
	EXPECT_TRUE(a3->value().range().is_singleton());
	EXPECT_EQ(bvec_val(a3->value().range().lbound()), 3221225985);
	EXPECT_EQ(bvec_val(a3->value().range().ubound()), 3221225985);

	EXPECT_EQ(a1->at(), IPAddressType::Subnet);
	EXPECT_EQ(a2->at(), IPAddressType::Subnet);
	EXPECT_EQ(a3->at(), IPAddressType::Address);
}


TEST(DstAddress4Test, syntax) {
	using namespace fwm;

	EXPECT_THROW(DstAddress::create("a1", "10.0.0.0.0", IPAddressModel::IP4Model, true), fwm::IPAddressError);
	EXPECT_THROW(DstAddress::create("a1", "72.12.260.0", IPAddressModel::IP4Model, true), fwm::IPAddressError);
	EXPECT_THROW(DstAddress::create("a1", "10.0.0.0/", IPAddressModel::IP4Model, true), fwm::IPAddressError);
	EXPECT_THROW(DstAddress::create("a1", "10.0..0", IPAddressModel::IP4Model, true), fwm::IPAddressError);
	EXPECT_THROW(DstAddress::create("a1", "", IPAddressModel::IP4Model, true), fwm::IPAddressError);
	EXPECT_THROW(DstAddress::create("a1", "192.168.0.10-192.168.0.0", IPAddressModel::IP4Model, true), fwm::IPAddressError);
	EXPECT_THROW(DstAddress::create("a1", "-192.168.0.0", IPAddressModel::IP4Model, true), fwm::IPAddressError);
	EXPECT_THROW(DstAddress::create("a1", "-", IPAddressModel::IP4Model, true), fwm::IPAddressError);
	EXPECT_THROW(DstAddress::create("a1", "/", IPAddressModel::IP4Model, true), fwm::IPAddressError);
	EXPECT_THROW(DstAddress::create("a1", "192.IPAddressModel::IP4Model.10.0/8", IPAddressModel::IP4Model, true), fwm::IPAddressError);
}


TEST(DstAddress4Test, to_string) {
	using namespace fwm;

	std::unique_ptr<const DstAddress> a1{ DstAddress::create("a1", "10.0.0.0/8", IPAddressModel::IP4Model, true) };
	std::unique_ptr<const DstAddress> a2{ DstAddress::create("a2", "10.0.0.0/255.0.0.0", IPAddressModel::IP4Model, true) };
	std::unique_ptr<const DstAddress> a3{ DstAddress::create("a3", "192.0.2.1", IPAddressModel::IP4Model, true) };
	std::unique_ptr<const DstAddress> a4{ DstAddress::create("a4", "192.0.2.1-192.0.2.2", IPAddressModel::IP4Model, true) };
	std::unique_ptr<const DstAddress> a5{ DstAddress::create("a5", "192.0.2.1-192.0.2.3", IPAddressModel::IP4Model, true) };
	std::unique_ptr<const DstAddress> a6{ DstAddress::create("a6", "0.0.0.0/0", IPAddressModel::IP4Model, true) };
	std::unique_ptr<const DstAddress> a7{ DstAddress::any4(IPAddressModel::IP4Model) };

	EXPECT_EQ(a1->to_string(), "10.0.0.0/8");
	EXPECT_EQ(a2->to_string(), "10.0.0.0/8");
	EXPECT_EQ(a3->to_string(), "192.0.2.1");
	EXPECT_EQ(a4->to_string(), "192.0.2.1/31");
	EXPECT_EQ(a5->to_string(), "192.0.2.1+2");
	EXPECT_EQ(a6->to_string(), "0.0.0.0/0");
}


TEST(DstAddress4Test, bdd) {
	using namespace fwm;

	std::unique_ptr<const DstAddress> a1{ DstAddress::create("a1", "10.0.0.0/8", IPAddressModel::IP4Model, true) };
	std::unique_ptr<const DstAddress> a2{ DstAddress::create("a2", "10.4.1.0/24", IPAddressModel::IP4Model, true) };
	std::unique_ptr<const DstAddress> a3{ DstAddress::create("a3", "192.168.0.0/24", IPAddressModel::IP4Model, true) };
	std::unique_ptr<const DstAddress> a4{ DstAddress::create("a4", "192.168.0.1-192.168.0.100", IPAddressModel::IP4Model, true) };
	std::unique_ptr<const DstAddress> a5{ DstAddress::create("a5", "192.168.0.50-192.168.0.110", IPAddressModel::IP4Model, true) };

	EXPECT_TRUE(a2->is_subset(*a1));
	EXPECT_TRUE(a3->is_disjoint(*a1));
	EXPECT_TRUE(a4->overlaps(*a5));
	EXPECT_TRUE(a5->overlaps(*a4));

	EXPECT_EQ(a1->compare(*a2), MnodeRelationship::superset);
	EXPECT_EQ(a2->compare(*a1), MnodeRelationship::subset);
	EXPECT_EQ(a5->compare(*a4), MnodeRelationship::overlap);
	EXPECT_EQ(a4->compare(*a5), MnodeRelationship::overlap);
}
