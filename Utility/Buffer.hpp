//Copyright © 2021 Charles Kerr. All rights reserved.

#ifndef Buffer_hpp
#define Buffer_hpp

#include <string>
#include <vector>
#include <utility>
#include <cstdint>
#include <limits>
#include <sstream>
#include <stdexcept>
using namespace std::string_literals ;

/******************************************************************************
 Buffer
 	A buffer for binary data.  Allows access (read/write) by varying data
 types.
 ******************************************************************************/
//===============================================================
class Buffer {
public:
	/*************************************************************************
	 Constants used by the class
	 *************************************************************************/
	static constexpr std::size_t infinity = std::numeric_limits<std::size_t>::max();

private:
	/*************************************************************************
	 Data used by the class
	 *************************************************************************/
	std::vector<std::uint8_t> _data ;
	std::size_t _index ;
	
public:
	/************************************************************************
	 Position/Relative classs to allow indexing via streams
	 ***********************************************************************/
	struct Position {
		std::size_t position ;
		Position(std::size_t position) {
			this->position = position;
		}
	};
	struct Relative {
		std::int64_t relative ;
		Relative(std::int16_t relative) {
			this->relative = relative ;
		}
	};
	
private:
	/************************************************************************
	 protected Methods
	 ***********************************************************************/
	bool sizeCheck(std::size_t size,bool expand=true);
	std::int32_t displaySize(std::int32_t radix) const;
public:
	/************************************************************************
	 public Methods
	 ***********************************************************************/
	Buffer(std::size_t size = 0 );
	Buffer(const std::vector<std::uint8_t> &data);
	Buffer(const std::uint8_t *ptr,std::size_t size );
	
	const std::vector<std::uint8_t>& raw() const ;
	std::vector<std::uint8_t> & raw() ;
	
	std::size_t size() const ;
	void size(std::size_t size) ;
	bool empty() const {return _data.empty();}

	std::size_t position() const ;
	Buffer& position(std::size_t position) ;
	Buffer& relative(std::int64_t relative) ;
	
	std::stringstream description(std::size_t columns=8, std::int32_t radix=16) const ;
	
	std::size_t write(const std::string &input, std::size_t index=infinity,std::size_t numchar = infinity,bool expand = true);
	std::size_t write(const bool input,std::size_t index = infinity,bool expand = true);
	std::size_t write( const std::vector<std::uint8_t> &source,std::size_t index = infinity, std::size_t numchar = infinity,bool expand = true);
	std::size_t write( const std::uint8_t *ptrsource,std::size_t numchar, std::size_t index = infinity,bool expand = true);
	std::size_t write(const char* strvalue,std::size_t index = infinity, std::size_t numchar = infinity,bool expand = true);
	
	
	std::size_t read(std::vector<std::uint8_t> &dest,std::size_t index = infinity, std::size_t numchar = infinity);
	void copy(std::vector<std::uint8_t> &dest, std::size_t index, std::size_t numchar = infinity) const;
	
	std::size_t read(std::uint8_t *ptr, std::size_t numchar, std::size_t index = infinity);
	void copy(std::uint8_t *ptr, std::size_t numchar, std::size_t index) const ;
	
	const std::uint8_t & operator[](std::size_t offset) const ;
	std::uint8_t & operator[](std::size_t offset)  ;

	std::uint8_t * mutableBytes() ;
	const std::uint8_t * bytes() const ;

	/************************************************************************
	 Template methods
	 ************************************************************************/
	/************************************************************************
	 Write methods
	 ************************************************************************/
	//=====================================================================
	// Writing an integer type
	template <typename T>
	typename std::enable_if< std::is_integral_v<T>,std::size_t>::type
	write( T input,std::size_t index = infinity,bool expand = true){
		if (index == infinity){
			index = _index ;
		}
		if (_data.size() < index+sizeof(T)){
			// We need to increase the size of the buffer
			if (expand) {
				size(index+sizeof(T));
			}
			else {
				throw std::out_of_range("Write exceeds buffer size. Index is: "s+std::to_string(index) + " Size of value to write: "s+std::to_string(sizeof(T)) + " Buffer size: "s + std::to_string(_data.size()));
				
			}
		}
		std::copy(reinterpret_cast<unsigned char*>(&input),reinterpret_cast<unsigned char*>(&input)+sizeof(T),_data.begin()+index);
		_index = index + sizeof(T) ;
		return _index ;
	}

