//Copyright © 2021 Charles Kerr. All rights reserved.

#include "TileInfo.hpp"
#include "StringUtility.hpp"
#include "UOAlerts.hpp"

#include <iostream>
#include <fstream>
#include <filesystem>
#include <sstream>
using namespace std::string_literals;
namespace UO {
	const std::array<std::string,64> tile_info::_flag_names={
		"background"s,"weapon"s,"transparent"s,"translucent"s,"wall"s,
		"damaging"s,"impassable"s,"wet"s,"unknownBit9"s,"surface"s,"bridge"s,
		"stackable"s,"window"s,"noShoot"s,"articleA"s,"articleAn"s,"articleThe"s,
		"foliage"s,"partialHue"s,"noHouse"s,"map"s,"container"s,"wearable"s,"lightSource"s,
		"animated"s,"hoverOver","noDiagnol"s,"armor"s,"door"s,"stairBack"s,
		"stairRight"s,"alphaBlend"s,"useNewart"s,"artUsed"s,"unknownBit36"s,
		"noShadow"s,"pixelBleed"s,"animatedOnce"s,"uknownBit40"s,"uknownBit41"s,
		"uknownBit42"s,"uknownBit43"s,"uknownBit44"s,"multiMove"s,"uknownBit46"s,
		"uknownBit47"s,"uknownBit48"s,"uknownBit49"s,"uknownBit50"s,"uknownBit51"s,
		"uknownBit52"s,"uknownBit53"s,"uknownBit54"s,"uknownBit55"s,"uknownBit56"s,
		"uknownBit57"s,"uknownBit58"s,"uknownBit59"s,"uknownBit60"s,"uknownBit61"s,
		"uknownBit61"s,"uknownBit62"s"uknownBit63"s,"uknownBit64"s
	};
	//===============================================================
	tile_info::tile_info(TileType type){
		this->type = type ;
		flag = 0;
		// Terrain
		texture = 0 ;
		// Art
		weight = 0 ;
		quality = 0;
		miscData = 0;
		unk2 = 0;
		quantity = 0;
		animid = 0;
		unk3 = 0;
		hue = 0;
		stackingOffset = 0;
		value = 0;
		height = 0;

	}
	//===============================================================
	std::string tile_info::description() const {
		std::stringstream output ;
		if (!name.empty()){
			output <<"\tname = "s<<name <<"\n"s;
		}
		output <<"\tflag = "s<<strutil::numtostr(flag,2,true,64)<<"\n"s;
		switch (type) {
			case TileType::terrain:
				output << descTerrain();
				break;
			case TileType::art:
				output << descArt() ;
				break;
			default:
				break;
		}
		return  output.str();
	}
	//===============================================================
	std::string tile_info::descFlag(std::uint64_t flag) {
		std::stringstream output ;
		std::uint64_t one = 1;
		bool oneshot = true ;
		for (auto i= 0 ; i<64;i++){
			if (flag & (one<<i)){
				if(oneshot){
					output<<_flag_names[i];
					oneshot = false;
				}
				else {
					output<<":"s<<_flag_names[i];
				}
			}
		}
		return output.str();
	}
	//===============================================================
	std::string tile_info::csvRow() const {
		std::stringstream output ;
		output << name<<","<<descFlag(flag);
		switch (type) {
			case TileType::terrain:
				output << rowTerrain();
				break;
			case TileType::art:
				output << rowArt() ;
				break;
			default:
				break;
		}
		return output.str();
	}
	//===============================================================
	std::string tile_info::csvTitle() const{
		std::stringstream output ;
		output << "Name,Flag"s;
		switch (type) {
			case TileType::terrain:
				output << rowTitleTerrain();
				break;
			case TileType::art:
				output << rowTitleArt() ;
				break;
			default:
				break;
		}
		return output.str();
	}
	//===============================================================
	std::string tile_info::descTerrain() const {
		std::stringstream output ;
		output <<"\ttexture = "s << strutil::numtostr(texture,16,true,4)<<"\n"s;
		return output.str();
	}
	//===============================================================
	std::string tile_info::rowTerrain() const {
		std::stringstream output ;
		output << ","s<<strutil::numtostr(texture,16,true,4);
		return output.str();
	}
	//===============================================================
	std::string tile_info::rowTitleTerrain()const{
		return ",Texture"s;
	}
	//===============================================================
	std::string tile_info::rowArt() const {
		std::stringstream output ;
		output << ","s<<strutil::numtostr(weight)<<","s<<strutil::numtostr(quality)<<","s<<
		strutil::numtostr(miscData,16,true,4)<<","s<<strutil::numtostr(unk2,16,true,2)<<
		","s<<strutil::numtostr(quantity)<<","s<<strutil::numtostr(animid,16,true,4)<<","s<<
		strutil::numtostr(unk3,16,true,2)<<strutil::numtostr(hue,16,true,2)<<","s<<
		strutil::numtostr(stackingOffset)<<","s<<strutil::numtostr(value)<<","s<<strutil::numtostr(height) ;
		
		return output.str();
	}
	//===============================================================
	std::string tile_info::rowTitleArt()const{
		return ",Weight,Quality,miscData,unk2,Quantity,AmimationID,unk3,Hue,stackingOffset,Value,Height"s;
	}

