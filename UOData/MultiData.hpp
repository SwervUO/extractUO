//Copyright © 2021 Charles Kerr. All rights reserved.

#ifndef MultiData_hpp
#define MultiData_hpp

#include <string>
#include <cstdint>
#include <map>
#include <vector>
#include <array>
#include "UOPData.hpp"
#include "IDXMul.hpp"
#include "TileInfo.hpp"
namespace UO {
	//===============================================================
	class MultiData : public IDXMul, public UOPData {
	private:
		static const std::map<std::size_t,std::string> _multi_names ;
		static const std::string _uop_file ;
		static const std::string _hash_format ;
		static const std::string _idx_file ;
		static const std::string _mul_file ;
		static constexpr std::size_t _hash_entries = 8500 ; // 908592
		static constexpr std::size_t hs_size = 908592;
		std::map<std::size_t,multi_structure> _multis;
		bool _useHS ;
		std::string _multicollection_file ;
		std::string nameForID(std::size_t id) const ;
		void saveMultiCollectoin(const std::vector<std::uint8_t> &data);
	protected:
		
		// UOP overrides
		bool processEntry(std::size_t entry, std::size_t index, const std::vector<std::uint8_t> &data)final;
		bool processHash(std::uint64_t hash,std::size_t entry , const std::vector<std::uint8_t> &data) final;
		bool nonIndexHash(std::uint64_t hash, std::size_t entry, const std::vector<std::uint8_t> &data) final;

		// IDX overrides
		// Provides the data associated with the corresponding record number
		void recordData(std::uint32_t record_number, std::uint32_t extra, std::vector<std::uint8_t> &record_data) final;

	public:
		std::size_t maxID() const ;
		bool hasMulti(std::uint16_t multiid) const;
		multi_structure multiFor(std::uint16_t multiid) const ;
		std::map<std::size_t,multi_structure> allMultis() const ;
		
		MultiData(const std::string &uodir_uopfile = "");
		MultiData(const std::string &idxpath,const std::string &mulpath);

		std::string multiCollectionBin() const ;
		void multiCollectionBin(const std::string &filename) ;
		
		bool load(const std::string &uodir_uopfile);
		bool load(const std::string &idxpath, const std::string &mulpath);
		
		
	};
}
#endif /* MultiData_hpp */
