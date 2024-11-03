/*!
* This file is part of RulesAnalyzer
*
* Copyright (C) 2024 Jean-Noel Meurisse
* SPDX-License-Identifier: GPL-3.0-only
*
*/
#pragma once
#include "global.h"

#include <string>

#include "ostore/addressobject.h"
#include "ostore/fqdnresolver.h"
#include "ostore/objectdictionary.h"
#include "model/ipaddress.h"
#include "model/mconfig.h"
#include "tools/csvreader.h"


namespace fos {
	using namespace fwm;

	class AddressObjects : public ObjectDictionary<AddressObject>
	{
	public:
		AddressObjects(const CsvReaderConfig& reader_config, const ModelConfig& core_config, FqdnResolver& fqdn_resolver);

		virtual const AddressObject* get(const std::string& name) const override;
		virtual const AddressObject* make_object(const csv::CsvValues& values) const override;
		static const ObjectParameters& parameters();

	private:
		/* Returns true if the string is a valid ip address or ip address range.
		*/
		bool is_ip_address(const std::string& addr) const;

		/* Returns the address type of an IP address.
		*/
		IPAddressType get_ip_address_type(const std::string& addr) const;

		/* The address model of all addresses stored in this dictionary.
		*/
		const IPAddressModel _ip_model;

		/* Should we run a strict IP address parser
		*/
		const bool _strict_ip_parser;

		/* A reference to the hostname resolver.
		*/
		FqdnResolver& _fqdn_resolver;
	};

}
