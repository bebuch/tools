#ifndef _tools_camera_sequence_hpp_INCLUDED_
#define _tools_camera_sequence_hpp_INCLUDED_

#include "bitmap_sequence.hpp"


namespace tools {


	/// \brief List of tools::bitmap_sequence< T >
	template < typename T >
	using camera_sequence = std::vector< bitmap_sequence< T > >;

	/// \brief List of tools::bitmap_pointer_sequence< T >
	template < typename T >
	using camera_pointer_sequence = std::vector< std::shared_ptr< bitmap_pointer_sequence< T > > >;

	/// \brief List of tools::bitmap_pointer_sequence< T >
	template < typename T >
	using camera_const_pointer_sequence = std::vector< std::shared_ptr< bitmap_const_pointer_sequence< T > const > >;

	template < typename T >
	camera_const_pointer_sequence< T > make_const(camera_pointer_sequence< T >&& data){
		camera_const_pointer_sequence< T > result;
		result.reserve(data.size());

		for(auto& v: data){
			result.push_back(make_const(std::move(v)));
		}

		return result;
	}

	template < typename T >
	std::shared_ptr< camera_const_pointer_sequence< T >  const > make_const(std::shared_ptr< camera_pointer_sequence< T > >&& data){
		return std::make_shared< camera_const_pointer_sequence< T >  const >(make_const(std::move(*data)));
	}

}

#endif
