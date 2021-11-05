//Copyright © 2021 Charles Kerr. All rights reserved.

#include "Color.hpp"
#include "StringUtility.hpp"
#include <algorithm>

using namespace std::string_literals;
namespace IMG {
	
	ColorType Color::defaultType = ColorType::rgb ;
	
	//===============================================================
	Color::LookupColorFailed::LookupColorFailed(const Color &color): std::runtime_error("Unable to find color in lookup: "s+color.description()){
		alpha = color.channels()[_alpha];
		red = color.channels()[_red];
		green = color.channels()[_green];
		blue = color.channels()[_blue];
	}
	//===============================================================
	Color::LookupIndexFailed::LookupIndexFailed(int index): std::runtime_error("Unable to find index ("s+std::to_string(index)+") in lookup."s){
		this->index = index ;
	}
	//===============================================================
	Color::LoadStreamFailure::LoadStreamFailure(): std::runtime_error("Read error on input stream."s){
		
	}


	//===============================================================
	std::array<std::uint8_t,4> Color::parse(std::uint32_t value) const{
		std::array<std::uint8_t,4> rvalue ;
		for (auto i=0;i<4;i++){
			rvalue[i] = (value>>(i*8))&0xFF;
		}
		return rvalue ;
	}

	//===============================================================
	void Color::convert(std::uint32_t color, ColorType type) {
		auto chan = parse(color);
		switch(type){
			case ColorType::rgb:
				_channels[_alpha] = 0;
				_channels[_red] = chan[2] ;
				_channels[_green] = chan[1] ;
				_channels[_blue] = chan[0] ;
				break;
			case ColorType::rgba:
				_channels[_alpha] = chan[0];
				_channels[_red] = chan[3] ;
				_channels[_green] = chan[2] ;
				_channels[_blue] = chan[1] ;
				break;
			case ColorType::argb:
				_channels[_alpha] = chan[3] ;
				_channels[_red] = chan[2] ;
				_channels[_green] = chan[1] ;
				_channels[_blue] = chan[0] ;
				break;
			case ColorType::bgr:
				_channels[_alpha] = 0;
				_channels[_red] = chan[0] ;
				_channels[_green] = chan[1] ;
				_channels[_blue] = chan[2] ;
				break;
			case ColorType::bgra:
				_channels[_alpha] = chan[0];
				_channels[_red] = chan[1] ;
				_channels[_green] = chan[2] ;
				_channels[_blue] = chan[3] ;
				break;
			case ColorType::abgr:
				_channels[_alpha] = chan[3] ;
				_channels[_red] = chan[0] ;
				_channels[_green] = chan[1] ;
				_channels[_blue] = chan[2] ;
				break;

		}
		
	}

	//===============================================================
	void Color::convert(std::uint16_t color){
		_channels[_alpha] = ((color>>15)&0x1)*255 ;
		auto red  = static_cast<uint8_t>(((color>>10) & 0x1f));
		_channels[_red] = (red * 8) + (red%8) ;
		auto green = static_cast<uint8_t>(((color>>5) & 0x1f));
		_channels[_green] = (green * 8) + (green%8) ;
		auto blue = static_cast<uint8_t>(((color>>0) & 0x1f));
		_channels[_blue] = (blue * 8) + (blue%8) ;
	}

	//===============================================================
	std::uint8_t Color::find(const std::vector<Color> &lookup) const {
		auto index = 0 ; ;
		for (const auto &entry :lookup){
			if (entry == *this) {
				return index ;
			}
			index++;
		}
			// We didn't find it!
		throw LookupColorFailed(*this);
	}


	//===============================================================
	Color::Color(std::uint8_t red, std::uint8_t green, std::uint8_t blue, std::uint8_t alpha){
		_channels[_red]=red ;
		_channels[_green] = green;
		_channels[_blue] = blue;
		_channels[_alpha] = _alpha;
	}
	//===============================================================
	Color::Color(float red, float green, float blue, float alpha){
		_channels[_red]= static_cast<uint8_t>((red*255)) ;
		_channels[_green] = static_cast<uint8_t>((green*255)) ;
		_channels[_blue] = static_cast<uint8_t>((blue*255)) ;
		_channels[_alpha] = static_cast<uint8_t>((alpha*255)) ;
	}
	//===============================================================
	Color::Color(std::uint32_t color,ColorType type){
		convert(color,type);
	}
	//===============================================================
	Color::Color(std::uint16_t color){
		convert(color);
	}

