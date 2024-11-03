#include <gtest/gtest.h>
#include "model/domains.h"

TEST(Domains, size) {
	using namespace fwm;

	ASSERT_EQ(ZoneDomain::nbits(), 8);
	ASSERT_EQ(ZoneDomain::min(), 0);
	ASSERT_EQ(ZoneDomain::max(), 255);

	ASSERT_EQ(SrcAddress4Domain::nbits(), 32);
	ASSERT_EQ(SrcAddress4Domain::min(), 0);
	ASSERT_EQ(SrcAddress4Domain::max(), UINT32_MAX);

	ASSERT_EQ(SrcAddress6Domain::nbits(), 128);
	ASSERT_EQ(SrcAddress6Domain::min(), 0);
	ASSERT_EQ(SrcAddress6Domain::max().lower(), UINT64_MAX);
	ASSERT_EQ(SrcAddress6Domain::max().upper(), UINT64_MAX);

	ASSERT_EQ(DstAddress4Domain::nbits(), 32);
	ASSERT_EQ(DstAddress4Domain::min(), 0);
	ASSERT_EQ(DstAddress4Domain::max(), UINT32_MAX);

	ASSERT_EQ(DstAddress6Domain::nbits(), 128);
	ASSERT_EQ(DstAddress6Domain::min(), 0);
	ASSERT_EQ(DstAddress6Domain::max().lower(), UINT64_MAX);
	ASSERT_EQ(DstAddress6Domain::max().upper(), UINT64_MAX);

	ASSERT_EQ(DstTcpPortDomain::nbits(), 16);
	ASSERT_EQ(DstTcpPortDomain::min(), 0);
	ASSERT_EQ(DstTcpPortDomain::max(), UINT16_MAX);

	ASSERT_EQ(DstUdpPortDomain::nbits(), 16);
	ASSERT_EQ(DstUdpPortDomain::min(), 0);
	ASSERT_EQ(DstUdpPortDomain::max(), UINT16_MAX);

}
