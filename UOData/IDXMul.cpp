//Copyright © 2021 Charles Kerr. All rights reserved.

#include "IDXMul.hpp"
#include "UOAlerts.hpp"
#include <iostream>
#include <fstream>
#include <filesystem>


using namespace std::string_literals;

namespace UO {
	//===============================================================
	// Process the files
	void IDXMul::processFiles(const std::string &idxpath, const std::string &mulpath){
		std::array<std::uint32_t,3> _record ;
		std::ifstream idx(idxpath,std::ios::binary) ;
		if (!idx.is_open()){
			throw FileOpen(idxpath) ;
		}
		_mulfile_size = std::filesystem::file_size(std::filesystem::path(mulpath));
		std::ifstream mul(mulpath,std::ios::binary) ;
		if (!mul.is_open()){
			throw FileOpen(mulpath) ;
		}
		// Get the size of the idx file
		idx.seekg(0,std::ios::end);
		auto record_total = static_cast<std::uint32_t>(idx.tellg()/12);
		idx.seekg(0, std::ios::beg);
		// Notify the subclass the record count ;
		entryCount(record_total);
		std::uint32_t entry = 0 ;
		while (!idx.eof() && idx.good()){
			idx.read(reinterpret_cast<char*>(_record.data()),4);
			idx.read(reinterpret_cast<char*>(_record.data()+1),4);
			idx.read(reinterpret_cast<char*>(_record.data()+2),4);
			if ((idx.gcount()==4) && idx.good() && !idx.eof()) {
				// Process the record, the read was good
				if ((_record[0]<0xFFFFFFFE) && (_record[1]>0)) {
					// This is a valid offset into data
					mul.seekg(_record[0],std::ios::beg) ;
					std::vector<std::uint8_t> data(_record[1],0);
					mul.read(reinterpret_cast<char*>(data.data()),_record[1]);
					// Was the read good?
					if (mul.gcount() != _record[1]) {
						idx.close();
						mul.close();
						throw StreamError(mulpath);
					}
					//Notify the subclass about the data
					recordData(entry, _record[2], data);
				}
			}
			entry++;
		}
		idx.close();
		mul.close();
		// Notify the subclass we are through reading the data
		readingComplete();
	}

}
