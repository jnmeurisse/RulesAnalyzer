#include <gtest/gtest.h>
#include <memory>

#include "model/address.h"
#include "model/zone.h"
#include "model/service.h"
#include "model/application.h"
#include "model/user.h"
#include "model/url.h"
#include "model/network.h"
#include "model/predicate.h"
#include "model/rule.h"
#include "model/firewall.h"
#include "model/analyzer.h"

using namespace fwm;

static const Predicate* create_predicate(fwm::Network& network,
	std::string src,
	std::string dst,
	std::string svc,
	bool negate_src = false,
	bool negate_dst = false)
{
	SrcZoneGroupPtr sz = std::make_unique<SrcZoneGroup>("src-z", network.get_src_zone("any"));
	SrcAddressGroupPtr sa = std::make_unique<SrcAddressGroup>("src-g", network.get_src_address(src));
	DstZoneGroupPtr dz = std::make_unique<DstZoneGroup>("dst-z", network.get_dst_zone("any"));
	DstAddressGroupPtr da = std::make_unique<DstAddressGroup>("dst-g", network.get_dst_address(dst));
	ServiceGroupPtr services = std::make_unique<ServiceGroup>("svc", network.get_service(svc));
	ApplicationGroupPtr applications = std::make_unique<ApplicationGroup>("app", network.get_application("any"));
	UserGroupPtr users = std::make_unique<UserGroup>("user", network.get_user("any"));
	UrlGroupPtr urls = std::make_unique<UrlGroup>("url", network.get_url("any"));

	PredicatePtr p = std::make_unique<const Predicate>(
		Sources{ sz.release(), sa.release(), negate_src },
		Destinations{ dz.release(), da.release(), negate_dst },
		services.release(),
		applications.release(),
		users.release(),
		urls.release());

	// Expect groups ownership is moved to the Predicate object.
	EXPECT_EQ(sa.get(), nullptr);
	EXPECT_EQ(sz.get(), nullptr);
	EXPECT_EQ(da.get(), nullptr);
	EXPECT_EQ(dz.get(), nullptr);
	EXPECT_EQ(services.get(), nullptr);
	EXPECT_EQ(applications.get(), nullptr);
	EXPECT_EQ(users.get(), nullptr);

	return p.release();
}


TEST(Analyzer6, any) {
	// Define network objects
	ModelConfig model_config;
	model_config.ip_model = IPAddressModel::IP6Model;
	model_config.strict_ip_parser = true;
	Network network(model_config);
	network.register_src_address("corporate_net", "::10.0.0.0/104");
	network.register_dst_address("corporate_net", "::10.0.0.0/104");


	// Configure the test firewall
	network.add(new Firewall("test", network));
	Firewall *firewall = network.get("test");

	{
		DstAddressGroupPtr dany = std::make_unique<DstAddressGroup>("dst-any", network.get_dst_address("any"));
		Analyzer analyzer(firewall->acl(), network.config().ip_model);
		RuleList anyrules = analyzer.check_any(*dany);
		EXPECT_EQ(anyrules.size(), 0);
	}

	firewall->add_rule(new Rule(
		"rule1",
		1,
		RuleStatus::ENABLED,
		RuleAction::ALLOW,
		create_predicate(network, "any", "corporate_net", "any"),
		*firewall)
	);

	firewall->add_rule(new Rule(
		"rule2",
		2,
		RuleStatus::ENABLED,
		RuleAction::ALLOW,
		create_predicate(network, "any", "any", "any"),
		*firewall)
	);

	{
		DstAddressGroupPtr dany = std::make_unique<DstAddressGroup>("dst-any", network.get_dst_address("any"));
		Analyzer analyzer(firewall->acl(), network.config().ip_model);
		RuleList anyrules = analyzer.check_any(*dany);
		EXPECT_EQ(anyrules.size(), 1);
	}


	{
		DstAddressGroupPtr dany = std::make_unique<DstAddressGroup>("dst-any", network.get_dst_address("corporate_net"));
		Analyzer analyzer(firewall->acl(), network.config().ip_model);
		RuleList anyrules = analyzer.check_any(*dany);
		EXPECT_EQ(anyrules.size(), 2 /* 10.0.0.8 is a subset of any */);
	}
}


