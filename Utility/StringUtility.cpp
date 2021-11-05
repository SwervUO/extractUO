//Copyright © 2021 Charles Kerr. All rights reserved.

#include "StringUtility.hpp"

#include <algorithm>
#include <cctype>
#include <charconv>
#include <sstream>
#include <iomanip>
#include <time.h>
#include <filesystem>

namespace strutil {
	//=====================================================================
	// Define the constant "whitespace"
	// Whitespace is space,tab,vertical tab,feed,newline,carriage return
	const std::string whitespace = " \t\v\f\n\r";

	//=====================================================================
	// Trim utilities
	//=====================================================================
	//=====================================================================
	std::string rtrim(const std::string &value) {
		auto loc = value.find_last_not_of(whitespace) + 1;
		return value.substr(0,loc);
	}

	//=====================================================================
	std::string ltrim(const std::string &value) {
		auto loc = value.find_first_not_of(whitespace) ;
		if (loc == std::string::npos){
			return std::string();
		}
		return value.substr(loc);

	}
	
	//=====================================================================
	std::string trim(const std::string &value){
		return ltrim(rtrim(value));
	}

	//=====================================================================
	std::string simplify(const std::string &value) {
		auto temp = trim(value) ;
		auto append = false ;
		std::string rvalue ;
		for (auto &ch : temp){
			if (!std::isspace(static_cast<int>(ch))){
				append = true ;
				rvalue += ch;
			}
			else {
				if (append) {
					rvalue += ' ';
					append = false ;
				}
			}
		}
		return rvalue ;
	}
	//=====================================================================
	// Case utilities
	//=====================================================================
	//=====================================================================
	std::string upper(const std::string& value) {
		auto input = value ;
		std::transform(input.begin(), input.end(), input.begin(),
				   [](unsigned char c){ return std::toupper(c); } // correct
				   );
		return input;

	}
	
	//=====================================================================
	std::string lower(const std::string& value) {
		auto input = value ;
		std::transform(input.begin(), input.end(), input.begin(),
				   [](unsigned char c){ return std::tolower(c); } // correct
				   );
		return input;

	}
	
	//=====================================================================
	// String manipulation (remove remaining based on separator, split)
	//=====================================================================
	//=====================================================================
	std::string strip(const std::string &value , const std::string &sep,bool pack){
		auto loc = value.find(sep) ;
		auto rvalue =""s;
		if (loc == std::string::npos){
			rvalue = value;
		}
		else {
			rvalue = value.substr(0,loc) ;
		}
		if (pack) {
			rvalue = trim(rvalue) ;
		}
		return rvalue ;
	}
	
	//=====================================================================
	std::pair<std::string,std::string> split(const std::string &value , const std::string &sep){
		
		auto size = sep.size() ;
		auto first = value ;
		auto second = ""s ;
		
		auto loc = value.find(sep) ;
		if (loc != std::string::npos){
			first = trim(value.substr(0,loc)) ;
			loc = loc+size ;
			if (loc < value.size()){
				second = trim(value.substr(loc));
			}
		}
		
		return std::make_pair(first,second);
	}
/*
	//=====================================================================
	std::vector<std::string> parse(const std::string& value,const std::string& sep){
		std::vector<std::string> rvalue ;
		auto temp = value ;
		do {
			auto [first,second] = split(temp,",");
			rvalue.push_back(first);
			temp = second ;
		} while (!temp.empty());

		
		return rvalue;
	}

*/
	std::vector<std::string> parse(const std::string& value, const std::string& sep) {
		std::vector<std::string> rvalue ;
		
		std::vector<std::string::size_type> offsets;
		std::string::size_type loc = 0 ;
		do {
			loc = value.find(sep,loc) ;
			if (loc != std::string::npos) {
				offsets.push_back(loc) ;
				loc = loc+sep.size() ;
	
			}
		}while (loc < value.size()) ;
		loc = 0 ;
		for (auto index : offsets){
			rvalue.push_back(trim(value.substr(loc,index-loc)));
			loc = index + sep.size() ;
			
		}
		if (loc < value.size()){
			rvalue.push_back(trim(value.substr(loc)));
		}
		
		return rvalue ;
	}

