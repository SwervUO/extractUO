//Copyright © 2021 Charles Kerr. All rights reserved.

#ifndef IDXMul_hpp
#define IDXMul_hpp
/*******************************************************************************
 	Reads a UO IDX (index) type file and its assocated data (mul file)
 
 	And IDX type file is structured as a series of records that indicate an offset
 	into the data file, the length of data (bytes) and one extral 32 bit information
 	that can be used to indciate height/width (two 16 bits packed in the 32 bit),
 	an one off index (for the sound.mul file), or whatever.
 	If that record has no data associated with it (unused, invalid, etc), the offset
 	will be 0xFFFFFFFF or 0xFFFFFFFE depending on the idx file.  Or the length will
 	be zero.
 	Format for each record is:
 		std::uint32_t	offset
 		std::uint32_t	length
 		std::uint32_t	extra
 */
#include <string>
#include <cstdint>
#include <vector>
#include <array>
namespace UO {
	//===============================================================
	class IDXMul {
	private:
		
	protected:
		// Size of the mul file
		std::uint64_t _mulfile_size ;
		
		// Informs the subclass of the number of entries (valid/invalid) if
		// any sizeing wants to be performed
		virtual void entryCount(std::uint32_t number_of_entries){};
		
		// Provides the data associated with the corresponding record number
		virtual void recordData(std::uint32_t record_number, std::uint32_t extra, std::vector<std::uint8_t> &record_data)=0;
		// Informs the subclass that reading the data has completed
		virtual void  readingComplete(){} ;
		
		// Process the files
		void processFiles(const std::string &idxpath, const std::string &mulpath);
		
	public:
		virtual ~IDXMul() = default ;
	};
}
#endif /* IDXMul_hpp */
