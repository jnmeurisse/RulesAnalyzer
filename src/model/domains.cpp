/*!
* This file is part of RulesAnalyzer
*
* Copyright (C) 2024 Jean-Noel Meurisse
* SPDX-License-Identifier: GPL-3.0-only
*
*/
#include "domains.h"
#include "domain.h"

namespace fwm {

	class DomainImpl : public Domain
	{
	public:
		DomainImpl(DomainType dt, uint16_t nbits) :
			Domain(dt, nbits) {}

		virtual const bvec& var() const override {
			return Domains::get_bvec(dt());
		};
	};


	class SourceAddressDomain : public DomainImpl
	{
	public:
		SourceAddressDomain() : DomainImpl(DomainType::SrcAddress, 32) {};
	};


	class DestinationAddressDomain : public DomainImpl
	{
	public:
		DestinationAddressDomain() : DomainImpl(DomainType::DstAddress, 32) {};
	};


	class ProtocolDomain : public DomainImpl
	{
	public:
		ProtocolDomain() : DomainImpl(DomainType::Protocol, 4) {};
	};


	class DestinationTcpPortDomain : public DomainImpl
	{
	public:
		DestinationTcpPortDomain() : DomainImpl(DomainType::TcpDstPort, 16) {};
	};


	class DestinationUdpPortDomain : public DomainImpl
	{
	public:
		DestinationUdpPortDomain() : DomainImpl(DomainType::UdpDstPort, 16) {};
	};


	class IcmpTypeDomain : public DomainImpl
	{
	public:
		IcmpTypeDomain() : DomainImpl(DomainType::IcmpType, 8) {};
	};


	class ApplicationDomain : public DomainImpl
	{
	public:
		ApplicationDomain() : DomainImpl(DomainType::Application, 10) {};
	};


	class SourceZoneDomain : public DomainImpl
	{
	public:
		SourceZoneDomain() : DomainImpl(DomainType::SrcZone, 6) {};
	};


	class DestinationZoneDomain : public DomainImpl
	{
	public:
		DestinationZoneDomain() : DomainImpl(DomainType::DstZone, 6) {};
	};


	class UserDomain : public DomainImpl
	{
	public:
		UserDomain() : DomainImpl(DomainType::User, 10) {};
	};


	Domains::Domains()
	{
		_domains.push_back(new SourceAddressDomain());
		_domains.push_back(new DestinationAddressDomain());
		_domains.push_back(new ProtocolDomain());
		_domains.push_back(new DestinationTcpPortDomain());
		_domains.push_back(new DestinationUdpPortDomain());
		_domains.push_back(new IcmpTypeDomain());
		_domains.push_back(new ApplicationDomain());
		_domains.push_back(new SourceZoneDomain());
		_domains.push_back(new DestinationZoneDomain());
		_domains.push_back(new UserDomain());
	}


	Domains::~Domains()
	{
		for (int dn = 0; dn < _domains.size(); dn++)
			delete _domains[dn];
	}


	void Domains::init(int node_size, int cache_size)
	{
		int err;

		// Initialize the bdd library.
		bdd_autoreorder(BDD_REORDER_NONE);
		err = bdd_init(node_size, cache_size);
		if (err < 0) goto error;

		// Allocate all variables.
		int nvars = 0;
		for (int dn = 0; dn < _domains.size(); dn++) {
			nvars += _domains[dn]->nbits();
		}
		err = ::bdd_setvarnum(nvars);
		if (err < 0) goto error;

		// Create a vector of variables for each domain.
		for (int dn = 0, offset = 0; dn < _domains.size(); dn++) {
			_vecs.push_back(bvec_var(_domains[dn]->nbits(), offset, 1));
			offset += _domains[dn]->nbits();
		}
		return;

	error:
		throw std::runtime_error("bdd initialization error : " + std::string(bdd_errstring(err)));
	}


	void Domains::reset()
	{
		_vecs.clear();
		bdd_done();
	}


	void Domains::check_dn(int dn)
	{
		if (_domains.size() == 0)
			throw std::runtime_error("domains not allocated");

		if (dn < 0 || dn >= _domains.size())
			throw std::runtime_error("invalid domain number");
	}


	const Domain& Domains::get(DomainType dt)
	{
		const int dn = static_cast<int>(dt);

		check_dn(dn);
		return *Domains::_domains[dn];
	}


	const bvec& Domains::get_bvec(DomainType dt)
	{
		const int dn = static_cast<int>(dt);

		check_dn(dn);
		if (_vecs.size() != _domains.size())
			throw std::runtime_error("domains not initialized");

		return Domains::_vecs[dn];
	}


	std::vector<bvec> Domains::_vecs;
	std::vector<Domain *> Domains::_domains;
}
