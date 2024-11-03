/*!
* This file is part of RulesAnalyzer
*
* Copyright (C) 2024 Jean-Noel Meurisse
* SPDX-License-Identifier: GPL-3.0-only
*
*/
#pragma once
#include "global.h"

#include <stdexcept>
#include <list>
#include <map>
#include <memory>
#include <string>
#include <utility>
#include "fmt/core.h"

namespace fwm {

	template<typename T>
	class Cache {
	public:
		Cache();
		Cache(const T&) = delete;
		Cache& operator=(const T&) = delete;

		const T* get(const std::string& name) const;
		const T* set(const T* item);

	private:
		std::map<const std::string, std::unique_ptr<const T>> _items;
	};


	template<typename T>
	Cache<T>::Cache() :
		_items{}
	{
	}


	template<typename T>
	const T* Cache<T>::get(const std::string& name) const
	{
		const auto it = _items.find(name);

		if (it == _items.end())
			return nullptr;
		else
			return it->second.get();
	}


	template<typename T>
	const T* Cache<T>::set(const T* item)
	{
		const std::string& name = item->name();

		// insert the name-value pair into the map or rase an exception if the item is in the cache.
		const auto it =_items.insert(std::make_pair(name, item));
		if (!it.second)
			throw std::runtime_error(fmt::format("internal error : item '{}' already in the cache", name));

		return item;
	}

}
