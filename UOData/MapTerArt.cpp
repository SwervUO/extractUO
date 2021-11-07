//

#include "MapTerArt.hpp"
#include "TileInfo.hpp"
#include "StringUtility.hpp"
#include "RadarColor.hpp"
#include <filesystem>
#include <iostream>
#include <algorithm>

using namespace std::string_literals;
namespace UO {
	//===============================================================
	MapTerArt::MapTerArt(std::size_t mapnumber,std::int32_t width, std::int32_t height){
		_mapnumber = mapnumber ;
		auto [defwidth,defheight] = _mapsizes[_mapnumber];
		if (width == 0) {
			width = defwidth;
		}
		if (height == 0 ) {
			height = defheight;
		}
		_width = width;
		_height = height;
	}
	
	//===============================================================
	void MapTerArt::load(const std::string &uodir) {
		_terrain.load(uodir, _mapnumber,_width,_height) ;
		_art.load(uodir,uodir,_mapnumber,_width,_height);
	}
	//===============================================================
	void MapTerArt::loadTerrain(const std::string &mapmul_uop){
		_terrain.load(mapmul_uop,_mapnumber,_width,_height);
	}
	//===============================================================
	void MapTerArt::loadArt(const std::string &idxfile,const std::string &mulfile){
		_art.load(idxfile, mulfile, _mapnumber,_width,_height);
	}
	
	//===============================================================
	std::size_t MapTerArt::applyTerrainDiff(const std::string &difflfile,const std::string &difffile){
		return _terrain.applyDiff(difflfile, difffile);
	}
	
	//===============================================================
	std::size_t MapTerArt::applyArtDiff(const std::string &difflfile,const std::string &diffifile,const std::string &difffile){
		return _art.applyDiff(difflfile, diffifile, difffile);
	}

	//===============================================================
	std::vector<tile_st> MapTerArt::tiles(std::int32_t x, std::int32_t y,bool sort_height) const {
		std::vector<tile_st> alltiles ;
		alltiles.push_back(_terrain.at(x,y));
		auto arttiles = _art.at(x,y) ;
		for (const auto &tile:arttiles){
			alltiles.push_back(tile);
		}
		if (sort_height){
			std::sort(alltiles.rbegin(),alltiles.rend());
		}
		else {
			std::sort(alltiles.rbegin(),alltiles.rend(),[](const UO::tile_st &a,const UO::tile_st &b){
				return a.lowerAltitude(b);
			});
		}
		return  alltiles;
	}
	//===============================================================
	const tile_st& MapTerArt::terrain(std::int32_t x, std::int32_t y) const {
		return _terrain.at(x,y);
	}
	//===============================================================
	tile_st& MapTerArt::terrain(std::int32_t x, std::int32_t y){
		return _terrain.at(x,y);
	}
	//===============================================================
	std::vector<tile_st>& MapTerArt::art(std::int32_t x, std::int32_t y) {
		return _art.at(x,y);
	}
	//===============================================================
	const std::vector<tile_st>& MapTerArt::art(std::int32_t x, std::int32_t y) const{
		return _art.at(x,y);
	}
	
	//===============================================================
	IMG::Bitmap MapTerArt::radar(const RadarColor &palette,bool include_art){
		if (!include_art){
			return _terrain.radar(palette);
		}
		else {
			IMG::Bitmap bitmap(_width,_height,0);
			for (auto y=0;y<_height;y++){
				for (auto x=0; x<_width;x++){
					auto alltiles = tiles(x,y,true) ;
					if (alltiles[0].info.type==UO::TileType::art){
						bitmap.at(x,y)= palette[alltiles[0].tileid+0x4000];
					}
					else {
						bitmap.at(x,y)= palette[alltiles[0].tileid];
					}
				}
			}
			return bitmap;
		}
		
	}

}