	//===============================================================
	std::string tile_info::descArt() const {
		std::stringstream output ;

		output << "\tweight = "s <<weight <<"\n"s;
		output << "\tquality = "s <<quality <<"\n"s;
		output << "\tmiscData = "s << miscData <<"\n"s;
		output << "\tunk2 = "s << unk2<<"\n"s;
		output << "\tquantity = "s << quantity<<"\n"s;
		output << "\tanimid = "s << strutil::numtostr(animid,16,true,4)<<"\n"s;
		output << "\tunk3 = "s << unk3<<"\n"s;
		output << "\thue = "s << hue<<"\n"s;
		output << "\tstackingOffset = "s << stackingOffset<<"\n"s;
		output << "\tvalue = "s << value << "\n"s;
		output << "\theight = "s << height << "\n";
		
		return output.str();
	}

	//===============================================================
	tile_st::tile_st() {
		info  = nullptr;
		x  = 0;
		y = 0;
		z = 0;
		artHue = 0 ;
		multiFlag = 0;
		isStatic = false ;
		isMulti = false ;

	}
	
	/************************************************************************
	 TileInfo
	 ************************************************************************/
	//===============================================================
	const std::string TileInfo::_tiledata_mul = "tiledata.mul"s;
	
	
	//===============================================================
	void TileInfo::loadTerrain(const std::string &pathname, std::ifstream &input){
		_terrain.clear();
		_terrain.reserve(0x4000);
		std::uint32_t value32 = 0 ;
		std::uint64_t value64 =  0;
		char string_buffer[21];

		for (auto i = 0 ; i < 0x4000; i++){
			
			if (_usedHS) {
				if ( (((i & 0x1F)==0) && (i>0)) || (i == 1)){
					input.read(reinterpret_cast<char*>(&value32),4); // read off the group header
				}
			}
			else {
				if ( (i & 0x1f) == 0) {
					input.read(reinterpret_cast<char*>(&value32),4); // read off the group header
				}
			}
			tile_info info(TileType::terrain) ;
			
			if (_usedHS) {
				input.read(reinterpret_cast<char*>(&value64),8);
				info.flag = value64 ;
				
			}
			
			else{
				input.read(reinterpret_cast<char*>(&value32),4);
				info.flag = value32 ;
			}
			
			input.read(reinterpret_cast<char*>(&info.texture),2);
			input.read(string_buffer,20);
			string_buffer[input.gcount()]=0 ;
			info.name = std::string(string_buffer) ;
			if (input.good() && !input.eof()){
				_terrain.push_back(info);
			}
			else {
				throw StreamError(pathname);
			}
			
		}

		
	}
	//===============================================================
	void TileInfo::loadArt(const std::string &pathname, std::ifstream &input){
		_art.clear() ;
		_art.reserve(0xFFFE) ;
		char string_buffer[21];
		std::uint32_t  value32 = 0 ;
		std::uint64_t value64 = 0;
		std::uint8_t value8 = 0 ;
		std::uint16_t value16 = 0;

		auto loopcount = 0;
		do {
			
			
			if (input.eof()) {
				break;
			}
			if ( (loopcount & 0x1f) == 0) {
				input.read(reinterpret_cast<char*>(&value32),4); // read off the group header
			}
			tile_info  info(TileType::art) ;
			if (_usedHS) {
				input.read(reinterpret_cast<char*>(&value64),8);
				info.flag = value64;
				
			}
			else {
				input.read(reinterpret_cast<char*>(&value32),4);
				info.flag = static_cast<unsigned long long>(value32);
				
			}
			
			input.read(reinterpret_cast<char*>(&value8),1);
			info.weight = value8;
			input.read(reinterpret_cast<char*>(&value8),1);
			info.quality = value8 ;
			//misc data
			input.read(reinterpret_cast<char*>(&value16),2);
			info.miscData = value16 ;
			//Second unkown
			input.read(reinterpret_cast<char*>(&value8),1);
			info.unk2 = value8 ;
			// Quantity
			input.read(reinterpret_cast<char*>(&value8),1);
			info.quantity = value8;
			// Animation
			input.read(reinterpret_cast<char*>(&value16),2);
			info.animid = value16 ;
			
			//Third a byte
			input.read(reinterpret_cast<char*>(&value8),1);
			info.unk3 = value8 ;
			
			// Hue/Value
			input.read(reinterpret_cast<char*>(&value8),1);
			info.hue = value8;
			//stacking offset
			input.read(reinterpret_cast<char*>(&value8),1);
			info.stackingOffset = value8 ;
			// value
			input.read(reinterpret_cast<char*>(&value8),1);
			info.value = value8 ;
			// Height
			input.read(reinterpret_cast<char*>(&value8),1);
			info.height = value8 ;
			// Name
			
			input.read(string_buffer,20);
			string_buffer[input.gcount()]=0 ;
			info.name = std::string(string_buffer);
			if (input.good() && !input.eof()){
				_art.push_back(info);
			}
			
			loopcount++;
		}while ( (!input.eof()) && (input.good()) );

	}

