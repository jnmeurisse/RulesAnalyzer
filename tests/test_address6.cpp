#include <gtest/gtest.h>
#include <memory>

#include "buddy/bdd.h"
#include "buddy/bvec.h"
#include "model/address.h"


TEST(SrcAddress6Test, address) {
	using namespace fwm;
	std::unique_ptr<const SrcAddress> a1{ SrcAddress::create("a1", "::192.0.2.1", IPAddressModel::IP6Model, true) };
	std::unique_ptr<const SrcAddress> a2{ SrcAddress::create("a2", "::c000:201", IPAddressModel::IP6Model, true) };
	std::unique_ptr<const SrcAddress> a3{ SrcAddress::create("a3", "::c0:0:2:1", IPAddressModel::IP6Model, true) };

	EXPECT_TRUE(a1->value().range().is_singleton());
	EXPECT_EQ(a1->at(), IPAddressType::Address);
	EXPECT_EQ(a1->version(), 6);
	EXPECT_EQ(a1->value().range().lbound().bitnum(), 128);
	EXPECT_EQ(a1->value().range().lbound().bitnum(), a1->value().range().nbits());
	EXPECT_EQ(bvec_val(a1->value().range().lbound()), 3221225985);
	EXPECT_EQ(bvec_val(a1->value().range().ubound()), 3221225985);

	EXPECT_TRUE(a2->value().range().is_singleton());
	EXPECT_EQ(a2->at(), IPAddressType::Address);
	EXPECT_EQ(a2->version(), 6);
	EXPECT_EQ(a2->value().range().lbound().bitnum(), 128);
	EXPECT_EQ(a2->value().range().lbound().bitnum(), a2->value().range().nbits());
	EXPECT_EQ(bvec_val(a2->value().range().lbound()), 3221225985);
	EXPECT_EQ(bvec_val(a2->value().range().ubound()), 3221225985);

	EXPECT_TRUE(a1->equal(*a2));
	EXPECT_TRUE(a2->equal(*a1));

	EXPECT_TRUE(a3->value().range().is_singleton());
	EXPECT_EQ(a3->at(), IPAddressType::Address);
	EXPECT_EQ(a3->version(), 6);
	EXPECT_EQ(a3->value().range().lbound().bitnum(), 128);
	EXPECT_EQ(a3->value().range().lbound().bitnum(), a3->value().range().nbits());
	EXPECT_EQ(bvec_val(a3->value().range().lbound()), 54043195528577025);
	EXPECT_EQ(bvec_val(a3->value().range().ubound()), 54043195528577025);
}


TEST(SrcAddress6Test, subnet) {
	using namespace fwm;
	std::unique_ptr<const SrcAddress> a1{ SrcAddress::create("a1", "2001:db8:0002:0001::/64", IPAddressModel::IP6Model, true) };
	std::unique_ptr<const SrcAddress> a2{ SrcAddress::create("a2", "2001:db8:2:1::/64", IPAddressModel::IP6Model, true) };

	EXPECT_TRUE(a1->equal(*a2));
	EXPECT_TRUE(a2->equal(*a1));

	EXPECT_EQ(a1->at(), IPAddressType::Subnet);
	EXPECT_EQ(a2->at(), IPAddressType::Subnet);

	EXPECT_EQ(a1->to_string(), a2->to_string());

	s_uint128 a1_bvec_val_l = bvec_val128(a1->value().range().lbound());
	uint128_t a1_val_l("42540766411285010726989958283712790528", 10);
	EXPECT_EQ(uint128_t(a1_bvec_val_l.high64, a1_bvec_val_l.low64), a1_val_l);

	s_uint128 a1_bvec_val_h = bvec_val128(a1->value().range().ubound());
	uint128_t a1_val_h("42540766411285010745436702357422342143", 10);
	EXPECT_EQ(uint128_t(a1_bvec_val_h.high64, a1_bvec_val_h.low64), a1_val_h);
}


