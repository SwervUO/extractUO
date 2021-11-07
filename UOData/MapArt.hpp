//

#ifndef MapArt_hpp
#define MapArt_hpp

#include <string>
#include <cstdint>
#include <vector>
#include <map>
#include <utility>
#include "UOMapBase.hpp"
#include "Block.hpp"
#include "IDXMul.hpp"

namespace UO {
	//===============================================================
	class MapArt : public IDXMul, public UOMapBase {
		
	private:
		static const std::string _idx_file ;
		static const std::string _mul_file ;
		static const std::string _dif_file ;
		static const std::string _difl_file ;
		static const std::string _difi_file ;
		
		std::string _idxfile ;
		std::string _mulfile ;
		std::string _diffile ;
		std::string _diflfile ;
		std::string _dififile ;
		
		std::vector<tile_st> _empty_tiles;
		
		std::map<std::size_t , StaticBlock> _blocks ;
		
		std::size_t _diffcount ;
		
		void buildStrings(std::size_t mapnumber) ;
		
		void recordData(std::uint32_t record_number, std::uint32_t extra, std::vector<std::uint8_t> &record_data) final;
		
	public:
		
		std::int32_t mapArtWidth()const {return _width;}
		std::int32_t mapArtHeight()const {return _height;} 
		
		std::vector<tile_st>& at(std::int32_t x, std::int32_t y)  ;
		const std::vector<tile_st>& at(std::int32_t x, std::int32_t y) const ;

		std::size_t applyDiff(const std::string &difflfile,const std::string &diffifile,const std::string &difffile);
		
		std::size_t diffCount() const {return _diffcount;}

		bool load(const std::string &idxfile,const std::string &mulfile,std::size_t mapnumber,std::int32_t width=0, std::int32_t height=0);
		
		UO::StaticBlock& block(std::size_t number){return _blocks[number];}

	};
	
}

#endif /* MapArt_hpp */
