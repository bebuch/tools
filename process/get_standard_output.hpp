/// \file tools/get_standard_output.hpp
/// \author Benjamin Buch (benni.buch@gmail.com)
/// \brief Execute a progam and get its output
///
/// Copyright (c) 2014-2015 Benjamin Buch (benni dot buch at gmail dot com)
///
/// Distributed under the Boost Software License, Version 1.0. (See accompanying
/// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
///
#ifndef _tools_get_standard_output_hpp_INCLUDED_
#define _tools_get_standard_output_hpp_INCLUDED_

#include <string>

#include <boost/process.hpp>
#include <boost/iostreams/device/file_descriptor.hpp>
#include <boost/iostreams/stream.hpp>


namespace tools {


	inline std::string get_standard_output(std::string const& cmd) {
		using namespace boost::iostreams;
		using namespace boost::process::initializers;
		using namespace boost::process;
		auto pipe = create_pipe();
		{
			file_descriptor_sink sink(pipe.sink, close_handle);
			execute(set_cmd_line(cmd), bind_stdout(sink), bind_stderr(sink));
		}
		file_descriptor_source source(pipe.source, close_handle);
		stream< file_descriptor_source > is(source);
		std::string result;
		for(std::string line; std::getline(is, line);) result += line + "\n";
		return result;
	}


}


#endif
