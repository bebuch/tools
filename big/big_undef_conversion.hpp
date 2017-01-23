/// \file tools/big_undef_conversion.hpp
/// \author Benjamin Buch (benni.buch@googlemail.com)
/// \date 14.12.2012
/// \brief undef_conversion
///
/// Copyright (c) 2012-2015 Benjamin Buch (benni dot buch at gmail dot com)
///
/// Distributed under the Boost Software License, Version 1.0. (See accompanying
/// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
///
#ifndef _undef_conversion_hpp_INCLUDED_
#define _undef_conversion_hpp_INCLUDED_

#include <cmath>
#include <limits>
#include <algorithm>
#include <utility>


#if defined(_MSC_VER) && _MSC_VER <= 1800
#define constexpr const
#endif


namespace tools { namespace big {


	template < typename T >
	inline bool isnan(T const& value){
		return value != value;
	}

	constexpr float undef = 3.402823466e38f;

	template < typename container >
	container convert_undef_to_nan(container const& image){
		typedef typename container::value_type value_type;

		container result(image.size());
		std::transform(image.begin(), image.end(), result.begin(), [](value_type value){
			return value >= undef ? std::numeric_limits< value_type >::quiet_NaN() : value;
		});

		return std::move(result);
	}

	template < typename container >
	container convert_nan_to_undef(container const& image){
		typedef typename container::value_type value_type;

		container result(image.size());
		std::transform(image.begin(), image.end(), result.begin(), [](value_type value){
			return isnan(value) ? undef : value;
		});

		return std::move(result);
	}


} }

#endif