TEST(SrcAddress6Test, range) {
	using namespace fwm;
	std::unique_ptr<const SrcAddress> a1{ SrcAddress::create("a1", "::-ffff:ffff:ffff:ffff:ffff:ffff:ffff:ffff", IPAddressModel::IP6Model, true) };
	std::unique_ptr<const SrcAddress> a2{ SrcAddress::create("a2", "::/0", IPAddressModel::IP6Model, true) };

	EXPECT_TRUE(a1->equal(*a2));
	EXPECT_TRUE(a2->equal(*a1));

	s_uint128 a1_bvec_val_l = bvec_val128(a1->value().range().lbound());
	EXPECT_EQ(uint128_t(a1_bvec_val_l.high64, a1_bvec_val_l.low64), SrcAddress6Domain::min());
	s_uint128 a1_bvec_val_h = bvec_val128(a1->value().range().ubound());
	EXPECT_EQ(uint128_t(a1_bvec_val_h.high64, a1_bvec_val_h.low64), SrcAddress6Domain::max());

	s_uint128 a2_bvec_val_l = bvec_val128(a2->value().range().lbound());
	EXPECT_EQ(uint128_t(a2_bvec_val_l.high64, a2_bvec_val_l.low64), SrcAddress6Domain::min());
	s_uint128 a2_bvec_val_h = bvec_val128(a2->value().range().ubound());
	EXPECT_EQ(uint128_t(a2_bvec_val_h.high64, a2_bvec_val_h.low64), SrcAddress6Domain::max());

	std::unique_ptr<const SrcAddress> a3{ SrcAddress::create("a3", "2001:db8:2:1::1-2001:db8:2:1::1", IPAddressModel::IP6Model, true) };
	EXPECT_TRUE(a3->value().range().is_singleton());

	uint128_t a3_val("42540766411285010726989958283712790529", 10);

	bvec a4_bvec(128, { a3_val.lower(), a3_val.upper() } );
	EXPECT_TRUE(bvec_equ(a3->value().range().ubound(), a4_bvec) == bddtrue);
	EXPECT_TRUE(bvec_equ(a3->value().range().lbound(), a4_bvec) == bddtrue);

	const s_uint128 a3_bvec_val_l = bvec_val128(a3->value().range().lbound());
	EXPECT_EQ(uint128_t(a3_bvec_val_l.high64, a3_bvec_val_l.low64), a3_val);
	const s_uint128 a3_bvec_val_u = bvec_val128(a3->value().range().ubound());
	EXPECT_EQ(uint128_t(a3_bvec_val_u.high64, a3_bvec_val_u.low64), a3_val);

	EXPECT_EQ(a1->at(), IPAddressType::Subnet);
	EXPECT_EQ(a2->at(), IPAddressType::Subnet);
	EXPECT_EQ(a3->at(), IPAddressType::Address);
}


TEST(SrcAddress6Test, syntax) {
	using namespace fwm;

	EXPECT_THROW(SrcAddress::create("a1", ":::::", IPAddressModel::IP6Model, true), fwm::IPAddressError);
	EXPECT_THROW(SrcAddress::create("a1", "2001:gb8:2:1::", IPAddressModel::IP6Model, true), fwm::IPAddressError);
	EXPECT_THROW(SrcAddress::create("a1", "2001:db8:2:1::/", IPAddressModel::IP6Model, true), fwm::IPAddressError);
	EXPECT_THROW(SrcAddress::create("a1", "2001:db8:2:1:::", IPAddressModel::IP6Model, true), fwm::IPAddressError);
	EXPECT_THROW(SrcAddress::create("a1", "", IPAddressModel::IP6Model, true), fwm::IPAddressError);
	EXPECT_THROW(SrcAddress::create("a1", "2001:db8:2:1::1-2001:db8:2:1::0", IPAddressModel::IP6Model, true), fwm::IPAddressError);
	EXPECT_THROW(SrcAddress::create("a1", "-2001:db8:2:1::1", IPAddressModel::IP6Model, true), fwm::IPAddressError);
	EXPECT_THROW(SrcAddress::create("a1", "-", IPAddressModel::IP6Model, true), fwm::IPAddressError);
	EXPECT_THROW(SrcAddress::create("a1", "/", IPAddressModel::IP6Model, true), fwm::IPAddressError);
	EXPECT_THROW(SrcAddress::create("a1", "2001:db8:2:1::1/8", IPAddressModel::IP6Model, true), fwm::IPAddressError);
}


TEST(SrcAddress6Test, to_string) {
	using namespace fwm;

	std::unique_ptr<const SrcAddress> a0{ SrcAddress::create("a0", "::/0", IPAddressModel::IP6Model, true) };
	std::unique_ptr<const SrcAddress> a1{ SrcAddress::create("a1", "2001:db8:2:1::1", IPAddressModel::IP6Model, true) };
	std::unique_ptr<const SrcAddress> a2{ SrcAddress::create("a2", "10.0.0.0/255.0.0.0", IPAddressModel::IP64Model, true) };
	std::unique_ptr<const SrcAddress> a3{ SrcAddress::create("a3", "2001:db8:2:1::1-2001:db8:2:1::3", IPAddressModel::IP6Model, true) };
	std::unique_ptr<const SrcAddress> a4{ SrcAddress::create("a4", "2001:db8:2:1::0-2001:db8:2:1::3", IPAddressModel::IP6Model, true) };

	EXPECT_EQ(a0->to_string(), "::/0");
	EXPECT_EQ(a1->to_string(), "2001:db8:2:1::1");
	EXPECT_EQ(a2->to_string(), "10.0.0.0/8");
	EXPECT_EQ(a3->to_string(), "2001:db8:2:1::1+2");
	EXPECT_EQ(a4->to_string(), "2001:db8:2:1::/126");
}


