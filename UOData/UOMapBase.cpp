//

#include "UOMapBase.hpp"
#include <iostream>
using namespace std::string_literals;
namespace UO {
	//===============================================================
	const std::array<std::pair<std::int32_t,std::int32_t>,6> UOMapBase::_mapsizes = {
		std::make_pair(7168,4096),std::make_pair(7168,4096),std::make_pair(2304,1600),std::make_pair(2560,2048),std::make_pair(1448,1448),std::make_pair(1280,4096)
	};
	
	
	//===============================================================
	std::int32_t UOMapBase::calcBlock(std::int32_t x, std::int32_t y) const{
		std::int32_t xBlock = x/8 ;
		std::int32_t yBlock = y/8;
		
		return (xBlock * (_height/8)) + yBlock;
		
	}
	//===============================================================
	std::pair<std::int32_t,std::int32_t> UOMapBase::baseXY(std::int32_t blocknumber) const{
		auto column = blocknumber / (_height/8) ;
		auto offset = blocknumber % (_height/8) ;
		return std::make_pair((column * 8) ,offset*8) ;
	}

}
