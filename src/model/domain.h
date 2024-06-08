/*!
* This file is part of RulesAnalyzer
*
* Copyright (C) 2024 Jean-Noel Meurisse
* SPDX-License-Identifier: GPL-3.0-only
*
*/
#pragma once
#include <vector>
#include "bdd.h"
#include "bvec.h"

namespace fwm {

	/*
	 */
	enum class DomainType : int
	{
		SrcAddress = 0,
		DstAddress = 1,
		Protocol = 2,
		TcpDstPort = 3,
		UdpDstPort = 4,
		IcmpType = 5,
		Application = 6,
		SrcZone = 7,
		DstZone = 8,
		User = 9
	};

	/*
	 *
	 */
	class Domain
	{
	public:
		/* Returns the cardinality of this finite domain.
		*/
		size_t size() const;

		/* The upper value of this finite domain.
		*/
		uint32_t upper() const;

		/* Number of bits
		*/
		inline uint16_t nbits() const { return _nbits; }

		/* This domain type
		*/
		inline DomainType dt() const { return _dt; }

		/* Returns the binary vector that encodes this domain
		*/
		virtual const bvec& var() const = 0;

	protected:
		Domain(DomainType dt, uint16_t nbits);

	private:
		// The domain type
		const DomainType _dt;

		// Number of bits in the binary vector
		const uint16_t _nbits;
	};

}
