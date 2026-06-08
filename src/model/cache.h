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

		std::shared_ptr<const T> get(const std::string& name) const;
		std::shared_ptr<const T> set(const std::shared_ptr<const T> item);

	private:
		std::map<const std::string, std::shared_ptr<const T>> _items;
	};


	template<typename T>
	Cache<T>::Cache() :
		_items{}
	{
	}


	template<typename T>
	std::shared_ptr<const T> Cache<T>::get(const std::string& name) const
	{
		const auto it = _items.find(name);

		if (it == _items.end())
			return nullptr;
		else
			return it->second;
	}


	template<typename T>
	std::shared_ptr<const T> Cache<T>::set(const std::shared_ptr<const T> item)
	{
		if (!item)
			throw std::runtime_error("internal error : null pointer cache::set");

		const std::string& name = item->name();

		// insert the name-value pair into the map or raise an exception if the 
		// item name exists in the cache.
		const auto result =_items.insert(std::make_pair(name, item));
		if (!result.second)
			throw std::runtime_error(fmt::format("internal error : item '{}' already in the cache", name));

		return item;
	}

}
