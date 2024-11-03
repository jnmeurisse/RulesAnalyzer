/*!
* This file is part of RulesAnalyzer
*
* Copyright (C) 2024 Jean-Noel Meurisse
* SPDX-License-Identifier: GPL-3.0-only
*
*/
#include "model/domain.h"

#include <climits>
#include <cassert>

#include "model/domains.h"
#include "model/rangeimpl.h"
#include "model/ipv4range.h"
#include "model/ipv6range.h"


namespace fwm {
	Domain::Domain(DomainType dt, Range* range) :
		_dt{ dt },
		_range{ std::unique_ptr<Range>(range) }
	{
		assert(range->nbits() > 0 && range->nbits() <= 128);
	}


	const bvec& Domain::var() const
	{
		return Domains::get().get_var(_dt);
	}


	Range* ZoneDomain::create_range(uint16_t lbound, uint16_t ubound)
	{
		return new Range16(ZoneDomain::nbits(), lbound, ubound);
	}


	Range* ZoneDomain::create_full_range()
	{
		return ZoneDomain::create_range(ZoneDomain::min(), ZoneDomain::max());
	}


	Range* ZoneDomain::create_singleton(uint16_t value)
	{
		return ZoneDomain::create_range(value, value);
	}


	SrcZoneDomain::SrcZoneDomain() :
		ZoneDomain(DomainType::SrcZone, ZoneDomain::create_full_range())
	{
	}


	DstZoneDomain::DstZoneDomain() :
		ZoneDomain(DomainType::DstZone, ZoneDomain::create_full_range())
	{
	}


	Range* Address4Domain::create_range(uint32_t lbound, uint32_t ubound)
	{
		return new Ipv4Range(lbound, ubound);
	}


	Range* Address4Domain::create_full_range()
	{
		return Address4Domain::create_range(Address4Domain::min(), Address4Domain::max());
	}


	Range* Address4Domain::create_singleton(uint32_t value)
	{
		return Address4Domain::create_range(value, value);
	}


	SrcAddress4Domain::SrcAddress4Domain() :
		Address4Domain(DomainType::SrcAddress4, Address4Domain::create_full_range())
	{
	}


	DstAddress4Domain::DstAddress4Domain() :
		Address4Domain(DomainType::DstAddress4, Address4Domain::create_full_range())
	{
	}


	Range* Address6Domain::create_range(uint128_t lbound, uint128_t ubound, bool ipv4_format)
	{
		return new Ipv6Range(lbound, ubound, ipv4_format);
	}


	Range* Address6Domain::create_full_range()
	{
		return Address6Domain::create_range(Address6Domain::min(), Address6Domain::max(), false);
	}


	Range* Address6Domain::create_singleton(uint128_t value, bool ipv4_format)
	{
		return Address6Domain::create_range(value, value, ipv4_format);
	}


	SrcAddress6Domain::SrcAddress6Domain() :
		Address6Domain(DomainType::SrcAddress6, Address6Domain::create_full_range())
	{
	}


	DstAddress6Domain::DstAddress6Domain() :
		Address6Domain(DomainType::DstAddress6, Address6Domain::create_full_range())
	{
	}


	ProtocolDomain::ProtocolDomain() :
		Domain(DomainType::Protocol, ProtocolDomain::create_full_range())
	{
	}


	Range* ProtocolDomain::create_range(uint16_t lbound, uint16_t ubound)
	{
		return new Range16(ProtocolDomain::nbits(), lbound, ubound);
	}


	Range* ProtocolDomain::create_full_range()
	{
		return ProtocolDomain::create_range(ProtocolDomain::min(), ProtocolDomain::max());
	}


	Range* ProtocolDomain::create_singleton(uint16_t value)
	{
		return ProtocolDomain::create_range(value, value);
	}


	Range* PortDomain::create_range(uint16_t lbound, uint16_t ubound)
	{
		return new Range16(PortDomain::nbits(), lbound, ubound);
	}


	Range* PortDomain::create_full_range()
	{
		return PortDomain::create_range(PortDomain::min(), PortDomain::max());
	}


	Range* PortDomain::create_singleton(uint16_t value)
	{
		return PortDomain::create_range(value, value);
	}


	DstTcpPortDomain::DstTcpPortDomain() :
		PortDomain(DomainType::DstTcpPort, PortDomain::create_full_range())
	{
	}


	DstUdpPortDomain::DstUdpPortDomain() :
		PortDomain(DomainType::DstUdpPort, PortDomain::create_full_range())
	{
	}


	IcmpTypeDomain::IcmpTypeDomain() :
		Domain(DomainType::IcmpType, IcmpTypeDomain::create_full_range())
	{
	}


	Range* IcmpTypeDomain::create_range(uint16_t lbound, uint16_t ubound)
	{
		return new Range16(IcmpTypeDomain::nbits(), lbound, ubound);
	}


	Range* IcmpTypeDomain::create_full_range()
	{
		return IcmpTypeDomain::create_range(IcmpTypeDomain::min(), IcmpTypeDomain::max());
	}


	Range* IcmpTypeDomain::create_singleton(uint16_t value)
	{
		return IcmpTypeDomain::create_range(value, value);
	}


	ApplicationDomain::ApplicationDomain() :
		Domain(DomainType::Application, ApplicationDomain::create_full_range())
	{
	}


	Range* ApplicationDomain::create_range(uint16_t lbound, uint16_t ubound)
	{
		return new Range16(ApplicationDomain::nbits(), lbound, ubound);
	}


	Range* ApplicationDomain::create_full_range()
	{
		return ApplicationDomain::create_range(ApplicationDomain::min(), ApplicationDomain::max());
	}


	Range* ApplicationDomain::create_singleton(uint16_t value)
	{
		return ApplicationDomain::create_range(value, value);

	}


	UserDomain::UserDomain() :
		Domain(DomainType::User, UserDomain::create_full_range())
	{
	}


	Range* UserDomain::create_range(uint16_t lbound, uint16_t ubound)
	{
		return new Range16(UserDomain::nbits(), lbound, ubound);
	}


	Range* UserDomain::create_full_range()
	{
		return UserDomain::create_range(UserDomain::min(), UserDomain::max());
	}


	Range* UserDomain::create_singleton(uint16_t value)
	{
		return UserDomain::create_range(value, value);
	}


	UrlDomain::UrlDomain() :
		Domain(DomainType::Url, UrlDomain::create_full_range())
	{
	}


	Range* UrlDomain::create_range(uint16_t lbound, uint16_t ubound)
	{
		return new Range16(UrlDomain::nbits(), lbound, ubound);
	}


	Range* UrlDomain::create_full_range()
	{
		return UrlDomain::create_range(UrlDomain::min(), UrlDomain::max());
	}


	Range* UrlDomain::create_singleton(uint16_t value)
	{
		return UrlDomain::create_range(value, value);
	}


}
