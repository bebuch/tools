/// \file tools/parse_list.hpp
/// \author Benjamin Buch (benni.buch@gmail.com)
/// \brief Parse a separated list
///
/// Copyright (c) 2012-2015 Benjamin Buch (benni dot buch at gmail dot com)
///
/// Distributed under the Boost Software License, Version 1.0. (See accompanying
/// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
///
#ifndef _tools_parse_list_hpp_INCLUDED_
#define _tools_parse_list_hpp_INCLUDED_

#include "to_string.hpp"

#include <exception>


namespace tools{


	template < typename Container >
	Container parse_list(std::string const& list, char separator = ','){
		using value_type = typename Container::value_type;

		std::istringstream is(list);
		Container result;

		while(is){
			char sep;
			value_type value;
			is >> value;

			if (!is){
				throw std::runtime_error("parse_list (data: " + list + ") can not parse value. (Successfully parsed values: " + to_string(list, separator) + ")");
			}

			result.emplace_back(std::move(value));

			is >> sep;
			if (!is) break;

			if (sep != separator){
				throw std::runtime_error("parse_list (data: " + list + ") wrong separator '" + sep + "' != '" + separator + "'. (Successfully parsed values: " + to_string(list, separator) + ")");
			}
		}

		return result;
	}


}


#endif
