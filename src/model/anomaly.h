/*!
* This file is part of RulesAnalyzer
*
* Copyright (C) 2024 Jean-Noel Meurisse
* SPDX-License-Identifier: GPL-3.0-only
*
*/
#pragma once
#include "global.h"

#include <list>
#include <memory>
#include <string>

#include "model/rule.h"
#include "model/rulelist.h"


namespace fwm {

	enum class RuleAnomalyScope {
		FullyMaskedRule,
		PartiallyMaskedRule
	};

	std::string to_string(RuleAnomalyScope anomaly_scope);

	enum class RuleAnomalyLevel {
		Warning,
		Error
	};

	std::string to_string(RuleAnomalyLevel anomaly_level);

	enum class RuleAnomalyType {
		Shadowing,
		Redundancy,
		Correlation,
		Generalization,
		RedundancyOrCorrelation
	};


	class RuleAnomalyDetails abstract {
	public:
		virtual ~RuleAnomalyDetails() {};
		virtual void output(Cell& cell, const Rule& rule) const = 0;

		virtual RuleAnomalyScope anomaly_scope() const noexcept { return _anomaly_scope; }
		inline RuleAnomalyLevel anomaly_level() const noexcept { return _anomaly_level; }
		inline RuleAnomalyType anomaly_type() const noexcept { return _anomaly_type; }

	protected:
		RuleAnomalyDetails(RuleAnomalyScope anomaly_scope, RuleAnomalyLevel anomaly_level, RuleAnomalyType anomaly_type);

	private:
		const RuleAnomalyScope _anomaly_scope;
		const RuleAnomalyLevel _anomaly_level;
		const RuleAnomalyType _anomaly_type;
	};


	class RuleAnomalyShadowed : public RuleAnomalyDetails {
	public:
		explicit RuleAnomalyShadowed(const RuleList& shadowing_rules);
		virtual void output(Cell& cell, const Rule& rule) const override;

	private:
		const RuleList _shadowing_rules;
	};


	class RuleAnomalyFullRedundant : public RuleAnomalyDetails {
	public:
		RuleAnomalyFullRedundant(const RuleList& redundant_rules);
		virtual void output(Cell& cell, const Rule& rule) const override;

	private:
		const RuleList _redundant_rules;
	};


	class RuleAnomalyPartialRedundant : public RuleAnomalyDetails {
	public:
		RuleAnomalyPartialRedundant(const RuleList& redundant_rules);
		virtual void output(Cell& cell, const Rule& rule) const override;

	private:
		const RuleList _redundant_rules;
	};


	class RuleAnomalyCorrelated : public RuleAnomalyDetails {
	public:
		RuleAnomalyCorrelated(const RuleList& correlated_rules);
		virtual void output(Cell& cell, const Rule& rule) const override;

	private:
		const RuleList _correlated_rules;
	};


	class RuleAnomalyRedundantOrCorrelated : public RuleAnomalyDetails {
	public:
		RuleAnomalyRedundantOrCorrelated(const RuleList& redundant_rules, const RuleList& correlated_rules);
		virtual void output(Cell& cell, const Rule& rule) const override;

	private:
		const RuleList _redundant_rules;
		const RuleList _correlated_rules;
	};


	class RuleAnomalyGeneralization : public RuleAnomalyDetails {
	public:
		RuleAnomalyGeneralization(const RuleList& matching_rules);
		virtual void output(Cell& cell, const Rule& rule) const override;

	private:
		const RuleList _matching_rules;
	};


	class RuleAnomaly
	{
	public:
		RuleAnomaly(const Rule& rule, const RuleAnomalyDetails* anomaly_details);
		void output(Cell& cell) const;

		inline const Rule& rule() const noexcept { return _rule; }
		inline const RuleAnomalyDetails& details() const noexcept { return *_details; }

	private:
		const Rule& _rule;
		std::unique_ptr<const RuleAnomalyDetails> _details;
	};

	using RuleAnomalyPtr = std::unique_ptr<RuleAnomaly>;


	class RuleAnomalies : public std::list<RuleAnomalyPtr>
	{
	public:
		RuleAnomalies();
		Table output_anomalies(bool show_rule_name) const;

		bool missing_deny_all;
	};

}
