#pragma once

#include <algorithm>
#include <cassert>
#include <functional>
#include <set>
#include <vector>
#include <type_traits>

#include "model/mnode.h"
#include "model/table.h"
#include "tools/strutil.h"


namespace fwm {

	// forward declaration of Group class
	template<typename T>
	class Group;


	/**
	 * Represents a group hierarchy of items of shared pointer to type T.
	*/
	template<typename T>
	class Group : public NamedMnode {
		static_assert(std::is_base_of<NamedMnode, T>::value, "invalid Group<T> template usage");
	
	public:
		using fn_parser_cb = std::function<void(const T*)>;

	private:
		// A member of a group can be an item or another group
		class Member abstract {
		public:
			virtual bdd make_bdd() const = 0;
			virtual bool contains(const T* item) const = 0;
			virtual void parse(fn_parser_cb& parser_callback) const = 0;
			virtual size_t size() const = 0;
			virtual std::list<std::string> names() const = 0;
		};

		class ItemMember : public Member {
		public:
			ItemMember(const std::shared_ptr<const T> item) : _item(item) {}

			virtual bdd make_bdd() const { return _item->make_bdd(); }

			virtual bool contains(const T* item) const override { return item == _item.get(); }

			virtual void parse(fn_parser_cb& parser_callback) const override
			{
				parser_callback(_item->get());
			}

			virtual size_t size() const override { return 1; }

			virtual std::list<std::string> names() const override
			{
				return std::list<std::string>(_item->name());
			}

		private:
			const std::shared_ptr<const T> _item;
		};

		class GroupMember : public Member {
		public:
			GroupMember(const std::shared_ptr<Group<T>> group) : _group(group) {}

			virtual bdd make_bdd() const { return _group->make_bdd(); }

			virtual bool contains(const T* item) const override { return _group->contains(item); }

			virtual void parse(fn_parser_cb& parser_callback) const override
			{
				_group->parse(parser_callback);
			}

			virtual size_t size() const override
			{
				return _group->size();
			}

			virtual std::list<std::string> names() const override
			{
				return _group->names();
			}

		private:
			const std::shared_ptr<const Group<T>> _group;
		};

	public:
		Group() = delete;

		/**
		 * Allocates an empty group.
		*/
		Group(const std::string& name);

		/**
		 * Allocates and initializes a group with a unique member.
		*/
		Group(const std::string& name, const std::shared_ptr<const T> item);
		Group(const std::string& name, const T* item);

		/**
		 * Virtual destructor.
		*/
		virtual ~Group() {}

		/**
		 * Adds an item to this group.
		*/
		void add_member(const std::shared_ptr<const T> item);
		void add_member(const T* item);

		/**
		 * Adds a sub-group to this group.
		*/
		void add_member(const std::shared_ptr<const Group<T>> group);
		void add_member(const Group<T>* group);

		/**
		 * Returns true if the given item is a child of this group or a sub-group.
		*/
		bool contains(const T* item) const;

		/**
		 * Returns true if the given group is a child of this group or a sub-group.
		*/
		bool contains(const Group<T>* group) const;

		/**
		 * Returns all unique names in this group and sub-groups.
		*/
		std::vector<std::string> names() const;

		/**
		 * Returns all unique items in this group and sub-groups.
		*/
		std::vector<const T*> items() const;

		/**
		 * Parses this group.
		*/
		void parse(fn_parser_cb& parser_callback) const;

		/**
		 * Appends all items in this group to the given table cell.
		*/
		virtual void write_to_cell(Cell& cell, MnodeInfoType info_type) const;

		/**
		 * Returns true if this group is empty, false otherwise.
		*  This group is considered empty if it does not contain any items.
		*/
		bool empty() const;

		/**
		 * Returns the member names as a string.
		*/
		virtual std::string to_string() const override;

		/**
		 * Creates the binary decision diagram.
		*/
		virtual bdd make_bdd() const override;

