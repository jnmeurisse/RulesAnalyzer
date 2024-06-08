/*!
* This file is part of RulesAnalyzer
*
* Copyright (C) 2024 Jean-Noel Meurisse
* SPDX-License-Identifier: GPL-3.0-only
*
*/
#pragma once
#include <exception>
#include <list>
#include <map>
#include <memory>
#include <string>
#include <utility>

#include "tools/strutil.h"

namespace fwm {

	template<typename T>
	class Cache {
	public:
		Cache();
		const T* get(const std::string& name) const;
		const T* set(const std::string& name, const T* item);
		std::list<const T*> items() const;

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
		auto it = _items.find(name);

		if (it == _items.end())
			return nullptr;
		else
			return it->second.get();
	}


	template<typename T>
	const T* Cache<T>::set(const std::string& name, const T* item)
	{
		// check if the item name exists in this map
		const auto it = _items.find(name);

		if (it != _items.end()) {
			throw std::runtime_error(string_format("item %s already in the cache", name.c_str()));
		}
		else {
			// No, insert the name-value pair to the map
			_items.insert(std::make_pair(name, item));
		}

		return item;
	}


	template<typename T>
	std::list<const T*> Cache<T>::items() const
	{
		std::list<const T*> items{};

		for (auto it = _items.cbegin(); it != _items.cend(); ++it)
			items.push_back(it->second.get());

		return items;
	}

}
