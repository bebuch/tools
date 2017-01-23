/// \file tools/on_destruct.hpp
/// \author Benjamin Buch (benni.buch@gmail.com)
/// \brief Execute a function on object destruction
///
/// Copyright (c) 2012-2015 Benjamin Buch (benni dot buch at gmail dot com)
///
/// Distributed under the Boost Software License, Version 1.0. (See accompanying
/// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
///
#ifndef _tools_on_destruct_hpp_INCLUDED_
#define _tools_on_destruct_hpp_INCLUDED_

#include "exception_catcher.hpp"

#include <functional>


namespace tools{


	template < typename Log, typename F >
	class on_destruct_t{
	public:
		on_destruct_t(F&& f): f_(std::forward< F >(f)), active_(true) {}

		on_destruct_t(on_destruct_t&& m): f_(std::move(m.f_)), active_(m.active_) { m.stop(); }

		~on_destruct_t(){
			if(!active_) return;

			// destructor never throw
			exception_catcher([](Log& os){ os << "on_destruct"; }, [this]{
				f_();
			});
		}

		void stop(){
			active_ = false;
		}

	private:
		on_destruct_t(on_destruct_t const&);

		F f_;
		bool active_;
	};


	template < typename Log = tools::never_log, typename F >
	on_destruct_t< Log, F > on_destruct(F&& f){
		return on_destruct_t< Log, F >(std::forward< F >(f));
	}


}


#endif
