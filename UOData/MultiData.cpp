//Copyright © 2021 Charles Kerr. All rights reserved.

#include "MultiData.hpp"
#include "StringUtility.hpp"
#include "TileInfo.hpp"
#include "TileData.hpp"
#include "Buffer.hpp"
#include <iostream>
#include <fstream>
#include <filesystem>
#include <algorithm>


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
		
		std::size_t offset = 4 ;  // skip the first 32 bit word
		std::uint32_t count ;
		count = (*reinterpret_cast<const std::uint32_t *>((data.data()+offset))) ;
		offset = offset + 4 ;
		multi_structure structure ;
		for (unsigned int i = 0 ; i < count ; i++){
			multi_st component ;
			component.flag = 0 ;
			component.tileid  = (*reinterpret_cast<const std::uint16_t *>((data.data()+offset))) ;
			component.info = TileData::shared().art(component.tileid);
			offset = offset + 2 ;
			component.x = (*reinterpret_cast<const std::int16_t *>((data.data()+offset))) ;
			offset = offset + 2 ;
			component.y = (*reinterpret_cast<const std::int16_t *>((data.data()+offset))) ;
			offset = offset + 2 ;
			component.z = (*reinterpret_cast<const std::int16_t *>((data.data()+offset))) ;
			offset = offset + 2 ;
			auto flags = (*reinterpret_cast<const std::uint16_t *>((data.data()+offset))) ;
			offset = offset + 2 ;
			switch (flags) {
				default:
				case 0: 					// background
					component.flag = 1 ;
					break;
				case 1:					// none
					break;
				case 257:					// generic
					component.flag = 0x800;
					break;
			}
			auto clilocs = (*reinterpret_cast<const std::uint32_t *>((data.data()+offset))) ;
			offset = offset + 4 ;
			if (clilocs != 0 ) {
				offset = offset + (clilocs * 4);
			}

			structure.components.push_back(component);
		}
		// Now we need to find min/max
		std::sort(structure.components.rbegin(),structure.components.rend(),[](const multi_st &lhs,multi_st &rhs) {
			return lhs.x < rhs.x;
		});
		structure.max_x = structure.components.begin()->x;
		structure.min_x = structure.components.rbegin()->x;
		std::sort(structure.components.rbegin(),structure.components.rend(),[](const multi_st &lhs,multi_st &rhs) {
			return lhs.y < rhs.y;
		});
		structure.max_y = structure.components.begin()->y;
		structure.min_y = structure.components.rbegin()->y;
		std::sort(structure.components.rbegin(),structure.components.rend(),[](const multi_st &lhs,multi_st &rhs) {
			return lhs.z < rhs.z;
		});
		structure.max_z = structure.components.begin()->z;
		structure.min_z = structure.components.rbegin()->z;
		_multis.insert_or_assign(index, structure);

		
		
		return true ;
	}
	//===============================================================
	bool MultiData::processHash(std::uint64_t hash,std::size_t entry , const std::vector<unsigned char> &data) {
		
		if (hash == 0x126D1E99DDEDEE0A){
			saveMultiCollectoin(data);
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
		if (record_data.empty()){
			return ;
		}
		multi_structure structure ;
		structure.name = nameForID(record_number);
		Buffer buffer(record_data) ;
		std::int16_t value16;
		std::uint32_t flag32 ;
		std::uint64_t flag64 ;
		while (buffer.position() < buffer.size()){
			multi_st multi ;
			buffer >> multi.tileid ;
			multi.info = TileData::shared().art(multi.tileid);
			buffer >> value16 ;
			multi.x = value16;
			buffer >> value16 ;
			multi.y = value16;
			buffer >> value16 ;
			multi.z = value16;
			if (_useHS){
				buffer>>flag64;
				multi.flag = flag64;
			}
			else {
				buffer >> flag32 ;
				multi.flag = flag32 ;
			}
			structure.components.push_back(multi);
		}
		// Now we need to find min/max
		std::sort(structure.components.rbegin(),structure.components.rend(),[](const multi_st &lhs,multi_st &rhs) {
			return lhs.x < rhs.x;
		});
		structure.max_x = structure.components.begin()->x;
		structure.min_x = structure.components.rbegin()->x;
		std::sort(structure.components.rbegin(),structure.components.rend(),[](const multi_st &lhs,multi_st &rhs) {
			return lhs.y < rhs.y;
		});
		structure.max_y = structure.components.begin()->y;
		structure.min_y = structure.components.rbegin()->y;
		std::sort(structure.components.rbegin(),structure.components.rend(),[](const multi_st &lhs,multi_st &rhs) {
			return lhs.z < rhs.z;
		});
		structure.max_z = structure.components.begin()->z;
		structure.min_z = structure.components.rbegin()->z;
		_multis.insert_or_assign(record_number, structure);
	}
	
	//===============================================================
	// Private methods
	
	std::string MultiData::nameForID(std::size_t id) const{
		auto iter = _multi_names.find(id) ;
		if (iter == _multi_names.end()){
			return ""s;
		}
		return iter->second;
	}
	//===============================================================
	void MultiData::saveMultiCollectoin(const std::vector<std::uint8_t> &data){
		if(!_multicollection_file.empty() && !data.empty()){
			std::ofstream output(_multicollection_file,std::ios::binary);
			if (output.is_open()){
				output.write(reinterpret_cast<const char*>(data.data()),data.size());
			}
			output.close();
		}
	}

	
	// Public methods
	
	//===============================================================
	std::size_t MultiData::maxID() const {
		if (_multis.empty()){
			return 0;
		}
		return _multis.rbegin()->first + 1;
	}
	//===============================================================
	bool MultiData::hasMulti(std::uint16_t multiid) const{
		auto iter = _multis.find(multiid);
		return iter!= _multis.end();
	}
	//===============================================================
	std::map<std::size_t,multi_structure> MultiData::allMultis() const {
		return _multis ;
	}
	//===============================================================
	multi_structure MultiData::multiFor(std::uint16_t multiid) const {
		auto iter = _multis.find(multiid);
		if (iter != _multis.end()){
			return iter->second ;
		}
		return multi_structure();
	}
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
