/*!
* This file is part of RulesAnalyzer
*
* Copyright (C) 2024 Jean-Noel Meurisse
* SPDX-License-Identifier: GPL-3.0-only
*
*/
#pragma once
#include "global.h"

#include <memory>
#include <string>
#include <buddy/bvec.h>

#include "model/domain.h"
#include "model/mnode.h"
#include "model/range.h"


namespace fwm {

	/**
	 * A Mvalue represents a range of integer values over a specific domain.
	 */
	class Mvalue : public Mnode {
	public:
		/**
		 * Allocates a model value.
		 *
		 * @param dt A domain type.
		 * @param range A range of integer values.  The range must be dynamically allocated.
		 *              The range object is owned by this class.
		 *
		*/
		explicit Mvalue(const DomainType dt, const Range* range);

		/**
		 * Creates a binary decision diagram from this interval.
		 *
		 * The generated bdd tests if the range is within the domain range.
		*/
		virtual bdd make_bdd() const override;

		/**
		 * Returns this Mvalue as a string.
		*/
		virtual std::string to_string() const;

		/**
		 * Returns the range of this MValue.
		*/
		inline const Range& range() const noexcept { return *_range; }

	private:
		// The domain to which the range of values is associated.
		const Domain& _domain;

		// The range of values
		const RangePtr _range;

		// Is true when the range covers the whole domain.  This flag is
		// used to simplify the bdd evaluation to bddtrue.
		const bool _all;
	};

	using MvaluePtr = std::unique_ptr<const Mvalue>;
}
