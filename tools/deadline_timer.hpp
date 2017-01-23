/// \file tools/deadline_timer.hpp
/// \author Benjamin Buch (benni.buch@gmail.com)
/// \brief Class deadline_timer
///
/// Copyright (c) 2013-2015 Benjamin Buch (benni dot buch at gmail dot com)
///
/// Distributed under the Boost Software License, Version 1.0. (See accompanying
/// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
///
#ifndef _tools_deadline_timer_hpp_INCLUDED_
#define _tools_deadline_timer_hpp_INCLUDED_

#include "exception_catcher.hpp"
#include "make_string.hpp"

#include <boost/asio.hpp>

#include <functional>
#include <mutex>
#include <future>


namespace tools{


	class deadline_timer{
	public:
		/// \brief Initiate with a callback and an intervall in milliseconds
		template < typename Log, typename Callback, typename InitCallback, typename Rep, typename Period >
		deadline_timer(Log&& log, Callback&& callback, InitCallback&& init_callback, std::chrono::duration< Rep, Period > intervall, bool periodic = true):
			intervall_(std::chrono::duration_cast< waitable_timer::duration >(intervall)),
			periodic_(periodic),
			started_(false),
			loop_([this, log, init_callback]{
				while(!exception_catcher(decltype(log)(log), [&] {
					init_callback();
					io_.run();
				}));
			}),
			intern_callback_([this, callback](boost::system::error_code const& error){
				if(periodic_ && started_) timer_.expires_from_now(intervall_);

				if(error){
					if(error.value() == boost::asio::error::operation_aborted) return;

					throw error;
				}else{
					callback();
				}

				if(periodic_ && started_) timer_.async_wait(intern_callback_);
			}),
			timer_(io_)
			{}

		/// \brief wait for end of woring thread
		~deadline_timer()try{
			stop();
		}catch(std::exception const& e){
			std::cerr << "deadline_timer deconstructed with exception, use member function stop() to catch exceptions: " << e.what() << std::endl;
		}catch(...){
			std::cerr << "deadline_timer deconstructed with unknown exception, use member function stop() to catch exceptions" << std::endl;
		}

		/// \brief Call the callback all intervall_ms
		void start(){
			std::unique_lock< std::mutex > lock(mutex_);

			started_ = false;

			timer_.expires_from_now(intervall_);
			if(future_.valid()) future_.wait();

			started_ = true;

			timer_.async_wait(intern_callback_);
			io_.reset();

			future_ = std::async(std::launch::async, loop_);
		}

		/// \brief Stop calling callback
		void stop() {
			std::unique_lock< std::mutex > lock(mutex_);

			started_ = false;

			timer_.cancel();
			if(future_.valid()) future_.wait();
		}


	private:
		/// \brief Used asio waitable_timer type
		using waitable_timer = boost::asio::basic_waitable_timer< std::chrono::system_clock >;

		/// \brief Duration 
		waitable_timer::duration const intervall_;

		bool const periodic_;

		bool started_;

		std::function< void() > const loop_;
		std::function< void(boost::system::error_code const&) > const intern_callback_;

		boost::asio::io_service io_;
		waitable_timer timer_;

		std::mutex mutex_;

		std::future< void > future_;
	};


}



#endif
