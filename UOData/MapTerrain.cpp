//Copyright © 2021 Charles Kerr. All rights reserved.

#include "MapTerrain.hpp"
#include "StringUtility.hpp"

#include <iostream>
#include <fstream>
#include <filesystem>
#include <algorithm>
#include "UOAlerts.hpp"
#include "Buffer.hpp"
#include "RadarColor.hpp"
#include "TileInfo.hpp"
using namespace std::string_literals;
namespace UO {
	//===============================================================
	const std::string MapTerrain::_hash_format = "build/map{}legacymul/{8}.dat"s;
	const std::string MapTerrain::_mul_format ="map{}.mul"s ;
	const std::string MapTerrain::_uop_format ="map{}LegacyMUL.uop"s ;
	const std::string MapTerrain::_diffl_file = "mapdifl{}.mul";
	const std::string MapTerrain::_diff_file = "mapdif{}.mul";

	
	//===============================================================
	void MapTerrain::buildStrings(std::size_t mapnumber){
		auto mapnum = std::to_string(mapnumber);
		_hashformat = strutil::format(_hash_format,mapnum);
		_mulfile = strutil::format(_mul_format,mapnum);
		_uopfile = strutil::format(_uop_format,mapnum);
		_difffile =strutil::format(_diff_file,mapnum);
		_difflfile =strutil::format(_diffl_file,mapnum);
	}

	//===============================================================
	void MapTerrain::processBlock(std::size_t blocknum, const std::vector<std::uint8_t> &data){
		if (blocknum < _blocks.size()){
			//std::cout <<"Processing block " << blocknum << std::endl;
			_blocks[blocknum] = MapBlock(data);
		}
	}
	//===============================================================
	bool MapTerrain::processEntry(std::size_t entry, std::size_t index, const std::vector<std::uint8_t> &data) {
		//
		auto block = index * 4096 ;
		std::vector<std::uint8_t> blockdata(196);
		std::size_t offset = 0 ;
		while (offset < data.size()) {
			std::copy(data.begin()+offset,data.begin()+offset+blockdata.size(),blockdata.begin()) ;
			processBlock(block,blockdata);
			block++ ;
			offset += blockdata.size() ;
		}
		
		
		return true ;
	}
	
	//===============================================================
	void MapTerrain::readMul(const std::string &mulpath){
		_usedUOP = false ;
		_diffcount = 0;
		std::ifstream input(mulpath,std::ios::binary) ;
		if (!input.is_open()){
			throw FileOpen(mulpath);
		}
		std::vector<std::uint8_t> chunk(196,0);
		std::size_t blocknum = 0 ;
		while (!input.eof() && input.good()){
			input.read(reinterpret_cast<char*>(chunk.data()),196);
			if (input.gcount()== 196){
				processBlock(blocknum, chunk);
				blocknum++;
			}
		}
	}

	//===============================================================
	std::size_t MapTerrain::applyDiff(const std::string &difflfile, const std::string &difffile){
		auto difflpath = std::filesystem::path(difflfile) ;
		if (std::filesystem::is_directory(difflpath)){
			difflpath /= _difflfile ;
		}
		auto diffpath = std::filesystem::path(difffile) ;
		if (std::filesystem::is_directory(diffpath)){
			diffpath /= _difffile ;
		}
		if (!std::filesystem::exists(difflpath)){
			return 0;
		}
		std::ifstream diffl(difflpath.string(), std::ios::binary);
		if (!diffl.is_open()){
			throw FileOpen(difflpath.string());
		}
		std::ifstream diff(diffpath.string(), std::ios::binary);
		if (!diff.is_open()){
			throw FileOpen(diffpath.string());
		}
		std::vector<std::uint8_t> dif_data(196,0);
		_diffcount = 0 ;
		std::uint32_t blocknumber ;
		while (!diffl.eof() && diffl.good()){
			diffl.read(reinterpret_cast<char*>(&blocknumber),4);
			if (diffl.gcount()==4){
				diff.read(reinterpret_cast<char*>(dif_data.data()),196);
				_blocks[blocknumber] = MapBlock(dif_data);
				_diffcount++;
			}
		}
		diffl.close();
		diff.close();
		return _diffcount;
	}
	
	//===============================================================
	bool MapTerrain::load(const std::string &datapath,std::size_t mapnumber,std::int32_t width, std::int32_t height){
		_usedUOP = true;
		_diffcount = 0 ;
		buildStrings(mapnumber);
		_blocks.clear();
		auto [def_width,def_height] = _mapsizes[mapnumber];
		if (width == 0 ){
			width = def_width;
		}
		if (height ==0){
			height = def_height ;
		}
		_width = width ;
		_height = height ;
		_blocks.clear() ;
		_blocks.resize((width/8) * (height/8));
		auto path = std::filesystem::path(datapath) ;
		if (std::filesystem::is_directory(path)){
			// It is a directory!
			// Try UOP first
			auto uoppath = path / std::filesystem::path(_uopfile) ;
			if (std::filesystem::exists(uoppath)){
				loadUOP(uoppath.string(), 0x300 , _hashformat);
			}
			else {
				auto mulpath = path / std::filesystem::path(_mulfile);
				readMul(mulpath.string());
			}
			
		}
		if (path.extension()== ".uop"s){
			// it is an uop!
			loadUOP(path.string(), 0x300 , _hashformat);
		}
		else {
			// It must be a mul?
			readMul(datapath);
		}
		return false ;
	}
	
	//===============================================================
	tile_st& MapTerrain::at(std::int32_t x, std::int32_t y)  {
		auto blocknum = calcBlock(x, y);
		//std::cout <<"blocknum was " << blocknum << std::endl;
		auto [xbase,ybase] = baseXY(blocknum);
		//std::cout << "xbase is "<<xbase << " ybase is " <<ybase << std::endl;
		return (_blocks[blocknum]).at(x-xbase,y-ybase);
	}
	//===============================================================
	const tile_st& MapTerrain::at(std::int32_t x, std::int32_t y)  const {
		auto blocknum = calcBlock(x, y);
		//std::cout <<"blocknum was " << blocknum << std::endl;
		auto [xbase,ybase] = baseXY(blocknum);
		//std::cout << "xbase is "<<xbase << " ybase is " <<ybase << std::endl;
		return (_blocks[blocknum]).at(x-xbase,y-ybase);
	}
	//===============================================================
	IMG::Bitmap MapTerrain::radar(const RadarColor &radar)  {
		//std::cout <<"Block width "<<_width/8 <<" block height " << _height/8 << std::endl;
		IMG::Bitmap bitmap(_width,_height,0xffffff) ;
		for (auto y=0; y< _height;y++){
			for (auto x=0;x<_width;x++){
				auto tile = at(x,y) ;
				//std::cout <<"name: "<<tile.info.name <<" id " << strutil::numtostr(tile.tileid,16,true,4)<< std::endl;
				//auto color = radar[tile.tileid];
				bitmap.at(x,y) = radar[at(x,y).tileid];
			}
		}
		return bitmap;
	}
}
