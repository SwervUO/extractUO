//Copyright © 2021 Charles Kerr. All rights reserved.

#include "Bitmap.hpp"
#include "StringUtility.hpp"
#include <algorithm>
#include <fstream>

using namespace std::string_literals;
namespace IMG {
	
	//===============================================================
	std::vector<Color> Bitmap::generateColorLookup(){
		std::vector<Color> lookup ;
		
		for (auto x=0; x< _width;x++){
			for (auto y=0;y<_height;y++){
				auto iter = std::find(lookup.cbegin(),lookup.cend(),at(x,y));
				if (iter == lookup.end()){
					lookup.push_back(*iter);
				}
			}
		}
		return lookup ;
	}
	
	//===============================================================
	Bitmap::Bitmap(std::size_t width, std::size_t height, std::uint32_t fill){
		_palette = std::vector<Color>(0);
		size(width,height,fill);
	}
	//=====================================================================
	Bitmap::Bitmap(const std::string &filepath) : Bitmap(0,0){
		open(filepath);
	}
	
	//======================================================================
	Bitmap::FileType Bitmap::typeOf(const std::string &filepath) const {
		std::ifstream input(filepath,std::ios::binary);
		FileType type = FileType::invalid ;
		if (!input.is_open()){
			throw OpenFileFailure(filepath);
		}
		std::uint32_t sig =0;
		input.read(reinterpret_cast<char*>(&sig),sizeof(sig));
		if (input.gcount()!=sizeof(sig)){
			throw InvalidFile(filepath, sig, 0);
		}
		if ((sig&0xFF)== bmp_sig::_valid_indicator) {
			type = bmp;
		}
		else if (sig == _raw_signature){
			type = raw ;
		}
		else {
			throw InvalidFile(filepath, sig, 0);
		}
		input.close() ;
		return type ;
	}
	//===============================================================
	std::pair<std::size_t,std::size_t> Bitmap::size() const {
		return std::make_pair(_width, _height);
	}
	//===============================================================
	void Bitmap::size(std::size_t width, std::size_t height, std::uint32_t fill ){
		_width = width ;
		_height = height ;
		for (auto &entry : _lines){
			entry.size(width,fill);
		}
		_lines.resize(height,ScanLine(width,fill));
	}
	//===============================================================
	Bitmap& Bitmap::fill(std::uint32_t color){
		for (auto &entry : _lines){
			entry.fill(color);
		}
		return *this;
	}
	//===============================================================
	Bitmap& Bitmap::invert() {
		std::reverse(_lines.begin(), _lines.end());
		return *this;
	}
	//===============================================================
	Bitmap& Bitmap::flip() {
		for (auto &entry:_lines){
			entry.flip();
		}
		return *this;
	}

	//===============================================================
	bool Bitmap::empty() const {
		return (_width==0) || (_height==0) ;
	}

	//===============================================================
	const Color& Bitmap::at(std::size_t x, std::size_t y) const {
		return _lines[y][x] ;
	}
	//===============================================================
	Color& Bitmap::at(std::size_t x, std::size_t y)  {
		return _lines[y][x] ;

	}
	
	//===============================================================
	ScanLine& Bitmap::row(std::size_t y) {
		return _lines[y];
	}
	//===============================================================
	const ScanLine & Bitmap::row(std::size_t y) const {
		return _lines[y];
	}

	
	//===============================================================
	Bitmap Bitmap::copy(std::size_t x, std::size_t y , std::size_t width, std::size_t height ) const{
		if (width == 0 ){
			width = _width ;
		}
		if (height == 0) {
			height = _height ;
		}
		height = std::min((_height-y),height);
		width = std::min((_width-x),width);
		Bitmap temp(width,height);
		for (auto horiz=0;horiz<width;horiz++){
			for (auto vert=0;vert<height;vert++){
				temp.at(horiz,vert) = at(vert+x,horiz+y);
			}
		}
		return temp ;
	}
	//===============================================================
	void Bitmap::paste(const Bitmap &source, std::size_t xloc, std::size_t yloc, std::size_t width, std::size_t height){
		
		auto [srcwidth,srcheight] = source.size() ;
		if (width == 0 ){
			width = srcwidth ;
		}
		if (height == 0) {
			height =srcheight ;
		}
		height = std::min((_height-yloc),height);
		width = std::min((_width-xloc),width);
		for (auto x=0 ; x< width; x++){
			for (auto y=0; y< height; y++){
				at(x+xloc,y+yloc) = source.at(x,y);
			}
		}
	}
	
