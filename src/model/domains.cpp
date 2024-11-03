/*!
* This file is part of RulesAnalyzer
*
* Copyright (C) 2024 Jean-Noel Meurisse
* SPDX-License-Identifier: GPL-3.0-only
*
*/
#include "model/domains.h"

#include <stdexcept>

#include "model/domain.h"


namespace fwm {

	Domains::Domains()
	{
		// Warning : initialization order must match the DomainType order.
		_domains.push_back(new SrcZoneDomain());
		_domains.push_back(new SrcAddress4Domain());
		_domains.push_back(new SrcAddress6Domain());
		_domains.push_back(new DstZoneDomain());
		_domains.push_back(new DstAddress4Domain());
		_domains.push_back(new DstAddress6Domain());
		_domains.push_back(new ProtocolDomain());
		_domains.push_back(new DstTcpPortDomain());
		_domains.push_back(new DstUdpPortDomain());
		_domains.push_back(new IcmpTypeDomain());
		_domains.push_back(new ApplicationDomain());
		_domains.push_back(new UserDomain());
		_domains.push_back(new UrlDomain());
	}


	Domains::~Domains()
	{
		reset_bdd();
		for (int dn = 0; dn < _domains.size(); dn++)
			delete _domains[dn];
	}


	Domains& Domains::get()
	{
		static Domains domains;
		return domains;
	}


	void Domains::init_bdd(int node_size, int cache_size)
	{
		int err = 0;

		if (_vars.size() == 0) {
			// Initialize the bdd library.
			bdd_autoreorder(BDD_REORDER_NONE);
			err = bdd_init(node_size, cache_size);
			if (err < 0) goto error;

			// Allocate all variables.
			int nvars = 0;
			for (int dn = 0; dn < _domains.size(); dn++) {
				nvars += _domains[dn]->range().nbits();
			}
			err = ::bdd_setvarnum(nvars);
			if (err < 0) goto error;

			// Create a vector of variables for each domain.
			for (int dn = 0, offset = 0; dn < _domains.size(); dn++) {
				const int nbits = _domains[dn]->range().nbits();
				_vars.push_back(bvec_var(nbits, offset, 1));
				offset += nbits;
			}
		}

		return;

	error:
		throw std::runtime_error("bdd initialization error : " + std::string(bdd_errstring(err)));
	}


	void Domains::reset_bdd()
	{
		_vars.clear();
		bdd_done();
	}


	void Domains::check_dn(int dn) const
	{
		if (_domains.size() == 0)
			throw std::runtime_error("internal error : domains not allocated");

		if (dn < 0 || dn >= _domains.size())
			throw std::runtime_error("internal error : invalid domain number");
	}


	const Domain& Domains::operator[](DomainType dt) const
	{
		const int dn = static_cast<int>(dt);

		check_dn(dn);
		return *Domains::_domains[dn];
	}


	const bvec& Domains::get_var(DomainType dt) const
	{
		const int dn = static_cast<int>(dt);

		check_dn(dn);
		if (_vars.size() != _domains.size())
			throw std::runtime_error("internal error : domains not initialized");

		return _vars[dn];
	}

}
