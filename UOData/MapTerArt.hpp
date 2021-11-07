//

#ifndef MapTerArt_hpp
#define MapTerArt_hpp

#include <string>
#include <cstdint>

#include "MapTerrain.hpp"
#include "MapArt.hpp"
#include "UOMapBase.hpp"
#include "Bitmap.hpp"
namespace UO {
	struct tile_st ;
	class RadarColor;
	//===============================================================
	class MapTerArt : public UOMapBase{
	private:
		MapTerrain _terrain ;
		MapArt _art ;
		
		std::size_t _mapnumber ;
		
	public:
		MapTerArt(std::size_t mapnumber,std::int32_t width, std::int32_t height) ;
		
		void load(const std::string &uodir) ;
		void loadTerrain(const std::string &mapmul_uop);
		void loadArt(const std::string &idxfile,const std::string &mulfile);
		
		bool uop() const {return _terrain.uop();}
		
		std::uint32_t mapWidth() const {return _width;}
		std::uint32_t mapHeight() const {return _height;}
		
		std::size_t applyTerrainDiff(const std::string &difflfile,const std::string &difffile);
		
		std::size_t applyArtDiff(const std::string &difflfile,const std::string &diffifile,const std::string &difffile);
		
		std::size_t mapDiff() const ;
		std::size_t artDiff() const ;
		
		// The tiles are sorted by altitude. If sort_height, they are sorted
		// by altitude+height.
		std::vector<tile_st> tiles(std::int32_t x, std::int32_t y,bool sort_height=false) const ;
		
		const tile_st& terrain(std::int32_t x, std::int32_t y) const ;
		tile_st& terrain(std::int32_t x, std::int32_t y);
		
		std::vector<tile_st>& art(std::int32_t x, std::int32_t y) ;
		const std::vector<tile_st>& art(std::int32_t x, std::int32_t y) const ;

		IMG::Bitmap radar(const RadarColor &palette,bool include_art = true);
	};
}

#endif /* MapTerArt_hpp */
