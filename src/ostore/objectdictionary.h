/*!
* This file is part of RulesAnalyzer
*
* Copyright (C) 2024 Jean-Noel Meurisse
* SPDX-License-Identifier: GPL-3.0-only
*
*/
#pragma once
#include "global.h"

#include <map>
#include <memory>
#include <list>
#include <string>
#include <utility>
#include <vector>

#include "ostore/ostoreconfig.h"
#include "tools/csvreader.h"
#include "tools/match.h"
#include "tools/logger.h"
#include "tools/strutil.h"


namespace fos {

	using ObjectParameters = std::vector<csv::CsvColumn>;

	template <typename T>
	class ObjectDictionary {
	public:
		ObjectDictionary(const std::string& name, const CsvReaderConfig& reader_config);

		/* Loads a dictionary from a .csv file.
		*/
		int load(csv::CsvReader& reader);

		/* Clears this dictionary.
		*/
		virtual void clear();

		/* Adds an item of type T into this dictionary.
		*/
		virtual void add(const T* item);

		/* Delete the item named 'name' from this dictionary.
		*/
		virtual void del(const std::string& name);

		/* Returns the item named 'name' from this dictionary.
		*/
		virtual const T* get(const std::string& name) const;

		/* Returns a list of items having a name matching the give wildcard.
		*/
		virtual std::list<const T*> query(const std::string& wildcard) const;

		/* Returns the number of items in this dictionary.
		*/
		size_t size() const { return _items.size(); }

		/* Returns the name of this dictionary.
		*/
		const std::string& name() const noexcept { return _name; }

	protected:
		// a reference to the application logger
		Logger* const _logger;

		/* Allocate and initialize a firewall object from the cvs values.
		*/
		virtual const T* make_object(const csv::CsvValues& values) const = 0;

		/* Splits the given string into a vector of strings.
		*/
		std::vector<std::string> split(const std::string& str) const;

	private:
		// Name of this object dictionary, used only for debugging purpose
		std::string _name;

		// a reference to the csv reader configuration
		const CsvReaderConfig& _reader_config;

		// All objects in this dictionary
		std::map<const std::string, std::unique_ptr<const T>> _items;

		void add(const std::string& name, const T*item);
	};


	template<typename T>
	inline ObjectDictionary<T>::ObjectDictionary(const std::string& name, const CsvReaderConfig& reader_config) :
		_name{ name },
		_reader_config{ reader_config },
		_items{},
		_logger{ Logger::get_logger() }
	{
	}

	template<typename T>
	int ObjectDictionary<T>::load(csv::CsvReader& reader)
	{
		csv::CsvValues values;
		int counter = 0;

		while (reader.next_row(values)) {
			const T* obj = make_object(values);
			if (obj) {
				add(obj);
				counter++;
			}
		}

		return counter;
	}

	template<typename T>
	void ObjectDictionary<T>::clear()
	{
		_items.clear();
	}

	template<typename T>
	void ObjectDictionary<T>::add(const T* item)
	{
		if (item)
			add(item->name(), item);
	}


	template<typename T>
	inline void ObjectDictionary<T>::del(const std::string& name)
	{
		_items.erase(name);
	}


	template<typename T>
	void ObjectDictionary<T>::add(const std::string& name, const T* item)
	{
		auto it = _items.find(name);
		if (it == _items.end()) {
			// Insert a new object
			_items.insert(std::make_pair(name, item));
		}
		else
		{
			// Replace an existing object
			it->second.reset(item);
		}
	}


	template<typename T>
	const T* ObjectDictionary<T>::get(const std::string& name) const
	{
		auto it = _items.find(name);

		if (it == _items.end())
			return nullptr;
		else
			return it->second.get();
	}


	template<typename T>
	inline std::vector<std::string> ObjectDictionary<T>::split(const std::string& str) const
	{
		return rat::split(str, _reader_config.csv_list_delimiter);
	}


	template<typename T>
	std::list<const T*> ObjectDictionary<T>::query(const std::string& wildcard) const
	{
		std::list<const T*> selected_items;

		for (auto it = _items.cbegin(); it != _items.cend(); it++) {
			const T* item = it->second.get();
			if (rat::match(wildcard, item->name()))
				selected_items.push_back(it->second.get());
		}

		return selected_items;
	}

}
