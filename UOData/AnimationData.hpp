//

#ifndef AnimationData_hpp
#define AnimationData_hpp

#include <string>
#include <cstdint>
#include <map>
#include <vector>
#include <utility>
#include "IDXMul.hpp"
#include "Bitmap.hpp"
#include "Color.hpp"

class Buffer;

namespace UO{
 	//===============================================================
	class AnimationData : public IDXMul {
	private:
		static const std::string _idx_file ;
		static const std::string _mul_file ;
		
		
		std::map<std::size_t, std::vector<std::uint8_t> > _animations ;
		
		std::vector<IMG::Bitmap> convert(const std::vector<std::uint8_t> &data);
		IMG::Bitmap convert(const std::array<std::uint16_t,256> &palette, Buffer &buffer);
		std::pair<std::string,std::string> getFileName(std::int32_t fileid);
		
		// Provides the data associated with the corresponding record number
		void recordData(std::uint32_t record_number, std::uint32_t extra, std::vector<std::uint8_t> &record_data) final;

	public:
		std::size_t maxID() const ;
		bool hasAnimation(std::size_t animid) const ;
		
		std::vector<IMG::Bitmap> animation(std::size_t animid) ;
		
		
		void open(const std::string &idxfile, const std::string &mulfile);
		void open(const std::string &uodir, std::int32_t fileid=0);
		AnimationData();
		AnimationData(const std::string &uodir, std::int32_t fileid=0);
		AnimationData(const std::string &idxfile, const std::string &mulfile);

	};
	
}
#endif /* AnimationData_hpp */
