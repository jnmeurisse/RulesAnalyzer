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
			const SrcZone* src_zone,
			const SrcAddressGroup& src_addr_grp,
			const DstZone* dst_zone,
			const DstAddressGroup& dst_addr_grp,
			const ServiceGroup& svc_grp,
			const ApplicationGroup* app_grp,
			const UserGroup* usr_grp,
			const UrlGroup* url_grp
		) const;

	private:
		const RuleList _acl;
	};

}
