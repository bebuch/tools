/// \file tools/exec.hpp
/// \author Benjamin Buch (benni.buch@gmail.com)
/// \brief Execute a progam and parse or log its output
///
/// Copyright (c) 2014-2015 Benjamin Buch (benni dot buch at gmail dot com)
///
/// Distributed under the Boost Software License, Version 1.0. (See accompanying
/// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
///
#ifndef _tools_exec_hpp_INCLUDED_
#define _tools_exec_hpp_INCLUDED_

#include "settings.hpp"
#include "get_standard_output.hpp"

#define NOMINMAX 1
#define BOOST_SPIRIT_THREADSAFE
#define BOOST_SPIRIT_SINGLE_GRAMMAR_INSTANCE
#include <boost/property_tree/xml_parser.hpp>
#undef interface


namespace tools {

	
	/// \brief Mask all special Characters in a string
	inline std::string mask_non_print(std::string const& str) {
		std::string result;
		for(auto c : str) {
			switch(c) {
			case 0: result += "\\0"; break;
			case 1: result += "\\x01"; break;
			case 2: result += "\\x02"; break;
			case 3: result += "\\x03"; break;
			case 4: result += "\\x04"; break;
			case 5: result += "\\x05"; break;
			case 6: result += "\\x06"; break;
			case 7: result += "\\a"; break;
			case 8: result += "\\b"; break;
			case 9: result += "\\t"; break;
			case 10: result += "\\n"; break;
			case 11: result += "\\v"; break;
			case 12: result += "\\f"; break;
			case 13: result += "\\r"; break;
			case 14: result += "\\0E"; break;
			case 15: result += "\\0F"; break;
			case 16: result += "\\10"; break;
			case 17: result += "\\11"; break;
			case 18: result += "\\12"; break;
			case 19: result += "\\13"; break;
			case 20: result += "\\14"; break;
			case 21: result += "\\15"; break;
			case 22: result += "\\16"; break;
			case 23: result += "\\17"; break;
			case 24: result += "\\18"; break;
			case 25: result += "\\19"; break;
			case 26: result += "\\1A"; break;
			case 27: result += "\\e"; break;
			case 28: result += "\\1C"; break;
			case 29: result += "\\1D"; break;
			case 30: result += "\\1E"; break;
			case 31: result += "\\1F"; break;
			case '\\': result += "\\\\"; break;
			default: result += c;
			}
		}
		return result;
	}


	/// \brief Execute a program, parse output as XML and add it as child to pt
	template < typename Log >
	inline void exec_and_add_xml(std::string program, std::shared_ptr< boost::property_tree::ptree > pt) {
		boost::algorithm::trim(program);
		if(program.empty()) return;
		tools::log([&program](Log& os){ os << "Call program '" << program << "' and read standard output as XML"; }, [&]{
			auto xml = tools::get_standard_output(program);
			std::istringstream is(xml);
			boost::property_tree::ptree add;
			try{
				read_xml(is, add, boost::property_tree::xml_parser::trim_whitespace);
			}catch(std::exception const& error) {
				throw std::runtime_error(
					std::string("read_xml: ") +
					error.what() +
					"; Program '" +
					program +
					"' output: " +
					mask_non_print(xml)
				);
			}
			for(auto const& node : add) {
				pt->add_child(node.first, node.second);
			}
		});
	}

	/// \brief Execute a program, parse output as XML and add it as child to pt
	///
	/// All parameters are converted to strings and separated by a space added as parameters for the program.
	/// There is no protected via "" for the parameters!
	template < typename Log, typename ... Params >
	inline void exec_and_add_xml(std::string program, std::shared_ptr< boost::property_tree::ptree > pt, Params&& ... params) {
		boost::algorithm::trim(program);
		if(program.empty()) return;
		exec_and_add_xml< Log >(program + " " + make_string_separated_by(' ', std::forward< Params >(params) ...), pt);
	}


	/// \brief Execute a program and add output to the given Log-Stream
	///
	/// All parameters are converted to strings and separated by a space added as parameters for the program.
	/// There is no protected via "" for the parameters!
	template < typename Log, typename ... Params >
	inline void exec_and_add_to_log(std::string program, Params&& ... params) {
		boost::algorithm::trim(program);
		if(program.empty()) return;
		program += " " + make_string_separated_by(' ', std::forward< Params >(params) ...);
		tools::log([&program](Log& os){
			os << "Call program '" << program << "'" << ", Output of program '" << program << "': " << mask_non_print(tools::get_standard_output(program));;
		});
	}


}


#endif
