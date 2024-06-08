#include <gtest\gtest.h>
#include "model/domains.h"

TEST(Domains, size) {
	using namespace fwm;

	const Domain& d_src = Domains::get(DomainType::SrcAddress);
	ASSERT_EQ(d_src.nbits(), 32);
	ASSERT_EQ(d_src.size(), 4294967296);

	const Domain& d_dst = Domains::get(DomainType::DstAddress);
	ASSERT_EQ(d_dst.nbits(), 32);
	ASSERT_EQ(d_dst.size(), 4294967296);

	const Domain& tcp_ports = Domains::get(DomainType::TcpDstPort);
	ASSERT_EQ(tcp_ports.nbits(), 16);
	ASSERT_EQ(tcp_ports.size(), 1 << 16);

	const Domain& udp_ports = Domains::get(DomainType::UdpDstPort);
	ASSERT_EQ(udp_ports.nbits(), 16);
	ASSERT_EQ(udp_ports.size(), 1 << 16);
}
