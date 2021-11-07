//

#include "RadarColor.hpp"
#include "UOAlerts.hpp"
#include <iostream>
#include <fstream>
#include <filesystem>

using namespace std::string_literals;
namespace UO {

	//===============================================================
	const std::string RadarColor::_mul_file = "radarcol.mul";
	
	//===============================================================
	RadarColor::RadarColor(const std::string &uodir_mulfile){
		if (!uodir_mulfile.empty()){
			load(uodir_mulfile);
		}
	}
	//===============================================================
	void RadarColor::load(const std::string &uodir_mulfile){
		auto path = std::filesystem::path(uodir_mulfile);
		if (std::filesystem::is_directory(path)){
			path = path / std::filesystem::path(_mul_file);
		}
		if (!std::filesystem::exists(path)){
			throw FileOpen(path.string());
		}
		auto size = std::filesystem::file_size(path);
		_colors.resize(size/2,0);
		std::ifstream input(path.string(),std::ios::binary);
		input.read(reinterpret_cast<char*>(_colors.data()),_colors.size()*2);
		if (input.gcount() != _colors.size() *2){
			throw StreamError(path.string());
		}
		
	}
	
	//===============================================================
	const std::uint16_t & RadarColor::operator[](std::size_t index) const{
		return _colors[index];
	}
	//===============================================================
	std::uint16_t & RadarColor::operator[](std::size_t index) {
		return _colors[index];
	}

}
