/// \file tools/make_string.hpp
/// \author Benjamin Buch (benni.buch@gmail.com)
/// \brief Make a string using std::ostringstream
///
/// Copyright (c) 2011-2015 Benjamin Buch (benni dot buch at gmail dot com)
///
/// Distributed under the Boost Software License, Version 1.0. (See accompanying
/// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
///
#ifndef _tools_make_string_hpp_INCLUDED_
#define _tools_make_string_hpp_INCLUDED_

#include <sstream>
#include <iomanip>


namespace tools{


	namespace impl{ namespace make_string{


		template < typename Stream >
		inline void stream_output(Stream& s){}

		template < typename Stream, typename Head >
		inline void stream_output(Stream& s, Head&& head){
			s << std::forward< Head >(head);
		}

		template < typename Stream, typename Head, typename ... T >
		inline void stream_output(Stream& s, Head&& head, T&& ... args){
			s << std::forward< Head >(head);
			stream_output(s, std::forward< T >(args) ...);
		}

		template < typename Stream, typename Separator >
		inline void stream_output_separated_by(Stream& s, Separator const& separator){}

		template < typename Stream, typename Separator, typename Head >
		inline void stream_output_separated_by(Stream& s, Separator const& separator, Head&& head){
			s << std::forward< Head >(head);
		}

		template < typename Stream, typename Separator, typename Head, typename ... T >
		inline void stream_output_separated_by(Stream& s, Separator const& separator, Head&& head, T&& ... args){
			s << std::forward< Head >(head) << separator;
			stream_output_separated_by(s, separator, std::forward< T >(args) ...);
		}


	} }


	template < typename ... T >
	inline std::string make_string(T&& ... args){
		std::ostringstream os;
		os << std::boolalpha;
		impl::make_string::stream_output(os, std::forward< T >(args) ...);
		return os.str();
	}

	template < typename Separator, typename ... T >
	inline std::string make_string_separated_by(Separator const& separator, T&& ... args){
		std::ostringstream os;
		os << std::boolalpha;
		impl::make_string::stream_output_separated_by(os, separator, std::forward< T >(args) ...);
		return os.str();
	}


}


#endif
