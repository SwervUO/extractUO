//Copyright © 2021 Charles Kerr. All rights reserved.

#ifndef TileData_hpp
#define TileData_hpp

#include <string>
#include <cstdint>

#include "TileInfo.hpp"
namespace UO {
	//===============================================================
	class TileData : public TileInfo {
	private:
		TileData(const std::string &uodir_tiledata);
	public:
		TileData(const TileData&) = delete ;
		TileData & operator=(const TileData&) = delete;
		
		static TileData & shared(const std::string & input="");
		bool read(const std::string &uodir_tiledata);
		
		bool HS() const ;
		
	};
}
#endif /* TileData_hpp */
