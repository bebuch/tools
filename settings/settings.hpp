/// \file tools/settings.hpp
/// \author Benjamin Buch (benni.buch@gmail.com)
/// \date 10.07.2013
/// \brief Using ini-Files
///
/// Copyright (c) 2013-2015 Benjamin Buch (benni dot buch at gmail dot com)
///
/// Distributed under the Boost Software License, Version 1.0. (See accompanying
/// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
///
#ifndef _tools_settings_hpp_INCLUDED_
#define _tools_settings_hpp_INCLUDED_

#include "string_to.hpp"
#include "exception_catcher.hpp"
#include "timed_log.hpp"

#include <boost/algorithm/string/trim.hpp>
#include <boost/optional.hpp>

#include <mutex>
#include <unordered_map>
#include <tuple>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <stdexcept>


namespace tools {


	template < typename Log >
	class local_settings{
	public:
		local_settings(std::string const& filename):
			filename_(filename)
		{
			log([this](Log& os){ os << "Read ini-File '" << filename_ << "'"; }, [this]{
				std::ifstream is(filename_.c_str());

				if(!is || !is.is_open()){
					throw std::runtime_error("Can't open " + filename_);
				}

				std::size_t i = 0;
				for(std::string line, section, comment; getline(is, line); ++i){
					boost::algorithm::trim(line);

					if(line.empty()){
						comment += "\n";
					}else if(line[0] == '[' && line[line.length() - 1] == ']'){
						section = boost::algorithm::trim_copy(line.substr(1, line.length() - 2));

						auto iter = std::get< 0 >(data_.insert(
							std::make_pair(section, std::make_tuple(option_list_type(), i, ""))
						));

						std::get< 2 >(iter->second) += comment;
						comment = "";
					}else if(line[0] == '#' || line[0] == ';'){
						comment += line + "\n";
					}else{
						auto split = find(line.begin(), line.end(), '=');
						std::string key(line.begin(), split);
						std::string value(split, line.end());

						if(!value.empty()){
							value = value.substr(1);
						}

						boost::algorithm::trim(key);
						boost::algorithm::trim(value);

						log([&](Log& os){ os << filename_ << " register [" << section << "]." << key << " = " << value; }, [&]{
							std::get< 0 >(data_[section]).insert(
								make_pair(key, std::make_tuple(value, i, comment))
							);

							comment = "";
						});
					}
				}
			});
		}


		template < typename T >
		T get(std::pair< std::string, std::string > const& option)const{
			return get< T >(std::get< 0 >(option), std::get< 1 >(option));
		}

		template < typename T >
		T get(std::string const& section, std::string const& key)const{
				auto result = get_optional< T >(section, key);
				if(result) return *result;

				throw std::runtime_error(filename_ + ": No such node ([" + section + "]." + key + ")");
		}

		template < typename T >
		T get(std::pair< std::string, std::string > const& option, T&& default_value)const{
			return get< T >(std::get< 0 >(option), std::get< 1 >(option), std::forward< T >(default_value));
		}

		template < typename T >
		T get(std::string const& section, std::string const& key, T&& default_value)const{
			return log([&](Log& os){ os << filename_ << " get [" << section << "]." << key << " (default: '" << default_value << "')"; }, [&](){
				auto result = do_get_optional< T >(section, key);
				return result ? *result : default_value;
			});
		}


		template < typename T >
		boost::optional< T > get_optional(std::pair< std::string, std::string > const& option)const{
			return get_optional< T >(std::get< 0 >(option), std::get< 1 >(option));
		}

		template < typename T >
		boost::optional< T > get_optional(std::string const& section, std::string const& key)const{
			return log([&](Log& os){ os << filename_ << " get [" << section << "]." << key; }, [&](){
				return do_get_optional< T >(section, key);
			});
		}

		template < typename T >
		std::unordered_map< std::string, T > section(std::string const& name)const{
			std::lock_guard< std::mutex > lock(mutex_);

			return log([this, &name](Log& os){ os << filename_ << " get complete section [" << name << "]"; }, [this, &name]{
				std::unordered_map< key_type, T > result;

				auto iter = data_.find(name);
				if(iter == data_.end()) return result;

				for(auto const& entry: std::get< 0 >(iter->second)){
					try{
						result.emplace(entry.first, string_to< T >(std::get< 0 >(entry.second)));
					}catch(std::runtime_error const& error){
						throw std::runtime_error(
							filename_ + " [" + name + "]." + entry.first + " = " +
							std::get< 0 >(entry.second) + ": " + error.what()
						);
					}
				}

				return result;
			});
		}


