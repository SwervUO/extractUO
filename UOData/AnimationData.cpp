//

#include "AnimationData.hpp"
#include "UOAlerts.hpp"
#include "StringUtility.hpp"
#include "Buffer.hpp"
#include <iostream>
#include <filesystem>
#include <array>

using namespace std::string_literals;
namespace UO{
	//===============================================================
	const std::string AnimationData::_idx_file = "anim{}.idx"s ;
	const std::string AnimationData::_mul_file = "anim{}.mul"s;

	/*************************************************************************
	 WORD[256] Palette
	 DWORD FrameCount
	 DWORD[FrameCount] FrameOffset
	 
	 Frame data:
	 Seek from the end of Palette plus FrameOffset[FrameNum] bytes to find the start of Frame
	 WORD ImageCenterX
	 WORD ImageCenterY
	 WORD Width
	 WORD Height
	 data stream
	 ***************************************************/
	//===============================================================
	std::vector<IMG::Bitmap> AnimationData::convert(const std::vector<std::uint8_t>  &data){
		std::vector<IMG::Bitmap> rvalue;
		if (data.empty()){
			return rvalue ;
		}
		Buffer buffer(data) ;
		std::array<std::uint16_t,256>  palette ;
		std::uint16_t color ;
		for (auto i=0 ; i<256;i++){
			buffer>>color ;
			palette[i] = color ;
		}
		auto end_pallette = buffer.position() ;
		std::uint32_t framecount ;
		buffer >> framecount ;
		std::vector<std::uint32_t> offsets(framecount,0);
		for (auto i=0 ; i<framecount ; i++){
			buffer >> (offsets[i]) ;
		}
		for (auto i = 0 ; i < framecount ; i++){
			// Get each frame
			buffer.position(end_pallette+offsets[i]);
			auto bitmap = convert(palette,buffer) ;
			rvalue.push_back(bitmap);

		}
		
		return rvalue;
	}
	//===============================================================
	IMG::Bitmap AnimationData::convert(const std::array<std::uint16_t,256> &palette, Buffer &buffer){
		
		std::int32_t mask = (0x200 << 22) | (0x200 <<12) ;
		std::int32_t header ;
		std::int16_t xCenter ;
		std::int16_t yCenter ;
		std::uint16_t width ;
		std::uint16_t height ;
		buffer >> xCenter;
		buffer >> yCenter;
		buffer >> width ;
		buffer >> height ;
		if ((width==0)|| (height==0)){
			return IMG::Bitmap(0,0,0xffffff);
		}
		IMG::Bitmap bitmap(width,height,0xffffff);
		
		std::int32_t xBase = xCenter- 0x200 ;
		std::int32_t yBase = (yCenter+height)-0x200;
		std::int32_t offset = xBase ;
		offset += yBase *width ;
		
		
		buffer >> header ;
		while (header != 0x7fff7fff){
			header ^=mask ;  // mask off bits
			auto current = offset + ((((header >> 12) & 0x3FF) * width)  + ((header >> 22) & 0x3FF));
			auto end = current + (header & 0xFFF);
			std::uint8_t color ;
			while (current < end){
				buffer >> color ;
				bitmap.at(current%width,current/width)=palette[color] ;
				current++;
			}
			
			
			buffer >> header ;
		}
		

		return bitmap ;
	}

	//===============================================================
	std::pair<std::string,std::string> AnimationData::getFileName(std::int32_t fileid){
		
		if ((fileid >=6 ) || (fileid ==1) || (fileid<0)){
			throw InvalidAnimationFileID(fileid);
		}
		if (fileid==0){
			auto idxfile = strutil::format(_idx_file,""s) ;
			auto mulfile = strutil::format(_mul_file,""s);
			return std::make_pair(idxfile, mulfile);
		}
		else {
			auto idxfile = strutil::format(_idx_file,std::to_string(fileid)) ;
			auto mulfile = strutil::format(_mul_file,std::to_string(fileid));
			return std::make_pair(idxfile, mulfile);

		}
	}
	
	//===============================================================
	// Provides the data associated with the corresponding record number
	void AnimationData::recordData(std::uint32_t record_number, std::uint32_t extra, std::vector<std::uint8_t> &record_data) {
		_animations.insert_or_assign(record_number, record_data);
	}
	
	//===============================================================
	std::size_t AnimationData::maxID() const {
		if (_animations.empty()){
			return 0;
		}
		return _animations.crbegin()->first + 1 ;
	}
	//===============================================================
	bool AnimationData::hasAnimation(std::size_t animid) const {
		return (_animations.find(animid) != _animations.end() );
	}
	
	//===============================================================
	std::vector<IMG::Bitmap> AnimationData::animation(std::size_t animid) {
		auto iter = _animations.find(animid) ;
		if (iter== _animations.end()){
			return std::vector<IMG::Bitmap>();
		}
		return convert(iter->second);
	}
	
	
	//===============================================================
	void AnimationData::open(const std::string &idxfile, const std::string &mulfile){
		_animations.clear();
		processFiles(idxfile, mulfile);

	}
	//===============================================================
	void AnimationData::open(const std::string &uodir, std::int32_t fileid){
		auto [idx_file,mul_file] = getFileName(fileid);
		auto path = std::filesystem::path(uodir);
		auto idxfile = path / std::filesystem::path(idx_file);
		auto mulfile = path / std::filesystem::path(mul_file);
		open(idxfile,mulfile);
	}
	//===============================================================
	AnimationData::AnimationData(){
		
	}
	//===============================================================
	AnimationData::AnimationData(const std::string &uodir, std::int32_t fileid){
		open(uodir,fileid);
	}
	//===============================================================
	AnimationData::AnimationData(const std::string &idxfile, const std::string &mulfile){
		open(idxfile,mulfile);
	}

}
