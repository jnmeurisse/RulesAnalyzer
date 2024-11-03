/*!
* This file is part of RulesAnalyzer
*
* Copyright (C) 2024 Jean-Noel Meurisse
* SPDX-License-Identifier: GPL-3.0-only
*
*/
#pragma once
#include "global.h"

#include <memory>
#include <string>

#include "model/mnode.h"
#include "model/group.h"
#include "model/protocol.h"
#include "model/ports.h"


namespace fwm {

	class Service abstract : public NamedMnode
	{
	public:
		/**
		 * no copy constructor for this abstract class
		 */
		Service(const Service& other) = delete;

		/**
		 * Creates a service from a service definition.
		 *
		 * @param name The name of this service.
		 * @package proto_port The protocol and port definition.
		 *
		 * @return a Service
		 * @throws ServiceError if the protocol and port has an invalid syntax.
		 *
		*/
		static const Service* create(const std::string& name, const std::string& proto_port);

		/**
		 * Creates an any service.
		 *
		 * @return A service representing all protocols and ports.
		*/
		static const Service* any();

		/**
		 * Creates a clone of this service.
		 *
		 * @return a service
		*/
		virtual const Service* clone() const = 0;

		/**
		 * Creates a binary decision diagram from this service.
		*/
		virtual bdd make_bdd() const override;

		/**
		 * Returns the protocol (tcp/upd/icmp).
		*/
		inline const Protocol& protocol() const noexcept { return *_protocol; }

		/**
		 * Returns the port(s).
		*/
		inline const Ports& ports() const noexcept { return *_ports; }

		/**
		 * Returns the service as a string.
		*/
		virtual std::string to_string() const override;

	protected:
		/*
		 * Allocates a service
		*/
		Service(const std::string& name, const Protocol* protocol, const Ports* ports);

	private:
		// The protocol.
		const std::unique_ptr<const Protocol> _protocol;

		// The ports.
		const std::unique_ptr<const Ports> _ports;
	};


	/**
	 * An UserList represents a list of Users.
	*/
	class ServiceList : public NamedMnodeList<Service>
	{
	public:
		ServiceList() = default;

		/**
		 *  Sort the list by service type and then by name.
		*/
		ServiceList& sort() override;
	};

	using ServiceListPtr = std::unique_ptr<ServiceList>;


	/*
	 * A ServiceGroup is a group of services.
	 *
	*/
	class ServiceGroup : public Group<Service>
	{
	public:
		using Group<Service>::Group;

		/**
		 * Clones this group.
		*/
		virtual ServiceGroup* clone() const override;

		/**
		 * Returns true when a service group contains the default services of an application.
		*/
		virtual bool is_app_services() const;
	};

	using ServiceGroupPtr = std::unique_ptr<ServiceGroup>;


	/*
	 * A AnyServiceGroup is a group of source services containing only a any service.
	 *
	*/
	class AnyServiceGroup final : public ServiceGroup
	{
	public:
		AnyServiceGroup();
		virtual ~AnyServiceGroup();

		/**
		 * Clones this group.
		*/
		virtual ServiceGroup* clone() const override;

		/**
		 * Returns a bddtrue decision diagram.
		*/
		virtual bdd make_bdd() const override;
	};


	class TcpService final : public Service {
	public:
		TcpService(const std::string& name, uint16_t lower_port, uint16_t upper_port);
		virtual const Service* clone() const override;

	private:
		TcpService(const std::string& name, const Ports* ports);
	};


	class UdpService final : public Service {
	public:
		UdpService(const std::string& name, uint16_t lower_port, uint16_t upper_port);
		virtual const Service* clone() const override;

	private:
		UdpService(const std::string& name, const Ports* ports);
	};


	class IcmpService final : public Service {
	public:
		IcmpService(const std::string& name, uint16_t type);
		IcmpService(const std::string& name, uint16_t lower_port, uint16_t upper_port);

		virtual const Service* clone() const override;

	private:
		IcmpService(const std::string& name, const Ports* ports);
	};

}
