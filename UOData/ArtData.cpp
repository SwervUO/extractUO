//

#include "ArtData.hpp"
#include "Buffer.hpp"
#include "UOAlerts.hpp"
#include <iostream>
#include <filesystem>
#include <algorithm>
using namespace std::string_literals;
namespace UO {
	//===============================================================
	const std::string ArtData::_uop_file = "artLegacyMUL.uop"s;
	const std::string ArtData::_idx_file = "artidx.mul"s;
	const std::string ArtData::_mul_file = "art.mul"s;
	const std::string ArtData::_hash_format = "build/artlegacymul/{8}.tga"s;
	
	//===============================================================
	IMG::Bitmap ArtData::convertTerrain( const std::vector<std::uint8_t> &data) const{
		IMG::Bitmap bitmap(0,0,0xFFFFFF);
		
		if (data.empty())
		{
			return bitmap;
		}
		auto width = 44 ;
		auto height = 44 ;
		bitmap.size(width,height,0xFFFFFF);
		const std::uint16_t *ptrData = reinterpret_cast<const std::uint16_t*>(data.data());
		
		auto run = 2 ;
		int xloc = 21 ;
		auto data_index = 0 ;
		for (auto height = 0 ; height<22;height++){
			for (auto offset = 0  ; offset < run; offset++){
				bitmap.at(xloc+offset,height) = *(ptrData + data_index) ; //pixel[data_index] ;
				data_index += 1 ;
			}
			xloc-- ;
			run +=2 ;
		}
		xloc = 0 ;
		run = 44 ;
		for (auto height = 22 ; height<44;height++){
			for (auto offset = 0  ; offset < run; offset++){
				bitmap.at(xloc+offset,height) = *(ptrData + data_index);//pixel[data_index] ;
				data_index += 1 ;
			}
			xloc++ ;
			run -=2 ;
		}
		return bitmap;
		
	}
	//===============================================================
	//Format of the art data
	// 	std::uint32_t unknown (between 1 and 0xFFFF)
	// 	std::uint16_t width   if width>=1024 or == 0 invalid
	//	std::uint16_t height  if height>=1024 or == 0 invalid
	//    std::uint16_t[height]  offsets in this data stream for row data from
	//				    the end of the offset table
	//Row data format
	//	std::uint16_t offset into the image (from current x for the colors to start)
	//	std::uint16_t run  How many colors exist
	//	std::uint16_t colors[run]  // colors to do
	//	above repeated until offset+run == 0
	//
	IMG::Bitmap ArtData::convertArt( const std::vector<std::uint8_t> &data) const {
		const std::uint16_t *ptrData = reinterpret_cast<const std::uint16_t*>(data.data());
		auto delta = 2 ;
		auto width = *(ptrData+delta);
		auto height = *(ptrData+(delta+1));
		if ((width >= 1024) || (width ==0)|| (height >=1024) || (height ==0)){
			throw InvalidArtSize( width, height);
		}
		IMG::Bitmap bitmap(width,height,0xFFFFFF);
		std::vector<unsigned short> offsets(height,0);
		// Read in the row offsets for the data
		for (auto index = 0 ; index < height; index++){
			offsets[index] = *(ptrData+(2+index+delta));
		}
		auto x= 0 ;
		auto y = 0 ;
		// Set up an offset to just past the offset table
		auto dataindex = height+2+delta ;
		// Point to the first data for y=0
		auto curindex = dataindex + offsets[y] ;
		// loop through each y
		std::vector<int> test ;
		while (y < height){
			auto xoffset = *(ptrData+curindex++) ;
			auto run = *(ptrData+curindex++);
			if ((xoffset+run) >= 2048) {
				return bitmap;
			}
			else if ((xoffset+run)!=0) {
				x += xoffset ;
				for (auto i = 0 ; i< run; i++){
					bitmap.at(x++,y) = *(ptrData+curindex++);
				}
			}
			else {
				x = 0 ;
				y++ ;
				test.push_back(curindex-dataindex);
				curindex = dataindex + offsets[y];
			}
		}
		return bitmap ;
	}
	//===============================================================
	std::vector<std::uint8_t> ArtData::convertTerrain(const IMG::Bitmap &bitmap) const {
		Buffer pixels ;
		auto run = 2 ;
		int xloc = 21 ;
		auto data_index = 0 ;
		for (auto height = 0 ; height<22;height++){
			for (auto offset = 0  ; offset < run; offset++){
				pixels<<(bitmap.at(xloc+offset,height).color());
				data_index += 1 ;
			}
			xloc-- ;
			run +=2 ;
		}
		xloc = 0 ;
		run = 44 ;
		for (auto height = 22 ; height<44;height++){
			for (auto offset = 0  ; offset < run; offset++){
				pixels<<(bitmap.at(xloc+offset,height).color());
				data_index += 1 ;
			}
			xloc++ ;
			run -=2 ;
		}
		
		return pixels.raw();
		
	}
	//===============================================================
	std::vector<std::uint8_t> ArtData::convertArt(const IMG::Bitmap &bitmap) const {
		Buffer pixels ;
		pixels << static_cast<std::uint32_t>(1234) ;
		auto [width,height] = bitmap.size() ;
		if ((width >= 1024) || (width == 0 )){
			throw InvalidArtSize( static_cast<std::size_t>(width), static_cast<std::size_t>(height));
		}
		if ((height >= 1024) || (height == 0 )){
			throw InvalidArtSize( static_cast<std::size_t>(width), static_cast<std::size_t>(height));
		}
		pixels << static_cast<std::uint16_t>(width) ;
		pixels <<static_cast<std::uint16_t>(height) ;
		// Build the offset table ;
		for (auto i=0 ; i < height ; i++){
			pixels << static_cast<std::uint16_t>(0);
		}
		auto data_start = pixels.position();
		// Now get the data
		std::vector<std::uint16_t> offsets(height,0);
		for (auto i=0 ; i < height ; i++){
			offsets[i] = static_cast<std::uint16_t>(pixels.position() - data_start);
			auto values = bitmap.row(i).encode(IMG::Color(static_cast<std::uint32_t>(0xFFFFFF)), false);
			for (const auto &entry: values){
				pixels << static_cast<std::uint16_t>(entry.offset) << static_cast<std::uint16_t>(entry.colors.size());
				for (const auto &color:entry.colors){
					pixels << color.color() ;
				}
			}
		}
		// Ok, now go back and update the offsets
		pixels.position(8);
		for (auto off : offsets){
			pixels<<off;
		}
		return pixels.raw();
	}

