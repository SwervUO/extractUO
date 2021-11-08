//

#include "LightData.hpp"
#include <iostream>
#include <algorithm>
#include "UOAlerts.hpp"
#include <filesystem>
using namespace std::string_literals;
//===============================================================
namespace UO {
	
	const std::string LightData::_idx_file = "lightidx.mul"s;
	const std::string LightData::_mul_file = "light.mul"s;

	//===============================================================
	void LightData::recordData(std::uint32_t record_number, std::uint32_t extra, std::vector<std::uint8_t> &record_data) {
		auto height = static_cast<std::uint32_t>((extra & 0xFFFF));
		auto width = static_cast<std::uint32_t>(((extra>>16) & 0xFFFF));

		std::vector<std::uint8_t> tempdata(8 + record_data.size(),0) ;
		
		*reinterpret_cast<std::uint32_t*>(tempdata.data()) = width ;
		*reinterpret_cast<std::uint32_t*>(tempdata.data()+4) = height ;
		std::copy(record_data.begin(),record_data.end(),tempdata.data()+8);
		_lights.insert_or_assign(record_number,tempdata);
		
	}
	//===============================================================
	IMG::Bitmap  LightData::convert(const std::vector<std::uint8_t> &data) const {
		const std::uint32_t *ptr = reinterpret_cast<const std::uint32_t *>(data.data());
		auto width = *ptr ;
		auto height = *(ptr+1);
		IMG::Bitmap bitmap(width,height);
		
		for (auto y = 0 ; y < height ;y++){
			for (auto x= 0 ; x < width ; x++){
				auto index = std::abs(static_cast<std::int8_t>(data[8+ x + (y*width)]));
				std::uint16_t color = ((index<<10 )| (index <<5) | index);
				bitmap.at(x,y) = color ;
			}
		}
		 
		return bitmap ;
	}

	//===============================================================
	std::size_t LightData::maxID() const {
		if (_lights.empty()){
			return 0;
		}
		return _lights.rbegin()->first+1;
	}
	//===============================================================
	IMG::Bitmap LightData::bitmap(std::size_t index){
		auto iter = _lights.find(index) ;
		if (iter == _lights.end()){
			return IMG::Bitmap();
		}
		return convert(_lights[index]);
	}
	//===============================================================
	LightData::LightData(const std::string &uodir){
		load(uodir);
	}
	//===============================================================
	void LightData::load(const std::string &uodir){
		if (uodir.empty()){
			return ;
		}
		auto path = std::filesystem::path(uodir) ;
		auto idxfile = path /_idx_file ;
		auto mulfile = path /_mul_file;
		load(idxfile.string(),mulfile.string());
	}
	//===============================================================
	void LightData::load(const std::string &idxfile,const std::string &mulfile){
		processFiles(idxfile, mulfile);
	}

}
