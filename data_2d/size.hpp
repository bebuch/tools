/// \file tools/size.hpp
/// \author Benjamin Buch (benni.buch@gmail.com)
/// \date 07.05.2009
/// \brief Class template tools::size
///
/// Copyright (c) 2009-2015 Benjamin Buch (benni dot buch at gmail dot com)
///
/// Distributed under the Boost Software License, Version 1.0. (See accompanying
/// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
///
#ifndef _tools_size_hpp_INCLUDED_
#define _tools_size_hpp_INCLUDED_


namespace tools {


	/// \brief A class for manipulating sizes
	/// \tparam ValueType Type of the size data
	template < typename ValueType >
	class size{
	public:
		/// \brief Type of the positions
		using value_type = ValueType;


		/// \brief The width
		value_type& width(){
			return width_;
		}

		/// \brief The height
		value_type& height(){
			return height_;
		}


		/// \brief The width
		value_type const& width()const{
			return width_;
		}

		/// \brief The height
		value_type const& height()const{
			return height_;
		}


		/// \brief Set width and height
		void set(value_type const& width, value_type const& height){
			width_ = width;
			height_ = height;
		}


	#if !defined(_MSC_VER) || _MSC_VER > 1800
		/// \brief Constructs a size with width 0 and height 0
		size() = default;

		/// \brief Constructs a copy
		size(size const&) = default;

		/// \brief Constructs a copy
		size(size&&) = default;
	#else
		size(): width_(), height_() {}
	#endif

		/// \brief Constructs a size width width and height
		size(value_type const& width, value_type const& height):
			width_(width), height_(height) {}


	#if !defined(_MSC_VER) || _MSC_VER > 1800
		/// \brief Copy assignment
		size& operator=(size const&) = default;

		/// \brief Move assignment
		size& operator=(size&&) = default;
	#endif


		/// \brief Get true, if width and height are positiv
		bool is_positive()const{
			return width() >= value_type() && height() >= value_type();
		}


		/// \brief Get width * height
		value_type const point_count()const{
			return width() * height();
		}

	private:
		value_type width_;
		value_type height_;
	};

	template < typename ValueType >
	bool operator==(size< ValueType > const& a, size< ValueType > const& b){
		return a.width() == b.width() && a.height() == b.height();
	}

	template < typename ValueType >
	bool operator!=(size< ValueType > const& a, size< ValueType > const& b){
		return a != b;
	}


}

#endif