	//================================================================
	bool isIntegral(const std::string &value) {
		auto temp = lower(trim(value)) ;
		if (temp.empty()){
			return false ;
		}
		if ((temp == "true")|| (temp=="false")){
			return true ;
		}
		if (temp.find("0x")==0){
			temp = temp.substr(2) ;
			if (temp.find_first_not_of("0123456789abcdef")!= std::string::npos){
				return false ;
			}
			return true;
		}
		else if (temp.find("0b")==0){
			temp = temp.substr(2) ;
			if (temp.find_first_not_of("01")!= std::string::npos){
				return false ;
			}
			return true ;
			
		}
		else if (temp.find("0o")==0){
			temp = temp.substr(2) ;
			if (temp.find_first_not_of("01234567")!= std::string::npos){
				return false ;
			}
			return true ;
			
		}
		else if (temp.find("0d")==0){
			temp = temp.substr(2) ;
			if (temp.find_first_not_of("0123456789")!= std::string::npos){
				return false ;
			}
			return true ;
			
		}
		else if (temp.find("-")==0) {
			temp = trim(temp.substr(1));
		}
		if (temp.find_first_not_of("0123456789")!= std::string::npos){
			return false ;
		}
		
		return true ;
	}
	//=======================================================================
	// Make some convience functions from strtonum to return a value (allows
	// user to use auto value = func(), versus having to define value ahead
	
	// Char methods
	unsigned char strtouc(const std::string &input,int base ){
		return strtonum<unsigned char>(input,base) ;
	}
	char strtoc(const std::string &input,int base ){
		return strtonum<char>(input,base) ;
	}
	
	// Short methods
	unsigned short strtous(const std::string &input,int base){
		return strtonum<unsigned short>(input,base) ;
	}
	short strtos(const std::string &input,int base ){
		return strtonum<short>(input,base) ;
	}
	
	// int methods
	unsigned int strtoui(const std::string &input,int base ){
		return strtonum<unsigned int>(input,base) ;
	}
	
	int strtoi(const std::string &input,int base){
		return strtonum<int>(input,base) ;
	}
	
	// long methods
	unsigned long strtoul(const std::string &input,int base ){
		return strtonum<unsigned long>(input,base) ;
	}
	long strtol(const std::string &input,int base ){
		return strtonum<long>(input,base) ;
	}
	
	// Long Long methods
	unsigned long long strtoull(const std::string &input,int base ){
		return strtonum<unsigned long long>(input,base) ;
	}
	long long strtoll(const std::string &input,int base){
		return strtonum<long long>(input,base) ;
	}
	
	// Bool
	bool strtob(const std::string &input,int base ){
		return strtonum<bool>(input,base) ;
	}

	//=====================================================================
	// Formatting utilites
	
	std::string format(const std::string& fmt) {
		return fmt ;
	}
	
	//=====================================================================
	// time utilites
	std::string timeToString(const std::chrono::system_clock::time_point &t)
	{
		std::time_t time = std::chrono::system_clock::to_time_t(t);
#if defined( _MSC_VER )
#pragma warning(disable : 4996)
#endif
		std::string time_str = std::ctime(&time);
#if defined( _MSC_VER )
#pragma warning(enable : 4996)
#endif
		time_str.resize(time_str.size() - 1);
		return time_str;
	}
	//=====================================================================
	std::chrono::system_clock::time_point stringToTime(const std::string &str,const std::string &format){
		std::stringstream timbuf(str);
		tm converted;
		
		timbuf >> std::get_time(&converted, format.c_str());
		
		auto ntime = mktime(&converted);
		return std::chrono::system_clock::from_time_t(ntime) ;
	}
	
	
	// This checks if the dir_file is a directory or file. If a directory, then it appends the file to it
	// Otherwise, it leaves it a lone.  Note, the directory must exist, or it will consider it a file.
	std::string completePath(const std::string &dir_file, const std::string &file){
		auto path = std::filesystem::path(dir_file) ;
		if (std::filesystem::is_directory(path)){
			// append the filename to the
			path = path / std::filesystem::path(file);
		}
		return path.string();
	}

}
