/*!
* This file is part of RulesAnalyzer
*
* Copyright (C) 2024 Jean-Noel Meurisse
* SPDX-License-Identifier: GPL-3.0-only
*
*/
#include "model/anomaly.h"
#include "fmt/core.h"

namespace fwm {

	std::string to_string(RuleAnomalyScope anomaly_scope)
	{
		std::string anomaly_type_str;

		switch (anomaly_scope) {
		case RuleAnomalyScope::FullyMaskedRule:
			anomaly_type_str = "Fully masked";
			break;

		case RuleAnomalyScope::PartiallyMaskedRule:
			anomaly_type_str = "Partially masked";
			break;

		default:
			anomaly_type_str = "not available";
			break;
		}

		return anomaly_type_str;
	}


	std::string to_string(RuleAnomalyLevel anomaly_level)
	{
		std::string anomaly_level_str;

		switch (anomaly_level)
		{
		case RuleAnomalyLevel::Warning:
			anomaly_level_str = "warning";
			break;

		case RuleAnomalyLevel::Error:
			anomaly_level_str = "error";
			break;

		default:
			anomaly_level_str = "not available";
			break;
		}

		return anomaly_level_str;
	}



	RuleAnomalyDetails::RuleAnomalyDetails(
		RuleAnomalyScope anomaly_scope,
		RuleAnomalyLevel anomaly_level,
		RuleAnomalyType anomaly_type
	):
		_anomaly_scope{ anomaly_scope },
		_anomaly_level{ anomaly_level },
		_anomaly_type{ anomaly_type }
	{
	}


	RuleAnomalyShadowed::RuleAnomalyShadowed(
		const RuleList& shadowing_rules
	) :
		RuleAnomalyDetails(RuleAnomalyScope::FullyMaskedRule, RuleAnomalyLevel::Error, RuleAnomalyType::Shadowing),
		_shadowing_rules{ shadowing_rules }
	{
	}


	void RuleAnomalyShadowed::output(Cell& cell, const Rule& rule) const
	{
		cell
			.append_nl("Shadowed rule")
			.append(fmt::format(
				"packets are {} by {} ",
				rule.action() == RuleAction::ALLOW ? "denied" : "accepted",
				_shadowing_rules.size() > 1 ? "combined rules" : "rule"))
			.append(_shadowing_rules.id_list());
	}


	RuleAnomalyFullRedundant::RuleAnomalyFullRedundant(
		const RuleList& redundant_rules
	) :
		RuleAnomalyDetails(RuleAnomalyScope::FullyMaskedRule, RuleAnomalyLevel::Error, RuleAnomalyType::Redundancy),
		_redundant_rules{ redundant_rules }
	{
	}


	void RuleAnomalyFullRedundant::output(Cell& cell, const Rule& rule) const
	{
		cell
			.append_nl("Redundant rule")
			.append(fmt::format(
				"packets are {} by {} ",
				rule.action() == RuleAction::ALLOW ? "accepted" : "denied",
				_redundant_rules.size() > 1 ? "combined rules" : "rule"))
			.append(_redundant_rules.id_list());
	}


	RuleAnomalyPartialRedundant::RuleAnomalyPartialRedundant(
		const RuleList& redundant_rules
	) :
		RuleAnomalyDetails(RuleAnomalyScope::PartiallyMaskedRule, RuleAnomalyLevel::Error, RuleAnomalyType::Redundancy),
		_redundant_rules{ redundant_rules }
	{
	}


	void RuleAnomalyPartialRedundant::output(Cell& cell, const Rule& rule) const
	{
		cell
			.append("Rule")
			.append(_redundant_rules.size() > 1 ? "s " : " ")
			.append(_redundant_rules.id_list())
			.append_nl()
			.append(fmt::format(
				"{} redundant with this rule",
				_redundant_rules.size() > 1 ? "are all" : "is")
			);
	}


	RuleAnomalyCorrelated::RuleAnomalyCorrelated(
		const RuleList& correlated_rules
	) :
		RuleAnomalyDetails(RuleAnomalyScope::PartiallyMaskedRule, RuleAnomalyLevel::Warning, RuleAnomalyType::Correlation),
		_correlated_rules{ correlated_rules }
	{
	}


