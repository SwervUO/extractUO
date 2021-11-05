//Copyright © 2021 Charles Kerr. All rights reserved.

#ifndef Color_hpp
#define Color_hpp

#include <string>
#include <array>
#include <cstdint>
#include <type_traits>
#include <tuple>
#include <stdexcept>

namespace IMG {
	//===============================================================
	enum class ColorType {argb,rgba,rgb,abgr,bgra,bgr};
	//===============================================================
	class Color {
	private:
		
		std::array<std::uint8_t,4> _channels ;
		
		std::array<std::uint8_t,4> parse(std::uint32_t value) const;
		
		void convert(std::uint32_t color, ColorType type) ;
		void convert(std::uint16_t color);

		std::uint8_t find(const std::vector<Color> &lookup) const ;
		static constexpr auto _red = 0 ;
		static constexpr auto _green = 1 ;
		static constexpr auto _blue = 2 ;
		static constexpr auto _alpha = 3 ;

	public:
		static ColorType defaultType ;

		struct LookupColorFailed : public std::runtime_error {
			std::uint8_t alpha ;
			std::uint8_t red;
			std::uint8_t green;
			std::uint8_t blue ;
			LookupColorFailed(const Color &color);
		};
		struct LookupIndexFailed : public std::runtime_error {
			int index ;
			LookupIndexFailed(int index);
		};
		struct LoadStreamFailure : public std::runtime_error {
			LoadStreamFailure();
		};

		Color(std::uint8_t red, std::uint8_t green, std::uint8_t blue, std::uint8_t alpha=255);
		Color(float red, float green, float blue, float alpha=1.0);
		Color(std::uint32_t color,ColorType type=Color::defaultType);
		Color(std::uint16_t color=0);
		Color(const Color&) = default ;

		const std::uint8_t& red() const { return _channels[_red];}
		std::uint8_t& red()  { return _channels[_red];}
		const std::uint8_t& green() const { return _channels[_green];}
		std::uint8_t& green()  { return _channels[_green];}
		const std::uint8_t& blue() const { return _channels[_blue];}
		std::uint8_t& blue()  { return _channels[_blue];}
		const std::uint8_t& alpha() const { return _channels[_alpha];}
		std::uint8_t& alpha()  { return _channels[_alpha];}

		
		std::string description() const;
		
		std::uint32_t color(ColorType type) const;
		std::uint16_t color() const ;
		
		const std::array<std::uint8_t,4>& channels() const ;
		std::array<std::uint8_t,4>& channels();

		bool isEqual(const Color &color) const ;
		bool isEqual(const Color &color,bool compare_alpha) const ;

		bool isGrey() const ;

		//=============================================================
		bool operator==(const Color &value) const ;
		bool operator==(std::uint16_t value) const ;
		bool operator==(std::uint32_t value) const ;
		bool operator!=(const Color &value) const ;
		bool operator!=(std::uint16_t value) const ;
		bool operator!=(std::uint32_t value) const ;

		Color & operator=(const std::uint16_t value);
		Color & operator=(const std::uint32_t value);
		

		template<typename T>
		typename std::enable_if< std::is_integral_v<T> && !std::is_same_v<T, bool> ,std::tuple<T,T,T,T>>::type
		components() {
			return std::make_tuple(_channels[_alpha],_channels[_red],_channels[_green],_channels[_blue]);
		}
		template<typename T>
		typename std::enable_if< std::is_floating_point<T>::value,std::tuple<T,T,T,T>>::type
		components() {
			return std::make_tuple(static_cast<T>(_channels[_alpha])/255.0,static_cast<T>(_channels[_red])/255.0,static_cast<T>(_channels[_green])/255.0,static_cast<T>(_channels[_blue])/255.0);
		}

	};
}

#endif /* Color_hpp */
