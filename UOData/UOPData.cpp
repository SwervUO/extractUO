//Copyright © 2021 Charles Kerr. All rights reserved.

#include "UOPData.hpp"
#include "StringUtility.hpp"
#include "UOAlerts.hpp"
#include <algorithm>
#include <fstream>
#include <limits>
namespace UO {
	
	/*************************************************************************
	 table_entry methods
	 ************************************************************************/
	//===============================================================
	UOPData::table_entry::table_entry(){
		offset = 0 ;
		header_length = _entry_size ;
		compressed_length = 0 ;
		decompressed_length = 0 ;
		identifer = 0;
		data_block_hash = 0 ;
		compression = 0 ;
		
	}
	//===============================================================
	UOPData::table_entry & 	UOPData::table_entry::load(std::istream &input) {
		input.read(reinterpret_cast<char*>(&offset),sizeof(offset));
		input.read(reinterpret_cast<char*>(&header_length),sizeof(header_length));
		input.read(reinterpret_cast<char*>(&compressed_length),sizeof(compressed_length));
		input.read(reinterpret_cast<char*>(&decompressed_length),sizeof(decompressed_length));
		input.read(reinterpret_cast<char*>(&identifer),sizeof(identifer));
		input.read(reinterpret_cast<char*>(&data_block_hash),sizeof(data_block_hash));
		input.read(reinterpret_cast<char*>(&compression),sizeof(compression));
		return *this ;
	}
	//===============================================================
	UOPData::table_entry &	UOPData::table_entry::save(std::ostream &output) {
		output.write(reinterpret_cast<char*>(&offset),sizeof(offset));
		output.write(reinterpret_cast<char*>(&header_length),sizeof(header_length));
		output.write(reinterpret_cast<char*>(&compressed_length),sizeof(compressed_length));
		output.write(reinterpret_cast<char*>(&decompressed_length),sizeof(decompressed_length));
		output.write(reinterpret_cast<char*>(&identifer),sizeof(identifer));
		output.write(reinterpret_cast<char*>(&data_block_hash),sizeof(data_block_hash));
		output.write(reinterpret_cast<char*>(&compression),sizeof(compression));
		return *this ;
	}

	/*************************************************************************
	 					UOPData methods
	 ************************************************************************/
	/************************************************************************
	 zlib wrappers for compression
	 ***********************************************************************/
	//=============================================================================
	std::vector<uint8_t> UOPData::decompress(const std::vector<uint8_t> &source, std::size_t decompressed_size) const{
		// uLongf is from zlib.h
		auto srcsize = static_cast<uLongf>(source.size()) ;
		auto destsize = static_cast<uLongf>(decompressed_size);
		std::vector<uint8_t> dest(decompressed_size,0);
		auto status = uncompress2(dest.data(), &destsize, source.data(), &srcsize);
		if (status != Z_OK){
			dest.clear() ;
			dest.resize(0) ;
			return dest ;
		}
		dest.resize(destsize);
		return dest ;
	}
	//=============================================================================
	std::vector<uint8_t> UOPData::compress(const std::vector<uint8_t> &source) const {
		auto size = compressBound(source.size());
		std::vector<uint8_t> rdata(size,0);
		auto status = compress2(reinterpret_cast<Bytef*>(rdata.data()), &size, reinterpret_cast<const Bytef*>(source.data()), static_cast<uLongf>(source.size()),Z_DEFAULT_COMPRESSION);
		if (status != Z_OK){
			rdata.clear();
			return rdata ;
		}
		rdata.resize(size) ;
		return rdata;
	}

	
	/************************************************************************
	 Hash routines
	 ***********************************************************************/
	
