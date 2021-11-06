//Copyright © 2021 Charles Kerr. All rights reserved.

#ifndef UOAlerts_hpp
#define UOAlerts_hpp
/******************************************************************************
 Provides exceptions that may be encountered while using the UO classes
 ******************************************************************************/
#include <string>
#include <cstdint>
#include <stdexcept>
namespace UO {
	enum class AlertType {base,fileopen,streamerror,unknownhash,invaliduop,invalidtileid,invalidartsize,invalidanimfileid};
	//===============================================================
	// Base alert, to allow one to catch just this if desired
	struct UOAlert : public std::runtime_error {
		AlertType alert_type ;
		UOAlert(const std::string & name);
	};
	
	//===============================================================
	// File open alert, unable to open file
	struct FileOpen : public UOAlert {
		std::string filepath ;
		FileOpen(const std::string & filepath);
	};

	//===============================================================
	// Stream Error, error reading on the file stream
	struct StreamError : public UOAlert {
		std::string filepath ;
		StreamError(const std::string &filepath="");
	};
	//===============================================================
	// Unknown Hash in a UOP file
	struct UnknownHash : public UOAlert {
		std::string filepath ;
		std::uint64_t hash ;
		UnknownHash(std::uint64_t hash , const std::string &filepath="");
	};
	//===============================================================
	// Invalid UOP file
	struct InvalidUOP : public UOAlert {
		std::string filepath ;
		std::uint32_t version ;
		std::uint32_t signature ;
		InvalidUOP(std::uint32_t signature, std::uint32_t version, const std::string &filepath="");
	};

	//===============================================================
	// Invalid UOP file
	struct InvalidTileID : public UOAlert {
		std::string tiletype ;
		std::size_t tileid ;
		InvalidTileID(std::string tiletype, std::size_t tileid);
	};
	//===============================================================
	// Invalid ArtSize file
	struct InvalidArtSize : public UOAlert {
		std::size_t width ;
		std::size_t height ;
		
		InvalidArtSize( std::size_t width,std::size_t height);
	};
	//===============================================================
	// Invalid ArtSize file
	struct InvalidAnimationFileID : public UOAlert {
		std::int32_t fileid ;
		
		InvalidAnimationFileID( std::int32_t fileid);
	};

}
#endif /* UOAlerts_hpp */
