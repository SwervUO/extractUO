//Copyright © 2021 Charles Kerr. All rights reserved.
#ifndef StringUtility_hpp
#define StringUtility_hpp
#include <iostream>

#include <string>
#include <type_traits>
#include <array>
#include <vector>
#include <charconv>
#include <utility>
#include <chrono>
#include <sstream>

using namespace std::string_literals ;

namespace strutil {
	extern const std::string whitespace ;
	constexpr auto max_number_size = 10000;
	//=====================================================================
	// Trim utilities
	//=====================================================================
	
	//========================================================================
	std::string ltrim(const std::string &value) ;
	//========================================================================
	std::string rtrim(const std::string &value) ;
	//========================================================================
	std::string trim(const std::string &value);
	//========================================================================
	std::string simplify(const std::string &value);
	
	//=====================================================================
	// Case utilities
	//=====================================================================
	
	//========================================================================
	std::string upper(const std::string &value) ;
	//========================================================================
	std::string lower(const std::string &value) ;
	
	//=====================================================================
	// String manipulation (remove remaining based on separator, split,parse)
	//=====================================================================
	//========================================================================
	std::string strip(const std::string &value , const std::string &sep="//",bool pack = true);
	//========================================================================
	std::pair<std::string,std::string> split(const std::string &value , const std::string &sep=",");
	//========================================================================
	std::vector<std::string> parse(const std::string& value,
						 const std::string& sep = ",");
	
	//=====================================================================
	// Number to/from String utilities
	//=====================================================================
	
	//========================================================================
	//Handle bool conversion to string
	template <typename T>
	typename std::enable_if< std::is_same_v<T,bool>,std::string>::type
	numtostr(T value, int base = 10, bool base_indicator = false,
		   int min_char=0){
		if (value) {
			return "true"s;
		}
		else {
			return "false"s;
		}
		
	}
	
	//========================================================================
	//Handle integer conversion to string
	template <typename T>
	typename std::enable_if<std::is_integral_v<T> && !std::is_same_v<T,bool>,std::string>::type
	numtostr(T value, int base = 10, bool base_indicator = false,
		   int min_char=0,char pad='0'){
		std::array<char,max_number_size> str ;
		if (base == 0) {
			base = 10 ;
		}
		if (auto [pc,ec] = std::to_chars(str.data(), str.data()+str.size(), value,base) ;
		    ec == std::errc()){
			// Ok was successful, now make the string ;
			auto rvalue = std::string(str.data(),pc) ;
			// Check if we need to pad with zeros//
			if (rvalue.size()< min_char) {
				auto head = std::string(min_char - rvalue.size(),pad) ;
				rvalue = head+rvalue ;
			}
			// do we append the base?
			if (base_indicator) {
				switch (base) {
					case 10:
						rvalue = "0d"s + rvalue ;
						break;
					case 16:
						rvalue = "0x"s + rvalue ;
						break;
					case 2:
						rvalue = "0b"s + rvalue ;
						break;
					case 8:
						rvalue = "0o"s + rvalue ;
						break;
						
					default:
						break;
				}
			}
			return rvalue ;
		}
		else {
			// Failed to convert
			return "0"s;
		}
	}
	
	//========================================================================
	//Handle string conversion to integer
	template <typename T>
	typename std::enable_if<std::is_integral_v<T> && !std::is_same_v<T,bool>,T>::type
	strtonum(const std::string &input,  int base = 10){
		T rvalue;
		auto temp = trim(lower(input));
		if (temp == "true"s){
			rvalue = static_cast<T>(1) ;
			return rvalue;
		}
		else if (temp == "false"s){
			rvalue = static_cast<T>(0) ;
			return rvalue ;
		}
		// we are now to "true" Integers
		int embedded_base = 0 ;
		if (temp.find("0x"s) == 0){
			embedded_base = 16;
			temp = temp.substr(2);
		}
		else if (temp.find("0d"s) == 0){
			embedded_base = 10 ;
			temp = temp.substr(2);
		}
		else if (temp.find("0o"s) == 0){
			embedded_base = 8 ;
			temp = temp.substr(2);
		}
		else if (temp.find("0b"s) == 0){
			
			embedded_base = 2 ;
			temp = temp.substr(2);
		}
		
		if (embedded_base != 0) {
			base = embedded_base;
			
		}
		if (base == 0){
			base = 10 ;
		}
		
		
		if (auto [msg,ec] = std::from_chars(temp.data(), temp.data()+temp.size(), rvalue, base);
		    ec == std::errc()){
			
			return rvalue ;
		}
		else if (ec == std::errc::invalid_argument)
		{
			//std::cout << "That isn't a number.\n";
			rvalue = 0;
			return rvalue ;
		}
		else if (ec == std::errc::result_out_of_range)
		{
			//std::cout << "This number is larger than an supplied value.\n";
			rvalue = 0;
			return rvalue ;
		}
		else {
			// Unknown error
			rvalue = 0;
			return rvalue;
		}
	}
	//========================================================================
	//Handle string conversion to bool
	template <typename T>
	typename std::enable_if<std::is_integral_v<T> && std::is_same_v<T,bool>,T>::type
	strtonum(const std::string &input, int base = 10){
		auto temp = trim(lower(input));
		if (temp == "true"s){
			return true;
		}
		else if (temp == "false"s) {
			return false;
		}
		else {
			return strtonum<unsigned long long>(input,base) != 0;
		}
	}
	
	//=======================================
	bool isIntegral(const std::string &value);

	//=======================================================================
	// Make some convience functions from strtonum to return a value (allows
	// user to use auto value = func(), versus having to define value ahead
	
	// Char methods
	unsigned char strtouc(const std::string &input,int base = 10);
	
	char strtoc(const std::string &input,int base = 10);
	
	// Short methods
	unsigned short strtous(const std::string &input,int base = 10);
	short strtos(const std::string &input,int base = 10);
	
	// int methods
	unsigned int strtoui(const std::string &input,int base = 10);
	int strtoi(const std::string &input,int base = 10);
	
	// long methods
	unsigned long strtoul(const std::string &input,int base = 10);
	long strtol(const std::string &input,int base = 10);
	
	// Long Long methods
	unsigned long long strtoull(const std::string &input,int base = 10);
	long long strtoll(const std::string &input,int base = 10);
	
	// Bool
	bool strtob(const std::string &input,int base = 10);
	
	//===================================================================
	//  string formatting utilities
	
	// Define base format, which does nothing but return the call
	std::string format(const std::string &fmt) ;
	
	// And now the real format workhorse
	template<typename T, typename... Targs>
	std::string
	format(const std::string &fmt,  T value, Targs... Fargs){
		
		std::stringstream rvalue ;
		auto input = fmt ;
		auto loc = input.find("{}") ;
		if (  loc != std::string::npos) {
			rvalue << input.substr(0,loc);
			rvalue << value ;
			if ((loc+2) < input.size()) {
				rvalue << format(input.substr(loc+2),Fargs...) ;
				return rvalue.str() ;
			}
		}
		return input ;

	}
	
	std::string timeToString(const std::chrono::system_clock::time_point &t);
	std::chrono::system_clock::time_point stringToTime(const std::string &str, const std::string &format = "%a %b %d %H:%M:%S %Y"s);
	
	// This checks if the dir_file is a directory or file. If a directory, then it appends the file to it
	// Otherwise, it leaves it a lone.  Note, the directory must exist, or it will consider it a file.
	std::string completePath(const std::string &dir_file, const std::string &file);
}

#endif /* StringUtility_hpp */