TEST(Analyzer6, deny)
{
	// Define network objects
	ModelConfig model_config;
	model_config.ip_model = IPAddressModel::IP6Model;
	model_config.strict_ip_parser = true;
	Network network(model_config);
	network.register_src_address("corporate_net", "::10.0.0.0/104");
	network.register_dst_address("corporate_net", "::10.0.0.0/104");


	// Configure the test firewall
	network.add(new Firewall("test", network));
	Firewall *firewall = network.get("test");

	{
		DstAddressGroupPtr dany = std::make_unique<DstAddressGroup>("dst-any", network.get_dst_address("any"));
		Analyzer analyzer(firewall->acl(), network.config().ip_model);
		RuleList anyrules = analyzer.check_any(*dany);
		EXPECT_EQ(anyrules.size(), 0);
	}

	firewall->add_rule(new Rule(
		"rule1",
		1,
		RuleStatus::ENABLED,
		RuleAction::ALLOW,
		create_predicate(network, "any", "corporate_net", "any"),
		*firewall)
	);

	firewall->add_rule(new Rule(
		"rule2",
		2,
		RuleStatus::ENABLED,
		RuleAction::DENY,
		create_predicate(network, "any", "any", "any"),
		*firewall)
	);

	Analyzer analyzer(firewall->acl(), network.config().ip_model);
	RuleList denyrules = analyzer.check_deny();
	EXPECT_EQ(denyrules.size(), 1);
	ASSERT_EQ(denyrules.front()->id(), 2);
}



static bool interrupt_cb()
{
	return false;
}

TEST(Analyzer6, shadowing) {

	// Define network objects
	ModelConfig model_config;
	model_config.ip_model = IPAddressModel::IP6Model;
	model_config.strict_ip_parser = true;
	Network network(model_config);
	network.register_src_address("R_10.1.1.0/25", "::10.1.1.0/121");
	network.register_src_address("R_10.1.1.128/25", "::10.1.1.128/121");
	network.register_src_address("R_172.16.1.0/24", "::172.16.1.0/120");
	network.register_dst_address("R_192.168.1.0/24", "::192.168.1.0/120");
	network.register_service("http", "tcp/80");

	// Configure the test firewall
	network.add(new Firewall("test", network));
	Firewall *firewall = network.get("test");

	{
		Analyzer analyzer(firewall->acl(), network.config().ip_model);
		RuleAnomalies anomalies = analyzer.check_anomaly(interrupt_cb);
		EXPECT_EQ(anomalies.size(), 0);
	}

	// Add rules
	firewall->add_rule(new Rule(
		"rule1",
		1,
		RuleStatus::ENABLED,
		RuleAction::DENY,
		create_predicate(network, "R_10.1.1.0/25", "any", "any"),
		*firewall)
	);

	firewall->add_rule(new Rule(
		"rule2",
		2,
		RuleStatus::ENABLED,
		RuleAction::ALLOW,
		create_predicate(network, "R_10.1.1.0/25", "R_192.168.1.0/24", "any"),
		*firewall)
	);

	{
		Analyzer analyzer(firewall->acl(), network.config().ip_model);
		RuleAnomalies anomalies = analyzer.check_anomaly(interrupt_cb);
		EXPECT_EQ(anomalies.size(), 1);

		if (anomalies.size() == 1) {
			EXPECT_EQ(anomalies.front()->rule().id(), 2);
			EXPECT_EQ(anomalies.front()->details().anomaly_scope(), RuleAnomalyScope::FullyMaskedRule);
			EXPECT_EQ(anomalies.front()->details().anomaly_level(), RuleAnomalyLevel::Error);
			EXPECT_EQ(anomalies.front()->details().anomaly_type(),  RuleAnomalyType::Shadowing);
		}
	}
}
