/*!
* This file is part of RulesAnalyzer
*
* Copyright (C) 2024 Jean-Noel Meurisse
* SPDX-License-Identifier: GPL-3.0-only
*
*/
#pragma once

#include "bvec.h"
#include "domain.h"
#include "mnode.h"

namespace fwm {

	/**
	 * An Interval represents a set of integer values
	 */
	class Interval : public Mnode {
	public:
		Interval(const DomainType dt, uint32_t lower, uint32_t upper);

		/* Creates a binary decision diagram from this interval.
		*/
		virtual bdd make_bdd() const override;

		/* Returns the cardinality of this set of values.
		*/
		size_t size() const noexcept;

		/* Returns the lower bound of this interval.
		*/
		inline uint32_t lower() const noexcept { return _lower; }

		/* Returns the upper bound of this interval.
		*/
		inline uint32_t upper() const noexcept { return _upper; }

		/* Returns this interval as a string
		*/
		virtual std::string to_string() const;

	private:
		const Domain& _domain;
		const uint32_t _lower;
		const uint32_t _upper;
	};

}
