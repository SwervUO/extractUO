//

#include "MapArt.hpp"
#include "StringUtility.hpp"
#include "UOAlerts.hpp"
#include <iostream>
#include <filesystem>
#include <fstream>

using namespace std::string_literals;
namespace UO {
	//===============================================================
	const std::string MapArt::_idx_file ="staidx{}.mul"s;
	const std::string MapArt::_mul_file = "statics{}.mul"s;
	const std::string MapArt::_dif_file = "stadif{}.mul"s;
	const std::string MapArt::_difl_file = "stadifl{}.mul"s;
	const std::string MapArt::_difi_file = "stadifi{}.mul"s;

	//===============================================================
	void MapArt::buildStrings(std::size_t mapnumber){
		auto map = std::to_string(mapnumber);
		_idxfile = strutil::format(_idx_file,map);
		_mulfile = strutil::format(_mul_file,map);
		_diffile = strutil::format(_dif_file,map);
		_dififile = strutil::format(_difi_file,map);
		_diflfile = strutil::format(_difl_file,map);
	}
	
	//===============================================================
	void MapArt::recordData(std::uint32_t record_number, std::uint32_t extra, std::vector<std::uint8_t> &record_data){
		_blocks.insert_or_assign(record_number, StaticBlock(record_data));
	}
	
	
	//===============================================================
	std::vector<tile_st>& MapArt::at(std::int32_t x, std::int32_t y)  {
		auto blocknum = calcBlock(x, y);
		auto iter = _blocks.find(blocknum);
		if (iter == _blocks.end()){
			return _empty_tiles;
		}
		
		auto [xbase,ybase] = baseXY(blocknum);
		
		return iter->second.at(x-xbase,y-ybase);
	}
	//===============================================================
	const std::vector<tile_st>& MapArt::at(std::int32_t x, std::int32_t y) const {
		auto blocknum = calcBlock(x, y);
		auto iter = _blocks.find(blocknum);
		if (iter == _blocks.end()){
			return _empty_tiles;
		}
		
		auto [xbase,ybase] = baseXY(blocknum);
		
		return iter->second.at(x-xbase,y-ybase);
	}

	//===============================================================
	std::size_t MapArt::applyDiff(const std::string &difflfile,const std::string &diffifile,const std::string &difffile){
		auto difflpath = std::filesystem::path(difflfile);
		if (std::filesystem::is_directory(difflpath)){
			difflpath /= _diflfile ;
		}
		auto diffipath = std::filesystem::path(diffifile);
		if (std::filesystem::is_directory(diffipath)){
			diffipath /= _dififile ;
		}
		auto diffpath = std::filesystem::path(difffile);
		if (std::filesystem::is_directory(diffpath)){
			diffpath /= _diffile ;
		}
		if (!std::filesystem::exists(difflpath)){
			return 0;
		}

		std::ifstream difl(difflpath.string(),std::ios::binary);
		if (!difl.is_open()){
			throw FileOpen(difflpath.string());
		}
		std::ifstream difi(diffipath.string(),std::ios::binary);
		if (!difi.is_open()){
			throw FileOpen(diffipath.string());
		}
		std::ifstream dif(diffpath.string(),std::ios::binary);
		if (!dif.is_open()){
			throw FileOpen(diffpath.string());
		}

		_diffcount = 0 ;
		std::uint32_t blocknumber ;
		std::uint32_t offset ;
		std::uint32_t length ;
		while (!difl.eof() && difl.good()){
			difl.read(reinterpret_cast<char*>(&blocknumber),4);
			if (difl.gcount()==4){
				difi.read(reinterpret_cast<char*>(&offset),4);
				difi.read(reinterpret_cast<char*>(&length),4);
				difi.seekg(4,std::ios::cur);
				if ((offset ==0xFFFFFFFF) || (offset == 0xFFFFFFFE) ||(length==0)){
					auto iter = _blocks.find(blocknumber);
					if (iter != _blocks.end()){
						_blocks.erase(iter);
					}
				}
				else {
					std::vector<std::uint8_t> data(length,0);
					dif.read(reinterpret_cast<char*>(data.data()),data.size());
					_blocks.insert_or_assign(blocknumber, StaticBlock(data));
				}
				_diffcount++;
			}
		}
		difi.close();
		difl.close();
		dif.close();
		return _diffcount;
	}

	//===============================================================
	bool MapArt::load(const std::string &idxfile,const std::string &mulfile,std::size_t mapnumber,std::int32_t width, std::int32_t height){
		buildStrings(mapnumber);
		_blocks.clear();
		_diffcount=0;
		auto [defwidth ,defheight] =  _mapsizes[mapnumber];
		if (width==0){
			width = defwidth;
		}
		if (height==0){
			height = defheight;
		}
		_width=width;
		_height = height;
		
		auto idxpath = std::filesystem::path(idxfile);
		if (std::filesystem::is_directory(idxpath)){
			idxpath /= std::filesystem::path(_idxfile);
		}
		auto mulpath = std::filesystem::path(mulfile);
		if (std::filesystem::is_directory(mulpath)){
			mulpath /= std::filesystem::path(_mulfile);
		}
		processFiles(idxpath.string(), mulpath.string());
		
		return true;
	}


}