	/************************************************************************
	 Read methods
	 ************************************************************************/
	//=====================================================================
	// Reading an integer type
	template <typename T>
	typename std::enable_if<std::is_integral_v<T> && !std::is_same_v<T, bool>,T>::type
	read(std::size_t index = infinity){
		T rvalue ;
		if (index == infinity){
			index = _index ;
		}
		auto amount = sizeof(T) ;
		if (_data.size() < index+amount){
			throw std::out_of_range("Read exceeds buffer size. Index is: "s+std::to_string(index) + ". Size of value to read: "s+std::to_string(sizeof(T)) + ". Buffer size: "s + std::to_string(_data.size())+"."s);
		}
		std::copy(_data.data()+index,_data.data()+index+amount,reinterpret_cast<unsigned char*>(&rvalue));
		_index = index+amount ;
		return rvalue ;
	}
	//=====================================================================
	// Reading an bool type (one byte in the Buffer)
	template <typename T>
	typename std::enable_if<std::is_integral_v<T> && std::is_same_v<T, bool>,T>::type
	read(std::size_t index = infinity){
		if (index == infinity){
			index = _index ;
		}
		auto value = static_cast<unsigned char>(0) ;
		auto amount = sizeof(value) ;
		if (_data.size() < index+amount){
			throw std::out_of_range("Read exceeds buffer size. Index is: "s+std::to_string(index) + ". Size of value to read: "s+std::to_string(amount) + ". Buffer size: "s + std::to_string(_data.size())+"."s);
		}
		std::copy(_data.data()+index,_data.data()+index+amount,reinterpret_cast<unsigned char*>(&value));
		_index = index+amount ;
		return static_cast<T>(value) ;
	}
	//=====================================================================
	// Reading an String type
	template <typename T>
	typename std::enable_if<std::is_same_v<T, std::string>,std::string>::type
	read(std::size_t index=infinity,std::size_t numchar=infinity){
		if (index==infinity){
			index = _index ;
			if (index >= _data.size()){
				throw std::out_of_range("Read exceeds buffer size. Index is: "s+std::to_string(index) + ". Size of value to read: [end of buffer or \\0]."s + " Buffer size: "s + std::to_string(_data.size())+"."s);
			}
		}
		else {
			if ((_index+numchar) >= _data.size()){
				throw std::out_of_range("Read exceeds buffer size. Index is: "s+std::to_string(index) + ". Size of value to read: "s + std::to_string(numchar) + ". Buffer size: "s + std::to_string(_data.size())+"."s);

			}
		}
		std::vector<unsigned char> buffer ;
		auto amount = static_cast<std::string::size_type>(numchar) ;
		if (numchar==infinity){
			// we need to find the first null terminator
			
			auto iter = std::find(_data.begin()+index,_data.end(),static_cast<unsigned char>(0));
			amount = std::distance(_data.begin()+index, iter);
		}
		if (_data.size() < index+amount){
			amount = _data.size()-index ;
			if (amount <= 0){
				return std::string();
			}
		}
		buffer.resize(amount,0);
		std::copy(_data.begin()+index,_data.begin()+index+amount,buffer.begin());
		auto nullat = std::find(buffer.begin(),buffer.end(),0);
		if (nullat != buffer.end()){
			auto tamount = std::distance(buffer.begin(),nullat);
			buffer.resize(tamount) ;
		}
		
		_index = index+amount ;
		return (std::string(reinterpret_cast<char*>(buffer.data()),buffer.size()));
	}

	//=====================================================================
	// Copying an integer type (good on const Buffers, as they dont change the index)
	template <typename T>
	typename std::enable_if<std::is_integral_v<T> && !std::is_same_v<T, bool>,T>::type
	copy(std::size_t index ) const{
		T rvalue ;
		auto amount = sizeof(T) ;
		if (_data.size() < index+amount){
			throw std::out_of_range("Read exceeds buffer size. Index is: "s+std::to_string(index) + ". Size of value to read: "s+std::to_string(sizeof(T)) + ". Buffer size: "s + std::to_string(_data.size())+"."s);
		}
		std::copy(_data.data()+index,_data.data()+index+amount,reinterpret_cast<unsigned char*>(&rvalue));
		return rvalue ;
	}

