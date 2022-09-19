#include "FileIO.h"
#include <CGAL/Polyhedron_3.h>

// get file path
#include <string>
#include <filesystem>
#include <unistd.h>
#include <stdio.h>

#define DATA_PATH "/home/fengyan/geocfd/data"

//using std::filesystem::current_path;


int main(int argc, const char** argv)
{
    std::cout << "-- activated data folder: " << DATA_PATH << '\n';
    std::cout<<"This is: "<<argv[0]<<'\n';
    
    char buffer[256];
    if (getcwd(buffer, sizeof(buffer)) != NULL) {
       printf("Current working directory : %s\n", buffer);
    } else {
       perror("getcwd() error");
       return 1;
    }

    //-- reading the (original)file with nlohmann json: https://github.com/nlohmann/json  
    // std::string filename = "/3dbag_v210908_fd2cee53_5907.json";
    // std::cout << "current reading file is: " << DATA_PATH + filename << '\n';
    // std::ifstream input(DATA_PATH + filename);
    // json j;
    // input >> j;
    // input.close();

    // // read certain building
    // FileIO fileIO;
    // std::string building_id = "NL.IMBAG.Pand.0503100000019695-0";
    // fileIO.read_certain_building(j, building_id);
    // std::cout << "number of vertices: " << fileIO.vertices.size() << '\n';
    
    // // test output
    // /*
    // std::cout << "vertices number: " << '\n';
    // for (const auto& so : fileIO.solids)
    //     for (const auto& se : so.shells)
    //         for (const auto& f : se.faces)
    //             for (const auto& r : f.rings)
    //             {
    //                 std::cout << "--------" << '\n';
    //                 for (const auto& indice : r.indices)
    //                     std::cout << indice << '\n';
    //             }
    // */

    // // write file
    // std::string writeFilename = "/SimpleBuildings.json";
    // //fileIO.write_json_file(DATA_PATH + writeFilename, 0); // second argument: indicating which solid is going to be written to the file
                    

	return 0;
}