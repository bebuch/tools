/// \file tools/io.hpp
/// \author Benjamin Buch (benni.buch@gmail.com)
/// \brief Some input/output tools
///
/// Copyright (c) 2013-2015 Benjamin Buch (benni dot buch at gmail dot com)
///
/// Distributed under the Boost Software License, Version 1.0. (See accompanying
/// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
///
#ifndef _tools_io_hpp_INCLUDED_
#define _tools_io_hpp_INCLUDED_

#include <iostream>


namespace tools{


	namespace io{


		template < class charT, class traits >
		inline bool test(std::basic_istream< charT, traits >& is, charT const& should_be){
			charT in;
			if(is){
				if(is >> in && in == should_be) return true;
				is.putback(in);
			}
			return false;
		}

		template < class charT, class traits, class ShouldBe >
		inline bool equal(std::basic_istream< charT, traits >& is, ShouldBe const& should_be){
			ShouldBe in;
			if(is && is >> in && in == should_be) return true;
			is.setstate(std::ios_base::failbit);
			return false;
		}

		template < class charT, class traits >
		inline bool equal(std::basic_istream< charT, traits >& is, charT const& should_be){
			auto result = test(is, should_be);
			if(!result) is.setstate(std::ios_base::failbit);
			return result;
		}


	}


}

#endif
