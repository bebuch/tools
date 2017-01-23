/// \file tools/make_weak.hpp
/// \author Benjamin Buch (benni.buch@gmail.com)
/// \brief Make a weak_ptr by a shared_ptr
///
/// Copyright (c) 2014-2015 Benjamin Buch (benni dot buch at gmail dot com)
///
/// Distributed under the Boost Software License, Version 1.0. (See accompanying
/// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
///
#ifndef _tools_weak_ptr_hpp_INCLUDED_
#define _tools_weak_ptr_hpp_INCLUDED_

#include <memory>


namespace tools{


	template < typename T >
	std::weak_ptr< T > make_weak(std::shared_ptr< T > const& ptr){
		return ptr;
	}


}

#endif