	void RuleAnomalyCorrelated::output(Cell& cell, const Rule& rule) const
	{
		cell
			.append_nl("Correlated rule")
			.append(fmt::format(
				"part of packets are {} by {} ",
				rule.action() == RuleAction::ALLOW ? "denied" : "accepted",
				_correlated_rules.size() > 1 ? "combined rules" : "rule"))
			.append(_correlated_rules.id_list());
	}


	RuleAnomalyRedundantOrCorrelated::RuleAnomalyRedundantOrCorrelated(
		const RuleList& redundant_rules,
		const RuleList& correlated_rules
	) :
		RuleAnomalyDetails(RuleAnomalyScope::PartiallyMaskedRule, RuleAnomalyLevel::Error, RuleAnomalyType::RedundancyOrCorrelation),
		_redundant_rules{ redundant_rules },
		_correlated_rules{ correlated_rules }
	{
	}


	void RuleAnomalyRedundantOrCorrelated::output(Cell& cell, const Rule& rule) const
	{
		if (_correlated_rules.size() > 0)
			cell.append_nl("Redundant or correlated rule");
		else
			cell.append_nl("Redundant rule");

		cell
			.append(
				fmt::format(
					"part of packets are {} by {} ",
					rule.action() == RuleAction::ALLOW ? "allowed" : "denied",
					_redundant_rules.size() > 1 ? "combined rules" : "rule"
				))
			.append(_redundant_rules.id_list());

		if (_correlated_rules.size() > 0) {
			cell
				.append_nl()
				.append(
					fmt::format(
						"part of packets are {} by {} ",
						rule.action() == RuleAction::ALLOW ? "denied" : "allowed",
						_correlated_rules.size() > 1 ? "combined rules" : "rule"
					))
				.append(_correlated_rules.id_list());
		}
	}


	RuleAnomalyGeneralization::RuleAnomalyGeneralization(
		const RuleList& matching_rules
	) :
		RuleAnomalyDetails(RuleAnomalyScope::PartiallyMaskedRule, RuleAnomalyLevel::Warning, RuleAnomalyType::Generalization),
		_matching_rules{ matching_rules }
	{
	}


	void RuleAnomalyGeneralization::output(Cell& cell, const Rule& rule) const
	{
		cell
			.append("Generalization of rule")
			.append(_matching_rules.size() > 1 ? "s " : " ")
			.append(_matching_rules.id_list());
	}


	RuleAnomaly::RuleAnomaly(const Rule& rule, const RuleAnomalyDetails* anomaly_details) :
		_rule{ rule },
		_details{ anomaly_details }
	{
	}


	void RuleAnomaly::output(Cell& cell) const
	{
		_details->output(cell, _rule);
	}



	RuleAnomalies::RuleAnomalies() :
		std::list<RuleAnomalyPtr>(),
		missing_deny_all{false}
	{
	}


	Table RuleAnomalies::output_anomalies(bool show_rule_name) const
	{
		Table::Headers columns {
			"id",
			"name",
			"src.zone",
			"dst.zone",
			"anomaly",
			"level",
			"details"
		};

		Table::WrapPositions wrap_positions{
			0,
			0,
			0,
			0,
			0,
			0,
			40
		};

		if (!show_rule_name) {
			columns.erase(columns.begin() + 1);
			wrap_positions.erase(wrap_positions.begin() + 1);
		}

		Table anomaly_table{ columns, wrap_positions };
		for (auto& anomaly : *this) {
			Row& row = anomaly_table.add_row();
			int col_num = 0;

			row.cell(col_num++).append(anomaly->rule().id());
			if (show_rule_name)
				row.cell(col_num++).append(anomaly->rule().name());
			anomaly->rule().predicate().src_zones().write_to_cell(row.cell(col_num++), MnodeInfoType::NAME);
			anomaly->rule().predicate().dst_zones().write_to_cell(row.cell(col_num++), MnodeInfoType::NAME);

			const RuleAnomalyDetails& anomaly_details = anomaly->details();
			row.cell(col_num++).append(to_string(anomaly_details.anomaly_scope()));
			row.cell(col_num++).append(to_string(anomaly_details.anomaly_level()));
			anomaly->output(row.cell(col_num++));
		}

		return anomaly_table;
	}

}
