//
//  main.cpp
//  extractUO
//
//  Created by Charles Kerr on 11/5/21.
//
/******************************************************************************
 This program will extract informaton from the Ultima Online @Broadsword games
 data files.  One can choose to extract the tile information (both terrain/art, artwork (terrain/art) , textures, and gumps.
 The program is executed with a requirement of at least two parameters:
 		The directory where the UO data files reside
 		The directory where the data extracted will be place.
 
 		The output directory will have the following directory structure
 		created if it doesn't exist:
 			terrain/ (where all terrain artwork will be placed)
 			art/ (where all art artwork will be be placed)
 			textures/ (where all textures will be placed)
 			gumps/ (where all gump artwork will be placed)
 
 		The terrain and art info information is placed in comma delimited files
 		and placed in the output directory (terrain.csv and art.csv respectfully).
 
 	If no flags are added on the command line, the program will extract all the information.  The user can control the output by placing flags after the output directory:
 		--info  (produce the info files)
 		--terrain (produce the terrain artwork)
 		--art (produce the art artwork)
 		--texture (produce the textures)
 		--gumps (produce the gump artwork)
 
 	For the terrain/art,gumps: if they have uop versions, those are used first. If
 	the uop file can not be found, then the idx/mul file is used if appropriate.
 
 ***********************************************************************************/

#include <iostream>
#include <cstdlib>
#include <map>
#include <filesystem>
#include <fstream>
#include <string>
#include <stdexcept>

#include "StringUtility.hpp"
#include "TileData.hpp"
#include "ArtData.hpp"
#include "GumpData.hpp"
#include "TexMap.hpp"
#include "Bitmap.hpp"
#include "AnimationData.hpp"

using namespace std::string_literals;


bool _info = false ;
bool _terrain = false ;
bool _art = false ;
bool _texture = false ;
bool _gump = false ;
bool _animation = false ;

std::map<std::string,bool*> _flags {
	{"--info"s,&_info},{"--terrain"s,&_terrain},{"--art"s, &_art},
	{"--texture"s,&_texture},{"--gump"s,&_gump},{"--animation"s,&_animation}
};

