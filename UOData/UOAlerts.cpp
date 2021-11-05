//Copyright © 2021 Charles Kerr. All rights reserved.

#include "UOAlerts.hpp"
#include <iostream>
#include "StringUtility.hpp"
using namespace std::string_literals;
namespace UO {
	//===============================================================
	UOAlert::UOAlert(const std::string & name): std::runtime_error(name){
		alert_type = AlertType::base ;
	}

	//===============================================================
	FileOpen::FileOpen(const std::string & filepath): UOAlert("Unable to open: '"s+filepath+"'."s) {
		alert_type = AlertType::fileopen;
		this->filepath = filepath ;
	}
	//===============================================================
	StreamError::StreamError(const std::string & filepath): UOAlert("Error processing stream"s + (filepath.empty()?"."s:(" for file: '"s+filepath+"'."s)) ) {
		alert_type = AlertType::streamerror;
		this->filepath = filepath ;
	}
	
	//===============================================================
	UnknownHash::UnknownHash(std::uint64_t hash , const std::string &filepath) : UOAlert("Uknown hash: " +strutil::numtostr(hash,16,true,16)+" in '"s+filepath+"'"s) {
		alert_type = AlertType::unknownhash;
		this->filepath = filepath;
		this->hash = hash ;
	}
	//===============================================================
	InvalidUOP::InvalidUOP(std::uint32_t signature, std::uint32_t version, const std::string &filepath): UOAlert("Invalid UOP"s + (filepath.empty()?", "s:(" '"s+filepath+"', "s))+"Signature: "s + strutil::numtostr(signature,16,true,8)+", Version: "s + std::to_string(version)+"."s){
		alert_type = AlertType::invaliduop;
		this->filepath = filepath;
		this->signature = signature ;
		this->version = version;
	}
	
	//===============================================================
	InvalidTileID::InvalidTileID(std::string tiletype, std::size_t tileid): UOAlert("Invalid "s + tiletype+" id: "s+ strutil::numtostr(tileid,16,true,4)){
		alert_type = AlertType::invalidtileid;
		this->tileid = tileid;
		this->tiletype = tiletype ;
	}
	
	//===============================================================
	InvalidArtSize::InvalidArtSize( std::size_t width,std::size_t height): UOAlert("Invalid Art size: "s+std::to_string(width)+" , "s+std::to_string(height)){
		alert_type = AlertType::invalidartsize ;
		
		this->width = width ;
		this->height = height ;
	}
}
