/*!
* This file is part of RulesAnalyzer
*
* Copyright (C) 2024 Jean-Noel Meurisse
* SPDX-License-Identifier: GPL-3.0-only
*
*/
#pragma once
#include "global.h"

#include <cstdint>

#include "model/range.h"
#include "tools/uint128.h"


namespace fwm {

	enum class DomainType : int
	{
		SrcZone = 0,
		SrcAddress4 = 1,
		SrcAddress6 = 2,
		DstZone = 3,
		DstAddress4 = 4,
		DstAddress6 = 5,
		Protocol = 6,
		DstTcpPort = 7,
		DstUdpPort = 8,
		IcmpType = 9,
		Application = 10,
		User = 11,
		Url = 12
	};

	/**
	 * A domain represents a range of values for a model variable.
	 *
	 * A domain can be a
	 *      SrcZone         : the source zone of an IP packet
	 *      SrcAddress4     : the source address of an IPv4 packet
	 *      SrcAddress6     : the source address of an IPv6 packet
	 *      DstZone         : the destination address of an IP packet
	 *      DstAddress4     : the destination address of an IPv4 packet
	 *      DstAddress6     : the destination address of an IPv6 packet
	 *      Protocol        : the type of protocol (UDP, TCP or ICMP)
	 *      DstTcpPort      : the destination port of a TCP/IP packet
	 *      DstUdpPort      : the destination port of an UDP/IP packet
	 *      IcmpType        : the type of an ICMP/IP packet (code is not implemented)
	 *      Application     : the application id transported in the IP packet
	 *      User            : the user id sending the IP packet.
	 *
	 *  Source TCP and UDP ports are not modeled.  In most firewall configuration,
	 *  any source port is allowed.
	*/
	class Domain abstract
	{
	public:
		/**
		 * Returns this domain type.
		*/
		inline DomainType dt() const noexcept  { return _dt; }

		/**
		 * Returns the range covered by this domain.
		*/
		inline const Range& range() const noexcept { return *_range; }

		/**
		 * Returns the model variable associated to this domain.
		*/
		const bvec& var() const;

	protected:
		Domain(DomainType dt, Range* range);

	private:
		// The domain type
		const DomainType _dt;

		// The range
		const RangePtr _range;
	};


	class ZoneDomain abstract : public Domain
	{
	public:
		static Range* create_range(uint16_t lbound, uint16_t ubound);
		static Range* create_full_range();
		static Range* create_singleton(uint16_t value);

		static constexpr int nbits() { return 8; }
		static constexpr uint16_t min() { return 0; }
		static constexpr uint16_t max() { return (1 << nbits()) - 1; }

	protected:
		using Domain::Domain;
	};


	class SrcZoneDomain final : public ZoneDomain
	{
	public:
		SrcZoneDomain();
	};


	class DstZoneDomain final : public ZoneDomain
	{
	public:
		DstZoneDomain();
	};


	class Address4Domain abstract : public Domain
	{
	public:
		static Range* create_range(uint32_t lbound, uint32_t ubound);
		static Range* create_full_range();
		static Range* create_singleton(uint32_t value);

		static constexpr int nbits() { return 32; }
		static constexpr uint32_t min() { return 0; }
		static constexpr uint32_t max() { return std::numeric_limits<uint32_t>::max(); }

	protected:
		using Domain::Domain;
	};


	class SrcAddress4Domain final : public Address4Domain
	{
	public:
		SrcAddress4Domain();
	};


	class DstAddress4Domain final : public Address4Domain
	{
	public:
		DstAddress4Domain();
	};


	class Address6Domain abstract : public Domain
	{
	public:
		static Range* create_range(uint128_t lbound, uint128_t ubound, bool ipv4_format);
		static Range* create_full_range();
		static Range* create_singleton(uint128_t value, bool ipv4_format);

		static constexpr int nbits() { return 128; }
		static constexpr uint128_t min() { return uint128_t(); }
		static constexpr uint128_t max() { return std::numeric_limits<uint128_t>::max(); }

	protected:
		using Domain::Domain;
	};


	class SrcAddress6Domain final : public Address6Domain
	{
	public:
		SrcAddress6Domain();
	};


	class DstAddress6Domain final : public Address6Domain
	{
	public:
		DstAddress6Domain();
	};


	class ProtocolDomain final : public Domain
	{
	public:
		ProtocolDomain();

		static Range* create_range(uint16_t lbound, uint16_t ubound);
		static Range* create_full_range();
		static Range* create_singleton(uint16_t value);

		static constexpr int nbits() { return 2; }
		static constexpr uint16_t min() { return 0; }
		static constexpr uint16_t max() { return (1 << nbits()) - 1; }
	};


	class PortDomain abstract : public Domain
	{
	public:
		static Range* create_range(uint16_t lbound, uint16_t ubound);
		static Range* create_full_range();
		static Range* create_singleton(uint16_t value);

		static constexpr int nbits() { return 16; }
		static constexpr uint16_t min() { return 0; }
		static constexpr uint16_t max() { return std::numeric_limits<uint16_t>::max(); }

	protected:
		using Domain::Domain;
	};


	class DstTcpPortDomain final : public PortDomain
	{
	public:
		DstTcpPortDomain();
	};


	class DstUdpPortDomain final : public PortDomain
	{
	public:
		DstUdpPortDomain();
	};


	class IcmpTypeDomain final : public Domain
	{
	public:
		IcmpTypeDomain();

		static Range* create_range(uint16_t lbound, uint16_t ubound);
		static Range* create_full_range();
		static Range* create_singleton(uint16_t value);

		static constexpr int nbits() { return 16; }
		static constexpr uint16_t min() { return 0; }
		static constexpr uint16_t max() { return std::numeric_limits<uint16_t>::max(); }
	};


	class ApplicationDomain final : public Domain
	{
	public:
		ApplicationDomain();

		static Range* create_range(uint16_t lbound, uint16_t ubound);
		static Range* create_full_range();
		static Range* create_singleton(uint16_t value);

		static constexpr int nbits() { return 10; }
		static constexpr uint16_t min() { return 0; }
		static constexpr uint16_t max() { return (1 << nbits()) - 1; }
	};


	class UserDomain final : public Domain
	{
	public:
		UserDomain();

		static Range* create_range(uint16_t lbound, uint16_t ubound);
		static Range* create_full_range();
		static Range* create_singleton(uint16_t value);

		static constexpr int nbits() { return 11; }
		static constexpr uint16_t min() { return 0; }
		static constexpr uint16_t max() { return (1 << nbits()) - 1; }
	};


	class UrlDomain final : public Domain
	{
	public:
		UrlDomain();

		static Range* create_range(uint16_t lbound, uint16_t ubound);
		static Range* create_full_range();
		static Range* create_singleton(uint16_t value);

		static constexpr int nbits() { return 11; }
		static constexpr uint16_t min() { return 0; }
		static constexpr uint16_t max() { return (1 << nbits()) - 1; }
	};

}
