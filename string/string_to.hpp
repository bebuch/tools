/// \file tools/string_to.hpp
/// \author Benjamin Buch (benni.buch@gmail.com)
/// \brief Input/Output for class template std::vector
///
/// Copyright (c) 2010-2015 Benjamin Buch (benni dot buch at gmail dot com)
///
/// Distributed under the Boost Software License, Version 1.0. (See accompanying
/// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
///
#ifndef _tools_string_to_hpp_INCLUDED_
#define _tools_string_to_hpp_INCLUDED_

#include <string>
#include <iomanip>
#include <sstream>
#include <stdexcept>
#include <type_traits>

#include <boost/type_index.hpp>


namespace tools{


	template < typename T >
	inline T string_to(std::string&& value){
		std::istringstream is(std::move(value));

		T result;
		if(is >> std::boolalpha >> result) return result;

		throw std::runtime_error("Cannot convert '" + is.str() + "' to type '" + boost::typeindex::type_id<T>().pretty_name() + "'");
	}

	template < typename T >
	inline T string_to(std::string const& value){
		return string_to< T >(std::string(value));
	}

	template <>
	inline std::string string_to(std::string&& value){
		return std::move(value);
	}

	template <>
	inline std::string string_to(std::string const& value){
		return value;
	}

	template <>
	inline char string_to(std::string&& value){
		return static_cast< char >(string_to< std::conditional_t< std::is_signed< char >::value, short, unsigned short > >(std::move(value)));
	}

	template <>
	inline char string_to(std::string const& value){
		return static_cast< char >(string_to< std::conditional_t< std::is_signed< char >::value, short, unsigned short > >(value));
	}

	template <>
	inline signed char string_to(std::string&& value){
		return static_cast< signed char >(string_to< short >(std::move(value)));
	}

	template <>
	inline signed char string_to(std::string const& value){
		return static_cast< signed char >(string_to< short >(value));
	}

	template <>
	inline unsigned char string_to(std::string&& value){
		return static_cast< unsigned char >(string_to< unsigned short >(std::move(value)));
	}

	template <>
	inline unsigned char string_to(std::string const& value){
		return static_cast< unsigned char >(string_to< unsigned short >(value));
	}

	template <>
	inline wchar_t string_to(std::string&& value){
		return static_cast< wchar_t >(string_to< std::conditional_t< std::is_signed< wchar_t >::value, long, unsigned long > >(std::move(value)));
	}

	template <>
	inline wchar_t string_to(std::string const& value){
		return static_cast< wchar_t >(string_to< std::conditional_t< std::is_signed< wchar_t >::value, long, unsigned long > >(value));
	}

	//template <>
	//inline char16_t string_to(std::string&& value){
	//	return static_cast< char16_t >(string_to< unsigned short >(std::move(value)));
	//}

	//template <>
	//inline char16_t string_to(std::string const& value){
	//	return static_cast< char16_t >(string_to< unsigned short >(value));
	//}

	template <>
	inline char32_t string_to(std::string&& value){
		return static_cast< char32_t >(string_to< unsigned long >(std::move(value)));
	}

	template <>
	inline char32_t string_to(std::string const& value){
		return static_cast< char32_t >(string_to< unsigned long >(value));
	}


}

#endif