TEST(SrcAddress6Test, bdd) {
	using namespace fwm;

	std::unique_ptr<const SrcAddress> a1{ SrcAddress::create("a1", "10.0.0.0/8", IPAddressModel::IP6Model, false) };
	std::unique_ptr<const SrcAddress> a2{ SrcAddress::create("a2", "10.4.1.0/24", IPAddressModel::IP6Model, false) };
	std::unique_ptr<const SrcAddress> a3{ SrcAddress::create("a3", "192.168.0.0/24", IPAddressModel::IP6Model, false) };
	std::unique_ptr<const SrcAddress> a4{ SrcAddress::create("a4", "192.168.0.1-192.168.0.100", IPAddressModel::IP6Model, false) };
	std::unique_ptr<const SrcAddress> a5{ SrcAddress::create("a5", "192.168.0.50-192.168.0.110", IPAddressModel::IP6Model, false) };

	EXPECT_TRUE(a2->is_subset(*a1));
	EXPECT_TRUE(a3->is_disjoint(*a1));
	EXPECT_TRUE(a4->overlaps(*a5));
	EXPECT_TRUE(a5->overlaps(*a4));

	EXPECT_EQ(a1->compare(*a2), MnodeRelationship::superset);
	EXPECT_EQ(a2->compare(*a1), MnodeRelationship::subset);
	EXPECT_EQ(a5->compare(*a4), MnodeRelationship::overlap);
	EXPECT_EQ(a4->compare(*a5), MnodeRelationship::overlap);
}


TEST(SrcAddress6Test, any) {
	using namespace fwm;

	std::unique_ptr<const SrcAddress> a1{ SrcAddress::any6(IPAddressModel::IP6Model) };
	std::unique_ptr<const SrcAddress> a2{ SrcAddress::any6(IPAddressModel::IP64Model) };

	EXPECT_EQ(a1->name(), "any");
	EXPECT_EQ(a2->name(), "any6");
	EXPECT_EQ(a1->to_string(), "::/0");
	EXPECT_EQ(a2->to_string(), "::/0");
}


TEST(DstAddress6Test, address) {
	using namespace fwm;
	std::unique_ptr<const DstAddress> a1{ DstAddress::create("a1", "::192.0.2.1", IPAddressModel::IP6Model, true) };
	std::unique_ptr<const DstAddress> a2{ DstAddress::create("a2", "::c000:201", IPAddressModel::IP6Model, true) };
	std::unique_ptr<const DstAddress> a3{ DstAddress::create("a3", "::c0:0:2:1", IPAddressModel::IP6Model, true) };

	EXPECT_TRUE(a1->value().range().is_singleton());
	EXPECT_EQ(a1->at(), IPAddressType::Address);
	EXPECT_EQ(a1->version(), 6);
	EXPECT_EQ(a1->value().range().lbound().bitnum(), 128);
	EXPECT_EQ(a1->value().range().lbound().bitnum(), a1->value().range().nbits());
	EXPECT_EQ(bvec_val(a1->value().range().lbound()), 3221225985);
	EXPECT_EQ(bvec_val(a1->value().range().ubound()), 3221225985);

	EXPECT_TRUE(a2->value().range().is_singleton());
	EXPECT_EQ(a2->at(), IPAddressType::Address);
	EXPECT_EQ(a2->version(), 6);
	EXPECT_EQ(a2->value().range().lbound().bitnum(), 128);
	EXPECT_EQ(a2->value().range().lbound().bitnum(), a2->value().range().nbits());
	EXPECT_EQ(bvec_val(a2->value().range().lbound()), 3221225985);
	EXPECT_EQ(bvec_val(a2->value().range().ubound()), 3221225985);

	EXPECT_TRUE(a1->equal(*a2));
	EXPECT_TRUE(a2->equal(*a1));

	EXPECT_TRUE(a3->value().range().is_singleton());
	EXPECT_EQ(a3->at(), IPAddressType::Address);
	EXPECT_EQ(a3->version(), 6);
	EXPECT_EQ(a3->value().range().lbound().bitnum(), 128);
	EXPECT_EQ(a3->value().range().lbound().bitnum(), a3->value().range().nbits());
	EXPECT_EQ(bvec_val(a3->value().range().lbound()), 54043195528577025);
	EXPECT_EQ(bvec_val(a3->value().range().ubound()), 54043195528577025);

}


