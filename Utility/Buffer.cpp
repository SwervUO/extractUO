//Copyright © 2021 Charles Kerr. All rights reserved.

#include "Buffer.hpp"
#include <algorithm>
#include <cstring>
#include "StringUtility.hpp"

/*******************************************************************************
 Constructors
 *******************************************************************************/
//===============================================================
Buffer::Buffer(std::size_t size) {
	_data = std::vector<std::uint8_t>(size,0);
	_index = 0 ;
}
//===============================================================
Buffer::Buffer(const std::vector<std::uint8_t> &data): Buffer(data.size()){
	std::copy(data.cbegin(),data.cend(),_data.begin());
}
//===============================================================
Buffer::Buffer(const std::uint8_t *ptr,std::size_t size ):Buffer(size){
	std::copy(ptr,ptr+size,_data.begin());
}

/*******************************************************************************
 Underlying access
 *******************************************************************************/
//===============================================================
const std::vector<unsigned char>& Buffer::raw() const {
	return _data;
}
//===============================================================
std::vector<unsigned char> & Buffer::raw() {
	return _data;
}


/*******************************************************************************
Sizing
 *******************************************************************************/
//===============================================================
std::size_t Buffer::size() const {
	return _data.size();
}
//===============================================================
void Buffer::size(std::size_t size) {
	_data.resize(size,0);
	if (_index > _data.size()){
		_index = _data.size();
	}
}
//===============================================================
bool Buffer::sizeCheck(std::size_t size,bool expand){
	if (size > _data.size()){
		if (!expand){
			return false ;
		}
		this->size(size);
	}
	return true ;
}
/*******************************************************************************
 Positioning
 *******************************************************************************/
//===============================================================
std::size_t Buffer::position() const {
	return _index ;
}
//===============================================================
Buffer& Buffer::position(std::size_t position) {
	_index = position ;
	return *this;
}
//===============================================================
Buffer& Buffer::relative(std::int64_t relative){
	if (std::abs(relative)> _data.size()){
		throw std::out_of_range("Buffer Positioned beyond beginning. Current :"s+std::to_string(_index) + ". Relative: "s+std::to_string(relative)+"."s);
	}
	_index = _index + relative ;
	return *this;
}
//===============================================================
Buffer& operator>>(Buffer &buffer, Buffer::Position &value){
	buffer.position(value.position) ;
	return buffer ;
}
//===============================================================
Buffer& operator<<(Buffer &buffer, const Buffer::Position &value){
	buffer.position(value.position) ;
	return buffer ;

}
//===============================================================
Buffer& operator>>(Buffer &buffer, Buffer::Relative &value){
	buffer.relative(value.relative) ;
	return buffer ;

}
//===============================================================
Buffer& operator<<(Buffer &buffer, const Buffer::Relative &value){
	buffer.relative(value.relative) ;
	return buffer ;
}

/******************************************************************************
 Display formatting
 ******************************************************************************/
//=====================================================================
std::int32_t Buffer::displaySize(std::int32_t radix) const {
	switch (radix) {
		case 10:
			return 3 ;
		case 16:
			return 2;
		case 2:
			return 8;
		case 8:
			return 3;
		default:
			return 4 ;
	}
}

//=====================================================================
std::stringstream Buffer::description(std::size_t columns, std::int32_t radix) const {
	
	std::stringstream output ;
	std::stringstream ascii ;
	std::stringstream header ;
	auto count = 0 ;
	auto valuesize = displaySize(radix);
	auto withpad = valuesize+1 ;
	auto indexsize = 10 ; // We allocate 10 columns for display the index into buffer
	auto indexdivider = ": "s;
	
	
	header << std::string(indexsize+indexdivider.size(),' ') ;
	for (auto i = 0 ; i < std::min(columns,_data.size()) ;i++){
		header<<strutil::numtostr(i,10,false,valuesize+1,' ');
	}
	header<<"\n";
	output << header.str() ;
	
	for (auto & entry : _data){
		if ((count % columns)==0) {
			if (count != 0){
				output <<" "<<ascii.str();
				ascii.str(std::string());
				output <<"\n";
			}
			auto label = strutil::numtostr(count) ;
			auto pad = std::string(10-label.size(),' ');
			
			output<<pad<<label<<":  ";
		}
		output << strutil::numtostr(static_cast<unsigned char>(entry),radix,false,valuesize) << " ";
		if (std::isalpha(static_cast<unsigned char>(entry))!=0){
			ascii<<std::string(1,entry);
		}
		else {
			ascii<<".";
		}
		count++ ;
		
	}
	if (count != 0){
		auto tcount = (count%columns);
		auto tempsize =  (columns-tcount) * withpad + 1 ;
		output << std::string(tempsize,' ') <<ascii.str() <<"\n";
		
	}
	return output ;
}