	//=============================================================
	std::uint32_t Bitmap::padSize(std::uint32_t width, std::uint32_t bytes_per_pixel) const {
		return ((4-((width * bytes_per_pixel)%4))!=4)? (4-((width * bytes_per_pixel)%4))  : 0 ;
	}
	//=============================================================
	std::uint8_t Bitmap::index(const Color & color, const std::vector<Color> &lookup) const{
		auto iter = std::find(lookup.cbegin(),lookup.cend(),color);
		return static_cast<std::uint8_t>(std::distance(lookup.cbegin(),iter));
	}
	
	
	//=============================================================
	void Bitmap::loadBMP(const std::string &filepath){
		std::ifstream input(filepath,std::ios::binary);
		if (!input.is_open()){
			throw OpenFileFailure(filepath);
		}
		bmp_sig signature ;
		signature.load(input);
		if (!signature.valid()){
			throw InvalidFile(filepath,signature.indicator,0);
		}
		bmp_header header ;
		header.load(input) ;
		if (header.bits_per_pixel < 8){
			throw InvalidBitSize(header.bits_per_pixel);
		}
		if (header.compression != 0){
			throw InvalidCompression(header.compression);
		}
		_palette.clear() ;
		for (auto i = 0; i<header.colors_used;i++){
			std::uint32_t value = 0 ;
			input.read(reinterpret_cast<char*>(&value),sizeof(value));
			_palette.push_back(Color(value,ColorType::bgr));
			i++;
		}
		input.seekg(signature.offset);
		auto pad = padSize(header.width, header.bits_per_pixel/8);
		size(header.width,header.height);
		for (auto y=0 ; y<header.height;y++){
			for (auto x=0;x<header.width;x++){
				std::uint16_t value16 = 0 ;
				std::uint8_t value8 = 0 ;
				std::uint32_t value32 = 0 ;
				switch (header.bits_per_pixel){
					case 8:
						input.read(reinterpret_cast<char*>(&value8),1);
						at(x,header.height-(y+1)) = _palette[value8];
						break;
					case 16:
						input.read(reinterpret_cast<char*>(&value16),2);
						at(x,header.height-(y+1)) = Color(value16);
						break;
					case 24:
						input.read(reinterpret_cast<char*>(&value32),3);
						at(x,header.height-(y+1)) = Color(value32,ColorType::bgr);
						break;
					default:
						break;
				}
			}
			std::uint8_t value8 = 0 ;
			for (auto i=0 ; i < pad;i++){
				input.read(reinterpret_cast<char*>(&value8),1);
			}
		}
	}
	//=============================================================
	void Bitmap::loadRAW(const std::string &filepath){
		std::ifstream input(filepath,std::ios::binary);
		if (!input.is_open()){
			throw OpenFileFailure(filepath);
		}
		std::uint32_t sig ;
		std::uint32_t version;
		input.read(reinterpret_cast<char*>(&sig), sizeof(sig));
		input.read(reinterpret_cast<char*>(&version), sizeof(version));
		if ((sig != _raw_signature) || (version != _raw_version)) {
			throw InvalidFile(filepath,sig,version) ;
		}
		std::uint64_t width = 0 ;
		input.read(reinterpret_cast<char*>(&width), sizeof(width));
		std::uint64_t height = 0 ;;
		input.read(reinterpret_cast<char*>(&height), sizeof(height));
		size(width,height);
		std::uint32_t color ;
		std::uint64_t y = 0 ;
		for (auto &entry:_lines){
			std::uint64_t x = 0 ;
			for (auto &pixel:entry.colors()){
				input.read(reinterpret_cast<char*>(&color), sizeof(color));
				if (input.gcount()!= sizeof(color)){
					throw InputFailure(x, y);
				}
				pixel = Color(color,ColorType::argb);
				x++;
			}
			y++;
		}
		
	}
	//=============================================================
	void Bitmap::writeBMP(const std::string &filepath,std::uint32_t bits,const std::vector<Color> &lookup){
		std::ofstream output(filepath,std::ios::binary) ;
		if (!output.is_open()){
			throw OpenFileFailure(filepath);
		}
		bmp_sig signature ;
		bmp_header header ;
		header.bits_per_pixel = bits ;
		header.compression = 0 ;
		header.colors_used = 0 ;
		header.width = static_cast<std::uint32_t>(_width) ;
		header.height = static_cast<std::uint32_t>(_height) ;
		signature.offset = 54;
		auto pad = padSize(static_cast<uint32_t>(_width), bits/8);
		const std::vector<Color> *ptrLookup = nullptr ;
		if (bits == 8) {
			ptrLookup = &lookup ;
			if (lookup.empty()){
				ptrLookup = &_palette ;
				if (_palette.empty()){
					ptrLookup = nullptr ;
				}
			}
			header.colors_used = static_cast<std::uint32_t>(ptrLookup->size()) ;
			signature.offset += (header.colors_used * 4) ;
		}
		signature.save(output);
		header.save(output) ;
		if ((bits == 8) && (ptrLookup!=nullptr)) {
			for (const auto &entry: *ptrLookup){
				auto color = entry.color(ColorType::abgr);
				output.write(reinterpret_cast<char*>(&color),4);
			}
		}
		// write out the rows backwards
		for(auto iter = _lines.crbegin(); iter != _lines.crend();iter++){
			for (const auto &pixel: (*iter).colors() ){
				switch (header.bits_per_pixel){
					case 8:
					{
						auto color = index(pixel,*ptrLookup);
						output.write(reinterpret_cast<char*>(&color),1);
					}
						break;
						
					case 16:
					{
						auto value = pixel.color() ;
						output.write(reinterpret_cast<char*>(&value),2);
					}
						break;
						
					case 24:
					{
						auto value = pixel.color(ColorType::bgr) ;
						output.write(reinterpret_cast<char*>(&value),3);
					}
						break;
				}
			}
			std::uint8_t value = 0 ;
			for (auto i=0;i<pad;i++){
				output.write(reinterpret_cast<char*>(&value),1);
			}
		}
		std::uint32_t position = static_cast<std::uint32_t>(output.tellp()) ;
		output.seekp(2,std::ios::beg) ;
		output.write(reinterpret_cast<char*>(&position),4);

		
	}
	//=============================================================
	void Bitmap::writeRAW(const std::string &filepath){
		std::ofstream output(filepath,std::ios::binary);
		if (!output.is_open()){
			throw OpenFileFailure(filepath);
		}
		output.write(reinterpret_cast<const char*>(&_raw_signature), sizeof(_raw_signature));
		output.write(reinterpret_cast<const char*>(&_raw_version), sizeof(_raw_version));

		std::uint64_t value64 = _width ;
		output.write(reinterpret_cast<char*>(&value64), sizeof(value64));
		value64 = _height ;
		output.write(reinterpret_cast<char*>(&value64), sizeof(value64));
		for (const auto &entry :_lines) {
			for (const auto &color : entry.colors()){
				auto value = color.color(ColorType::argb);
				output.write(reinterpret_cast<char*>(&value),sizeof(value));
			}
		}
	}
	
