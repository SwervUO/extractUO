//

#ifndef RadarColor_hpp
#define RadarColor_hpp

#include <string>
#include <cstdint>
#include <vector>

namespace UO {
	//===============================================================
	class RadarColor {
	private:
		
		static const std::string _mul_file ;
		
		std::vector<std::uint16_t> _colors ;
		
	public:
		RadarColor(const std::string &uodir_mulfile="");
		void load(const std::string &uodir_mulfile);
		
		const std::uint16_t & operator[](std::size_t index) const;
		std::uint16_t & operator[](std::size_t index) ;

	};
}
#endif /* RadarColor_hpp */
