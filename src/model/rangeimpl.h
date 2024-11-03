/*!
* This file is part of RulesAnalyzer
*
* Copyright (C) 2024 Jean-Noel Meurisse
* SPDX-License-Identifier: GPL-3.0-only
*
*/
#pragma once
#include "global.h"

#include <cassert>
#include <cstdint>
#include <string>

#include "model/range.h"
#include "tools/uint128.h"


namespace fwm {
	/**
	* RangeImpl is a template that provides an implementation of the Range class.
	* Warning : T must be an unsigned integer type.
	*/
	template<typename T>
	class RangeImpl : public Range
	{
	public:
		explicit RangeImpl(int nbits, const T &lbound, const T &ubound);

		virtual bvec lbound() const override;
		virtual bvec ubound() const override;

		virtual bool is_singleton() const override;
		virtual bool is_power_of_2() const override;
		virtual Range* clone() const override;

		virtual std::string to_string() const override;

	protected:
		// Lower and upper bounds of the range.
		const T _lbound;
		const T _ubound;
	};


	template<typename T>
	inline RangeImpl<T>::RangeImpl(int nbits, const T& lbound, const T& ubound) :
		Range(nbits),
		_lbound{ lbound },
		_ubound{ ubound }
	{
		assert(lbound <= ubound);
	};


	template<typename T>
	inline bvec RangeImpl<T>::lbound() const
	{
		return bvec(_nbits, _lbound);
	};


	template<>
	inline bvec RangeImpl<uint128_t>::lbound() const
	{
		return bvec(_nbits, s_uint128{ _lbound.lower(), _lbound.upper() });
	};


	template<typename T>
	inline bvec RangeImpl<T>::ubound() const
	{
		return bvec(_nbits, _ubound);
	};


	template<>
	inline bvec RangeImpl<uint128_t>::ubound() const
	{
		return bvec(_nbits, s_uint128{ _ubound.lower(), _ubound.upper() });
	};


	template<typename T>
	inline bool RangeImpl<T>::is_singleton() const
	{
		return _lbound == _ubound;
	};


	template<typename T>
	inline bool RangeImpl<T>::is_power_of_2() const
	{
		// Power of 2 test :
		//     range = ubound - lbound + 1
		//     (range > 0) and (range & (range-1)) == 0
		//
		// The generic implementation avoids to add one when computing the range.
		// Adding one will cause an integer overflow when lbound == 0 and
		// ubound == <T>_MAX.

		static const T zero(0);
		static const T one(1);

		const T diff = _ubound - _lbound;
		if (diff == zero)
			return true;

		if (diff == std::numeric_limits<T>::max())
			return true;

		return ((diff + one) & diff) == zero;
	}


	template<typename T>
	inline Range* RangeImpl<T>::clone() const
	{
		return new RangeImpl<T>(_nbits, _lbound, _ubound);
	};


	template<typename T>
	inline std::string RangeImpl<T>::to_string() const
	{
		if (_lbound == _ubound)
			return std::to_string(_lbound);
		else
			return std::to_string(_lbound) + "-" + std::to_string(_ubound);
	};


	template<>
	inline std::string RangeImpl<uint128_t>::to_string() const
	{
		if (_lbound == _ubound)
			return _lbound.str(10);
		else
			return _lbound.str(10) + "-" + _ubound.str(10);
	};


	class Range16 : public RangeImpl<uint16_t>
	{
	public:
		using RangeImpl<uint16_t>::RangeImpl;
	};


	class Range32 : public RangeImpl<uint32_t>
	{
	public:
		using RangeImpl<uint32_t>::RangeImpl;
	};


	class Range128 : public RangeImpl<uint128_t>
	{
	public:
		using RangeImpl<uint128_t>::RangeImpl;
	};

}
