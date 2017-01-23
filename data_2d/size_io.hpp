/// \file tools/size_io.hpp
/// \author Benjamin Buch (benni.buch@gmail.com)
/// \date 07.05.2009
/// \brief Input/Output for class template tools::size
///
/// Copyright (c) 2009-2015 Benjamin Buch (benni dot buch at gmail dot com)
///
/// Distributed under the Boost Software License, Version 1.0. (See accompanying
/// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
///
#ifndef _tools_size_io_hpp_INCLUDED_
#define _tools_size_io_hpp_INCLUDED_

#include "io.hpp"
#include "size.hpp"

#include <iostream>


namespace tools{


	template < typename charT, typename traits, typename T >
	std::basic_ostream< charT, traits >& operator<<(std::basic_ostream< charT, traits >& os, size< T > const& data){
		return os << data.width() << "x" << data.height();
	}


	template < typename charT, typename traits, typename T >
	std::basic_istream< charT, traits >& operator>>(std::basic_istream< charT, traits >& is, size< T >& data){
		size< T > tmp;
		is >> tmp.width();
		if(!tools::io::equal(is, 'x')) return is;
		is >> tmp.height();

		data = std::move(tmp);

		return is;
	}


}


#endif
