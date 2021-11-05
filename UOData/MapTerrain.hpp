//Copyright © 2021 Charles Kerr. All rights reserved.

#ifndef MapTerrain_hpp
#define MapTerrain_hpp

#include <string>
#include <cstdint>
#include <vector>

#include "UOPData.hpp"
namespace UO {
	//===============================================================
	class MapTerrain : public UOPData {
		
	private:
		static const std::string _hash_format ;
		static const std::string _mul_format ;
		static const std::string _uop_format ;
		
		std::string _hashformat ;
		std::string _uopfile ;
		std::string _mulfile ;
		
		void buildStrings(std::size_t mapnumber);
		
		bool _usedUOP ;
	protected:
		
		void processBlock(std::size_t blocknum, const std::vector<std::uint8_t> &data);
		
		bool processEntry(std::size_t entry, std::size_t index, const std::vector<std::uint8_t> &data) final ;

		void readMul(const std::string &mulpath);
	public:
		bool load(const std::string &datapath,std::size_t mapnumber);
		bool uop() const {return _usedUOP;}
	};
	
}

#endif /* MapTerrain_hpp */
