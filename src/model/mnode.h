/*!
* This file is part of RulesAnalyzer
*
* Copyright (C) 2024 Jean-Noel Meurisse
* SPDX-License-Identifier: GPL-3.0-only
*
*/
#pragma once
#include "bdd.h"
#include "moptions.h"

#include <string>


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
	* A Mnode can be an address, a protocol, a service, ... A Mnode
	* child implements the make_bdd method that creates a Binary Decision
	* Diagram for this node.
	*/
	class Mnode {
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

		/* Returns true if this node is disjoint of another node.
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
	* The base class of all named model objects.
	*/
	class NamedMnode : public Mnode {
	public:
		explicit NamedMnode(const std::string& name, const ModelOptions& options);

		/* Returns the name of the mnode.
		*/
		inline const std::string& name() const noexcept { return _name; }

		/* returns the value of the mnode as a string.
		*/
		virtual std::string to_string() const = 0;

		/* returns a reference to model options.
		*/
		inline const ModelOptions& options() const noexcept { return _options; }

		/* compare two named nodes by name
		*/
		int compare_name(const NamedMnode& other) const;

	private:
		const std::string _name;
		const ModelOptions& _options;
	};

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