	//===============================================================
	// Provides the data associated with the corresponding record number
	void ArtData::recordData(std::uint32_t record_number, std::uint32_t extra,std::vector<std::uint8_t> &record_data) {
		if (record_number < 0x4000) {
			_terrain.insert_or_assign(record_number, record_data);
		}
		else {
			_terrain.insert_or_assign(record_number-0x4000, record_data);

		}
	}
	
	//===============================================================
	bool ArtData::processEntry(std::size_t entry, std::size_t index, const std::vector<std::uint8_t> &data) {
		if (index < 0x4000) {
			_terrain.insert_or_assign(index, data);
		}
		else {
			_art.insert_or_assign(index-0x4000, data);
			
		}
		return true ;

	}

	//===============================================================
	void ArtData::openUOP(const std::string &uopfile){
		_art.clear();
		_terrain.clear();
		loadUOP(uopfile, 0xa761+0x4000 ,_hash_format);
	}
	/***********************************************************************
	 public methods
	 ************************************************************************/
	//===============================================================
	std::size_t ArtData::maxArt() const {
		if (_art.empty()){
			return 0;
		}
		return  _art.crbegin()->first + 1 ;
		
	}
	//===============================================================
	std::size_t ArtData::maxTerrain() const {
		if (_terrain.empty()){
			return 0;
		}
		return  _terrain.crbegin()->first + 1 ;
	}
	
	//===============================================================
	bool ArtData::hasArt(std::size_t tileid) const {
		if (_art.find(tileid)!= _art.cend()){
			return true ;
		}
		return false ;
	}
	//===============================================================
	bool ArtData::hasTerrain(std::size_t tileid) const {
		if (_terrain.find(tileid)!= _terrain.cend()){
			return true ;
		}
		return false ;

	}

	//===============================================================
	IMG::Bitmap ArtData::art(std::size_t tileid) const {
		auto iter = _art.find(tileid) ;
		if (iter == _art.end()){
			return IMG::Bitmap(0,0);
		}
		return convertArt(iter->second);
	}
	//===============================================================
	IMG::Bitmap ArtData::terrain(std::size_t tileid) const {
		auto iter = _terrain.find(tileid) ;
		if (iter == _terrain.end()){
			return IMG::Bitmap(0,0);
		}
		return convertTerrain(iter->second);
	}
	
	//===============================================================
	void ArtData::art(std::size_t tileid, const IMG::Bitmap &bitmap){
		_art.insert_or_assign(tileid, convertArt(bitmap));
	}
	//===============================================================
	void ArtData::terrain(std::size_t tileid, const IMG::Bitmap &bitmap){
		_terrain.insert_or_assign(tileid, convertTerrain(bitmap));

	}

	//===============================================================
	void ArtData::open(const std::string &uodir_uopfile){
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
	void ArtData::open(const std::string &idxfile, const std::string &mulfil){
		_art.clear();
		_terrain.clear();
		processFiles(idxfile, mulfil);
	}
	//===============================================================
	ArtData::ArtData(const std::string &uodir_uopfile){
		if (uodir_uopfile.empty()){
			return ;
		}
		open(uodir_uopfile);
	}
	//===============================================================
	ArtData::ArtData(const std::string &idxpath,const std::string& mulpath){
		open(idxpath,mulpath);
	}

}
