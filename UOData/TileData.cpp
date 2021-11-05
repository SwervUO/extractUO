//Copyright © 2021 Charles Kerr. All rights reserved.

#include "TileData.hpp"
#include <iostream>
#include <algorithm>

using namespace std::string_literals;
namespace UO {
	//===============================================================
	TileData::TileData(const std::string &uodir_tiledata){
		if (!uodir_tiledata.empty()){
			load(uodir_tiledata);
		}
	}
	
	//===============================================================
	TileData & TileData::shared(const std::string & input){
		static TileData instance(input);
		return instance ;
	}
	//===============================================================
	bool TileData::read(const std::string &uodir_tiledata){
		return load(uodir_tiledata);
	}
	//===============================================================
	bool TileData::HS() const { return _usedHS;}

}
