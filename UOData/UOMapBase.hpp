//

#ifndef UOMapBase_hpp
#define UOMapBase_hpp

#include <string>
#include <cstdint>
#include <array>
#include <utility>
namespace UO{
	//===============================================================
	class UOMapBase {
	protected:
		static const std::array<std::pair<std::int32_t,std::int32_t>,6> _mapsizes;
		std::int32_t _width ;
		std::int32_t _height ;
		
		std::int32_t calcBlock(std::int32_t x, std::int32_t y) const;
		std::pair<std::int32_t,std::int32_t> baseXY(std::int32_t blocknumber) const;
	public:
		
	};
}

#endif /* UOMapBase_hpp */
