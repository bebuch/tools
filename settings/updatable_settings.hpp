/// \file tools/updatable_settings.hpp
/// \author Benjamin Buch (benni.buch@gmail.com)
/// \date 10.07.2013
/// \brief Class template tools::updatable_settings
///
/// Copyright (c) 2013-2015 Benjamin Buch (benni dot buch at gmail dot com)
///
/// Distributed under the Boost Software License, Version 1.0. (See accompanying
/// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
///
#ifndef _tools_updatable_settings_hpp_INCLUDED_
#define _tools_updatable_settings_hpp_INCLUDED_

#include "make_string.hpp"
#include "settings.hpp"
#include "exception_catcher.hpp"

#include <boost/filesystem.hpp>

#include <limits>


namespace tools {


	template < typename Log >
	class local_updatable_settings: public local_settings< Log >{
	public:
		local_updatable_settings(std::string const& filename):
			local_settings< Log >(filename){}


		template < typename T >
		T get(std::pair< std::string, std::string > const& option)const{
			return local_settings< Log >::template get< T >(option);
		}

		template < typename T >
		T get(std::string const& section, std::string const& key)const{
			return local_settings< Log >::template get< T >(section, key);
		}

		template < typename T >
		T get(std::pair< std::string, std::string > const& option, T&& default_value){
			return get(std::get< 0 >(option), std::get< 1 >(option), std::forward< T >(default_value));
		}

		template < typename T >
		T get(std::string const& section, std::string const& key, T&& default_value){
			return log([&](Log& os){ os << this->filename_ << " get [" << section << "]." << key << " (default: '" << default_value << "')"; }, [&](){
				auto result = this->template do_get_optional< T >(section, key);
				if(result) return *result;

				insert(section, key, std::forward< T >(default_value));
				return default_value;
			});
		}

		template < typename T >
		boost::optional< T > get_optional(std::pair< std::string, std::string > const& option)const{
			return local_settings< Log >::template get_optional< T >(option);
		}

		template < typename T >
		boost::optional< T > get_optional(std::string const& section, std::string const& key)const{
			return local_settings< Log >::template get_optional< T >(section, key);
		}

		template < typename T >
		std::unordered_map< std::string, T > section(std::string const& name)const{
			return local_settings< Log >::template section< T >(name);
		}


		template < typename T >
		void put(std::pair< std::string, std::string > const& option, T&& value){
			put(std::get< 0 >(option), std::get< 1 >(option), std::forward< T >(value));
		}

		template < typename T >
		void put(std::string const& section, std::string const& key, T&& value){
			std::lock_guard< std::mutex > lock(this->mutex_);

			log([&](Log& os){ os << this->filename_ << " put [" << section << "]." << key << " = " << value; }, [&]{
				insert(section, key, std::forward< T >(value));
			});
		}

