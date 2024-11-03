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


namespace fwm {

	/**
	* A range defines all positive integer values between a lower bound and a upper bound
	* that can be stored in an integer having at least n bits and such that
	*         0 <= lbound <= ubound < 2^n
	*/
	class Range abstract
	{
	public:
		virtual ~Range() {}

		/**
		 * Returns the number of bits used to store a value in this range.
		*/
		inline int nbits() const noexcept { return _nbits; }

		/**
		 * Returns a binary vector representing the lower bound of this range.
		*/
		virtual bvec lbound() const = 0;

		/**
		 * Returns a binary vector representing the upper bound of this range.
		*/
		virtual bvec ubound() const = 0;

		/**
		 * Returns true when the lower bound equal the upper bound.
		*/
		virtual bool is_singleton() const = 0;

		/**
		 * Returns true when the range (ubound-lbound+1) is a power of 2.
		*/
		virtual bool is_power_of_2() const = 0;

		/**
		 * Allocates a copy of this range.
		*/
		virtual Range* clone() const = 0;

		/**
		 * Returns true if the other range equal to this range.
		 *
		 * Two ranges are equal if the number of bits representing the range is the
		 * same and bounds are identical.
		*/
		bool operator==(const Range& other) const;

		/**
		 * Returns a string representation of this range.
		*/
		virtual std::string to_string() const = 0;

	protected:
		explicit Range(int nbits);

		const int _nbits;
	};

	using RangePtr = std::unique_ptr<const Range>;

}
