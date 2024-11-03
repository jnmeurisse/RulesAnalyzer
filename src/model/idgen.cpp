#include "model/idgen.h"

#include <stdexcept>
#include <utility>


namespace fwm {

	IdGenerator::IdGenerator(const std::string& name, uint32_t ubound) :
		_name{ name },
		_ubound{ ubound },
		_next_id{ 1 },
		_id_table{}
	{}

	uint32_t IdGenerator::get_id(const std::string& str)
	{
		auto it = _id_table.find(str);

		if (it == _id_table.end()) {
			// this string is not yet registered, allocate a new id
			const uint32_t id = _next_id++;
			if (id >= _ubound)
				throw std::runtime_error(_name + " id generator overflow");

			// associate the string with the new generated id
			_id_table.insert(std::make_pair(str, id));
			return id;
		}
		else {
			return it->second;
		}
	}

}