	//=============================================================
	void Bitmap::open(const std::string &filepath){
		auto type = typeOf(filepath);
		open(filepath,type);
	}
	//=============================================================
	void Bitmap::open(const std::string &filepath, Bitmap::FileType type){
		switch (type) {
			case Bitmap::FileType::raw:
				loadRAW(filepath);
				break;
			case Bitmap::FileType::bmp:
				loadBMP(filepath);
				break;
			default:
				throw InvalidFile(filepath, 0, 0);
		}
	}
	//=============================================================
	void Bitmap::save(const std::string &filepath, Bitmap::FileType type,std::uint32_t bitsize, const std::vector<Color> &lookup ) {
		switch (type) {
			case Bitmap::FileType::raw:
				writeRAW(filepath);
				break;
			case Bitmap::FileType::bmp:
				writeBMP(filepath,bitsize,lookup);
				break;
			default:
				throw InvalidFile(filepath, 0, 0);
		}

	}

	/******************************************************************************
	 Bitmap::bmp_sig
	 ******************************************************************************/
	
	//=============================================================
	Bitmap::bmp_sig::bmp_sig() {
		reset();
	}
	//=============================================================
	void Bitmap::bmp_sig::reset(){
		indicator = _valid_indicator;
		size =0;
		unused = 0;
		offset =0;
		
	}
	//=============================================================
	void Bitmap::bmp_sig::load(std::istream &input) {
		input.read(reinterpret_cast<char*>(&indicator),sizeof(indicator));
		input.read(reinterpret_cast<char*>(&size),sizeof(size));
		input.read(reinterpret_cast<char*>(&unused),sizeof(unused));
		input.read(reinterpret_cast<char*>(&offset),sizeof(offset));
	}
	//=============================================================
	void Bitmap::bmp_sig::save(std::ostream &output){
		output.write(reinterpret_cast<char*>(&indicator),sizeof(indicator));
		output.write(reinterpret_cast<char*>(&size),sizeof(size));
		output.write(reinterpret_cast<char*>(&unused),sizeof(unused));
		output.write(reinterpret_cast<char*>(&offset),sizeof(offset));
	}
	
	//=============================================================
	bool Bitmap::bmp_sig::valid() const{
		return indicator==_valid_indicator;
	}
	/******************************************************************************
	 Bitmap::bmp_header
	 ******************************************************************************/
	
