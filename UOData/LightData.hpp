//

#ifndef LightData_hpp
#define LightData_hpp

#include <string>
#include <cstdint>
#include <map>
#include <vector>
#include "IDXMul.hpp"
#include <array>
#include "Bitmap.hpp"
namespace UO {
	//===============================================================
	class LightData : public IDXMul {
	private:
		
		static const std::string _idx_file ;
		static const std::string _mul_file ;
		
		std::map<std::size_t, std::vector<std::uint8_t>> _lights;
		
		std::array<std::uint16_t,64> _colors ;
		// Provides the data associated with the corresponding record number
		void recordData(std::uint32_t record_number, std::uint32_t extra, std::vector<std::uint8_t> &record_data) final;

		IMG::Bitmap convert(const std::vector<std::uint8_t> &data) const ;
		

	public:
		
		std::size_t maxID() const ;
		IMG::Bitmap bitmap(std::size_t index);
		LightData(const std::string &uodir);
		void load(const std::string &uodir);
		void load(const std::string &idxfile,const std::string &mulfile);
	};
}
#endif /* LightData_hpp */
