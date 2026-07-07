/*!
* This file is part of RulesAnalyzer
*
* Copyright (C) 2024 Jean-Noel Meurisse
* SPDX-License-Identifier: GPL-3.0-only
*
*/
#pragma once
#include "global.h"

#include <utility>

#include "model/address.h"
#include "model/application.h"
#include "model/network.h"
#include "model/rule.h"
#include "model/service.h"
#include "model/url.h"
#include "model/user.h"
#include "model/zone.h"

namespace fwm {
	class PacketTester
	{
	public:
		PacketTester(const RuleList& acl);

		std::pair<bool, const Rule*> is_packet_allowed(
			SrcZonePtr src_zone,
			SrcAddressGroupPtr src_addr_grp,
			DstZonePtr dst_zone,
			DstAddressGroupPtr dst_addr_grp,
			ServiceGroupPtr svc_grp,
			ApplicationGroupPtr app_grp,
			UserGroupPtr usr_grp,
			UrlGroupPtr url_grp
		) const;

	private:
		const RuleList _acl;
	};

}
