//

#include "TexMap.hpp"
#include "UOAlerts.hpp"
#include "Buffer.hpp"
#include <iostream>
#include <filesystem>
#include <algorithm>

using namespace std::string_literals;

namespace UO {
	//===============================================================
	const std::string TexMap::_idx_file ="texidx.mul"s;
	const std::string TexMap::_mul_file ="texmaps.mul"s;


	//===============================================================
	void TexMap::recordData(std::uint32_t record_number, std::uint32_t extra, std::vector<std::uint8_t> &record_data){
		if (!record_data.empty()){
			_data.insert_or_assign(record_number, record_data);
		}
	}

	//===============================================================
	IMG::Bitmap TexMap::convertData( const std::vector<std::uint8_t> &data )const  {
		auto width = 0 ;
		if (data.size() == 0x8000) {
			width = 128 ;
		}
		else if (data.size() == 0x2000){
			width = 64 ;
		}
		if (width == 0){
			return IMG::Bitmap(0,0);
		}
		auto height = width ;
		Buffer buffer(data) ;
		IMG::Bitmap bitmap(width,height);
		for (auto y = 0 ; y < height;y++){
			for (auto x= 0 ; x<width;x++){
				auto value = buffer.read<std::uint16_t>() ;
				bitmap.at(x,y) = IMG::Color(value);
			}
		}
		return bitmap ;
	}
	//===============================================================
	std::vector<std::uint8_t> TexMap::convertData(const IMG::Bitmap &bitmap ) const {
		auto [width,height] = bitmap.size() ;
		Buffer data(0);
		if ((width==height) && (width==128)) {
			data.size(0x8000);
		}
		if ((width==height) && (width==64)) {
			data.size(0x2000);
		}
		if (width == 0){
			return std::vector<std::uint8_t>();
		}
		
		
		for (auto y = 0 ; y < height;y++){
			for (auto x= 0 ; x<width;x++){
				data.write(bitmap.at(x,y).color()) ;
				
			}
		}
		return data.raw() ;
	}
	
	/************************************************************************
	 public methods
	 ************************************************************************/
	
	//===============================================================
	std::size_t TexMap::maxTexid() const {
		if (_data.empty()){
			return 0;
		}
		auto iter = _data.crbegin();
		return iter->first + 1 ;
	}
	//===============================================================
	bool TexMap::hasTexture(std::size_t tileid) const {
		auto iter = _data.find(tileid);
		if (iter != _data.end()){
			return true;
		}
		return false ;
	}
	//===============================================================
	IMG::Bitmap TexMap::texture(std::size_t tileid) const {
		if (!hasTexture(tileid)){
			return IMG::Bitmap(0,0);
		}
		auto iter = _data.find(tileid) ;
		return convertData(iter->second);
	}
	//===============================================================
	void TexMap::texture(std::size_t tileid, const IMG::Bitmap & bitmap){
		if (bitmap.empty()){
			auto iter = _data.find(tileid);
			if (iter != _data.end()){
				_data.erase(iter);
			}
		}
		_data.insert_or_assign(tileid, convertData(bitmap));
	}

	//===============================================================
	bool TexMap::open(const std::string &uodir){
		if (uodir.empty()){
			return false ;
		}
		auto path = std::filesystem::path(uodir) ;
		auto idxpath = path / std::filesystem::path(_idx_file);
		auto mulpath = path / std::filesystem::path(_mul_file);
		return open(idxpath.string(),mulpath.string());
	}
	//===============================================================
	bool TexMap::open(const std::string &idxfile, const std::string &mulfile){
		auto idxpath = std::filesystem::path(idxfile) ;
		auto mulpath = std::filesystem::path(mulfile) ;
		if (!std::filesystem::exists(idxpath) || !std::filesystem::exists(mulpath) ){
			return false ;
		}
		_data.clear();
		processFiles(idxfile, mulfile);
		return true;
	}

	//===============================================================
	TexMap::TexMap(const std::string& uodir){
		if (uodir.empty()){
			return ;
		}
		open(uodir);
	}
	//===============================================================
	TexMap::TexMap(const std::string &idxfile,const std::string &mulfile){
		open(idxfile,mulfile);
	}

}
