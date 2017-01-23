#ifndef _tools_bitmap_sequence_hpp_INCLUDED_
#define _tools_bitmap_sequence_hpp_INCLUDED_

#include "bitmap.hpp"

#include <vector>
#include <memory>


namespace tools {


	/// \brief Vector of tools::bitmap< T >
	template < typename T >
	using bitmap_sequence = std::vector< bitmap< T > >;

	/// \brief Vector of shared pointer to tools::bitmap< T >
	template < typename T >
	using bitmap_pointer_sequence = std::vector< std::shared_ptr< bitmap< T > > >;

	/// \brief Vector of shared pointer to tools::bitmap< T >
	template < typename T >
	using bitmap_const_pointer_sequence = std::vector< std::shared_ptr< bitmap< T > const > >;

	template < typename T >
	bitmap_const_pointer_sequence< T > make_const(bitmap_pointer_sequence< T >&& data){
		bitmap_const_pointer_sequence< T > result;
		result.reserve(data.size());

		for(auto& v: data){
			result.push_back(std::move(v));
		}

		return result;
	}

	template < typename T >
	std::shared_ptr< bitmap_const_pointer_sequence< T > const > make_const(std::shared_ptr< bitmap_pointer_sequence< T > >&& data){
		return std::make_shared< bitmap_const_pointer_sequence< T >  const >(make_const(std::move(*data)));
	}


}

#endif
