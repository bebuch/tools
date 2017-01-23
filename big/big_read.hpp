/// \file tools/big_read.hpp
/// \author Benjamin Buch (benni.buch@gmail.com)
/// \date 18.05.2009
/// \brief template function tools::big::read
///
/// Copyright (c) 2009-2015 Benjamin Buch (benni dot buch at gmail dot com)
///
/// Distributed under the Boost Software License, Version 1.0. (See accompanying
/// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
///
#ifndef _tools_big_read_hpp_INCLUDED_
#define _tools_big_read_hpp_INCLUDED_

#include "big_types.hpp"
#include "big_exception.hpp"
#include "big_undef_conversion.hpp"

#include <string>
#include <fstream>
#include <cstdint>

namespace tools { namespace big {


	/// \brief Header of a big file
	struct header{
		std::uint16_t width;
		std::uint16_t height;
		std::uint16_t type;
		std::uint32_t placeholder;
	};

	/// \brief Loads a big file by a given filename
	/// \throw tools::big::big_error
	template < typename BitmapType >
	void read(BitmapType& bitmap, std::string const& filename);

	/// \brief Loads a big file from a std::istream
	/// \throw tools::big::big_error
	template < typename BitmapType >
	void read(BitmapType& bitmap, std::istream& is);

	/// \brief Loads the type-information of a big file from a std::istream
	/// \throw tools::big::big_error
	header read_header(std::istream& is);

	/// \brief Loads the data of a big file from a std::istream
	/// First you must use the read_header-function to read the header and resize the bitmap
	/// \throw tools::big::big_error
	template < typename BitmapType >
	void read_data(BitmapType& bitmap, std::istream& is);


	//=============================================================================
	// Implementation
	//=============================================================================

	template < typename BitmapType >
	void read(BitmapType& bitmap, std::string const& filename){
		std::ifstream is(filename.c_str(), std::ios_base::in | std::ios_base::binary);

		if(!is.is_open()){
			throw big_error("Can't open file: " + filename);
		}

		try{
			read(bitmap, is);
		}catch(big_error const& error){
			throw big_error(std::string(error.what()) + ": " + filename);
		}
	}

	template < typename BitmapType >
	void read(BitmapType& bitmap, std::istream& is){
		header header = read_header(is);

		if(header.type != big::type< typename BitmapType::value_type >::value){
			throw big_error("Type in file is not compatible");
		}

		// The last 4 bits in the type get the size of a single value
		if((header.type & 0x000F) != sizeof(typename BitmapType::value_type)){
			throw big_error("Size type in file is not compatible");
		}

		bitmap.resize(header.width, header.height);

		read_data(bitmap, is);
	}

	inline header read_header(std::istream& is){
		header header; // big header informations

		// read the file header (10 Byte)
		is.read(reinterpret_cast< std::ifstream::char_type* >(&header.width),  2);
		is.read(reinterpret_cast< std::ifstream::char_type* >(&header.height), 2);
		is.read(reinterpret_cast< std::ifstream::char_type* >(&header.type),   2);
		is.read(reinterpret_cast< std::ifstream::char_type* >(&header.placeholder), 4);

		if(!is.good()){
			throw big_error("Can't read big header");
		}

		return header;
	}

	template < typename BitmapType >
	void read_data(BitmapType& bitmap, std::istream& is){
		is.read(
			reinterpret_cast< std::ifstream::char_type* >(bitmap.data()),
			bitmap.width() * bitmap.height() * sizeof(typename BitmapType::value_type)
		);

		if(!is.good()){
			throw big_error("Can't read big content");
		}

		if(std::numeric_limits< typename BitmapType::value_type >::has_quiet_NaN){
			bitmap = convert_undef_to_nan(std::move(bitmap));
		}
	}


} }

#endif
