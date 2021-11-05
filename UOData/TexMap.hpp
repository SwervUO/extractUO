//

#ifndef TexMap_hpp
#define TexMap_hpp

#include <string>
#include <cstdint>
#include "IDXMul.hpp"

#include <map>
#include <vector>

#include "Bitmap.hpp"
namespace UO {
	//===============================================================
	class TexMap : public IDXMul {
	private:
		static const std::string _idx_file ;
		static const std::string _mul_file ;
		
		std::map<std::size_t,std::vector<std::uint8_t>> _data ;
		// Provides the data associated with the corresponding record number
		void recordData(std::uint32_t record_number, std::uint32_t extra, std::vector<std::uint8_t> &record_data) final;

		IMG::Bitmap convertData(const std::vector<std::uint8_t> &data) const ;
		std::vector<std::uint8_t> convertData(const IMG::Bitmap &bitmap) const ;
		
	public:
		std::size_t maxTexid() const ;
		bool hasTexture(std::size_t tileid) const ;
		IMG::Bitmap texture(std::size_t tileid) const ;
		void texture(std::size_t tileid, const IMG::Bitmap & bitmap);
		
		bool open(const std::string &uodir);
		bool open(const std::string &idxfile, const std::string &mulfile);
		TexMap(const std::string& uodir="");
		TexMap(const std::string &idxfile,const std::string &mulfile);
	};
}
#endif /* TexMap_hpp */
