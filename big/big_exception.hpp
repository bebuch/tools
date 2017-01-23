/// \file tools/big_exception.hpp
/// \author Benjamin Buch (benni.buch@gmail.com)
/// \date 18.05.2009
/// \brief exception class tools::big::big_error
///
/// Copyright (c) 2009-2015 Benjamin Buch (benni dot buch at gmail dot com)
///
/// Distributed under the Boost Software License, Version 1.0. (See accompanying
/// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
///
#ifndef _tools_big_exception_hpp_INCLUDED_
#define _tools_big_exception_hpp_INCLUDED_

#include <string>
#include <stdexcept>

namespace tools { namespace big {


	/// \brief Exception class that will throws by load or save big files
	class big_error: public std::runtime_error{
	public:
		/// \brief Constructs an object of class big_error
		explicit big_error(std::string const& what_arg);
		/// \brief Constructs an object of class big_error
		explicit big_error(char const* what_arg);
	};


	//=============================================================================
	// Implementation
	//=============================================================================

	inline
	big_error::big_error(
		std::string const& what_arg
	):
		std::runtime_error(what_arg)
		{}

	inline
	big_error::big_error(
		char const* what_arg
	):
		std::runtime_error(what_arg)
		{}


} }

#endif
