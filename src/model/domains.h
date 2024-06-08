/*!
* This file is part of RulesAnalyzer
*
* Copyright (C) 2024 Jean-Noel Meurisse
* SPDX-License-Identifier: GPL-3.0-only
*
*/
#pragma once
#include <memory>
#include <vector>

#include "bvec.h"
#include "domain.h"

namespace fwm {

	class Domains
	{
	public:
		Domains();
		~Domains();

		static void init(int node_size, int cache_size);
		static void reset();

		static const Domain& get(DomainType dt);
		static const bvec& get_bvec(DomainType dt);

	private:
		static void check_dn(int dn);

		static std::vector<bvec> _vecs;
		static std::vector<Domain *> _domains;
	};
}