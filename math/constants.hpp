/// \file tools/constants.hpp
/// \author Benjamin Buch (benni.buch@gmail.com)
/// \brief Math constants
///
/// Copyright (c) 2011-2015 Benjamin Buch (benni dot buch at gmail dot com)
///
/// Distributed under the Boost Software License, Version 1.0. (See accompanying
/// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
///
#ifndef _tools_constants_hpp_INCLUDED_
#define _tools_constants_hpp_INCLUDED_


namespace tools{

	template < typename T >
	struct constant {
#if defined(_MSC_VER) && _MSC_VER <= 1800
		static T const pi;
		static T const e;
#else
		static constexpr T pi = T(3.1415926535897932384626433832795028841971693993751058209749445923078164062862089986280348253421L);
		static constexpr T e = T(2.7182818284590452353602874713526624977572470936999595749669676277240766303535475945713821785252L);
#endif
};

#if defined(_MSC_VER) && _MSC_VER <= 1800
	template < typename T >
	T const constant< T >::pi = T(3.1415926535897932384626433832795028841971693993751058209749445923078164062862089986280348253421L);

	template < typename T >
	T const constant< T >::e = T(2.7182818284590452353602874713526624977572470936999595749669676277240766303535475945713821785252L);
#endif


}


#endif
