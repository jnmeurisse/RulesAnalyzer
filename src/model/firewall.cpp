/*!
* This file is part of RulesAnalyzer
*
* Copyright (C) 2024 Jean-Noel Meurisse
* SPDX-License-Identifier: GPL-3.0-only
*
*/
#include "model/firewall.h"

#include <memory>
#include "model/network.h"


namespace fwm {

	Firewall::Firewall(const std::string& name, Network& network) :
		_name{ name },
		_network{ network },
		_rules{},
		_rule_list{},
		_rule_ids{}
	{
	}


	Firewall::Firewall(const Firewall& other, const std::string& name) :
		Firewall(name, other._network)
	{
		for (const auto& rule : other._rules)
			add_rule(new Rule(*rule));
	}


	void Firewall::clear()
	{
		_rule_list.clear();
		_rules.clear();
		_rule_ids.clear();
	}


	const RuleList Firewall::acl() const noexcept
	{
		return _rule_list.filter(RuleStatus::ENABLED);
	}


	void Firewall::add_rule(Rule* rule)
	{
		_rules.push_back(std::unique_ptr<Rule>(rule));

		Rule* tmp{ _rules.back().get() };
		_rule_list.push_back(tmp);
		_rule_ids[tmp->id()] = tmp;
	}


	Table Firewall::info() const
	{
		Table table{ { "info", "value" } };

		Row *row;

		row = &table.add_row();
		row->cell(0).append("Name");
		row->cell(1).append(name());

		row = &table.add_row();
		row->cell(0).append("Total Rules");
		row->cell(1).append(_rule_list.size());
		row = &table.add_row();
		row->cell(0).append("Disabled Rules");
		row->cell(1).append(_rule_list.filter(RuleStatus::DISABLED).size());
		row = &table.add_row();
		row->cell(0).append("Allow Rules");
		row->cell(1).append(_rule_list.filter(RuleAction::ALLOW).size());
		row = &table.add_row();
		row->cell(0).append("Deny Rules");
		row->cell(1).append(_rule_list.filter(RuleAction::DENY).size());
		row = &table.add_row();
		row->cell(0).append("Source zones");
		row->cell(1).append(acl().all_src_zones()->size());
		row = &table.add_row();
		row->cell(0).append("Destination zones");
		row->cell(1).append(acl().all_dst_zones()->size());
		row = &table.add_row();
		row->cell(0).append("Address objects");
		row->cell(1).append(acl().all_addresses()->size());
		row = &table.add_row();
		row->cell(0).append("Service objects");
		row->cell(1).append(acl().all_services()->size());
		row = &table.add_row();
		row->cell(0).append("Application objects");
		row->cell(1).append(acl().all_applications()->size());
		row = &table.add_row();
		row->cell(0).append("User objects");
		row->cell(1).append(acl().all_users()->size());
		row = &table.add_row();
		row->cell(0).append("Url objects");
		row->cell(1).append(acl().all_urls()->size());

		return table;
	}


	Rule* Firewall::get_rule(int rule_id)
	{
		auto it = _rule_ids.find(rule_id);
		if (it != _rule_ids.end())
			return it->second;
		else
			return nullptr;
	}


	const Rule* Firewall::get_rule(int rule_id) const
	{
		auto it = _rule_ids.find(rule_id);
		if (it != _rule_ids.cend())
			return it->second;
		else
			return nullptr;
	}


	RuleOutputOptions Firewall::make_output_options(bool show_object_name) const
	{
		RuleOutputOptions options;

		if (acl().have_names())
			options.add(RuleOutputOption::RULE_NAME);
		if (acl().have_negate())
			options.add(RuleOutputOption::NEGATE_ADDRESS);
		if (show_object_name) {
			options.add(RuleOutputOption::ADDRESS_NAME);
			options.add(RuleOutputOption::SERVICE_NAME);
		}
		if (_network.model_options.contains(ModelOption::Application))
			options.add(RuleOutputOption::APPLICATION_NAME);
		if (_network.model_options.contains(ModelOption::User))
			options.add(RuleOutputOption::USER_NAME);
		if (_network.model_options.contains(ModelOption::Url))
			options.add(RuleOutputOption::URL);

		return options;
	}


	Table Firewall::create_rules_table(const RuleOutputOptions& output_options) const
	{
		return acl().create_table(output_options);
	}

}
