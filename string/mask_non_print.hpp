/// \file tools/mask_non_print.hpp
/// \author Benjamin Buch (benni.buch@gmail.com)
/// \date 22.03.2015
/// \brief Mask all non print characters in a string
///
/// Copyright (c) 2015 Benjamin Buch (benni dot buch at gmail dot com)
///
/// Distributed under the Boost Software License, Version 1.0. (See accompanying
/// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
///
#ifndef _tools_mask_non_print_hpp_INCLUDED_
#define _tools_mask_non_print_hpp_INCLUDED_

#include <string>


namespace tools{


	inline std::string mask_non_print(std::string const& str){
		std::string result;
		for(auto c: str){
			switch(c){
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


}


#endif