	//===============================================================
	bool TileInfo::load(const std::string &uodir_tiledata){
		if (uodir_tiledata.empty()){
			return false ;
		}
		auto path = std::filesystem::path(uodir_tiledata);
		if (std::filesystem::is_directory(path)){
			path = path / std::filesystem::path(_tiledata_mul);
		}
		auto size = std::filesystem::file_size(path);
		_usedHS = false ;
		if (size == _size_for_HS){
			_usedHS = true ;
		}
		std::ifstream input(path.string(),std::ios::binary);
		if (!input.is_open()){
			throw FileOpen(path.string());
		}
		// We have a good stream, time to read
		loadTerrain(path.string(), input);
		loadArt(path.string(),input);
		return true ;
	}

	//===============================================================
	const tile_info& TileInfo::terrain(std::size_t tileid) const {
		if (tileid >= _terrain.size()){
			throw InvalidTileID("terrain", tileid);
		}
		return _terrain[tileid];
	}
	//===============================================================
	tile_info& TileInfo::terrain(std::size_t tileid){
		if (tileid >= _terrain.size()){
			throw InvalidTileID("terrain", tileid);
		}
		return _terrain[tileid];
	}
	
	//===============================================================
	const tile_info& TileInfo::art(std::size_t tileid) const {
		if (tileid >= _art.size()){
			throw InvalidTileID("art", tileid);
		}
		return _art[tileid];
	}
	//===============================================================
	tile_info& TileInfo::art(std::size_t tileid){
		if (tileid >= _art.size()){
			throw InvalidTileID("art", tileid);
		}
		return _art[tileid];
	}

}