	//=============================================================
	std::string Color::description() const {
		std::stringstream output ;
		output << "Alpha: " <<strutil::numtostr(_channels[_alpha],16,true,2) <<" , " ;
		output << "Red: " <<strutil::numtostr(_channels[_red],16,true,2) <<" , " ;
		output << "Green: " <<strutil::numtostr(_channels[_green],16,true,2) <<" , " ;
		output << "Blue: " <<strutil::numtostr(_channels[_blue],16,true,2)  ;
		return output.str();
	}
	//=============================================================
	std::uint32_t Color::color(ColorType type) const{
		std::array<std::uint32_t,4> chan ;
		switch(type){
			case ColorType::rgb:
				chan[3] = _channels[_alpha] ;
				chan[2] = _channels[_red]  ;
				chan[1] = _channels[_green]  ;
				chan[0] = _channels[_blue] ;
				break;
			case ColorType::rgba:
				chan[0] = _channels[_alpha] ;
				chan[3] = _channels[_red]  ;
				chan[2] = _channels[_green]  ;
				chan[1] = _channels[_blue] ;
				break;
			case ColorType::argb:
				chan[3] = _channels[_alpha]  ;
				chan[2] = _channels[_red]  ;
				chan[1] = _channels[_green] ;
				chan[0] = _channels[_blue]  ;
				break;
			case ColorType::bgr:
				chan[3] = _channels[_alpha] ;
				chan[0] = _channels[_red] ;
				chan[1] = _channels[_green] ;
				chan[2] = _channels[_blue] ;
				break;
			case ColorType::bgra:
				chan[0] = _channels[_alpha] ;
				chan[1] = _channels[_red]  ;
				chan[2] = _channels[_green]  ;
				chan[3] = _channels[_blue]  ;
				break;
			case ColorType::abgr:
				chan[3] = _channels[_alpha] ;
				chan[0] = _channels[_red] ;
				chan[1] = _channels[_green] ;
				chan[2] = _channels[_blue] ;
				break;
				
		}
		return (chan[3]<<24) | (chan[2]<<16)| (chan[1]<<8)| (chan[0]) ;
	}
	//=============================================================
	std::uint16_t Color::color() const {

		auto alpha = ((static_cast<std::uint16_t>(_channels[_alpha])/255) + (((static_cast<std::uint16_t>(_channels[_alpha])%255)>127)?1:0));
		auto red = (static_cast<std::uint16_t>(_channels[_red])/8)<<10;
		auto green = (static_cast<std::uint16_t>(_channels[_green])/8)<<5  ;
		auto blue = (static_cast<std::uint16_t>(_channels[_blue])/8)  ;
		return alpha | red | green | blue ;

	}
	//===============================================================
	const std::array<std::uint8_t,4>& Color::channels() const {
		return _channels;
	}
	//===============================================================
	std::array<std::uint8_t,4>& Color::channels(){
		return _channels;
	}

	//=============================================================
	bool Color::isEqual(const Color &color ) const {
		auto compare_alpha = true ;
		if ((defaultType == ColorType::rgb) || (defaultType == ColorType::bgr)){
			compare_alpha = false ;
		}
		return isEqual(color, compare_alpha);
	}
	//=============================================================
	bool Color::isEqual(const Color &color,bool compare_alpha) const {
		for (auto i=0;i<4;i++){
			if ((i!=_alpha) || compare_alpha) {
				if (_channels[i] != color._channels[i]){
					return false ;
				}
			}
		}
		return true ;

	}
	//=============================================================
	bool Color::isGrey() const {
		return (_channels[_red]==_channels[_green]) && (_channels[_blue]==_channels[_green]) ;
	}
	//=============================================================
	bool Color::operator==(const Color &value) const {
		return isEqual(value);
	}
	//=============================================================
	bool Color::operator==(std::uint16_t value) const {
		return isEqual(Color(value));
	}
	//=============================================================
	bool Color::operator==(std::uint32_t value) const {
		return isEqual(Color(value));
	}
	//=============================================================
	bool Color::operator!=(const Color &value) const {
		return !operator==(value);
	}
	//=============================================================
	bool Color::operator!=(std::uint16_t value) const {
		return !operator==(value);
	}
	//=============================================================
	bool Color::operator!=(std::uint32_t value) const {
		return !operator==(value);
	}

	//=============================================================
	Color & Color::operator=(const std::uint16_t value){
		auto temp = Color(value) ;
		std::copy(temp._channels.begin(),temp._channels.end(),_channels.begin());
		return *this;
	}
	//=============================================================
	Color & Color::operator=(const std::uint32_t value){
		auto temp = Color(value,defaultType) ;
		std::copy(temp._channels.begin(),temp._channels.end(),_channels.begin());
		return *this;

	}
	


}
