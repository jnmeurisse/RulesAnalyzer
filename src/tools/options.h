/*!
* This file is part of RulesAnalyzer
*
* Copyright (C) 2024 Jean-Noel Meurisse
* SPDX-License-Identifier: GPL-3.0-only
*
*/
#pragma once

#include <set>
#include <vector>

namespace rat {

	template<typename T>
	class Options : private std::set<T>
	{
	public:
		Options() = default;
		Options(const std::vector <T>& options);

		/* Returns true if this set contains the specified option.
		*/
		bool contains(T option) const;

		/* Adds the specified option to this set.
		*/
		void add(T option);

		/* Removes the specified option from this set.
		*/
		void remove(T option);

		/* All options.
		*
		* TODO: returns an iterator instead of a vector.
		*/
		std::vector<T> options() const;
	};


	template<typename T>
	inline Options<T>::Options(const std::vector<T>& options):
		std::set<T>(options.begin(), options.end())
	{
	}


	template<typename T>
	inline bool Options<T>::contains(T option) const
	{
		return Options<T>::find(option) != std::set<T>::end();
	}


	template<typename T>
	inline void Options<T>::add(T option)
	{
		Options<T>::insert(option);
	}


	template<typename T>
	inline void Options<T>::remove(T option)
	{
		Options<T>::erase(option);
	}


	template<typename T>
	inline std::vector<T> Options<T>::options() const
	{
		std::vector<T> result;

		for (T option : *this)
			result.push_back(option);

		return result;
	}

}
