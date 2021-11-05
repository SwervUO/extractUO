//Copyright © 2021 Charles Kerr. All rights reserved.

#ifndef TileInfo_hpp
#define TileInfo_hpp

#include <string>
#include <cstdint>
#include <vector>
#include <array>
namespace UO {
	//===============================================================
	using flag_t = std::uint64_t ;
	using tileid_t = std::uint16_t ;


	//===============================================================
	static constexpr tileid_t invalid_tileid = 0xFFFF ;
	
	//===============================================================
	//flag constants
	static constexpr flag_t none 		= 0x0000000000000000;
	static constexpr flag_t background 	= 0x0000000000000001;
	static constexpr flag_t weapon 	= 0x0000000000000002;
	static constexpr flag_t holdable 	= 0x0000000000000002;	// Alternative for weapon
	static constexpr flag_t transparent = 0x0000000000000004;
	// The tile is rendered with partial alpha-transparency.
	static constexpr flag_t translucent = 0x0000000000000008;
	// The tile is a wall.
	static constexpr flag_t wall 		= 0x0000000000000010;
	// The tile can cause damage when moved over.
	static constexpr flag_t damaging 	= 0x0000000000000020;
	// The tile may not be moved over or through.
	static constexpr flag_t impassable 	= 0x0000000000000040;
	static constexpr flag_t wet 		= 0x0000000000000080;
	static constexpr flag_t unknownBit9 = 0x0000000000000100;
	// The tile is a surface. It may be moved over, but not through.
	static constexpr flag_t surface 	= 0x0000000000000200;
	// The tile is a stair, ramp, or ladder.
	static constexpr flag_t bridge 	= 0x0000000000000400;
	static constexpr flag_t climable 	= 0x0000000000000400;  // alternative for bridge
	// The tile is stackable
	static constexpr flag_t generic 	= 0x0000000000000800;
	static constexpr flag_t stackable 	= 0x0000000000000800;  // alternative for generic
	// The tile is a window. Like NoShoot, tiles with this flag block line of sight.
	static constexpr flag_t window 	= 0x0000000000001000;
	// The tile blocks line of sight.
	static constexpr flag_t noShoot 	= 0x0000000000002000;
	// For single-amount tiles, the string "a " should be prepended to the tile name.
	static constexpr flag_t articleA 	= 0x0000000000004000;
	// For single-amount tiles, the string "a " should be prepended to the tile name.
	static constexpr flag_t articleAn 	= 0x0000000000008000;
	// Probably article The prepended to the tile name.
	static constexpr flag_t articleThe 	= 0x0000000000010000;
	static constexpr flag_t description	= 0x0000000000010000;  // alternative for articThe
	// The tile becomes translucent when walked behind. Boat masts also have this flag.
	static constexpr flag_t foliage 	= 0x0000000000020000;
	// Only gray pixels will be hued
	static constexpr flag_t partialHue 	= 0x0000000000040000;
	// NoHouse or Unknown. Needs further research.
	static constexpr flag_t noHouse 	= 0x0000000000080000;
	// The tile is a map--in the cartography sense. Unknown usage.
	static constexpr flag_t map 		= 0x0000000000100000;
	// The tile is a container.
	static constexpr flag_t container 	= 0x0000000000200000;
	// The tile may be equipped.
	static constexpr flag_t wearable 	= 0x0000000000400000;
	// The tile gives off light.
	static constexpr flag_t lightSource = 0x0000000000800000;
	// The tile is animated.
	static constexpr flag_t animated 	= 0x0000000001000000;
	// Gargoyles can fly over or NoDiagonal
	static constexpr flag_t hoverOver 	= 0x0000000002000000;
	// NoDiagonal (Unknown3).
	static constexpr flag_t noDiagnol 	= 0x0000000004000000;
	static constexpr flag_t armor 	= 0x0000000008000000;
	static constexpr flag_t roof 		= 0x0000000010000000;
	static constexpr flag_t door 		= 0x0000000020000000;
	static constexpr flag_t stairBack 	= 0x0000000040000000;
	static constexpr flag_t stairRight 	= 0x0000000080000000;
	
	// HS tile flags
	// Blend Alphas, tile blending.
	static constexpr flag_t alphaBlend 	= 0x0000000100000000;
	// Uses new art style? Something related to the nodraw tile?
	static constexpr flag_t useNewArt 	= 0x0000000200000000;
	// Has art being used?
	static constexpr flag_t artUsed 	= 0x0000000400000000;
	// Disallow shadow on this tile, lightsource? lava?
	static constexpr flag_t noShadow 	= 0x0000001000000000;
	// Let pixels bleed in to other tiles? Is this Disabling Texture Clamp?
	static constexpr flag_t pixelBleed 	= 0x0000002000000000;
	// Play tile animation once.
	static constexpr flag_t animatedOnce 	= 0x0000004000000000;
	
	// Movable multi? Cool ships and vehicles etc? Something related to the masts ???
	static constexpr flag_t multiMove 	= 0x0000100000000000;
	
	//=======================================================================
	// Define the basic tile structures
	enum class TileType { invalid,terrain,art};
	
	
	//===============================================================
	struct tile_info {
	private:
		static const std::array<std::string,64> _flag_names ;
		std::string descTerrain() const ;
		std::string descArt() const ;
		std::string rowTitleTerrain() const;
		std::string rowTitleArt() const ;
		std::string rowTerrain()const ;
		std::string rowArt() const ;
	public:
		TileType type ;
		// Common attributes
		std::string name ;
		std::uint64_t flag ;
		// Terrain only
		tileid_t texture ;
		// Art only
		std::uint8_t weight ;  // std::uint8_t in data
		std::uint8_t quality ; // std::uint8_t in data
		std::int16_t miscData ; // std::uint16_t in data
		std::uint8_t unk2 ;	// std::uint8_t in data
		std::uint8_t quantity ;	// std::uint8_t in data
		tileid_t	 animid ;
		std::uint8_t unk3 ;	// std::uint8_t in data
		std::uint8_t hue ;	// std::uint8_t in data
		std::uint8_t stackingOffset ;  // unk4
		std::uint8_t value ; // unk5 , std::uint8_t in data
		std::int16_t height ; // This is a uint8_t in the data,

		//Methods
		std::string description() const;
		
		static std::string descFlag(std::uint64_t flag)  ;
		virtual std::string csvRow() const ;
		virtual std::string csvTitle()const ;
		tile_info(TileType type=TileType::invalid);
	};
	
	//===============================================================
	struct tile_st {
		tile_info *info ;
		std::int32_t x ;
		std::int32_t y;
		std::int32_t z ;
		bool isStatic;
		bool isMulti;
		std::uint16_t artHue ;
		std::uint64_t multiFlag ;
		tile_st() ;
	};

	//===============================================================

	class TileInfo {
		
	private:
		
		static const std::string _tiledata_mul ;
		static constexpr auto _size_for_HS = 3188736;
		std::vector<tile_info> _terrain ;
		std::vector<tile_info> _art ;
		
		
		void loadTerrain(const std::string &pathname, std::ifstream &input);
		void loadArt(const std::string &pathname, std::ifstream &input);
	protected:
		bool _usedHS ;

	public:
		
		std::size_t sizeTerrain() const {return _terrain.size();}
		std::size_t sizeArt() const {return _art.size();}
		
		const tile_info& terrain(std::size_t tileid) const ;
		tile_info& terrain(std::size_t tileid);
		
		const tile_info& art(std::size_t tileid) const ;
		tile_info& art(std::size_t tileid);

		bool load(const std::string &uodir_tiledata);
	};
}

#endif /* TileInfo_hpp */
