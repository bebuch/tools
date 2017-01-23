/// \file tools/time_to_string.hpp
/// \author Benjamin Buch (benni.buch@gmail.com)
/// \date 18.11.2014
/// \brief template function tools::time_to_string
///
/// Copyright (c) 2014-2015 Benjamin Buch (benni dot buch at gmail dot com)
///
/// Distributed under the Boost Software License, Version 1.0. (See accompanying
/// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
///
#ifndef _tools_time_to_string_hpp_INCLUDED_
#define _tools_time_to_string_hpp_INCLUDED_


#include <iostream>
#include <iomanip>
#include <sstream>
#include <chrono>
#include <ctime>


namespace tools{


	template < typename CharT, typename Traits >
	inline std::basic_ostream< CharT, Traits >& time_to_string(std::basic_ostream< CharT, Traits >& os, std::chrono::system_clock::time_point const& time = std::chrono::system_clock::now()) {
		auto microseconds = std::chrono::duration_cast< std::chrono::microseconds >(time.time_since_epoch());
		auto localtime = std::chrono::system_clock::to_time_t(time);
		auto datetime = *std::localtime(&localtime);

		return os
			<< std::fixed << std::setfill('0')
			<< std::setw(4) << 1900 + datetime.tm_year << "-"
			<< std::setw(2) << 1 + datetime.tm_mon << "-"
			<< std::setw(2) << datetime.tm_mday << " "
			<< std::setw(2) << datetime.tm_hour << ":"
			<< std::setw(2) << datetime.tm_min << ":"
			<< std::setw(2) << datetime.tm_sec << " "
			<< std::setw(3) << microseconds.count() / 1000 % 1000 << '.'
			<< std::setw(3) << microseconds.count() % 1000;
	}

	template < typename CharT, typename Traits = std::char_traits< CharT >, typename Allocator = std::allocator< CharT > >
	inline std::basic_string< CharT, Traits, Allocator > time_to_string(std::chrono::system_clock::time_point const& time = std::chrono::system_clock::now()) {
		std::basic_ostringstream< CharT, Traits, Allocator > os;

		time_to_string(os, time);

		return os.str();
	}

	inline std::string time_to_string(std::chrono::system_clock::time_point const& time = std::chrono::system_clock::now()) {
		return time_to_string< char >(time);
	}


}


#endif