int main(int argc, const char * argv[]) {
	
	if (argc < 3) {
		std::cerr <<"Usage: extractUO uo_directory output_directory [--info] [--terrain] [--art] --texture] [--gump]"s << std::endl;
		return EXIT_FAILURE;
	}
	auto uodir = std::filesystem::path(std::string(argv[1])) ;
	auto outputdir = std::filesystem::path(std::string(argv[2]));
	if (!std::filesystem::exists(uodir)){
		std::cerr <<"UO directory does not exist: "<< uodir.string()<< std::endl;
		return EXIT_FAILURE;
	}
	if (!std::filesystem::exists(outputdir)){
		std::cerr <<"Output directory does not exist: "<< outputdir.string()<< std::endl;
		return EXIT_FAILURE;
	}
	// We now have the directories!
	if (argc == 3) {
		// we are doing everything!
		for (auto &[name,addr] : _flags){
			*addr = true ;
		}
	}
	else {
		// Ok, we now search for the flag that where specified
		for (auto i=3 ; i < argc; i++) {
			auto flag = std::string(argv[i]);
			auto iter = _flags.find(flag) ;
			if (iter == _flags.end()){
				std::cerr <<"Unknown flag: " << flag << std::endl;
				return EXIT_FAILURE;
			}
			*(iter->second) = true ;
		}
	}
	try {
		// Now, lets process!
		if (_info || _art || _terrain){
			std::cout <<"Loading tile information" << std::endl;
			UO::TileData::shared(uodir.string());
			std::cout <<"Loading artwork (also needed for --info)" << std::endl;
			UO::ArtData artwork(uodir.string());
			if (_info) {
				auto terinfo = outputdir / std::filesystem::path("terrain.csv"s);
				auto artinfo = outputdir / std::filesystem::path("art.csv"s);
				// output the terrain info
				std::ofstream output(terinfo.string()) ;
				std::cout <<"Extracting Terrain info" << std::endl;
				if (!output.is_open()){
					std::cerr <<"Unable to open: "s << terinfo.string()<<std::endl;
					return EXIT_FAILURE;
				}
				for (auto i= 0 ; i < 0x4000;i++){
					auto info = UO::TileData::shared().terrain(i);
					auto hasart = artwork.hasTerrain(i);
					if (i==0) {
						output << "Tile ID," << info.csvTitle()<<",Artwork"<<std::endl;
					}
					output<<strutil::numtostr(i,16,true,4)<<","s<<info.csvRow()<<","s<<(hasart?"true":"false")<<std::endl;
				}
				output.close();
				
				// output the art info
				std::cout <<"Extracting Art info" << std::endl;
				output.open(artinfo.string()) ;
				if (!output.is_open()){
					std::cerr <<"Unable to open: "s << artinfo.string()<<std::endl;
					return EXIT_FAILURE;
				}
				
				for (auto i= 0 ; i < 0xFFFF;i++){
					auto info = UO::TileData::shared().art(i);
					auto hasart = artwork.hasArt(i);
					if (i==0) {
						output << "Tile ID," << info.csvTitle()<<",Artwork"<<std::endl;
					}
					output<<strutil::numtostr(i,16,true,4)<<","s<<info.csvRow()<<","s<<(hasart?"true":"false")<<std::endl;
				}
				output.close();
				
			}
			if (_terrain){
				std::cout <<"Extracting Terrain artwork" << std::endl;
				auto path = outputdir / std::filesystem::path("terrain"s);
				if (!std::filesystem::exists(path)){
					std::filesystem::create_directory(path);
				}
				auto maxterrain = artwork.maxTerrain();
				for (auto i= 0 ; i< maxterrain;i++){
					if (artwork.hasTerrain(i)){
						auto bitmap = artwork.terrain(i);
						auto filename = path / std::filesystem::path(strutil::numtostr(i,16,true,4)+".bmp"s);
						bitmap.save(filename.string());
					}
				}
			}
			if (_art){
				std::cout <<"Extracting Art artwork" << std::endl;
				auto path = outputdir / std::filesystem::path("art"s);
				if (!std::filesystem::exists(path)){
					std::filesystem::create_directory(path);
				}
				auto maxart = artwork.maxArt();
				for (auto i= 0 ; i< maxart;i++){
					if (artwork.hasArt(i)){
						auto bitmap = artwork.art(i);
						auto filename = path / std::filesystem::path(strutil::numtostr(i,16,true,4)+".bmp"s);
						bitmap.save(filename.string());
					}
				}
			}
			
		}
		if (_texture){
			std::cout <<"Loading Textures"<<std::endl;
			UO::TexMap texture(uodir.string());
			std::cout <<"Extracting Textures" << std::endl;
			auto maxid = texture.maxTexid();
			auto path = outputdir / std::filesystem::path("textures");
			if (!std::filesystem::exists(path)){
				std::filesystem::create_directory(path);
			}
			for (auto i= 0 ; i< maxid;i++){
				if (texture.hasTexture(i)){
					auto bitmap = texture.texture(i);
					auto filename = path / std::filesystem::path(strutil::numtostr(i,16,true,4)+".bmp"s);
					bitmap.save(filename.string());
				}
			}
		}
		if (_gump){
			std::cout <<"Loading Gumps"<<std::endl;
			UO::GumpData gumps(uodir.string());
			std::cout <<"Extracting Gumps" << std::endl;
			auto maxid = gumps.maxGump();
			auto path = outputdir / std::filesystem::path("gumps");
			if (!std::filesystem::exists(path)){
				std::filesystem::create_directory(path);
			}
			for (auto i= 0 ; i< maxid;i++){
				if (gumps.hasGump(i)){
					auto bitmap = gumps.gump(i);
					auto filename = path / std::filesystem::path(strutil::numtostr(i,16,true,4)+".bmp"s);
					bitmap.save(filename.string());
				}
			}
		}
		if (_animation){
			auto animpath = outputdir / std::filesystem::path("animations"s);
			if (!std::filesystem::exists(animpath)){
				std::filesystem::create_directory(animpath);
			}
			for (auto i = 0; i <6;i++){
				if (i!= 1){
					auto path = animpath;
					if (i!=0){
						path /= std::filesystem::path("animation-"s+std::to_string(i));
					}
					else {
						path /= std::filesystem::path("base"s);
					}
					if (!std::filesystem::exists(path)){
						std::filesystem::create_directory(path);
					}
					std::cout <<"Loading Animation data: "<<i<<std::endl;
					UO::AnimationData  data(uodir,i);
					auto maxid = data.maxID();
					std::cout <<"Extracting Animation data: "<<i<<std::endl;
					for (auto j= 0 ; j<maxid;j++){
						if (data.hasAnimation(j)){
							auto anpath = path / std::filesystem::path("animID-"+strutil::numtostr(j,16,true,4));
							if (!std::filesystem::exists(anpath)){
								std::filesystem::create_directory(anpath);
							}
							auto frames = data.animation(j);
							for (auto k=0;k<frames.size();k++){
								auto path = anpath / std::filesystem::path("frame-"s+std::to_string(k));
								frames[k].save(path.string());
							}
						}
					}
				}
				
			}
		}
	}
	catch (const std::exception &e){
		std::cerr <<e.what()<<std::endl;
		return EXIT_FAILURE;
	}
	std::cout <<"Extraction Complete" << std::endl;
	return EXIT_SUCCESS;
}
