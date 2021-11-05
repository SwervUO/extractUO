//Copyright © 2021 Charles Kerr. All rights reserved.

#include "MapTerrain.hpp"
#include "StringUtility.hpp"

#include <iostream>
#include <fstream>
#include <filesystem>

#include "Buffer.hpp"
using namespace std::string_literals;
namespace UO {
	//===============================================================
	const std::string MapTerrain::_hash_format = "build/map{}legacymul/{8}.dat"s;
	const std::string MapTerrain::_mul_format ="map{}.mul"s ;
	const std::string MapTerrain::_uop_format ="map{}LegacyMUL.uop"s ;
	
	
	//===============================================================
	void MapTerrain::buildStrings(std::size_t mapnumber){
		auto mapnum = std::to_string(mapnumber);
		_hashformat = strutil::format(_hash_format,mapnum);
		_mulfile = strutil::format(_mul_format,mapnum);
		_uopfile = strutil::format(_uop_format,mapnum);
	}

	//===============================================================
	void MapTerrain::processBlock(std::size_t blocknum, const std::vector<std::uint8_t> &data){
	
		Buffer block(data) ;
		std::uint32_t header = 0;
		std::uint16_t tileid = 0;
		std::int8_t alt = 0;
		// Strip off the header
		block>>header ;
		for (auto y=0 ; y< 8 ; y++){
			for (auto x= 0 ; x<8 ; x++){
				block >> tileid ;
				block >> alt ;
			}
		}
	}
	//===============================================================
	bool MapTerrain::processEntry(std::size_t entry, std::size_t index, const std::vector<std::uint8_t> &data) {
		auto block = index * 0xc4000 ;
		Buffer column(data) ;
		auto number_of_block = data.size()/196 ;
		for (auto i=0 ;i < number_of_block;i++) {
			std::uint32_t header ;
			
		}
		
		return true ;
	}
	
	//===============================================================
	void MapTerrain::readMul(const std::string &mulpath){
		_usedUOP = false ;
	}

	
	
	//===============================================================
	bool MapTerrain::load(const std::string &datapath,std::size_t mapnumber){
		_usedUOP = true;
		buildStrings(mapnumber);
		
		auto path = std::filesystem::path(datapath) ;
		auto uopfile = strutil::format(_uop_format,std::to_string(mapnumber));
		auto mulfile = strutil::format(_mul_format,std::to_string(mapnumber));
		if (std::filesystem::is_directory(path)){
			// It is a directory!
			// Try UOP first
		}
		if (path.extension()== ".uop"s){
			// it is an uop!
		}
		else {
			// It must be a mul?
			readMul(datapath);
		}
		return false ;
	}
}
