/// \file tools/exception_catcher.hpp
/// \author Benjamin Buch (benni.buch@gmail.com)
/// \date 10.07.2013
/// \brief template function tools::exception_catcher catchs all exceptions
///
/// Copyright (c) 2013-2015 Benjamin Buch (benni dot buch at gmail dot com)
///
/// Distributed under the Boost Software License, Version 1.0. (See accompanying
/// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
///
#ifndef _tools_exception_catcher_hpp_INCLUDED_
#define _tools_exception_catcher_hpp_INCLUDED_

#include "log.hpp"

#include <memory>


namespace tools{


	struct clean_up_exit: std::runtime_error{
		clean_up_exit(): std::runtime_error("clean_up_exit"){}
	};


	namespace impl { namespace exception_catcher {


		template < typename ReturnType >
		class result_type{
		public:
			result_type(){}

			template < typename Body >
			result_type(Body&& body):
				result_(new ReturnType(body()))
				{}

			operator bool()const{
				return result_ ? true : false;
			}

			ReturnType result()const{
				if(result_){
					return *result_;
				}else{
					throw std::logic_error("exception_catcher result was accessed after failed execution");
				}
			}

		private:
			std::unique_ptr< ReturnType > result_;
		};

		template < typename ReturnType >
		struct control{
			using type = result_type< ReturnType >;

			template < typename Body >
			static type exec(Body&& body){
				return type(std::forward< Body >(body));
			}

			static type fail(){
				return type();
			}
		};

		template <>
		struct control< void >{
			using type = bool;

			template < typename Body >
			static type exec(Body&& body){
				body();
				return true;
			}

			static type fail(){
				return false;
			}
		};


		auto const error_message = "FATAL ERROR: exception_catcher terminates the program";


		template < typename Log, typename Function >
		inline void exec_log(Log& log, Function& f, std::string const& error_text){
			std::ostringstream os;
			os << std::boolalpha;
			set_os(log, os);
			log.first(os);
			impl::log::do_init(log);
			log.prefix(os);
			f(log);
			log.postfix(os);
			impl::log::get_os(log) << error_text;
			log.delimiter(os);
			log.output(impl::log::get_os(log).str());
		}


		template < typename Log, typename F, typename Body >
		inline auto do_exception_catcher(Log& log, F&& f, Body&& body)
			-> typename control< decltype(body()) >::type
		{
			{
				impl::log::check_log_type< Log, F >();

				set_body(log);

				struct on_destruct{
					on_destruct(Log& log, F& f): log_(log), f_(f) {}
					~on_destruct(){
						impl::log::catch_exceptions([&]{
							if(log_.is_active() || !error_text_.empty()) exec_log< Log >(log_, f_, error_text_);
						});
					}

					Log& log_;
					F& f_;
					std::string error_text_;
				} message(log, f);

				try{
					return control< decltype(body()) >::exec(std::forward< Body >(body));
				}catch(clean_up_exit const&){
					message.error_text_ = " (clean_up_exit)";
					throw;
				}catch(std::exception const& error){
					message.error_text_ = std::string(" (exception catched: ") + error.what() + ")";
				}catch(...){
					message.error_text_ = " (unknown exception catched)";
				}
			}

			return control< decltype(body()) >::fail();
		}

		template < typename F, typename Body >
		inline auto do_exception_catcher(never_log& log, F&& f, Body&& body)
			-> typename control< decltype(body()) >::type
		{
			try{
				return control< decltype(body()) >::exec(std::forward< Body >(body));
			}catch(clean_up_exit const&){
				throw;
			}catch(std::exception const& error){
				std::clog << "exception in conjunction with never_log-object (exception catched: " << error.what() << ")";
			}catch(...){
				std::clog << "exception in conjunction with never_log-object (unknown exception catched)";
			}

			return control< decltype(body()) >::fail();
		}


		template < typename Log, typename F, typename Body >
		inline auto exception_catcher(Log& log, F&& f, Body&& body)
			-> typename control< decltype(body()) >::type
			try
		{
			return do_exception_catcher(log, std::forward< F >(f), std::forward< Body >(body));
		}catch(clean_up_exit const&){
			throw;
		}catch(std::exception const& error){
			std::clog << error_message << ": " << error.what() << std::endl;
			throw clean_up_exit();
		}catch(...){
			std::clog << error_message << std::endl;
			throw clean_up_exit();
		}


	} }


	/// \brief Catch all exceptions
	/// Call the function and catch all exceptions throwing by the function. The name is emited
	/// via error_log together with the exception message.
	///
	/// As function the usage of a Lambda function is possible, which captures all variables by reference. ([&]{/* ... */})
	///
	/// If the Lambda function does not return anything, result will be a bool, indicating with false whether
	/// an exception appeared. Otherwise, the result will be a type that is convertible to bool. If and only
	/// if the conversion becomes true, accessability to the function result using member-function result()
	/// is permitted. Otherwise, result() will throw a std::logic_error.
	template < typename F, typename Body >
	inline auto exception_catcher(F&& f, Body&& body)
		-> typename impl::exception_catcher::control< decltype(body()) >::type
	{
		using Log = impl::log::extract_log_t< F >;

		Log log;
		return impl::exception_catcher::exception_catcher(log, std::forward< F >(f), std::forward< Body >(body));
	}


}

#endif
