//

#ifndef GumpData_hpp
#define GumpData_hpp

#include <string>
#include <cstdint>
#include <map>
#include <vector>

#include "Bitmap.hpp"
#include "IDXMul.hpp"
#include "UOPData.hpp"
namespace UO {
	//===============================================================
	class GumpData : public IDXMul, public UOPData {
	private:
		static const std::string _hash_format_1 ;
		static const std::string _hash_format_2 ;
		
		static const std::string _uop_file ;
		static const std::string _idx_file ;
		static const std::string _mul_file ;
		
		std::map<std::size_t,std::vector<std::uint8_t>> _gumps ;
		
		bool processEntry(std::size_t entry, std::size_t index, const std::vector<std::uint8_t> &data) final;
		void recordData(std::uint32_t record_number, std::uint32_t extra, std::vector<std::uint8_t> &record_data) final;
		// Data format for Gumps
		// std::uint32_t width
		// std::uint32_t height
		// std::uint32_t rowoffsets[height] ;
		
		// Row data is :
		//		std::uint16_t color
		//		std::utin16_t length ;
		IMG::Bitmap convert(const std::vector<std::uint8_t> &data) const;

		void openUOP(const std::string &uopfile);

	public:
		std::size_t maxGump() const ;
		
		bool hasGump(std::size_t tileid) const ;
		
		IMG::Bitmap gump(std::size_t tileid) const ;

		void open(const std::string &uodir_uopfile);
		void open(const std::string &idxfile, const std::string &mulfil);
		
		GumpData(const std::string &uodir_uopfile="");
		GumpData(const std::string &idxpath,const std::string& mulpath);

	};
}
#endif /* GumpData_hpp */