TEST(DstAddress6Test, subnet) {
	using namespace fwm;
	std::unique_ptr<const DstAddress> a1{ DstAddress::create("a1", "2001:db8:0002:0001::/64", IPAddressModel::IP6Model, true) };
	std::unique_ptr<const DstAddress> a2{ DstAddress::create("a2", "2001:db8:2:1::/64", IPAddressModel::IP6Model, true) };

	EXPECT_TRUE(a1->equal(*a2));
	EXPECT_TRUE(a2->equal(*a1));

	EXPECT_EQ(a1->at(), IPAddressType::Subnet);
	EXPECT_EQ(a2->at(), IPAddressType::Subnet);

	EXPECT_EQ(a1->to_string(), a2->to_string());

	s_uint128 a1_bvec_val_l = bvec_val128(a1->value().range().lbound());
	uint128_t a1_val_l("42540766411285010726989958283712790528", 10);
	EXPECT_EQ(uint128_t(a1_bvec_val_l.high64, a1_bvec_val_l.low64), a1_val_l);

	s_uint128 a1_bvec_val_h = bvec_val128(a1->value().range().ubound());
	uint128_t a1_val_h("42540766411285010745436702357422342143", 10);
	EXPECT_EQ(uint128_t(a1_bvec_val_h.high64, a1_bvec_val_h.low64), a1_val_h);
}


TEST(DstAddress6Test, range) {
	using namespace fwm;
	std::unique_ptr<const DstAddress> a1{ DstAddress::create("a1", "::-ffff:ffff:ffff:ffff:ffff:ffff:ffff:ffff", IPAddressModel::IP6Model, true) };
	std::unique_ptr<const DstAddress> a2{ DstAddress::create("a2", "::/0", IPAddressModel::IP6Model, true) };

	EXPECT_TRUE(a1->equal(*a2));
	EXPECT_TRUE(a2->equal(*a1));

	s_uint128 a1_bvec_val_l = bvec_val128(a1->value().range().lbound());
	EXPECT_EQ(uint128_t(a1_bvec_val_l.high64, a1_bvec_val_l.low64), SrcAddress6Domain::min());
	s_uint128 a1_bvec_val_h = bvec_val128(a1->value().range().ubound());
	EXPECT_EQ(uint128_t(a1_bvec_val_h.high64, a1_bvec_val_h.low64), SrcAddress6Domain::max());

	s_uint128 a2_bvec_val_l = bvec_val128(a2->value().range().lbound());
	EXPECT_EQ(uint128_t(a2_bvec_val_l.high64, a2_bvec_val_l.low64), SrcAddress6Domain::min());
	s_uint128 a2_bvec_val_h = bvec_val128(a2->value().range().ubound());
	EXPECT_EQ(uint128_t(a2_bvec_val_h.high64, a2_bvec_val_h.low64), SrcAddress6Domain::max());

	std::unique_ptr<const DstAddress> a3{ DstAddress::create("a3", "2001:db8:2:1::1-2001:db8:2:1::1", IPAddressModel::IP6Model, true) };
	EXPECT_TRUE(a3->value().range().is_singleton());

	uint128_t a3_val("42540766411285010726989958283712790529", 10);

	bvec a4_bvec(128, { a3_val.lower(), a3_val.upper() } );
	EXPECT_TRUE(bvec_equ(a3->value().range().ubound(), a4_bvec) == bddtrue);
	EXPECT_TRUE(bvec_equ(a3->value().range().lbound(), a4_bvec) == bddtrue);

	const s_uint128 a3_bvec_val_l = bvec_val128(a3->value().range().lbound());
	EXPECT_EQ(uint128_t(a3_bvec_val_l.high64, a3_bvec_val_l.low64), a3_val);
	const s_uint128 a3_bvec_val_u = bvec_val128(a3->value().range().ubound());
	EXPECT_EQ(uint128_t(a3_bvec_val_u.high64, a3_bvec_val_u.low64), a3_val);

	EXPECT_EQ(a1->at(), IPAddressType::Subnet);
	EXPECT_EQ(a2->at(), IPAddressType::Subnet);
	EXPECT_EQ(a3->at(), IPAddressType::Address);
}