		/**
		 * Returns the number of items in this group and subgroups.
		*/
		size_t size() const;

	private:
		// All members in this group
		std::list<std::unique_ptr<const Member>> _members;
	};


	template<typename T>
	inline Group<T>::Group(const std::string& name) :
		NamedMnode(name, ModelOptions::empty())
	{
	}


	template<typename T>
	inline Group<T>::Group(const std::string& name, const std::shared_ptr<const T> item) :
		Group(name)
	{
		add_member(item);
	}


	template<typename T>
	inline Group<T>::Group(const std::string& name, const T* item) :
		Group(name, std::shared_ptr<const T>(item))
	{
	}


	template<typename T>
	inline std::string Group<T>::to_string() const
	{
		return rat::strings_join(names(), ",", false);
	}


	template<typename T>
	inline bdd Group<T>::make_bdd() const
	{
		bdd condition{ bddfalse };

		for (const auto& member : _members)
			condition = condition | member->make_bdd();

		return condition;
	}


	template<typename T>
	inline size_t Group<T>::size() const
	{
		size_t size = 0;

		for (const auto& member : _members)
			size += member->size();

		return size;
	}


	template<typename T>
	inline void Group<T>::add_member(const std::shared_ptr<const T> item)
	{
		assert(item.get() != nullptr);

		//TOFIX
		//if (_items.insert(item.get()).second)
		//	_members.emplace_back(item);
	}


	template<typename T>
	inline void Group<T>::add_member(const T* item)
	{
		add_member(std::shared_ptr<const T>(item));
	}


	template<typename T>
	inline void Group<T>::add_member(const Group<T>* group)
	{
		assert(group != nullptr);

		//TOFIX
		//if (_groups.insert(group).second)
		//	_members.emplace_back(group);
	}


	template<typename T>
	inline bool Group<T>::contains(const T* item) const
	{
		bool found = false;
		//bool found = _members.find(item) != _members.end();

		//if (!found) {
			//for (auto it = _members.cbegin(); !found && it != _members.end(); it++) {
			//	if (it->is_group)
			//		found = it->group->contains(item);
			//}
		//}

		return found;
	}


	template<typename T>
	inline bool Group<T>::contains(const Group<T>* group) const
	{
		bool found = false;
		//bool found = _groups.find(group) != _groups.end();

		//if (!found) {
			//for (auto it = _members.cbegin(); !found && it != _members.end(); it++) {
			//	if (it->is_group)
			//		found = it->group->contains(group);
			//}
		//}

		return found;
	}


	template<typename T>
	inline std::vector<std::string> Group<T>::names() const
	{
		std::set<std::string> item_set;

		fn_parser_cb resolver_callback = [&](const T* item) -> void {
			item_set.insert(item->name());
			};

		parse(resolver_callback);

		return std::vector<std::string>(item_set.begin(), item_set.end());
	}


	template<typename T>
	inline std::vector<const T*> Group<T>::items() const
	{
		std::set<const T*> item_set;

		fn_parser_cb resolver_callback = [&](const T* item) -> void {
			item_set.insert(item);
		};

		parse(resolver_callback);

		return std::vector<const T*>(item_set.begin(), item_set.end());
	}


	template<typename T>
	inline void Group<T>::parse(fn_parser_cb& parser_callback) const
	{
		for (const auto& member : _members)
			member->parse(parser_callback);
	}


	template<typename T>
	inline void Group<T>::write_to_cell(Cell& cell, MnodeInfoType info_type) const
	{
		if (info_type == MnodeInfoType::NAME) {
			for (const T* item : items()) {
				cell.append_nl(item->name());
			}
		}
		else {
			for (const T* item : items()) {
				cell.append_nl(item->to_string());
			}
		}
	}


	template<typename T>
	inline bool Group<T>::empty() const
	{
		return items().size() == 0;
	}

}
