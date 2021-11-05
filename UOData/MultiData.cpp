//Copyright © 2021 Charles Kerr. All rights reserved.

#include "MultiData.hpp"
#include "StringUtility.hpp"

#include <iostream>
#include <fstream>
#include <filesystem>

using namespace std::string_literals;
namespace UO {
	//===============================================================
	const std::string MultiData::_hash_format = "build/multicollection/{6}.bin"s;
	const std::string MultiData::_uop_file = "MultiCollection.uop"s;
	const std::string MultiData::_idx_file = "multi.idx"s;
	const std::string MultiData::_mul_file = "multi.mul";

	
	// UOP overrides
	//===============================================================
	bool MultiData::processEntry(std::size_t entry, std::size_t index, const std::vector<unsigned char> &data){
		
		return true ;
	}
	bool MultiData::processHash(std::uint64_t hash,std::size_t entry , const std::vector<unsigned char> &data) {
		
		if (hash == 0x126D1E99DDEDEE0A){
			return false ;
		}
		 
		return true ;
	}
	//===============================================================
	bool MultiData::nonIndexHash(std::uint64_t hash, std::size_t entry, const std::vector<unsigned char> &data) {
		std::cerr <<"No index for entry "s <<entry<<" hash: " << strutil::numtostr(hash,16,true,16) << std::endl;
		return true ;
	}
	
	// IDX overrides
	//===============================================================
	// Provides the data associated with the corresponding record number
	void MultiData::recordData(std::uint32_t record_number, std::uint32_t extra, std::vector<std::uint8_t> &record_data){
	
	}
	
	// Public methods
	//===============================================================
	MultiData::MultiData(const std::string &uodir_uopfile){
		load(uodir_uopfile);
	}
	//===============================================================
	MultiData::MultiData(const std::string &idxpath,const std::string &mulpath){
		load(idxpath,mulpath);
	}

	
	//===============================================================
	bool MultiData::load(const std::string &uodir_uopfile){
		if (uodir_uopfile.empty()){
			return false ;
		}
		
		auto path = std::filesystem::path(uodir_uopfile);
		if (std::filesystem::is_directory(path)){
			// a directory!
			if (std::filesystem::exists((path/std::filesystem::path(_uop_file)))){
				// Process the uop file
				loadUOP((path/std::filesystem::path(_uop_file)).string(),8500, _hash_format);
			}
			else if ( std::filesystem::exists((path/std::filesystem::path(_idx_file))) &&  std::filesystem::exists((path/std::filesystem::path(_mul_file)))){
				// Process idx/mul
				processFiles((path/std::filesystem::path(_idx_file)).string(), (path/std::filesystem::path(_mul_file)).string());
			}
			else {
				return false ;
			}
		}
		else 	if (std::filesystem::exists((path/std::filesystem::path(_uop_file)))){
			// Process the uop file
			loadUOP(uodir_uopfile, _hash_entries, _hash_format);
		}
		else {
			return false ;
		}
		return true ;
	}
	//===============================================================
	bool MultiData::load(const std::string &idxpath, const std::string &mulpath){
		if (idxpath.empty() || mulpath.empty()){
			return false ;
		}
		if ( std::filesystem::exists(std::filesystem::path(_idx_file)) &&  std::filesystem::exists(std::filesystem::path(_mul_file))){
			processFiles(idxpath, mulpath);
			return true ;
		}
		return false ;
	}

}
