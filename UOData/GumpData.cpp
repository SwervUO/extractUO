//

#include "GumpData.hpp"
#include <iostream>
#include <algorithm>
#include <filesystem>

using namespace std::string_literals;
namespace UO {
	//===============================================================
	const std::string GumpData::_hash_format_1 = "build/gumpartlegacymul/{8}.tga"s ;
	const std::string GumpData::_hash_format_2 = "build/gumpartlegacymul/{7}.tga"s ;
	
	const std::string GumpData::_uop_file = "gumpartLegacyMUL.uop"s;
	const std::string GumpData::_idx_file = "gumpidx.mul"s;
	const std::string GumpData::_mul_file = "gumpart.mul"s;
	
	
	//===============================================================
	bool GumpData::processEntry(std::size_t entry, std::size_t index, const std::vector<std::uint8_t> &data) {
		if (data.size() <=8){
			return true ;
		}
		_gumps.insert_or_assign(index, data);
		return true ;
	}
	//===============================================================
	void GumpData::recordData(std::uint32_t record_number, std::uint32_t extra, std::vector<std::uint8_t> &record_data) {
		auto height = static_cast<std::uint32_t>((extra & 0xFFFF));
		auto width = static_cast<std::uint32_t>(((extra>>16) & 0xFFFF));
		//The first 8 bytes of the data represent the the width
		//(bytes 0-4) and height (bytes 4-8) of the gump
		std::vector<std::uint8_t> tempdata(record_data.size() + 8,0);
		
		auto ptr = reinterpret_cast<std::uint32_t*>(tempdata.data());
		*ptr = width ;
		*(ptr+1) = height ;
		std::copy(record_data.begin(),record_data.end(),tempdata.begin()+8);
		_gumps.insert_or_assign(record_number, tempdata);
		
	}
	//===============================================================
	IMG::Bitmap GumpData::convert(const std::vector<std::uint8_t> &data) const {
		if (data.size() <8){
			return IMG::Bitmap(0,0);
		}
		auto ptr = reinterpret_cast<const std::uint32_t*>(data.data());
		auto width = *ptr ;
		auto height = *(ptr+1) ;

		IMG::Bitmap bitmap(width,height,0xFFFFFF);
		if (bitmap.empty()){
			return bitmap ;
		}
		std::vector<std::uint8_t> newdata(data.size()-8,0);
		std::copy(data.begin()+8,data.end(),newdata.begin());
		
		std::vector<unsigned int> lookup(height,0) ;
		
		std::copy(newdata.data(),newdata.data()+(height*4),reinterpret_cast<unsigned char*>(lookup.data()));
		auto iter = lookup.begin() ;
		auto y = 0 ;
		while (iter != lookup.end()){
			auto amount = (newdata.size()) - ((*iter)*4) ;
			if ((iter+1) < lookup.end()){
				amount = ((*(iter+1)) - (*iter)) *4 ;
			}
			std::vector<unsigned short> rundata(amount/2,0);
			std::copy(newdata.data()+((*iter)*4),newdata.data()+((*iter)*4)+amount,reinterpret_cast<unsigned char*>(rundata.data()));
			auto piter = rundata.begin() ;
			auto x = 0 ;
			while (piter != rundata.end()){
				auto value = *(piter++);
				auto run = *(piter++) ;
				for (auto i=0 ; i< run ; i++){
					bitmap.at(x++,height - (y+1)) = value ;
				}
			}
			y++ ;
			iter++;
		}
		return bitmap;
	}
	
	//===============================================================
	void GumpData::openUOP(const std::string &uopfile){
		_gumps.clear();
		
		loadUOP(uopfile, 0x7FFFF ,_hash_format_1,_hash_format_2);
	}

	/************************************************************************
	 public methods
	 ***********************************************************************/
	std::size_t GumpData::maxGump() const {
		if (_gumps.empty()){
			return 0;
		}
		return _gumps.crbegin()->first + 1;
	}
	
	//===============================================================
	bool GumpData::hasGump(std::size_t tileid) const {
		return (_gumps.find(tileid) == _gumps.end())?false:true ;
	}
	
	//===============================================================
	IMG::Bitmap GumpData::gump(std::size_t tileid) const {
		auto iter = _gumps.find(tileid);
		if (iter == _gumps.end()){
			return IMG::Bitmap(0,0);
		}
		return convert(iter->second).invert();
	}

	
	//===============================================================
	void GumpData::open(const std::string &uodir_uopfile){
		if (uodir_uopfile.empty()){
			return ;
		}
		auto path = std::filesystem::path(uodir_uopfile);
		if (std::filesystem::is_directory(path)) {
			// test  few things
			auto uoppath = path / std::filesystem::path(_uop_file);
			if (std::filesystem::exists(uoppath)){
				openUOP(uoppath.string());
			}
			else {
				auto idxpath = path / std::filesystem::path(_idx_file);
				auto mulpath = path / std::filesystem::path(_mul_file);
				open(idxpath.string(),mulpath.string());
			}
		}
		else {
			openUOP(path.string());
		}
	}
	//===============================================================
	void GumpData::open(const std::string &idxfile, const std::string &mulfil){
		_gumps.clear();
		
		processFiles(idxfile, mulfil);
	}
	//===============================================================
	GumpData::GumpData(const std::string &uodir_uopfile){
		if (uodir_uopfile.empty()){
			return ;
		}
		open(uodir_uopfile);
	}
	//===============================================================
	GumpData::GumpData(const std::string &idxpath,const std::string& mulpath){
		open(idxpath,mulpath);
	}

}