/******************************************************************************
 Writing
 ******************************************************************************/
//=====================================================================
Buffer& operator<<(Buffer &buffer, const std::string &value){
	buffer.write(value) ;
	return buffer ;
}
//=====================================================================
Buffer& operator<<(Buffer &buffer, const char *value){
	buffer.write(value) ;
	return buffer ;
}

//=====================================================================
// Write a string
std::size_t Buffer::write(const std::string &input,std::size_t index ,std::size_t numchar,bool expand ){
	if (index == infinity){
		index = _index ;
	}
	std::string::size_type amount = static_cast<std::string::size_type>(numchar) ;
	if (numchar == infinity ){
		amount = input.size()+1 ;
	}
	
	if (_data.size() < index+amount){
		// We need to increase the size of the buffer
		if (expand) {
			this->size(index+amount) ;
		}
		else {
			throw std::out_of_range("Write exceeds buffer size. Index is: "s+std::to_string(index) + ". Size of value to write: "s+std::to_string(amount) + ". Buffer size: "s + std::to_string(_data.size())+"."s);
		}
		
	}
	std::vector<char> temp ;
	temp.resize(amount,0) ;
	auto intermediate = amount ;
	if (amount > (input.size()+1)) {
		intermediate = input.size()+1;
	}
	std::copy(input.c_str(),input.c_str()+intermediate,temp.data());
	std::copy(temp.begin(),temp.end(),_data.begin()+index);
	_index = index+amount ;
	return _index ;
}

//=====================================================================
// Write a boolean
std::size_t Buffer::write(const bool input,std::size_t index ,bool expand){
	char value = 0 ;
	
	if (index == infinity){
		index = _index ;
	}
	if (_data.size() < index+sizeof(value)){
		// We need to increase the size of the buffer
		if (expand) {
			this->size(index+sizeof(value)) ;
		}
		else {
			throw std::out_of_range("Write exceeds buffer size. Index is: "s+std::to_string(index) + ". Size of value to write: "s+std::to_string(sizeof(value)) + ". Buffer size: "s + std::to_string(_data.size())+"."s);
		}
	}
	if (input) {
		value = 1 ;
	}
	std::copy(reinterpret_cast<char*>(&value),reinterpret_cast<char*>(&value)+sizeof(value),_data.begin()+index);
	_index = index + sizeof(value) ;
	return _index ;
}
//=====================================================================
// Write a vector
std::size_t Buffer::write( const std::vector<std::uint8_t> &source,std::size_t index , std::size_t numchar ,bool expand){
	if (index == infinity){
		index = _index ;
	}
	if (numchar == infinity){
		numchar = source.size() ;
	}
	if (source.size() < numchar){
		throw std::out_of_range("Source has insufficent data ("s + std::to_string(source.size())+") for requested size: "s+std::to_string(numchar)+"."s);
	}
	if (_data.size() < (index+numchar)){
		if (expand){
			size(index+numchar);
		}
		else {
			throw std::out_of_range("Write exceeds buffer size. Index is: "s+std::to_string(index) + ". Size of value to write: "s+std::to_string(numchar) + ". Buffer size: "s + std::to_string(_data.size())+"."s);

		}
	}
	std::copy(source.cbegin(),source.cbegin()+numchar,_data.begin()+index);
	_index = index+numchar;
	return _index ;
}
//=====================================================================
// Write a ptr
std::size_t Buffer::write( const std::uint8_t *ptrsource,std::size_t numchar, std::size_t index ,bool expand ){
	if (index == infinity){
		index = _index ;
	}
	if (_data.size() < (index+numchar)){
		if (expand){
			size(index+numchar);
		}
		else {
			throw std::out_of_range("Write exceeds buffer size. Index is: "s+std::to_string(index) + ". Size of value to write: "s+std::to_string(numchar) + ". Buffer size: "s + std::to_string(_data.size())+"."s);
			
		}
	}
	std::copy(ptrsource,ptrsource+numchar,_data.begin()+index);
	_index = index+numchar;
	return _index ;
}

