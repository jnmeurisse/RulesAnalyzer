/*!
* This file is part of RulesAnalyzer
*
* Copyright (C) 2024 Jean-Noel Meurisse
* SPDX-License-Identifier: GPL-3.0-only
*
*/
#pragma once
#include "global.h"

#include <algorithm>
#include <cassert>
#include <functional>
#include <list>
#include <string>
#include <type_traits>

#include <buddy/bdd.h>
#include "model/moptions.h"


namespace fwm {

	/* Relations that can exist between two model nodes.
	*/
	enum class MnodeRelationship {
		equal,
		subset,
		superset,
		disjoint,
		overlap
	};

	/* Converts a Mnode relationship to a string.
	*/
	std::string to_string(MnodeRelationship relation);

	// forward declaration.
	class Bddnode;

	/*
	* A Mnode is the root abstract class of all model nodes.  A Mnode is
	* the root of all classes used to model the operation of a firewall.
	* A Mnode can be an address, a protocol, a service, a group of addresses, ...
	* A Mnode child implements the make_bdd method that creates a Binary Decision
	* Diagram for this node.
	*/
	class Mnode abstract {
	public:
		Mnode() = default;
		virtual ~Mnode() {}

		/* Creates a binary decision diagram for this node.
		*/
		virtual bdd make_bdd() const = 0;

		/* Compares two model nodes.
		*/
		MnodeRelationship compare(const Mnode& other) const;

		/* Returns true if this node is representing any value.
		*/
		bool is_any() const;

		/* Returns true if this node is representing an empty set.
		*/
		bool is_none() const;

		/* Returns true if this node is a subset of another node.
		*/
		bool is_subset(const Mnode& other) const;

		/* Returns true if this node is a superset of another node.
		*/
		bool is_superset(const Mnode& other) const;

		/* Returns true if this node is disjoint from another node.
		*/
		bool is_disjoint(const Mnode& other) const;

		/* Returns true if this node overlaps with another node.
		*/
		bool overlaps(const Mnode& other) const;

		/* Returns true if this node equals with another node.
		*/
		bool equal(const Mnode& other) const;

		/* Returns a bdd representing the negation of this node.
		*/
		Bddnode negate_if(bool condition) const;

	private:
		static MnodeRelationship compare(const bdd& a, const bdd& b);
	};


	/* Type of information a NamedMnode can convert to string.
	*/
	enum class MnodeInfoType {
		NAME,
		VALUE
	};


	/**
	* The base class of all named model objects belonging to a model.
	*/
	class NamedMnode abstract : public Mnode {
	public:
		explicit NamedMnode(const std::string& name, const ModelOptions& options);

		/**
		 * Returns the name of the mnode.
		*/
		inline const std::string& name() const noexcept { return _name; }

		/**
		* Returns the value of the mnode as a string.
		*/
		virtual std::string to_string() const = 0;

		/**
		 * Returns a reference to model options.
		*/
		inline const ModelOptions& options() const noexcept { return _options; }

		/**
		 * Compares two named nodes by name.
		 *
		 * The function returns an integer less than, equal to, or greater than
		 * zero if the name of this node is found, respectively, to be less than,
		 * to match, or be greater than the other node name.
		*/
		int compare_name(const NamedMnode& other) const;

	private:
		// The name of this model node.
		const std::string _name;

		// A reference to the model object.
		const ModelOptions& _options;
	};


	/*
	 * A NamedMnodeList is a template container of NamedMnode child objects that
	 * guarantees the unicity of the names added to the list.
	 */
	template <class T>
	class NamedMnodeList
	{
		static_assert(std::is_base_of<NamedMnode, T>::value, "invalid NamedMnodeList<T> template usage");

	public:
		NamedMnodeList() = default;
		virtual ~NamedMnodeList() {};

		/**
		 * Appends a named node to the list only if the name is not yet present.
		 */
		void push_back(const T* named_node)
		{
			if (_named_node_names.insert(named_node->name()).second)
				_named_node_list.push_back(named_node);
		}

		/**
		 * Removes a named node from the list.
		 *
		 * The function returns true if the name was found in the list of named noteds.
		 */
		bool remove(const std::string& name)
		{
			bool  found = false;

			if (_named_node_names.erase(name) == 1) {
				// the name is present in this list
				found = true;

				// find the node with the given name
				auto it = std::find_if(
							_named_node_list.begin(),
							_named_node_list.end(),
							[name](const T* named_node) -> bool
							{
								return named_node->name() == name;
							}
				);

				// remove the node from the list
				assert(it != std::end(_named_node_list));
				_named_node_list.erase(it);
			}

			return found;
		}

		/**
		 * Sorts the named nodes by their name.  The sort is case insensitive.
		 */
		virtual NamedMnodeList<T>& sort() {
			sort(
				[](const T* named_node_1, const T* named_node_2) -> bool
				{
					return named_node_1->compare_name(*named_node_2) < 0;
				}
			);

			return *this;
		};


		/**
		 * Returns all node names.
		 */
		std::vector<std::string> names() const
		{
			std::vector<std::string> all_names;

			all_names.reserve(_named_node_list.size());
			for (const T* node : _named_node_list){
				all_names.push_back(node->name());
			}

			return all_names;
		}

		/**
		 * Returns the number of nodes in this list.
		 */
		inline size_t size() const noexcept { return _named_node_list.size(); }

		using const_iterator = typename std::list<const T*>::const_iterator;

		/**
		 * Returns an iterator to the first element in the list.
		 */
		inline const_iterator begin() const { return _named_node_list.begin(); }

		/**
		 * Returns an iterator to an element following the last element in the list.
		 */
		inline const_iterator end() const { return _named_node_list.end(); }

	protected:
		/**
		 * Sorts the named nodes.
		 *
		 * Nodes are compared using `cmp` function that must returns true if the first node is
		 * before the second node.
		 */
		void sort(const std::function<bool(const T* named_node_1, const T* named_node_2)>& cmp)
		{
			_named_node_list.sort(cmp);
		}

	private:
		// All names.  This set is used to check the unicity of a name.
		std::set<std::string> _named_node_names;

		// The list of pointers to named nodes.
		std::list<const T*> _named_node_list;
	};


	/**
	 * Bddnode is a Mnode that encapsulates a bdd.
	 *
	*/
	class Bddnode : public Mnode {
	public:
		Bddnode();
		Bddnode(const Bddnode& other);
		Bddnode(const bdd& bddvar);

		inline virtual bdd make_bdd() const override { return _bdd; }

	private:
		bdd _bdd;
	};

}
