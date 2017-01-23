/// \file tools/name_generator.hpp
/// \author Benjamin Buch (benni.buch@gmail.com)
/// \date 22.04.2015
/// \brief class tools::name_generator
///
/// Copyright (c) 2015 Benjamin Buch (benni dot buch at gmail dot com)
///
/// Distributed under the Boost Software License, Version 1.0. (See accompanying
/// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
///
#ifndef _tools_name_generator_hpp_INCLUDED_
#define _tools_name_generator_hpp_INCLUDED_

#include <unordered_map>
#include <functional>
#include <sstream>
#include <utility>
#include <string>
#include <array>
#include <tuple>

#include <boost/variant.hpp>
#include <boost/config/warning_disable.hpp>
#include <boost/spirit/home/x3.hpp>
#include <boost/spirit/home/x3/directive.hpp>


namespace tools{


	template < typename T, typename ... U >
	struct first_of{
		using type = T;
	};

	template < typename T, typename ... U >
	using first_of_t = typename first_of< T, U ... >::type;


	namespace impl{ namespace name_generator{


		namespace x3 = boost::spirit::x3;
		using x3::char_;
		using x3::lit;

		struct variable_string: std::string{};

		x3::rule< class string, std::string > const string("string");
		x3::rule< class variable, variable_string > const variable("variable");

		auto const string_def =
			+(char_ - variable)
		;

		auto const variable_def =
			lit('$') >> '{' >> +(char_ - '}') >> '}'
		;

		BOOST_SPIRIT_DEFINE(
			string,
			variable
		)

		inline std::vector< boost::variant< std::string, std::size_t > > parse_pattern(std::string const& pattern, std::vector< std::string > const& variables){
			auto iter = pattern.begin();
			auto end = pattern.end();
			x3::ascii::space_type space;

			std::vector< boost::variant< std::string, variable_string > > parts;

			bool match = phrase_parse(iter, end, x3::no_skip[
				*(string | variable)
			], space, parts);

			if(!match || iter != end){
				throw std::runtime_error("Syntax error");
			}

			struct visitor: boost::static_visitor< boost::variant< std::string, std::size_t > >{
				visitor(std::vector< std::string > const& variables): variables(variables) {}

				std::vector< std::string > const& variables;

				boost::variant< std::string, std::size_t > operator()(std::string const& v){
					return v;
				}

				boost::variant< std::string, std::size_t > operator()(variable_string const& v){
					auto pos = static_cast< std::size_t >(std::find(variables.begin(), variables.end(), v) - variables.begin());
					if(pos >= variables.size()) throw std::runtime_error("Unknown variable '" + v + "'");
					return pos;
				}
			} v{variables};

			std::vector< boost::variant< std::string, std::size_t > > result;
			result.reserve(variables.size());
			for(auto& data: parts){
				result.push_back(boost::apply_visitor(v, data));
			}

			return result;
		}


	} }


	template < typename ... T >
	class name_generator{
	public:
		name_generator(
			std::string const& pattern,
			std::pair< std::string, std::function< std::string(T const&) > >&& ... variables
		):
			pattern_(impl::name_generator::parse_pattern(pattern, {std::move(variables.first) ...})),
			functions_{std::move(variables.second) ...}
			{}

		std::string operator()(T const& ... values)const{
			return get(std::index_sequence_for< T ... >(), values ...);
		}

		std::array< std::size_t, sizeof...(T) > use_count()const{
			std::array< std::size_t, sizeof...(T) > result{{0}};
			for(auto& data: pattern_){
				if(auto v = boost::get< std::size_t >(&data)){
					++result[*v];
				}
			}
			return result;
		}

	private:
		struct visitor: boost::static_visitor< std::string >{
			visitor(first_of_t< std::string, T >&& ... v):
				variables{{ std::move(v) ... }}
				{}

			std::array< std::string, sizeof...(T) > const variables;

			std::string operator()(std::string const& v){
				return v;
			}

			std::string operator()(std::size_t const& v){
				return variables[v];
			}
		};

		template < std::size_t ... I>
		std::string get(std::index_sequence< I ... >, T const& ... values)const{
			visitor v{
				std::get< I >(functions_)(values) ...
			};

			std::ostringstream os;
			for(auto& data: pattern_){
				os << boost::apply_visitor(v, data);
			}

			return os.str();
		}

		std::vector< boost::variant< std::string, std::size_t > > pattern_;
		std::tuple< std::function< std::string(T const&) > ... > functions_;
	};


	// TODO: replace by boost function types
	// -->
	template < typename F >
	struct argument{
	private:
		template < typename R, typename A >
		static A solve(R(F::*)(A));

		template < typename R, typename A >
		static A solve(R(F::*)(A)const);

		template < typename R, typename A >
		static A solve(R(F::*)(A)volatile);

		template < typename R, typename A >
		static A solve(R(F::*)(A)const volatile);

	public:
		using type = decltype(solve(&F::operator())) ;
	};

	template < typename R, typename A >
	struct argument< R(A) >{
		using type = A;
	};

	template < typename R, typename A >
	struct argument< R(*)(A) >{
		using type = A;
	};

	template < typename R, typename A >
	struct argument< R(&)(A) >{
		using type = A;
	};

	template < typename F >
	using argument_t = typename argument< F >::type;
	// <--


	template < typename String, typename ... T, typename ... U >
	auto make_name_generator(
		String&& pattern,
		std::pair< T, U >&& ... variables
	){
		return name_generator< argument_t< U > ... >(std::forward< String >(pattern), std::pair< std::string, std::function< std::string(argument_t< U > const&) > >(
			std::move(variables.first),
			std::move(variables.second)
		) ...);
	}

	template < typename String, typename ... T, typename ... U >
	auto make_name_generator(
		String&& pattern,
		std::array< bool, sizeof...(T) > const& must_have,
		std::pair< T, U >&& ... variables
	){
		auto result = name_generator< argument_t< U > ... >(pattern, std::pair< std::string, std::function< std::string(argument_t< U > const&) > >(
			variables.first,
			std::move(variables.second)
		) ...);

		std::array< std::string, sizeof...(T) > const variable{{ std::move(variables.first) ... }};

		auto const use_count = result.use_count();
		for(std::size_t i = 0; i < sizeof...(T); ++i){
			if(must_have[i] && use_count[i] == 0){
				throw std::runtime_error("Variable '" + variable[i] + "' must be used in " + std::string(pattern));
			}
		}

		return result;
	}

}


#endif
