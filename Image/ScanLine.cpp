//Copyright © 2021 Charles Kerr. All rights reserved.

#include "ScanLine.hpp"
#include <algorithm>
namespace IMG {
	//===============================================================
	ScanLine::ScanLine(std::size_t length,std::uint32_t fill){
		size(length,fill);
	}
	
	//===============================================================
	const Color& ScanLine::operator[](std::size_t index) const {
		return _colors[index];
	}
	//===============================================================
	Color& ScanLine::operator[](std::size_t index) {
		return _colors[index];
	}
	//===============================================================
	std::size_t ScanLine::size() const {
		return _colors.size();
	}
	//===============================================================
	void ScanLine::size(std::size_t length,std::uint32_t fill ) {
		_colors.resize(length,Color(fill,ColorType::argb));
	}


	//===============================================================
	void ScanLine::fill(std::uint32_t color, std::size_t startloc ,std::size_t length){
		if (length == 0){
			length = size() ;
		}
		auto fillcolor = Color(color,ColorType::argb);
		auto temp = length - startloc;
		auto actual = std::min(length,temp);
		for (auto i=0; i < actual; i++){
			_colors[i+startloc] = fillcolor;
		}
	}
	
	//===============================================================
	void ScanLine::flip() {
		std::reverse(_colors.begin(), _colors.end());
	}

	//===============================================================
	std::vector<ScanLine::run_length> ScanLine::encode(const Color &color,bool use_alpha) const{
		auto xloc = 0 ;
		
		std::vector<run_length> values ;
		auto lastx = 0 ;
		while (xloc < _colors.size()) {
			if (!_colors[xloc].isEqual(color, use_alpha)){
				struct run_length value ;
				value.offset = xloc-lastx;
				while (!_colors[xloc].isEqual(color, use_alpha)){
					value.colors.push_back(_colors[xloc]);
					lastx = xloc ;
					xloc++;
					if (xloc >= _colors.size()){
						break;
					}
				}
				values.push_back(value);
			}
			xloc++ ;
		}
		return values ;
	}


}