	//=============================================================================
	std::uint64_t UOPData::hashLittle2(const std::string& s)  {
		
		std::uint32_t length = static_cast<std::uint32_t>(s.size()) ;
		std::uint32_t a ;
		std::uint32_t b ;
		std::uint32_t c ;
		
		c = 0xDEADBEEF + static_cast<std::uint32_t>(length) ;
		a = c;
		b = c ;
		int k = 0 ;
		
		while (length > 12){
			a += (s[k]);
			a += (s[k+1] << 8);
			a += (s[k+2] << 16);
			a += (s[k+3] << 24);
			b += (s[k+4]);
			b += (s[k+5] << 8);
			b += (s[k+6] << 16);
			b += (s[k+7] << 24);
			c += (s[k+8]);
			c += (s[k+9] << 8);
			c += (s[k+10] << 16);
			c += (s[k+11] << 24);
			
			a -= c; a ^= c << 4 | c >> 28; c += b;
			b -= a; b ^= a << 6 | a >> 26; a += c;
			c -= b; c ^= b << 8 | b >> 24; b += a;
			a -= c; a ^= c << 16 | c >> 16; c += b;
			b -= a; b ^= a << 19 | a >> 13; a += c;
			c -= b; c ^= b << 4 | b >> 28; b += a;
			
			length -= 12 ;
			k += 12;
		}
		
		if (length != 0){
			// Notice the lack of breaks!  we actually want it to fall through
			switch (length) {
				case 12:
					c += (s[k+11] << 24);
				case 11:
					c += (s[k+10] << 16);
				case 10:
					c += (s[k+9] << 8);
				case 9:
					c += (s[k+8]);
				case 8:
					b += (s[k+7] << 24);
				case 7:
					b += (s[k+6] << 16);
				case 6:
					b += (s[k+5] << 8);
				case 5:
					b += (s[k+4]);
				case 4:
					a += (s[k+3] << 24);
				case 3:
					a += (s[k+2] << 16);
				case 2:
					a += (s[k+1] << 8);
				case 1: {
					a += (s[k]);
					c ^= b; c -= b << 14 | b >> 18;
					a ^= c; a -= c << 11 | c >> 21;
					b ^= a; b -= a << 25 | a >> 7;
					c ^= b; c -= b << 16 | b >> 16;
					a ^= c; a -= c << 4 | c >> 28;
					b ^= a; b -= a << 14 | a >> 18;
					c ^= b; c -= b << 24 | b >> 8;
					break;
				}
					
				default:
					break;
			}

		}
		
		return (static_cast<std::uint64_t>(b) << 32) | static_cast<std::uint64_t>(c) ;
	}
	
	//=============================================================================
	std::uint32_t UOPData::hashAdler32(const std::vector<std::uint8_t> &data)  {
		std::uint32_t a = 1 ;
		std::uint32_t b = 0 ;
		for (const auto &entry : data ) {
			a = (a + static_cast<std::uint32_t>(entry)) % 65521;
			b = (b + a) % 65521 ;
		}
		return (b<<16)| a ;
	}

	//=============================================================================
	std::string UOPData::format(const std::string& hashformat, std::size_t index)const {
		// How much do we pad?  Find the subsutition character
		auto pos = hashformat.find_first_of("{") ;
		if (pos == std::string::npos){
			// we are not subsituting anything, pass on the string
			return hashformat ;
		}
		
		auto loc = hashformat.find_first_of("}",pos+1) ;
		if (loc == std::string::npos){
			// we are not subsituting anything, pass on the string
			return hashformat ;
		}
		auto sub = strutil::numtostr(index,10,false,strutil::strtoi(hashformat.substr(pos+1,loc-(pos+1))));
		auto rvalue = hashformat;
		return rvalue.replace(pos, (loc-pos)+1, sub);
	}
	
	//===============================================================
	std::size_t UOPData::findIndex(const std::vector<std::uint64_t> &hashdata, std::uint64_t hash){
		auto iter = std::find(hashdata.cbegin(),hashdata.cend(),hash);
		if (iter != hashdata.cend()){
			return std::distance(hashdata.cbegin(), iter);

		}
		return std::numeric_limits<std::size_t>::max();
	}
	//===============================================================
	std::uint64_t UOPData::hashLittleFor(const std::string &hashstring, std::size_t index) const{
		auto formatted = format(hashstring,index);
		return hashLittle2(formatted);
	}
	
	
	//===============================================================
	bool UOPData::nonIndexHash(std::uint64_t hash, std::size_t entry, const std::vector<std::uint8_t> &data){
		std::cerr << "Hashlookup failed for entry "s << entry << " with a hash of " << strutil::numtostr(hash,16,true,16)<<std::endl;
		return true;
	}

	
	
