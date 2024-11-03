/*!
* This file is part of RulesAnalyzer
*
* Copyright (C) 2024 Jean-Noel Meurisse
* SPDX-License-Identifier: GPL-3.0-only
*
*/
#pragma once
#include "global.h"

#include <algorithm>
#include <list>
#include <set>
#include <stack>
#include <string>

#include "objectdictionary.h"


namespace fos {

	template <typename O, typename P>
	class GenericResolver
	{
	public:
		struct PoolMember {
			bool is_pool;
			union {
				const O* object;
				const P* pool;
			};

			PoolMember(const P* group) :
				is_pool{ true },
				pool{ group }
			{}

			PoolMember(const O* object) :
				is_pool{ false },
				object{ object }
			{}
		};

		using PoolMembers = std::list<PoolMember>;

		GenericResolver(const ObjectDictionary<O>& objects, const ObjectDictionary<P>& groups) :
			_objects{ objects },
			_pools{ groups }
		{
		}

		PoolMembers resolve(const P* pool, std::list<std::string>& unresolved) const
		{
			// The expanded list of members
			PoolMembers expanded_list;

			// A member in the pool hierarchy.
			struct member {
				const P* current_pool;
				int member_index;
			};

			// A chain of nodes in the hierarchy.
			std::stack<member> chain;

			// The set of visited nodes in the hierarchy.  The same
			// information is available in the chain of nodes however
			// it is not possible to search in a stack.
			std::set<const P*> visited;

			// Initialize the chain stack.
			chain.push(member{ pool, 0 });

			// Traverse the pool tree
			while (!chain.empty()) {
				member& top = chain.top();

				if (top.member_index < top.current_pool->members().size()) {
					// next member at the same level
					const std::string& node_name = top.current_pool->members()[top.member_index];
					top.member_index++;

					const O* object{ _objects.get(node_name) };
					if (object) {
						expanded_list.push_back(PoolMember(object));
					}
					else {
						const P* pool = _pools.get(node_name);
						if (pool) {
							// detect loops in nested pools
							if (!visited.insert(pool).second)
								throw std::runtime_error("pool: " + _pools.name() + " loop in pool '" + node_name + "'");

							// move down in the tree
							chain.push(member{ pool, 0 });
						}
						else {
							// not found....
							unresolved.push_back(node_name);
						}
					}
				}
				else {
					// move up in the tree
					expanded_list.push_back(PoolMember(top.current_pool));
					visited.erase(top.current_pool);
					chain.pop();
				}
			}

			return expanded_list;
		}

	private:
		const ObjectDictionary<O>& _objects;
		const ObjectDictionary<P>& _pools;
	};

}
