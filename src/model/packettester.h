/*!
* This file is part of RulesAnalyzer
*
* Copyright (C) 2024 Jean-Noel Meurisse
* SPDX-License-Identifier: GPL-3.0-only
*
*/
#pragma once
#include <utility>

#include "acl.h"
#include "address.h"
#include "application.h"
#include "network.h"
#include "rule.h"
#include "service.h"
#include "user.h"
#include "zone.h"

namespace fwm {
	class PacketTester
	{
	public:
		PacketTester(const Network& nw, const Acl& acl);

		std::pair<bool, const Rule*> is_packet_allowed(
			const SrcZone* src_zone,
			const SrcAddressGroup& src_addr,
			const DstZone* dst_zone,
			const DstAddressGroup& dst_addr,
			const ServiceGroup& svc,
			const ApplicationGroup* app,
			const UserGroup* usr
		) const;

	private:
		const Network& _nw;
		const Acl _acl;
	};
}