	//=============================================================
	Bitmap::bmp_header::bmp_header(){
		reset();
	}
	//=============================================================
	void Bitmap::bmp_header::reset(){
		size =40;
		width=0 ;
		height =0;
		planes =1;
		bits_per_pixel=16 ;
		compression =0;
		compressed_size=0 ;
		horizon_resolution=0 ;
		vertical_resolution =0;
		colors_used =0;
		important_colors=0 ;
	}
	//=============================================================
	void Bitmap::bmp_header::load(std::istream &input){
		input.read(reinterpret_cast<char*>(&size),sizeof(size));
		input.read(reinterpret_cast<char*>(&width),sizeof(width));
		input.read(reinterpret_cast<char*>(&height),sizeof(height));
		input.read(reinterpret_cast<char*>(&planes),sizeof(planes));
		input.read(reinterpret_cast<char*>(&bits_per_pixel),sizeof(bits_per_pixel));
		if (bits_per_pixel<8){
			throw InvalidBitSize(bits_per_pixel);
		}
		input.read(reinterpret_cast<char*>(&compression),sizeof(compression));
		
		if(compression != 0){
			throw InvalidCompression(compression);
		}
		input.read(reinterpret_cast<char*>(&compressed_size),sizeof(compressed_size));
		input.read(reinterpret_cast<char*>(&horizon_resolution),sizeof(horizon_resolution));
		input.read(reinterpret_cast<char*>(&vertical_resolution),sizeof(vertical_resolution));
		input.read(reinterpret_cast<char*>(&colors_used),sizeof(colors_used));
		input.read(reinterpret_cast<char*>(&important_colors),sizeof(important_colors));
	}
	//=============================================================
	void Bitmap::bmp_header::save(std::ostream &output){
		output.write(reinterpret_cast<char*>(&size),sizeof(size));
		output.write(reinterpret_cast<char*>(&width),sizeof(width));
		output.write(reinterpret_cast<char*>(&height),sizeof(height));
		output.write(reinterpret_cast<char*>(&planes),sizeof(planes));
		output.write(reinterpret_cast<char*>(&bits_per_pixel),sizeof(bits_per_pixel));
		output.write(reinterpret_cast<char*>(&compression),sizeof(compression));
		output.write(reinterpret_cast<char*>(&compressed_size),sizeof(compressed_size));
		output.write(reinterpret_cast<char*>(&horizon_resolution),sizeof(horizon_resolution));
		output.write(reinterpret_cast<char*>(&vertical_resolution),sizeof(vertical_resolution));
		output.write(reinterpret_cast<char*>(&colors_used),sizeof(colors_used));
		output.write(reinterpret_cast<char*>(&important_colors),sizeof(important_colors));
	}
	
	//=============================================================
	bool Bitmap::bmp_header::hasColorTable() const{
		return ((bits_per_pixel<16))?true:false;
	}
	//=============================================================
	std::uint32_t Bitmap::bmp_header::pixelSize() const{
		return bits_per_pixel/8 ;
	}
	
	/*************************************************************************
	 BMPFailures
	 *************************************************************************/
	//=============================================================
	InvalidCompression::InvalidCompression(std::uint32_t compress): BitmapAlert("Invalid compression level: "s+std::to_string(compress)+". Only compression level 0 supported."s){
		compression = compress ;
	}
	//=============================================================
	OpenFileFailure::OpenFileFailure(const std::string &file): BitmapAlert("Unable to open file: "s+file){
		filepath = file ;
	}
	//=============================================================
	InvalidBitSize::InvalidBitSize(std::uint32_t bits): BitmapAlert("Invalid Bit size: "s+std::to_string(bits)+". Only 8,16, and 24 supported."s){
		this->bits = bits ;
	}
	//=============================================================
	BitmapAlert::BitmapAlert(const std::string &msg) : std::runtime_error(msg){
		
	}
	//=============================================================
	InvalidFile::InvalidFile(const std::string &filepath,std::uint32_t signature, std::uint32_t version): BitmapAlert("Invalid bitmap file signuare/version. Signaure: "s+strutil::numtostr(signature,16,true,4)+" , Version: "s+strutil::numtostr(version,16,true,4)+" ."s) {
		this->signature = signature ;
		this->version = version ;
		this->filepath  = filepath ;
	}
	//=============================================================
	InputFailure::InputFailure(std::uint64_t x, std::uint64_t y) :BitmapAlert("Stream read error for pixel "s+std::to_string(x)+" , "s+ std::to_string(y)){
		this->xloc = x;
		this->yloc = y;
	}
}