//=====================================================================
// Write a C string
std::size_t Buffer::write(const char* strvalue,std::size_t index , std::size_t numchar,bool expand){
	if (index == infinity){
		index = _index ;
	}
	if (numchar == infinity){
		numchar = std::strlen(strvalue);
	}
	if (std::strlen(strvalue) < numchar) {
		throw std::out_of_range("Source has insufficent data ("s + std::to_string(std::strlen(strvalue))+") for requested size: "s+std::to_string(numchar)+"."s);
	}
	if (_data.size() < (index+numchar)){
		if (expand){
			size(index+numchar);
		}
		else {
			throw std::out_of_range("Write exceeds buffer size. Index is: "s+std::to_string(index) + ". Size of value to write: "s+std::to_string(numchar) + ". Buffer size: "s + std::to_string(_data.size())+"."s);
			
		}
	}
	std::copy(reinterpret_cast<const std::uint8_t*>(strvalue),reinterpret_cast<const std::uint8_t*>(strvalue)+numchar,_data.begin()+index);
	_index = index + numchar ;
	return _index ;
}

/******************************************************************************
 Reading
 ******************************************************************************/
//=====================================================================
Buffer& operator>>(Buffer &buffer, std::string &value){
	value = buffer.read<std::string>() ;
	return buffer ;
}
//=====================================================================
std::size_t Buffer::read(std::vector<std::uint8_t> &dest,std::size_t index , std::size_t numchar ){
	if (index == infinity){
		index = _index ;
	}
	if (numchar == infinity){
		numchar = dest.size();
	}
	if (dest.size() < numchar){
		throw std::out_of_range("Destination has insufficent data ("s + std::to_string(dest.size())+") for requested size: "s+std::to_string(numchar)+"."s);

	}
	if (_data.size() < (index+numchar)){
		throw std::out_of_range("Read exceeds buffer size. Index is: "s+std::to_string(index) + ". Size of value to read: "s+std::to_string(numchar) + ". Buffer size: "s + std::to_string(_data.size())+"."s);
	}
	std::copy(_data.begin()+index,_data.begin()+index+numchar,dest.begin());
	_index = index+numchar ;
	return _index ;
}
//=====================================================================
std::size_t Buffer::read(std::uint8_t *ptr, std::size_t numchar, std::size_t index ){
	if (index == infinity){
		index = _index ;
	}
	if (_data.size() < (index+numchar)){
		throw std::out_of_range("Read exceeds buffer size. Index is: "s+std::to_string(index) + ". Size of value to read: "s+std::to_string(numchar) + ". Buffer size: "s + std::to_string(_data.size())+"."s);
	}
	std::copy(_data.cbegin()+index,_data.cbegin()+index+numchar,ptr);
	_index = index+numchar ;
	return _index ;

}

/******************************************************************************
 Copying (doesn't impact Buffer index)
 ******************************************************************************/
//=====================================================================
void Buffer::copy(std::vector<std::uint8_t> &dest, std::size_t index, std::size_t numchar) const{
	if (index == infinity){
		index = _index ;
	}
	if (numchar == infinity){
		numchar = dest.size();
	}
	if (dest.size() < numchar){
		throw std::out_of_range("Destination has insufficent data ("s + std::to_string(dest.size())+") for requested size: "s+std::to_string(numchar)+"."s);
		
	}
	if (_data.size() < (index+numchar)){
		throw std::out_of_range("Read exceeds buffer size. Index is: "s+std::to_string(index) + ". Size of value to read: "s+std::to_string(numchar) + ". Buffer size: "s + std::to_string(_data.size())+"."s);
	}
	std::copy(_data.cbegin()+index,_data.cbegin()+index+numchar,dest.begin());
}
//=====================================================================
void Buffer::copy(std::uint8_t *ptr, std::size_t numchar, std::size_t index) const{
	if (_data.size() < (index+numchar)){
		throw std::out_of_range("Read exceeds buffer size. Index is: "s+std::to_string(index) + ". Size of value to read: "s+std::to_string(numchar) + ". Buffer size: "s + std::to_string(_data.size())+"."s);
	}
	std::copy(_data.cbegin()+index,_data.cbegin()+index+numchar,ptr);
}



/******************************************************************************
 Access
 ******************************************************************************/
//=====================================================================
const std::uint8_t & Buffer::operator[](std::size_t offset) const {
	return _data[offset];
}
//=====================================================================
std::uint8_t & Buffer::operator[](std::size_t offset) {
	return _data[offset] ;
}

//=====================================================================
std::uint8_t * Buffer::mutableBytes() {
	return _data.data();
}
//=====================================================================
const std::uint8_t * Buffer::bytes() const {
	return const_cast<std::uint8_t *>(_data.data());
}
