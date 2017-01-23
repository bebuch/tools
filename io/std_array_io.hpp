/// \file tools/std_array_io.hpp
/// \author Benjamin Buch (benni.buch@gmail.com)
/// \date 07.05.2009
/// \brief Input/Output for class template std::array
///
/// Copyright (c) 2009-2015 Benjamin Buch (benni dot buch at gmail dot com)
///
/// Distributed under the Boost Software License, Version 1.0. (See accompanying
/// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
///
#ifndef _tools_std_array_io_hpp_INCLUDED_
#define _tools_std_array_io_hpp_INCLUDED_

#include "io.hpp"

#include <iostream>
#include <array>


namespace tools{


	template < typename charT, typename traits, typename T, std::size_t N >
	std::basic_ostream< charT, traits >& operator<<(std::basic_ostream< charT, traits >& os, std::array< T, N > const& data){
		os << '{';

		if(N >= 1){
			os << data[0];
			for(std::size_t i = 1; i < N; ++i){
				os << ',' << data[i];
			}
		}

		return os << '}';
	}


	template < typename charT, typename traits, typename T, std::size_t N >
	std::basic_istream< charT, traits >& operator>>(std::basic_istream< charT, traits >& is, std::array< T, N >& data){
		if(!tools::io::equal(is, '{')) return is;

		std::array< T, N > tmp;
		if(N >= 1){
			is >> tmp[0];
			for(std::size_t i = 1; i < N; ++i){
				if(!tools::io::equal(is, ',')) return is;
				is >> tmp[i];
			}
		}

		if(!tools::io::equal(is, '}')) return is;

		data = std::move(tmp);

		return is;
	}


}


#endif