		void rewrite(){
			auto& mutex = this->mutex_;
			auto& data = this->data_;
			auto list = exception_catcher([this](Log& os){ os << "Generate '" << this->filename_ << "'"; }, [&]{
				std::lock_guard< std::mutex > lock(mutex);

				typedef std::tuple< key_type, value_type, line_type, comment_type > option_type;
				typedef std::vector< option_type > option_list_type;
				typedef std::tuple< section_type, option_list_type, line_type, comment_type > settings_type;
				typedef std::vector< settings_type > settings_list_type;

				settings_list_type list;
				list.reserve(data.size());
				for(auto const& entry: data){
					option_list_type elements;
					list.reserve(std::get< 0 >(entry.second).size());
					for(auto const& option: std::get< 0 >(entry.second)){
						auto const& key     = option.first;
						auto const& value   = std::get< 0 >(option.second);
						auto const& line    = std::get< 1 >(option.second);
						auto const& comment = std::get< 2 >(option.second);

						elements.emplace_back(option_type(key, value, line, comment));
					}

					std::sort(elements.begin(), elements.end(), [](option_type const& lhs, option_type const& rhs){
						return std::get< 2 >(lhs) < std::get< 2 >(rhs);
					});

					auto const& section = entry.first;
					auto const& line    = std::get< 1 >(entry.second);
					auto const& comment = std::get< 2 >(entry.second);

					list.emplace_back(settings_type(section, elements, line, comment));
				}

				std::sort(list.begin(), list.end(), [](settings_type const& lhs, settings_type const& rhs){
					return std::get< 2 >(lhs) < std::get< 2 >(rhs);
				});

				return list;
			});

			if(!list) return;

			tools::exception_catcher([this](Log& os){ os << "Write '" << this->filename_ << "'"; }, [&]{
				std::string name;
				std::string ext;

				auto pos = this->filename_.find('.');
				if(pos == std::string::npos) {
					name = this->filename_;
				} else {
					name = this->filename_.substr(0, pos);
					ext = this->filename_.substr(pos);
				}

				std::lock_guard< std::mutex > lock(write_mutex_);

				std::string update_name = name + "_update" + ext;
				std::ofstream os(update_name);
				if(!os || !os.is_open()){
					throw std::runtime_error("Can not open " + update_name);
				}

				for(auto const& section: list.result()){
					os << std::get< 3 >(section) << "[" << std::get< 0 >(section) << "]\n";
					for(auto const& option: std::get< 1 >(section)){
						os << std::get< 3 >(option) << std::get< 0 >(option) << " = " << std::get< 1 >(option) << "\n";
					}
				}

				if(!os) throw std::runtime_error("Error while writing " + update_name);

				os.close();

				std::string backup_name = name + "_backup" + ext;
				boost::filesystem::rename(this->filename_, backup_name);
				boost::filesystem::rename(update_name, this->filename_);
				boost::filesystem::remove(backup_name);
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


		template < typename T >
		void insert(std::string const& section, std::string const& key, T&& value){
			auto v = make_string(std::forward< T >(value));

			option_list_type& options =
				std::get< 0 >(
					this->data_.insert(std::make_pair(
						section,
						std::make_tuple(
							option_list_type(),
							std::numeric_limits< std::size_t >::max(),
							""
						)
					)).first->second
				);
			auto option =
				options.insert(std::make_pair(
					key,
					std::make_tuple(
						v,
						std::numeric_limits< std::size_t >::max(),
						""
					)
				));

			if(!option.second){
				std::get< 0 >(option.first->second) = v;
			}
		}


		std::mutex write_mutex_;
	};


	namespace config{ namespace settings{


		struct updatable_settings_instance{
			static local_updatable_settings< log >& instance();
		};


	} }


	template < typename Instance = config::settings::updatable_settings_instance >
	struct updatable_settings{
		template < typename T >
		static T get(std::pair< std::string, std::string > const& option){
			return Instance::instance().template get< T >(option);
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
		static void put(std::pair< std::string, std::string > const& option, T&& value){
			Instance::instance().put(std::get< 0 >(option), std::get< 1 >(option), std::forward< T >(value));
		}

		template < typename T >
		static void put(std::string const& section, std::string const& key, T&& value){
			Instance::instance().put(section, key, std::forward< T >(value));
		}

		template < typename T >
		static std::unordered_map< std::string, T > section(std::string const& name){
			return Instance::instance().template section< T >(name);
		}

		static void rewrite(){
			Instance::instance().rewrite();
		}


		template < typename ErrorLog >
		class exit_if_fail {
		public:
			template < typename T >
			static T get(std::pair< std::string, std::string > const& option){
				return catcher([&option]{ return updatable_settings::get< T >(option); });
			}

			template < typename T >
			static T get(std::string const& section, std::string const& key){
				return catcher([&section, &key]{ return updatable_settings::get< T >(section, key); });
			}

			template < typename T >
			static T get(std::pair< std::string, std::string > const& option, T&& default_value){
				return catcher([&option, &default_value]{ return updatable_settings::get(option, std::forward< T >(default_value)); });
			}

			template < typename T >
			static T get(std::string const& section, std::string const& key, T&& default_value){
				return catcher([&section, &key, &default_value]{ return updatable_settings::get(section, key, std::forward< T >(default_value)); });
			}

			template < typename T >
			static T get_optional(std::pair< std::string, std::string > const& option){
				return catcher([&option]{ return updatable_settings::get_optional< T >(option); });
			}

			template < typename T >
			static T get_optional(std::string const& section, std::string const& key){
				return catcher([&section, &key]{ return updatable_settings::get_optional< T >(section, key); });
			}

			template < typename T >
			static std::unordered_map< std::string, T > section(std::string const& name){
				return catcher([&name]{ return updatable_settings::section< T >(name); });
			}

		private:
			template < typename F >
			static auto catcher(F const& f) -> decltype(f()) try{
				auto result = tools::exception_catcher([](ErrorLog& os) { os << "exit if exception caught"; }, [&f] {
					return f();
				});

				if(!result) std::exit(1);

				return result.result();
			}catch(std::exception const& error){
				std::cerr << error.what() << std::endl;
				std::exit(1);
			}catch(...){
				std::exit(1);
			}
		};
	};


}


#endif
