/// \file tools/lazy_init.hpp
/// \author Benjamin Buch (benni.buch@gmail.com)
/// \brief Lazy initialization
///
/// Copyright (c) 2013-2015 Benjamin Buch (benni dot buch at gmail dot com)
///
/// Distributed under the Boost Software License, Version 1.0. (See accompanying
/// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
///
#ifndef _tools_lazy_init_hpp_INCLUDED_
#define _tools_lazy_init_hpp_INCLUDED_

#include <functional>
#include <mutex>


namespace tools {


	class lazy_init{
	public:
		template < typename Function >
		lazy_init(Function&& f): function_(std::forward< Function >(f)) {}

		void operator()(){
			std::call_once(called_, function_);
		}

	private:
		std::once_flag called_;
		std::function< void() > function_;
	};


}

#endif
