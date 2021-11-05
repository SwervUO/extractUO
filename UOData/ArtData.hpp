//

#ifndef ArtData_hpp
#define ArtData_hpp

#include <string>
#include <cstdint>
#include "IDXMul.hpp"
#include "UOPData.hpp"
#include <map>
#include <vector>
#include "Bitmap.hpp"

namespace UO {
	//===============================================================
	class ArtData : public IDXMul, public UOPData {
	private:
		static const std::string _uop_file ;
		static const std::string _idx_file ;
		static const std::string _mul_file ;
		static const std::string _hash_format ;
		std::map<std::size_t , std::vector<std::uint8_t>> _terrain ;
		std::map<std::size_t , std::vector<std::uint8_t>> _art ;

		
		IMG::Bitmap convertTerrain(  const std::vector<std::uint8_t> &data) const ;
		IMG::Bitmap convertArt( const std::vector<std::uint8_t> &data) const  ;
		
		std::vector<std::uint8_t> convertTerrain(  const IMG::Bitmap &data) const ;
		std::vector<std::uint8_t> convertArt( const IMG::Bitmap &data) const  ;

		// Provides the data associated with the corresponding record number
		void recordData(std::uint32_t record_number, std::uint32_t extra,std::vector<std::uint8_t> &record_data) final;

		bool processEntry(std::size_t entry, std::size_t index, const std::vector<std::uint8_t> &data) final;
		
		void openUOP(const std::string &uopfile);

	public:
		std::size_t maxArt() const ;
		std::size_t maxTerrain() const ;
		
		bool hasArt(std::size_t tileid) const ;
		bool hasTerrain(std::size_t tileid) const ;
		
		IMG::Bitmap art(std::size_t tileid) const ;
		IMG::Bitmap terrain(std::size_t tileid) const ;
		
		void art(std::size_t tileid, const IMG::Bitmap &bitmap);
		void terrain(std::size_t tileid, const IMG::Bitmap &bitmap);

		
		
		void open(const std::string &uodir_uopfile);
		void open(const std::string &idxfile, const std::string &mulfil);
		
		ArtData(const std::string &uodir_uopfile="");
		ArtData(const std::string &idxpath,const std::string& mulpath);
	};
}

#endif /* ArtData_hpp */
