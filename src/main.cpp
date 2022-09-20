#include "Polyhedron.h"
#include <CGAL/Polyhedron_3.h>

//#define DATA_PATH "/home/fengyan/geocfd/data"

int main(int argc, const char** argv)
{

   std::cout << "-- activated data folder: " << DATA_PATH << '\n';
   std::cout<<"This is: "<<argv[0]<<'\n';

   //  std::cout<<"newly-added\n";
   //std::cout<<"data path is: "<<mypath<<'\n';
    
   //  char buffer[256];
   //  if (getcwd(buffer, sizeof(buffer)) != NULL) {
   //     printf("Current working directory : %s\n", buffer);
   //  } else {
   //     perror("getcwd() error");
   //     return 1;
   //  }

   //-- reading the (original)file with nlohmann json: https://github.com/nlohmann/json  
   std::string filename = "/3dbag_v210908_fd2cee53_5907.json";
   std::cout << "current reading file is: " << DATA_PATH + filename << '\n';
   std::ifstream input(DATA_PATH + filename);
   json j;
   input >> j;
   input.close();

   //read certain building
   JsonHandler jhandle;
   std::string building_id = "NL.IMBAG.Pand.0503100000019695-0";
   jhandle.read_certain_building(j, building_id);
   std::cout << "number of vertices: " << jhandle.vertices.size() << '\n';
    
   // // test output
   //
   // std::cout << "vertices number: " << '\n';
   // for (const auto& so : jhandle.solids)
   //    for (const auto& se : so.shells)
   //       for (const auto& f : se.faces)
   //          for (const auto& r : f.rings) // for most cases, each face only contains one ring -> i.e. face [[0,1,2,3]] only has one ring
   //             {
   //                std::cout << "--------" << '\n';
   //                for (const auto& indice : r.indices)
   //                   std::cout << indice << '\n';
   //             }
    

   // build polyhedron test
   BuildPolyhedron::build_one_polyhedron(jhandle, 0);

   // write file
   std::string writeFilename = "/SimpleBuildings.json";
   jhandle.write_json_file(DATA_PATH + writeFilename, 0); // second argument: indicating which solid is going to be written to the file
                    

	return 0;
}