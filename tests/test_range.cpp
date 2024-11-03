#include <gtest/gtest.h>
#include <memory>

#include "buddy/bvec.h"
#include "model/range.h"
#include "model/rangeimpl.h"


TEST(Test_Range, range16) {
	using namespace fwm;

	RangePtr range_2 = std::make_unique<Range16>(2, 0, 4);

	EXPECT_EQ(range_2->nbits(), 2);
	EXPECT_EQ(range_2->lbound() == bvec_conpp(2, 0), bddtrue);
	EXPECT_EQ(range_2->ubound() == bvec_conpp(2, 4), bddtrue);
	EXPECT_FALSE(range_2->is_singleton());
	EXPECT_FALSE(range_2->is_power_of_2());


	RangePtr range_8 = std::make_unique<Range16>(8, 1, 4);
	EXPECT_EQ(range_8->nbits(), 8);
	EXPECT_EQ(range_8->lbound() == bvec_conpp(8, 1), bddtrue);
	EXPECT_EQ(range_8->ubound() == bvec_conpp(8, 4), bddtrue);
	EXPECT_FALSE(range_8->is_singleton());
	EXPECT_TRUE(range_8->is_power_of_2());


	RangePtr range_16 = std::make_unique<Range16>(16, 1, 32700);
	EXPECT_EQ(range_16->nbits(), 16);
	EXPECT_EQ(range_16->lbound() == bvec_conpp(16, 1), bddtrue);
	EXPECT_EQ(range_16->ubound() == bvec_conpp(16, 32700), bddtrue);
	EXPECT_FALSE(range_16->is_singleton());
	EXPECT_FALSE(range_16->is_power_of_2());
}


TEST(Test_Range, range32) {
	using namespace fwm;

	RangePtr range_2 = std::make_unique<Range32>(2, 0, 4);

	EXPECT_EQ(range_2->nbits(), 2);
	EXPECT_EQ(range_2->lbound() == bvec_conpp(2, 0), bddtrue);
	EXPECT_EQ(range_2->ubound() == bvec_conpp(2, 4), bddtrue);
	EXPECT_FALSE(range_2->is_singleton());
	EXPECT_FALSE(range_2->is_power_of_2());


	RangePtr range_8 = std::make_unique<Range32>(8, 1, 4);
	EXPECT_EQ(range_8->nbits(), 8);
	EXPECT_EQ(range_8->lbound() == bvec_conpp(8, 1), bddtrue);
	EXPECT_EQ(range_8->ubound() == bvec_conpp(8, 4), bddtrue);
	EXPECT_FALSE(range_8->is_singleton());
	EXPECT_TRUE(range_8->is_power_of_2());


	RangePtr range_16 = std::make_unique<Range32>(16, 1, 32700);
	EXPECT_EQ(range_16->nbits(), 16);
	EXPECT_EQ(range_16->lbound() == bvec_conpp(16, 1), bddtrue);
	EXPECT_EQ(range_16->ubound() == bvec_conpp(16, 32700), bddtrue);
	EXPECT_FALSE(range_16->is_singleton());
	EXPECT_FALSE(range_16->is_power_of_2());


	RangePtr range_32 = std::make_unique<Range32>(32, 1, 3270000);
	EXPECT_EQ(range_32->nbits(), 32);
	EXPECT_EQ(range_32->lbound() == bvec_conpp(32, 1), bddtrue);
	EXPECT_EQ(range_32->ubound() == bvec_conpp(32, 3270000), bddtrue);
	EXPECT_FALSE(range_32->is_singleton());
	EXPECT_FALSE(range_32->is_power_of_2());
}


TEST(Test_Range, range128) {
	using namespace fwm;

	RangePtr range_2 = std::make_unique<Range128>(2, 0, 4);

	EXPECT_EQ(range_2->nbits(), 2);
	EXPECT_EQ(range_2->lbound() == bvec_conpp(2, 0), bddtrue);
	EXPECT_EQ(range_2->ubound() == bvec_conpp(2, 4), bddtrue);
	EXPECT_FALSE(range_2->is_singleton());
	EXPECT_FALSE(range_2->is_power_of_2());


	RangePtr range_8 = std::make_unique<Range128>(8, 1, 4);
	EXPECT_EQ(range_8->nbits(), 8);
	EXPECT_EQ(range_8->lbound() == bvec_conpp(8, 1), bddtrue);
	EXPECT_EQ(range_8->ubound() == bvec_conpp(8, 4), bddtrue);
	EXPECT_FALSE(range_8->is_singleton());
	EXPECT_TRUE(range_8->is_power_of_2());


	RangePtr range_16 = std::make_unique<Range128>(16, 1, 32700);
	EXPECT_EQ(range_16->nbits(), 16);
	EXPECT_EQ(range_16->lbound() == bvec_conpp(16, 1), bddtrue);
	EXPECT_EQ(range_16->ubound() == bvec_conpp(16, 32700), bddtrue);
	EXPECT_FALSE(range_16->is_singleton());
	EXPECT_FALSE(range_16->is_power_of_2());


	RangePtr range_32 = std::make_unique<Range128>(32, 1, 3270000);
	EXPECT_EQ(range_32->nbits(), 32);
	EXPECT_EQ(range_32->lbound() == bvec_conpp(32, 1), bddtrue);
	EXPECT_EQ(range_32->ubound() == bvec_conpp(32, 3270000), bddtrue);
	EXPECT_FALSE(range_32->is_singleton());
	EXPECT_FALSE(range_32->is_power_of_2());

	uint128_t ubound = uint128_t(1) << 65;
	RangePtr range_128 = std::make_unique<Range128>(128, 1, ubound);
	EXPECT_EQ(range_128->nbits(), 128);
	EXPECT_EQ(range_128->lbound() == bvec_conpp(128, 1), bddtrue);
	EXPECT_EQ(range_128->ubound() == bvec_conpp128(128, { ubound.lower(), ubound.upper() }), bddtrue);
	EXPECT_FALSE(range_128->is_singleton());
	EXPECT_TRUE(range_128->is_power_of_2());
}


TEST(Test_Range, singleton) {
	using namespace fwm;

	for (int i = 0; i < 16; i++) {
		RangePtr range = std::make_unique<Range16>(16, 1 << i, 1 << i);
		EXPECT_TRUE(range->is_singleton());
		EXPECT_TRUE(range->is_power_of_2());
	}


	for (int i = 0; i < 32; i++) {
		RangePtr range = std::make_unique<Range32>(32, 1 << i, 1 << i);
		EXPECT_TRUE(range->is_singleton());
		EXPECT_TRUE(range->is_power_of_2());
	}


	for (int i = 0; i < 128; i++) {
		RangePtr range = std::make_unique<Range128>(128, 1 << i, 1 << i);
		EXPECT_TRUE(range->is_singleton());
		EXPECT_TRUE(range->is_power_of_2());
	}
}
