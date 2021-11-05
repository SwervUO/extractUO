//Copyright © 2021 Charles Kerr. All rights reserved.

#ifndef Bitmap_hpp
#define Bitmap_hpp

#include <string>
#include <vector>
#include "ScanLine.hpp"
#include <utility>
#include <cstdint>
#include <stdexcept>
#include <ostream>
#include <istream>
namespace IMG {
	//===============================================================
	//===============================================================
	class BitmapAlert : public std::runtime_error {
	public:
		BitmapAlert(const std::string & msg);
	};
	//===============================================================
	class InvalidCompression : public BitmapAlert {
	public:
		std::uint32_t compression;
		
		InvalidCompression(std::uint32_t compress);
	};
	//===============================================================
	class InvalidFile : public BitmapAlert {
	public:
		std::string filepath;
		std::uint32_t signature ;
		std::uint32_t version ;

		InvalidFile(const std::string &file,std::uint32_t sig,std::uint32_t version);
	};
	//===============================================================
	class OpenFileFailure : public BitmapAlert {
	public:
		std::string filepath;
		
		OpenFileFailure(const std::string &file);
	};
	//===============================================================
	class InvalidBitSize : public BitmapAlert {
	public:
		std::uint32_t bits ;
		
		InvalidBitSize(std::uint32_t bits);
	};
	//===============================================================
	class InputFailure : public BitmapAlert {
	public:
		std::uint64_t xloc ;
		std::uint64_t yloc ;
		InputFailure(std::uint64_t x, std::uint64_t y);
	};

	/************************************************************************
	 Bitmap
	 ***********************************************************************/
	
	class Bitmap {
	
	private:
		std::size_t _width ;
		std::size_t _height ;
		std::vector<ScanLine> _lines ;
		std::vector<Color> _palette ;
		static constexpr std::uint32_t _raw_signature = 0xF0F0 ;
		static constexpr std::uint32_t _raw_version = 0 ;
	
		struct bmp_sig {
			std::uint16_t indicator ;
			std::uint32_t size ;
			std::uint32_t unused ;
			std::uint32_t offset ;
			bmp_sig() ;
			void load(std::istream &input) ;
			void save(std::ostream &output);
			void reset();
			bool valid() const ;
		private:
			friend class Bitmap ;
			static constexpr std::uint16_t _valid_indicator = 19778; // equivalent to "BM"
		}  ;
		struct bmp_header {
			std::uint32_t size ;
			std::uint32_t width ;
			std::uint32_t height ;
			std::uint16_t planes ;
			std::uint16_t bits_per_pixel ;
			std::uint32_t compression ;
			std::uint32_t compressed_size ;
			std::uint32_t horizon_resolution ;
			std::uint32_t vertical_resolution ;
			std::uint32_t colors_used ;
			std::uint32_t important_colors ;
			bmp_header();
			void load(std::istream &input);
			void save(std::ostream &output);
			bool hasColorTable() const ;
			void reset();
			std::uint32_t pixelSize() const ;
		}  ;
	public:
		enum FileType {invalid,raw,bmp};
	private:
		std::vector<Color> generateColorLookup() ;

		std::uint32_t padSize(std::uint32_t width, std::uint32_t bytes_per_pixel) const ;
		void loadBMP(const std::string &filepath);
		void loadRAW(const std::string &filepath);
		
		void writeBMP(const std::string &filepath, std::uint32_t bitsize, const std::vector<Color> &lookup );
		void writeRAW(const std::string &filepath);
		
		std::uint8_t index(const Color & color, const std::vector<Color> &lookup) const;
		FileType typeOf(const std::string &filepath) const;

	public:
		void open(const std::string &filepath);
		void open(const std::string &filepath, FileType type);
		void save(const std::string &filepath, FileType type=FileType::bmp,std::uint32_t bitsize=16, const std::vector<Color> &lookup = std::vector<Color>());
		
		Bitmap(std::size_t width=0, std::size_t height=0, std::uint32_t fill=0);
		Bitmap(const std::string &filepath);
		std::pair<std::size_t,std::size_t> size() const ;
		void size(std::size_t width, std::size_t height, std::uint32_t fill = 0);
		
		Bitmap& fill(std::uint32_t color);
		Bitmap& invert() ;
		Bitmap& flip() ;
		bool empty() const ;
		
		const Color& at(std::size_t x, std::size_t y) const ;
		Color& at(std::size_t x, std::size_t y)  ;
		
		ScanLine& row(std::size_t y) ;
		const ScanLine& row(std::size_t y) const ;
		
		Bitmap copy(std::size_t x, std::size_t y , std::size_t width=0, std::size_t height=0 ) const;
		
		void paste(const Bitmap &source, std::size_t xloc, std::size_t yloc, std::size_t width=0, std::size_t height=0);
		

	};
}

#endif /* Bitmap_hpp */