	//===============================================================
	std::vector<std::uint64_t> UOPData::buildIndexHashes(const std::string &hashformat, std::size_t max_index){
		std::vector<std::uint64_t> hashstorage ;
		if (hashformat.empty()){
			return std::vector<std::uint64_t>();
		}
		hashstorage.reserve(max_index+1);
		for (auto i=0 ; i<= max_index;i++){
			hashstorage.push_back(hashLittleFor(hashformat, i));
		}
		return hashstorage;
	}

	
	/************************************************************************
	 Public  routines
	 ***********************************************************************/
	//===============================================================
	void UOPData::loadUOP(const std::string &filepath, std::size_t max_hashindex , const std::string &hashformat1, const std::string &hashformat2 ){
		std::ifstream input(filepath, std::ios::binary);
		if (!input.is_open()){
			throw FileOpen(filepath);
		}
		// Make sure this is a format and version we understand
		std::uint32_t sig  = 0 ;
		std::uint32_t version = 0 ;
		input.read(reinterpret_cast<char*>(&sig),sizeof(sig));
		input.read(reinterpret_cast<char*>(&version),sizeof(version));
		input.seekg(4,std::ios::cur);
		if ((version > _uop_version) || (sig != _uop_identifer)){
			throw InvalidUOP(sig, version,filepath);
		}
		auto hashstorage1 = buildIndexHashes(hashformat1,max_hashindex);
		auto hashstorage2 = buildIndexHashes(hashformat2, max_hashindex);
		
		std::uint64_t table_offset = 0;
		std::uint32_t tablesize = 0 ;
		std::uint32_t maxentry = 0 ;
		input.read(reinterpret_cast<char*>(&table_offset),sizeof(table_offset));
		input.read(reinterpret_cast<char*>(&tablesize),sizeof(tablesize));
		input.read(reinterpret_cast<char*>(&maxentry),sizeof(maxentry));
		
		// Read the table entries
		input.seekg(table_offset,std::ios::beg) ;
		std::vector<table_entry> entries ;
		entries.reserve(maxentry);
		while ((table_offset!= 0) && (!input.eof()) && input.good()){
			input.read(reinterpret_cast<char*>(&tablesize),sizeof(tablesize));
			input.read(reinterpret_cast<char*>(&table_offset),sizeof(table_offset));
			for (auto i=0 ;i < tablesize;i++){
				table_entry entry ;
				entry.load(input);
				entries.push_back(entry);
			}
			if ((table_offset!=0) && (!input.eof()) && input.good()){
				input.seekg(table_offset,std::ios::beg);
			}
		}
		auto current_entry = 0 ;
		for (auto &entry : entries){
			// Now loop through entries
			if ((entry.identifer != 0 ) && (entry.compressed_length != 0)) {
				input.seekg(entry.offset+entry.header_length,std::ios::beg) ;
				auto size = (entry.compression==0)?entry.decompressed_length : entry.compressed_length ;
				std::vector<std::uint8_t> uopdata(size,0);
				input.read(reinterpret_cast<char*>(uopdata.data()),size);
				if (entry.compression == 1){
					uopdata = decompress(uopdata, entry.decompressed_length);
				}
				// First see if we should even do anything with this hash
				if (processHash(entry.identifer, current_entry, uopdata)) {
					// Yes, we should!
					// Can we find an index?
					
					
					auto 	index = findIndex(hashstorage1, entry.identifer);
					if (index == std::numeric_limits<std::size_t>::max()){
						
						index = findIndex(hashstorage2, entry.identifer);
					}
					if (index == std::numeric_limits<std::size_t>::max()){
						
						if (!nonIndexHash(entry.identifer, current_entry, uopdata)){
							throw UnknownHash(entry.identifer,filepath);
						}
					}
					
					processEntry(current_entry, index, uopdata);
				}
			}
			current_entry++ ;
		}
		endUOPProcessing();
	}

}
