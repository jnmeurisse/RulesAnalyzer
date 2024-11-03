/*!
* This file is part of RulesAnalyzer
*
* Copyright (C) 2024 Jean-Noel Meurisse
* SPDX-License-Identifier: GPL-3.0-only
*
*/
#include "model/rule.h"

#include <cassert>
#include <utility>

#include "model/firewall.h"


namespace fwm {

	RuleAction operator!(RuleAction action)
	{
		return action == RuleAction::ALLOW
			? RuleAction::DENY
			: RuleAction::ALLOW;
	}


	Rule::Rule(const std::string& name, int id, RuleStatus status, RuleAction action, const Predicate* predicate, Firewall& firewall) :
		_name{ name },
		_id{ id },
		_firewall{ firewall },
		_status{ status },
		_action{ action },
		_predicate{ predicate }
	{
	}


	Rule::Rule(const Rule& other) :
		_name{ other._name },
		_id{ other._id },
		_firewall{ other._firewall },
		_status{ other._status },
		_action{ other._action },
		_predicate{ new Predicate(*other._predicate) }
	{
	}


	void Rule::write_to_row(Row& row, const RuleOutputOptions& options) const
	{
		int col_num = 0;

		row.cell(col_num++).append(id());
		if (options.contains(RuleOutputOption::RULE_NAME))
			row.cell(col_num++).append(name());
		row.cell(col_num++).append(action() == RuleAction::ALLOW ? "allow" : "deny");
		predicate().src_zones().write_to_cell(row.cell(col_num++), MnodeInfoType::NAME);
		predicate().dst_zones().write_to_cell(row.cell(col_num++), MnodeInfoType::NAME);
		if (options.contains(RuleOutputOption::NEGATE_ADDRESS))
			row.cell(col_num++).append(predicate().negate_src_addresses() ? "yes": "no");
		if (options.contains(RuleOutputOption::ADDRESS_NAME))
			predicate().src_addresses().write_to_cell(row.cell(col_num++), MnodeInfoType::NAME);
		predicate().src_addresses().write_to_cell(row.cell(col_num++), MnodeInfoType::VALUE);
		if (options.contains(RuleOutputOption::NEGATE_ADDRESS))
			row.cell(col_num++).append(predicate().negate_dst_addresses() ? "yes" : "no");
		if (options.contains(RuleOutputOption::ADDRESS_NAME))
			predicate().dst_addresses().write_to_cell(row.cell(col_num++), MnodeInfoType::NAME);
		predicate().dst_addresses().write_to_cell(row.cell(col_num++), MnodeInfoType::VALUE);
		if (options.contains(RuleOutputOption::SERVICE_NAME))
			predicate().services().write_to_cell(row.cell(col_num++), MnodeInfoType::NAME);
		predicate().services().write_to_cell(row.cell(col_num++), MnodeInfoType::VALUE);
		if (options.contains(RuleOutputOption::APPLICATION_NAME))
			predicate().applications().write_to_cell(row.cell(col_num++), MnodeInfoType::NAME);
		if (options.contains(RuleOutputOption::USER_NAME))
			predicate().users().write_to_cell(row.cell(col_num++), MnodeInfoType::NAME);
		if (options.contains(RuleOutputOption::URL))
			predicate().urls().write_to_cell(row.cell(col_num++), MnodeInfoType::NAME);
	}


	Table Rule::create_table(const RuleOutputOptions& options) const
	{
		Table rule_table{ { "attribute", "name", "value" } };
		Row* row;

		row = &rule_table.add_row();
		row->cell(0).append("name");
		row->cell(1).append(_name);
		row->cell(2).append(id());

		row = &rule_table.add_row();
		row->cell(0).append("status");
		row->cell(1).append(status() == RuleStatus::ENABLED ? "enabled" : "disabled");
		row->cell(2).append(status() == RuleStatus::ENABLED ? "1": "0");

		row = &rule_table.add_row();
		row->cell(0).append("action");
		row->cell(1).append(action() == RuleAction::ALLOW ? "allow" : "deny");
		row->cell(2).append(action() == RuleAction::ALLOW? "1" : "0");

		row = &rule_table.add_row();
		row->cell(0).append("src.zone");
		predicate().src_zones().write_to_cell(row->cell(1), MnodeInfoType::NAME);
		predicate().src_zones().write_to_cell(row->cell(2), MnodeInfoType::VALUE);

		row = &rule_table.add_row();
		row->cell(0).append("dst.zone");
		predicate().dst_zones().write_to_cell(row->cell(1), MnodeInfoType::NAME);
		predicate().dst_zones().write_to_cell(row->cell(2), MnodeInfoType::VALUE);

		row = &rule_table.add_row();
		row->cell(0).append(predicate().negate_src_addresses() ? "!src.addr" : "src.addr");
		predicate().src_addresses().write_to_cell(row->cell(1), MnodeInfoType::NAME);
		predicate().src_addresses().write_to_cell(row->cell(2), MnodeInfoType::VALUE);

		row = &rule_table.add_row();
		row->cell(0).append(predicate().negate_dst_addresses() ? "!dst.addr" : "dst.addr");
		predicate().dst_addresses().write_to_cell(row->cell(1), MnodeInfoType::NAME);
		predicate().dst_addresses().write_to_cell(row->cell(2), MnodeInfoType::VALUE);

		row = &rule_table.add_row();
		row->cell(0).append("services");
		predicate().services().write_to_cell(row->cell(1), MnodeInfoType::NAME);
		predicate().services().write_to_cell(row->cell(2), MnodeInfoType::VALUE);

		if (options.contains(RuleOutputOption::APPLICATION_NAME)) {
			row = &rule_table.add_row();
			row->cell(0).append("applications");
			predicate().applications().write_to_cell(row->cell(1), MnodeInfoType::NAME);
			predicate().applications().write_to_cell(row->cell(2), MnodeInfoType::VALUE);
		}

		if (options.contains(RuleOutputOption::USER_NAME)) {
			row = &rule_table.add_row();
			row->cell(0).append("users");
			predicate().users().write_to_cell(row->cell(1), MnodeInfoType::NAME);
			predicate().users().write_to_cell(row->cell(2), MnodeInfoType::VALUE);
		}

		if (options.contains(RuleOutputOption::URL)) {
			row = &rule_table.add_row();
			row->cell(0).append("urls");
			predicate().urls().write_to_cell(row->cell(1), MnodeInfoType::NAME);
			predicate().urls().write_to_cell(row->cell(2), MnodeInfoType::VALUE);
		}

		return rule_table;
	}


	bool Rule::is_default_app_svc() const
	{
		return predicate().services().is_app_services();
	}


	bool Rule::is_deny_all() const noexcept
	{
		return action() == RuleAction::DENY && predicate().is_any();
	}


	void Rule::set_rule_status(RuleStatus status)
	{
		_status = status;
	}

}
