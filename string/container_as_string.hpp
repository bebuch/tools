/// \file tools/container_as_string.hpp
/// \author Benjamin Buch (benni.buch@gmail.com)
/// \brief Output containers as separated list
///
/// Copyright (c) 2009-2015 Benjamin Buch (benni dot buch at gmail dot com)
///
/// Distributed under the Boost Software License, Version 1.0. (See accompanying
/// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
///
#ifndef _tools_container_as_string_hpp_INCLUDED_
#define _tools_container_as_string_hpp_INCLUDED_

#include <string>
#include <iomanip>
#include <sstream>


namespace tools{


	template < typename Container >
	std::string container_as_string(Container const& list, char separator = ','){
		if (list.empty()) return std::string();

		std::ostringstream os;
		auto i = list.cbegin();
		os << std::boolalpha << *i++;
		for (; i != list.cend(); ++i){
			os << separator << *i;
		}

		return os.str();
	}


}

#endif