	protected:
		typedef std::string section_type;
		typedef std::string key_type;
		typedef std::string value_type;
		typedef std::string comment_type;
		typedef std::size_t line_type;

		typedef std::tuple< value_type, line_type, comment_type > option_type;
		typedef std::unordered_map< key_type, option_type > option_list_type;
		typedef std::tuple< option_list_type, line_type, comment_type > extended_option_list_type;

		typedef std::unordered_map< section_type, extended_option_list_type > settings_type;

		std::mutex mutable mutex_;

		std::string const filename_;

		settings_type data_;

		template < typename T >
		boost::optional< T > do_get_optional(std::string const& section, std::string const& key)const{
			std::lock_guard< std::mutex > lock(mutex_);

			auto iter = data_.find(section);
			if(iter == data_.end()) return boost::none;

			auto const& option_list = std::get< 0 >(iter->second);
			auto option_iter = option_list.find(key);
			if(option_iter == option_list.end()) return boost::none;

			try{
				return string_to< T >(std::get< 0 >(option_iter->second));
			}catch(std::runtime_error const& error){
				throw std::runtime_error(
					filename_ + " [" + section + "]." + key + " = " +
					std::get< 0 >(option_iter->second) + ": " + error.what()
				);
			}
		}
	};


	namespace config{ namespace settings{


		struct log: timed_log< log >{
			static void output(std::string&& str);
		};

		struct settings_instance{
			static local_settings< log >& instance();
		};


	} }


	template < typename Instance = config::settings::settings_instance >
	struct settings{
		template < typename T >
		static T get(std::pair< std::string, std::string > const& option) {
			return Instance::instance().template get< T >(std::get< 0 >(option), std::get< 1 >(option));
		}

		template < typename T >
		static T get(std::string const& section, std::string const& key){
			return Instance::instance().template get< T >(section, key);
		}

		template < typename T >
		static T get(std::pair< std::string, std::string > const& option, T&& default_value){
			return Instance::instance().get(std::get< 0 >(option), std::get< 1 >(option), std::forward< T >(default_value));
		}

		template < typename T >
		static T get(std::string const& section, std::string const& key, T&& default_value){
			return Instance::instance().get(section, key, std::forward< T >(default_value));
		}

		template < typename T >
		static T get_optional(std::pair< std::string, std::string > const& option){
			return Instance::instance().template get_optional< T >(std::get< 0 >(option), std::get< 1 >(option));
		}

		template < typename T >
		static T get_optional(std::string const& section, std::string const& key){
			return Instance::instance().template get_optional< T >(section, key);
		}

		template < typename T >
		static std::unordered_map< std::string, T > section(std::string const& name){
			return Instance::instance().template section< T >(name);
		}

		template < typename ErrorLog >
		class exit_if_fail{
		public:
			template < typename T >
			static T get(std::pair< std::string, std::string > const& option){
				return catcher([&option]{ return settings::get< T >(option); });
			}

			template < typename T >
			static T get(std::string const& section, std::string const& key){
				return catcher([&section, &key] { return settings::get< T >(section, key); });
			}

			template < typename T >
			static T get(std::pair< std::string, std::string > const& option, T&& default_value){
				return catcher([&option, &default_value]{ return settings::get(option, std::forward< T >(default_value)); });
			}

			template < typename T >
			static T get(std::string const& section, std::string const& key, T&& default_value){
				return catcher([&section, &key, &default_value]{ return settings::get(section, key, std::forward< T >(default_value)); });
			}

			template < typename T >
			static T get_optional(std::pair< std::string, std::string > const& option){
				return catcher([&option]{ return settings::get_optional< T >(option); });
			}

			template < typename T >
			static T get_optional(std::string const& section, std::string const& key){
				return catcher([&section, &key]{ return settings::get_optional< T >(section, key); });
			}

			template < typename T >
			static std::unordered_map< std::string, T > section(std::string const& name){
				return catcher([&name]{ return settings::section< T >(name); });
			}

		private:
			template < typename F >
			static auto catcher(F const& f) -> decltype(f()) try {
				auto result = tools::exception_catcher([](ErrorLog& os) { os << "exit if exception caught"; }, [&f] {
					return f();
				});

				if(!result) std::exit(1);

				return result.result();
			} catch(std::exception const& error) {
				std::cerr << error.what() << std::endl;
				std::exit(1);
			} catch(...) {
				std::exit(1);
			}
		};
	};


}


#endif
