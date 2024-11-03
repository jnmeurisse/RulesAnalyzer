/*!
* This file is part of RulesAnalyzer
*
* Copyright (C) 2024 Jean-Noel Meurisse
* SPDX-License-Identifier: GPL-3.0-only
*
*/
#include <string.h>
#include "model/mnode.h"
#include "tools/strutil.h"

namespace fwm {

	std::string to_string(MnodeRelationship relation)
	{
		switch (relation)
		{
		case MnodeRelationship::disjoint:
			return "disjoint";
		case MnodeRelationship::equal:
			return "equal";
		case MnodeRelationship::overlap:
			return "overlap";
		case MnodeRelationship::subset:
			return "subset";
		case MnodeRelationship::superset:
			return "superset";
		default:
			return "internal error";
		}
	}


	MnodeRelationship Mnode::compare(const Mnode& other) const
	{
		return compare(make_bdd(), other.make_bdd());
	}


	bool Mnode::is_any() const
	{
		return make_bdd() == bddtrue;
	}


	bool Mnode::is_none() const
	{
		return make_bdd() == bddfalse;
	}


	bool Mnode::is_subset(const Mnode& other) const
	{
		const bdd a{ make_bdd() };
		const bdd b{ other.make_bdd() };

		return (a == b) || (bdd_imp(a, b) == bddtrue);
	}


	bool Mnode::is_superset(const Mnode& other) const
	{
		return other.is_subset(*this);
	}


	bool Mnode::is_disjoint(const Mnode& other) const
	{
		const bdd a{ make_bdd() };
		const bdd b{ other.make_bdd() };

		return bdd_and(a, b) == bddfalse;
	}


	bool Mnode::overlaps(const Mnode & other) const
	{
		return !is_disjoint(other);
	}


	bool Mnode::equal(const Mnode& other) const
	{
		const bdd a{ make_bdd() };
		const bdd b{ other.make_bdd() };

		return a == b;
	}


	Bddnode Mnode::negate_if(bool condition) const
	{
		return Bddnode(condition ? bdd_not(make_bdd()) : make_bdd());
	}


	MnodeRelationship Mnode::compare(const bdd& a, const bdd& b)
	{
		if (a == b)
			return MnodeRelationship::equal;

		else if (bdd_imp(a, b) == bddtrue)
			return MnodeRelationship::subset;

		else if (bdd_imp(b, a) == bddtrue)
			return MnodeRelationship::superset;

		else {
			bdd intersection = a & b;

			if (intersection == bddfalse)
				return MnodeRelationship::disjoint;
			else
				return MnodeRelationship::overlap;
		}
	}


	NamedMnode::NamedMnode(const std::string& name, const ModelOptions& options) :
		_name{ name },
		_options{ options }
	{
	}


	std::string NamedMnode::to_string() const
	{
		return _name;
	}


	int NamedMnode::compare_name(const NamedMnode& other) const
	{
		return ::strcasecmp(_name.c_str(), other._name.c_str());
	}


	Bddnode::Bddnode() :
		_bdd{}
	{
	}


	Bddnode::Bddnode(const bdd& bddvar) :
		_bdd{ bddvar }
	{
	}


	Bddnode::Bddnode(const Bddnode& other) :
		_bdd{ other._bdd }
	{
	}

}
