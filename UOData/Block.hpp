//

#ifndef Block_hpp
#define Block_hpp

#include <string>
#include <cstdint>
#include <array>
#include <vector>
#include "TileInfo.hpp"
namespace UO{
	
	//===============================================================
	class MapBlock {
	private:
		
		std::array<std::array<tile_st,8>,8> _data ; // 8 x 8 array
		
	public:
		const tile_st& at(std::int32_t x,std::int32_t y) const ;
		tile_st & at(std::int32_t x, std::int32_t y) ;
		void load(const std::vector<std::uint8_t> &data);
		MapBlock(const std::vector<std::uint8_t> &data = std::vector<std::uint8_t>());
		std::vector<std::uint8_t> blockData() const ;
		
	};
	
	//===============================================================
	class StaticBlock {
	private:
		std::array<std::array<std::vector<tile_st>,8>,8> _data ; // 8 x 8 array

	public:
		const std::vector<tile_st>& at(std::int32_t x,std::int32_t y) const ;
		std::vector<tile_st> & at(std::int32_t x, std::int32_t y) ;
		void load(const std::vector<std::uint8_t> &data);
		StaticBlock(const std::vector<std::uint8_t> &data = std::vector<std::uint8_t>());
		std::vector<std::uint8_t> blockData() const ;
	};
}
#endif /* Block_hpp */
