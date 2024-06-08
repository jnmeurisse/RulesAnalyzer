#pragma once

#include <algorithm>
#include <cassert>
#include <functional>
#include <list>
#include <set>
#include <vector>

#include "mnode.h"
#include "table.h"
#include "tools/strutil.h"


namespace fwm {

	// forward declaration of Group class
	template<typename T>
	class Group;

	/** A sortable list of const *T.
	*
	* A list that contains reference to const T and that can be sorted by
	* T.name() when calling sort_by_name.
	*/
	template<typename T>
	class sortable_list : public std::list<const T*>
	{
	public:
		/** default constructor.
		*/
		sortable_list() = default;

		/** default move constructor.
		*/
		sortable_list(sortable_list<T>&&) = default;

		/** default copy constructor.
		*/
		sortable_list(const sortable_list<T>&) = default;

		/** Sort the list by names.
		*
		* Sort is case insensitive.
		*/
		sortable_list<T> sort_by_name()
		{
			sort([](const T* object1, const T* object2) -> bool
			{
				return object1->compare_name(*object2) == -1;
			});

			return *this;
		}
	};


	/**
	* Represents a group hierarchy of items of pointer to type T.
	*/
	template<typename T>
	class Group : public NamedMnode {
	public:
		// A member of the group can be an item or another group
		struct Member {
			const bool is_group;
			const union {
				const T* item;
				const Group<T>* group;
			};

			Member(const T* item) :
				is_group{ false },
				item{ item }
			{}

			Member(const Group<T>* group) :
				is_group{ true },
				group{ group }
			{}
		};

		/* Adds an item to this group.
		*/
		void add_member(const T* item);

		/* Adds a sub-group to this group.
		*/
		void add_member(const Group<T>* group);

		/* Returns true if the given item is a child of this group or a sub-group.
		*/
		bool contains(const T* item) const;

		/* Returns true if the given group is a child of this group or a sub-group.
		*/
		bool contains(const Group<T>* group) const;

		/* Returns the name of direct members.
		*/
		std::vector<std::string> names() const;

		/* Returns all unique items in this group and sub-groups.
		*/
		sortable_list<T> items() const;

		/* Parses this group.
		*/
		void parse(std::function<void(const T*)> parser_callback) const;

		/* Appends all items in this group to the given table cell.
		*/
		virtual void output(Cell& cell, MnodeInfoType info_type) const;

		/* Returns true if this group is empty, false otherwise.
		*  This group is considered empty if it does not contain any items.
		*/
		bool empty() const;

		/* Returns the member names as a string.
		*/
		virtual std::string to_string() const override;

		/* Creates the binary decision diagram.
		*/
		virtual bdd make_bdd() const override;

		/* Returns the number of items in this group and subgroups.
		*/
		size_t size() const;

	protected:
		/* Allocates an empty group.
		*/
		Group(const std::string& name);

		/* Allocates and initialize a group with a unique member.
		*/
		Group(const std::string& name, const T* item);

	private:
		// All members in this group
		std::list<Member> _members;

		// Sets of direct members
		std::set<const T*> _items;
		std::set<const Group<T>*> _groups;
	};


	template<typename T>
	inline Group<T>::Group(const std::string& name) :
		NamedMnode(name, ModelOptions::empty())
	{
	}


	template<typename T>
	inline Group<T>::Group(const std::string& name, const T* item) :
		Group(name)
	{
		add_member(item);
	}


	template<typename T>
	inline std::string Group<T>::to_string() const
	{
		return strings_join(names(), ",", false);
	}


	template<typename T>
	inline bdd Group<T>::make_bdd() const
	{
		bdd condition{ bddfalse };

		for (const Member& member : _members) {
			if (member.is_group)
				condition = condition | member.group->make_bdd();
			else
				condition = condition | member.item->make_bdd();
		}

		return condition;
	}


	template<typename T>
	inline size_t Group<T>::size() const
	{
		size_t size = 0;

		for (const Member& member : _members) {
			if (member.is_group)
				size += member.group->size();
			else
				size += 1;
		}

		return size;
	}


	template<typename T>
	inline void Group<T>::add_member(const T* item)
	{
		assert(item != nullptr);

		if (_items.insert(item).second)
			_members.push_back(Member{ item });
	}


	template<typename T>
	inline void Group<T>::add_member(const Group<T>* group)
	{
		assert(group != nullptr);

		if (_groups.insert(group).second)
			_members.push_back(Member{ group });
	}


	template<typename T>
	inline bool Group<T>::contains(const T* item) const
	{
		bool found = _items.find(item) != _items.end();

		if (!found) {
			for (auto it = _members.cbegin(); !found && it != _members.end(); it++) {
				if (it->is_group)
					found = it->group->contains(item);
			}
		}
		return found;
	}


	template<typename T>
	inline bool Group<T>::contains(const Group<T>* group) const
	{
		bool found = _groups.find(group) != _groups.end();

		if (!found) {
			for (auto it = _members.cbegin(); !found && it != _members.end(); it++) {
				if (it->is_group)
					found = it->group->contains(group);
			}
		}

		return found;
	}


	template<typename T>
	inline std::vector<std::string> Group<T>::names() const
	{
		std::vector<std::string> names;

		for (const Member& member : _members) {
			if (member.is_group)
				names.push_back(member.group->name());
			else
				names.push_back(member.item->name());
		}
		
		return names;
	}


	template<typename T>
	inline sortable_list<T> Group<T>::items() const
	{
		sortable_list<T> items;
		std::set<const T*> item_set;

		auto resolver_callback = [&](const T* item) -> void{
			if (item_set.insert(item).second)
				items.push_back(item);
		};

		parse(resolver_callback);

		return items;
	}


	template<typename T>
	inline void Group<T>::parse(std::function<void(const T*)> parser_callback) const
	{
		for (const Member& member : _members) {
			if (member.is_group)
				member.group->parse(parser_callback);
			else
				parser_callback(member.item);
		}
	}

	template<typename T>
	inline void Group<T>::output(Cell& cell, MnodeInfoType info_type) const
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
