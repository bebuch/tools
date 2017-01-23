/// \file tools/point_io.hpp
/// \author Benjamin Buch (benni.buch@gmail.com)
/// \date 07.05.2009
/// \brief Input/Output for template tools::point
///
/// Copyright (c) 2009-2015 Benjamin Buch (benni dot buch at gmail dot com)
///
/// Distributed under the Boost Software License, Version 1.0. (See accompanying
/// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
///
#ifndef _tools_point_io_hpp_INCLUDED_
#define _tools_point_io_hpp_INCLUDED_

#include "io.hpp"
#include "point.hpp"

#include <iostream>


namespace tools {


	template < typename charT, typename traits, typename T >
	std::basic_ostream< charT, traits >& operator<<(std::basic_ostream< charT, traits >& os, point< T > const& data) {
		return os << data.x() << "x" << data.y();
	}


	template < typename charT, typename traits, typename T >
	std::basic_istream< charT, traits >& operator>>(std::basic_istream< charT, traits >& is, point< T >& data) {
		point< T > tmp;
		is >> tmp.x();
		if(!tools::io::equal(is, 'x')) return is;
		is >> tmp.y();

		data = std::move(tmp);

		return is;
	}


}


#endif