TEST(DstAddress6Test, syntax) {
	using namespace fwm;

	std::unique_ptr<const DstAddress> a0{ DstAddress::create("a0", "::/0", IPAddressModel::IP6Model, true) };
	std::unique_ptr<const DstAddress> a1{ DstAddress::create("a1", "2001:db8:2:1::1", IPAddressModel::IP6Model, true) };
	std::unique_ptr<const DstAddress> a2{ DstAddress::create("a2", "10.0.0.0/255.0.0.0", IPAddressModel::IP64Model, true) };
	std::unique_ptr<const DstAddress> a3{ DstAddress::create("a3", "2001:db8:2:1::1-2001:db8:2:1::3", IPAddressModel::IP6Model, true) };
	std::unique_ptr<const DstAddress> a4{ DstAddress::create("a4", "2001:db8:2:1::0-2001:db8:2:1::3", IPAddressModel::IP6Model, true) };

	EXPECT_EQ(a0->to_string(), "::/0");
	EXPECT_EQ(a1->to_string(), "2001:db8:2:1::1");
	EXPECT_EQ(a2->to_string(), "10.0.0.0/8");
	EXPECT_EQ(a3->to_string(), "2001:db8:2:1::1+2");
	EXPECT_EQ(a4->to_string(), "2001:db8:2:1::/126");

}


TEST(DstAddress6Test, to_string) {
	using namespace fwm;

	std::unique_ptr<const DstAddress> a0{ DstAddress::create("a0", "::/0", IPAddressModel::IP6Model, true) };
	std::unique_ptr<const DstAddress> a1{ DstAddress::create("a1", "2001:db8:2:1::1", IPAddressModel::IP6Model, true) };
	std::unique_ptr<const DstAddress> a2{ DstAddress::create("a2", "10.0.0.0/255.0.0.0", IPAddressModel::IP64Model, true) };
	std::unique_ptr<const DstAddress> a3{ DstAddress::create("a3", "2001:db8:2:1::1-2001:db8:2:1::3", IPAddressModel::IP6Model, true) };
	std::unique_ptr<const DstAddress> a4{ DstAddress::create("a4", "2001:db8:2:1::0-2001:db8:2:1::3", IPAddressModel::IP6Model, true) };

	EXPECT_EQ(a0->to_string(), "::/0");
	EXPECT_EQ(a1->to_string(), "2001:db8:2:1::1");
	EXPECT_EQ(a2->to_string(), "10.0.0.0/8");
	EXPECT_EQ(a3->to_string(), "2001:db8:2:1::1+2");
	EXPECT_EQ(a4->to_string(), "2001:db8:2:1::/126");
}


TEST(DstAddress6Test, bdd) {
	using namespace fwm;

	std::unique_ptr<const DstAddress> a1{ DstAddress::create("a1", "10.0.0.0/8", IPAddressModel::IP6Model, false) };
	std::unique_ptr<const DstAddress> a2{ DstAddress::create("a2", "10.4.1.0/24", IPAddressModel::IP6Model, false) };
	std::unique_ptr<const DstAddress> a3{ DstAddress::create("a3", "192.168.0.0/24", IPAddressModel::IP6Model, false) };
	std::unique_ptr<const DstAddress> a4{ DstAddress::create("a4", "192.168.0.1-192.168.0.100", IPAddressModel::IP6Model, false) };
	std::unique_ptr<const DstAddress> a5{ DstAddress::create("a5", "192.168.0.50-192.168.0.110", IPAddressModel::IP6Model, false) };

	EXPECT_TRUE(a2->is_subset(*a1));
	EXPECT_TRUE(a3->is_disjoint(*a1));
	EXPECT_TRUE(a4->overlaps(*a5));
	EXPECT_TRUE(a5->overlaps(*a4));

	EXPECT_EQ(a1->compare(*a2), MnodeRelationship::superset);
	EXPECT_EQ(a2->compare(*a1), MnodeRelationship::subset);
	EXPECT_EQ(a5->compare(*a4), MnodeRelationship::overlap);
	EXPECT_EQ(a4->compare(*a5), MnodeRelationship::overlap);
}


TEST(DstAddress6Test, any) {
	using namespace fwm;

	std::unique_ptr<const DstAddress> a1{ DstAddress::any6(IPAddressModel::IP6Model) };
	std::unique_ptr<const DstAddress> a2{ DstAddress::any6(IPAddressModel::IP64Model) };

	EXPECT_EQ(a1->name(), "any");
	EXPECT_EQ(a2->name(), "any6");
	EXPECT_EQ(a1->to_string(), "::/0");
	EXPECT_EQ(a2->to_string(), "::/0");
}
