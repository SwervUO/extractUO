//

#include "Block.hpp"
#include "Buffer.hpp"
#include "TileData.hpp"
#include "UOAlerts.hpp"
#include <iostream>
using namespace std::string_literals;
namespace UO{
	//===============================================================
	const tile_st& MapBlock::at(std::int32_t x,std::int32_t y) const {
		return (_data[x])[y] ;
	}
	//===============================================================
	tile_st & MapBlock::at(std::int32_t x, std::int32_t y) {
		return (_data[x])[y] ;
	}
	//===============================================================
	void MapBlock::load(const std::vector<std::uint8_t> &data){
		
		if (data.size() != 196){
			throw  InvalidMapBlockSize(data.size());
		}
		Buffer buffer(data) ;
		buffer.position(4) ; // scoot past the header
		tileid_t tileid ;
		std::int8_t altitude ;
		for (auto y = 0 ; y < 8 ; y++){
			for (auto x=0 ; x<8 ; x++){
				buffer >> tileid ;
				buffer >> altitude ;
				//tileid = x*y ;
				tile_st tile(tileid) ;
				tile.info = UO::TileData::shared().terrain(tileid);
				tile.z = altitude ;
				_data[x][y] = tile ;
			}
		}
	}
	//===============================================================
	MapBlock::MapBlock(const std::vector<std::uint8_t> &data ){
		
		if (!data.empty()){
			load(data);
		}
		
	}
	//===============================================================
	std::vector<std::uint8_t> MapBlock::blockData() const {
		Buffer buffer(196) ;
		buffer << static_cast<std::uint32_t>(1234) ;
		for (auto y = 0 ; y < 8 ; y++){
			for (auto x=0 ; x<8 ; x++){
				buffer << _data[x][y].tileid;
				buffer << static_cast<std::int8_t>(_data[x][y].z);
			}
		}

		return buffer.raw();
	}

	/***********************************************************************
	Static Block
	 **********************************************************************/
	//===============================================================
	const std::vector<tile_st>& StaticBlock::at(std::int32_t x,std::int32_t y) const {
		return (_data[x])[y] ;
	}
	//===============================================================
	std::vector<tile_st> & StaticBlock::at(std::int32_t x, std::int32_t y) {
		return _data[x][y] ;
	}
	//===============================================================
	void StaticBlock::load(const std::vector<std::uint8_t> &data){
		if (data.empty()){
			throw  InvalidMapBlockSize(0);
		}
		Buffer buffer(data) ;
		
		tileid_t tileid ;
		std::int8_t altitude ;
		std::uint8_t xoffset ;
		std::uint8_t yoffset ;
		std::uint16_t hue ;
		while (buffer.position()<buffer.size()){
			buffer >> tileid ;
			tile_st tile ;
			tile.tileid = tileid;
			tile.info = UO::TileData::shared().art(tileid) ;
			buffer>>xoffset;
			buffer >> yoffset ;
			buffer >> altitude ;
			buffer >> hue ;
			tile.z = altitude ;
			tile.isStatic = true ;
			tile.artHue = hue ;
			this->_data[xoffset][yoffset].push_back(tile);
		}

	}
	//===============================================================
	StaticBlock::StaticBlock(const std::vector<std::uint8_t> &data ){
		load(data);
	}
	//===============================================================
	std::vector<std::uint8_t> StaticBlock::blockData() const {
		Buffer buffer ;
		
		for (auto y = 0 ; y < 8 ; y++){
			for (auto x=0 ; x<8 ; x++){
				for (const auto &entry: _data[x][y] ){
					buffer << entry.tileid ;
					buffer << static_cast<std::uint8_t>(x);
					buffer << static_cast<std::uint8_t>(y);
					buffer << static_cast<std::int8_t>(entry.z);
					buffer << static_cast<std::uint16_t>(entry.artHue);
					
				}
			}
		}
		
		return buffer.raw();
	}
}
