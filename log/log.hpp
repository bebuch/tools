/// \file tools/log.hpp
/// \author Benjamin Buch (benni.buch@gmail.com)
/// \date 2013-2015
/// \brief A simple to use log-System
///
/// Copyright (c) 2013-2015 Benjamin Buch (benni dot buch at gmail dot com)
///
/// Distributed under the Boost Software License, Version 1.0. (See accompanying
/// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
///

#ifndef _tools_log_hpp_INCLUDED_
#define _tools_log_hpp_INCLUDED_

#include <functional>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <atomic>


namespace tools{


	class log_base;

	namespace impl{ namespace log{


		inline std::size_t unique_count(){
			static std::atomic< std::size_t > count(0);
			return count++;
		}


		std::ostringstream& get_os(log_base& log);
		std::size_t get_id(log_base& log);


		template < typename Lambda >
		struct extract_log_from_lambda {};

		template < typename F, typename R, typename Log >
		struct extract_log_from_lambda< R(F::*)(Log&)const >{
			using type = Log;
		};

		template < typename Lambda >
		struct extract_log {
			using type = typename extract_log_from_lambda< decltype(&Lambda::operator()) >::type;
		};

		template < typename Lambda >
		using extract_log_t = typename extract_log< Lambda >::type;

		template < typename T > struct Dummy{};


	} }


	class log_base{
	public:
		log_base(): has_body(false), id(impl::log::unique_count()){}

		static bool is_active(){
			return true;
		}

		static void first(std::ostringstream& os){}

		static void prefix(std::ostringstream& os){}

		static void postfix(std::ostringstream& os){}

		static void delimiter(std::ostringstream& os){
			os << '\n';
		}

		static void output(std::string&& str){
			std::clog << str << std::flush;
		}

	protected:
		bool has_body;

	private:
		std::size_t id;
		std::ostringstream* os;

		friend std::ostringstream& impl::log::get_os(log_base& log);
		friend std::size_t impl::log::get_id(log_base& log);

		friend void set_os(log_base& log, std::ostringstream& os){
			log.os = &os;
		}

		friend void set_body(log_base& log){
			log.has_body = true;
		}
	};

	template < typename T >
	inline log_base& operator<<(log_base& log, T&& v){
		impl::log::get_os(log) << std::forward< T >(v);
		return log;
	}

	struct never_log{};

	template < typename T >
	inline never_log& operator<<(never_log& log, T&& v){
		return log;
	}


	namespace impl{ namespace log{


		inline std::ostringstream& get_os(log_base& log){
			return *log.os;
		}

		inline std::size_t get_id(log_base& log){
			return log.id;
		}

		inline void do_init(log_base& log){
			get_os(log) << std::setfill('0') << std::setw(6) << get_id(log) << ' ';
		}

		inline void do_exception(std::ostringstream& os){
			os << " (failed by exception)";
		}


		template < typename Log, typename Function >
		inline void exec_log(Log& log, Function& f){
			std::ostringstream os;
			os << std::boolalpha;
			set_os(log, os);

			log.first(os);
			do_init(log);
			log.prefix(os);
			f(log);
			log.postfix(os);
			log.delimiter(os);
			log.output(get_os(log).str());
		}

		template < typename Log, typename Function >
		inline void exec_log(Log& log, Function& f, bool exception){
			std::ostringstream os;
			os << std::boolalpha;
			set_os(log, os);

			log.first(os);
			do_init(log);
			log.prefix(os);
			f(log);
			log.postfix(os);
			if(exception) do_exception(get_os(log));
			log.delimiter(os);
			log.output(get_os(log).str());
		}

		auto const error_message = "FATAL ERROR: Exception while writing a log message!";

		template < typename Function >
		inline void catch_exceptions(Function&& f)try{
			f();
		}catch(std::exception const& error){
			std::cerr << error_message << " (" << error.what() << ")\n" << std::flush;
		}catch(...){
			std::cerr << error_message << "\n" << std::flush;
		}


		template < typename Log >
		void check_log_base(){
			static_assert(std::is_base_of< log_base, Log >::value, "type Log have to be derived from log_base");
		}

		template < typename Log, typename F >
		void check_log_type(){
			check_log_base< Log >();
			check_log_base< extract_log_t< F > >();
			static_assert(std::is_same< Log, extract_log_t< F > >::value, "type Log have to be the same as in callback");
		}


		template < typename Log, typename F >
		inline void log(Log& log, F&& f){
			check_log_type< Log, F >();

			catch_exceptions([&]{
				if(log.is_active()) exec_log< Log >(log, f);
			});
		}

		template < typename Log, typename F, typename Body >
		inline auto log(Log& log, F&& f, Body&& body) -> decltype(body()){
			check_log_type< Log, F >();

			set_body(log);

			struct on_destruct{
				on_destruct(Log& log, F& f): log_(log), f_(f), exception_(false) {}
				~on_destruct(){
					catch_exceptions([&]{
						if(log_.is_active() || exception_) exec_log< Log >(log_, f_, exception_);
					});
				}

				Log& log_;
				F& f_;
				bool exception_;
			} message(log, f);

			try{
				return body();
			}catch(...){
				message.exception_ = true;
				throw;
			}
		}


		template < typename F >
		inline void log(never_log& log, F&& f){}

		template < typename F, typename Body >
		inline auto log(never_log& log, F&& f, Body&& body) -> decltype(body()){
			return body();
		}


	} }


	template < typename F >
	inline void log(F&& f){
		using Log = impl::log::extract_log_t< F >;

		Log log;
		impl::log::log(log, std::forward< F >(f));
	}

	template < typename F, typename Body >
	inline auto log(F&& f, Body&& body) -> decltype(body()){
		using Log = impl::log::extract_log_t< F >;

		Log log;
		return impl::log::log(log, std::forward< F >(f), std::forward< Body >(body));
	}


}


#endif
