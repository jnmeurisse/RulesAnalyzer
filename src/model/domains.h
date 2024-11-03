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
#include <vector>
#include <buddy/bvec.h>

#include "model/domain.h"


namespace fwm {

	/* A collection of domains.
	*/
	class Domains final
	{
	public:
		/* Returns the domains collection.
		*/
		static Domains& get();

		/* Initialize the bdd library.
		*/
		void init_bdd(int node_size, int cache_size);

		/* Reclaim memory used by the bdd library.
		*/
		void reset_bdd();

		/* Returns the domain of the given type.
		*/
		const Domain& operator[](DomainType dt) const;

		/* Returns the model variable of the given type.
		*/
		const bvec& get_var(DomainType dt) const;

	private:
		Domains();
		~Domains();

		void check_dn(int dn) const;

		std::vector<bvec> _vars;
		std::vector<Domain *> _domains;
	};

}
