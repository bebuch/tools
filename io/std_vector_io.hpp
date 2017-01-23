/// \file tools/std_vector_io.hpp
/// \author Benjamin Buch (benni.buch@gmail.com)
/// \date 07.05.2009
/// \brief Input/Output for class template std::vector
///
/// Copyright (c) 2009-2015 Benjamin Buch (benni dot buch at gmail dot com)
///
/// Distributed under the Boost Software License, Version 1.0. (See accompanying
/// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
///
#ifndef _tools_std_vector_io_hpp_INCLUDED_
#define _tools_std_vector_io_hpp_INCLUDED_

#include "io.hpp"

#include <iostream>
#include <vector>


namespace tools{


	template < typename charT, typename traits, typename T >
	std::basic_ostream< charT, traits >& operator<<(std::basic_ostream< charT, traits >& os, std::vector< T > const& data){
		os << '{';

		if(data.size() >= 1){
			os << data[0];
			for(std::size_t i = 1; i < data.size(); ++i){
				os << ',' << data[i];
			}
		}

		return os << '}';
	}


	template < typename charT, typename traits, typename T >
	std::basic_istream< charT, traits >& operator>>(std::basic_istream< charT, traits >& is, std::vector< T >& data){
		if(!tools::io::equal(is, '{')) return is;

		std::vector< T > tmp;
		T value;
		is >> value;
		tmp.push_back(value); // TODO: use emplace_back (C++14 kann Microschrott wiedermal noch nicht ...)

		for(;;){
			if(!tools::io::test(is, ',')){
				if(!tools::io::equal(is, '}')) return is;

				data = std::move(tmp);

				return is;
			}

			is >> value;
			tmp.push_back(value); // TODO: use emplace_back (C++14 kann Microschrott wiedermal noch nicht ...)
		}

		return is;
	}


}


#endif
