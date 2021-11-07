//Copyright © 2021 Charles Kerr. All rights reserved.

#ifndef MapTerrain_hpp
#define MapTerrain_hpp

#include <string>
#include <cstdint>
#include <vector>
#include <array>
#include <utility>
#include "UOMapBase.hpp"
#include "Block.hpp"
#include "UOPData.hpp"
#include "Bitmap.hpp"
namespace UO {
	class RadarColor;
	struct tile_st ;
	//===============================================================
	class MapTerrain : public UOPData, public UOMapBase {
		
	private:
		static const std::string _hash_format ;
		static const std::string _mul_format ;
		static const std::string _uop_format ;
		static const std::string _diffl_file ;
		static const std::string _diff_file ;
		
		std::string _hashformat ;
		std::string _uopfile ;
		std::string _mulfile ;
		std::string _difflfile ;
		std::string _difffile ;
		
		
		void buildStrings(std::size_t mapnumber);
		
		std::vector<MapBlock> _blocks ;
		bool _usedUOP ;
		
		std::size_t _diffcount ;
		
	protected:
		
		void processBlock(std::size_t blocknum, const std::vector<std::uint8_t> &data);
		
		bool processEntry(std::size_t entry, std::size_t index, const std::vector<std::uint8_t> &data) final ;

		void readMul(const std::string &mulpath);
	public:
		bool uop() const {return _usedUOP;}

		std::size_t applyDiff(const std::string &difflfile,const std::string &difffile);
		std::size_t diffCount() const {return _diffcount;}

		bool load(const std::string &datapath,std::size_t mapnumber,std::int32_t width=0, std::int32_t height=0);
		
		tile_st& at(std::int32_t x, std::int32_t y)  ;
		const tile_st& at(std::int32_t x, std::int32_t y) const ;
		
		IMG::Bitmap radar(const RadarColor &radar) ;
	};
	
}

#endif /* MapTerrain_hpp */