	//=====================================================================
	// Copying an bool type (one byte in the Buffer) (good on const Buffers, as they dont change the index)
	template <typename T>
	typename std::enable_if<std::is_integral_v<T> && std::is_same_v<T, bool>,T>::type
	copy(std::size_t index ) const{
		if (index == infinity){
			index = _index ;
		}
		auto value = static_cast<unsigned char>(0) ;
		auto amount = sizeof(value) ;
		if (_data.size() < index+amount){
			throw std::out_of_range("Read exceeds buffer size. Index is: "s+std::to_string(index) + ". Size of value to read: "s+std::to_string(amount) + ". Buffer size: "s + std::to_string(_data.size())+"."s);
		}
		std::copy(_data.data()+index,_data.data()+index+amount,reinterpret_cast<unsigned char*>(&value));
		return static_cast<T>(value) ;
	}
	//=====================================================================
	// Copy an String type (good on const Buffers, as they dont change the index)
	template <typename T>
	typename std::enable_if<std::is_same_v<T, std::string>,std::string>::type
	copy(std::size_t index,std::size_t numchar=infinity) const{
		if (index==infinity){
			index = _index ;
			if (index >= _data.size()){
				throw std::out_of_range("Read exceeds buffer size. Index is: "s+std::to_string(index) + ". Size of value to read: [end of buffer or \\0]."s + " Buffer size: "s + std::to_string(_data.size())+"."s);
			}
		}
		else {
			if ((_index+numchar) >= _data.size()){
				throw std::out_of_range("Read exceeds buffer size. Index is: "s+std::to_string(index) + ". Size of value to read: "s + std::to_string(numchar) + ". Buffer size: "s + std::to_string(_data.size())+"."s);
				
			}
		}
		std::vector<unsigned char> buffer ;
		auto amount = static_cast<std::string::size_type>(numchar) ;
		if (numchar==infinity){
			// we need to find the first null terminator
			
			auto iter = std::find(_data.begin()+index,_data.end(),static_cast<unsigned char>(0));
			amount = std::distance(_data.begin()+index, iter);
		}
		if (_data.size() < index+amount){
			amount = _data.size()-index ;
			if (amount <= 0){
				return std::string();
			}
		}
		buffer.resize(amount,0);
		std::copy(_data.begin()+index,_data.begin()+index+amount,buffer.begin());
		auto nullat = std::find(buffer.begin(),buffer.end(),0);
		if (nullat != buffer.end()){
			auto tamount = std::distance(buffer.begin(),nullat);
			buffer.resize(tamount) ;
		}
		return (std::string(reinterpret_cast<char*>(buffer.data()),buffer.size()));
	}

	
};
/******************************************************************************
 Positioning for streaming
 ******************************************************************************/
Buffer& operator>>(Buffer &buffer, Buffer::Position &value);
Buffer& operator<<(Buffer &buffer, const Buffer::Position &value);
Buffer& operator>>(Buffer &buffer, Buffer::Relative &value);
Buffer& operator<<(Buffer &buffer, const Buffer::Relative &value);

/******************************************************************************
 Reading for streaming
 ******************************************************************************/
Buffer& operator>>(Buffer &buffer, std::string &value);

template <typename T>
typename std::enable_if<std::is_integral_v<T> && !std::is_same_v<T, bool>,Buffer&>::type
operator>>(Buffer &buffer, T &value){
	value = buffer.read<T>() ;
	return buffer ;
}
/******************************************************************************
 Writing for streaming
 ******************************************************************************/
Buffer& operator<<(Buffer &buffer, const std::string &value);
Buffer& operator<<(Buffer &buffer, const char* value);

template <typename T>
typename std::enable_if<std::is_integral_v<T> && !std::is_same_v<T, bool>,Buffer&>::type
operator<<(Buffer &buffer, const T &value){
	buffer.write<T>(value) ;
	return buffer ;
}

#endif /* Buffer_hpp */
