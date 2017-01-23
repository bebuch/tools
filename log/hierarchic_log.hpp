#ifndef _tools_hierarchic_log_hpp_INCLUDED_
#define _tools_hierarchic_log_hpp_INCLUDED_

#include "log.hpp"

#include <thread>
#include <atomic>
#include <vector>
#include <unordered_map>


namespace tools{


	namespace impl{ namespace hierarchic_log{


		inline std::vector< std::size_t >& version_stack(){
			thread_local std::vector< std::size_t > value{0};
			return value;
		}

		inline std::vector< std::size_t > add_version(){
			std::vector< std::size_t >& stack = version_stack();

			++stack.back();

			auto result = stack;
			stack.push_back(0);

			return result;
		}

		inline void erase_version(){
			std::vector< std::size_t >& stack = version_stack();
			stack.pop_back();
		}

		inline std::size_t get_thread_number(std::thread::id id){
			static std::atomic_size_t counter(0);
			static std::unordered_map< std::thread::id, std::size_t > table;
			auto iter = table.find(id);
			if(iter == table.end()) return table[id] = counter++;
			return iter->second;
		}


	} }


	struct hierarchic_log_base{
		hierarchic_log_base(): version(impl::hierarchic_log::add_version()) {}
		~hierarchic_log_base(){ impl::hierarchic_log::erase_version(); }

		void first(std::ostringstream& os)const{
			os << std::setfill('0') << std::setw(4) << impl::hierarchic_log::get_thread_number(std::this_thread::get_id()) << ":";
		}

		void prefix(std::ostringstream& os)const{
			if(version.size() > 0) os << version[0];
			for(std::size_t i = 1; i < version.size(); ++i) os << '.' << version[i];
			os << ' ';
		}

		std::vector< std::size_t > version;
	};

	struct hierarchic_log: hierarchic_log_base, log_base{
		using hierarchic_log_base::first;
		using hierarchic_log_base::prefix;
	};


}


#endif
