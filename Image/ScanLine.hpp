//Copyright © 2021 Charles Kerr. All rights reserved.

#ifndef ScanLine_hpp
#define ScanLine_hpp

#include <string>
#include <vector>
#include <cstdint>
#include "Color.hpp"

namespace IMG {
	//===============================================================
	class ScanLine {
	private:
		std::vector<Color> _colors ;
		
	public:
		struct run_length {
			std::vector<Color> colors ;
			std::uint32_t offset ;
		};
		
		ScanLine(std::size_t length=0,std::uint32_t fill = 0);
		
		const std::vector<Color>& colors() const { return _colors;}
		std::vector<Color>& colors()  { return _colors;}

		const Color& operator[](std::size_t index) const ;
		Color& operator[](std::size_t index) ;
		
		std::size_t size() const ;
		void size(std::size_t length,std::uint32_t fill=0) ;
		
		
		void fill(std::uint32_t color, std::size_t startloc = 0,std::size_t length=0 );
		void flip() ;
		
		std::vector<run_length> encode(const Color &color,bool use_alpha) const;
	

	};
}

#endif /* ScanLine_hpp */
