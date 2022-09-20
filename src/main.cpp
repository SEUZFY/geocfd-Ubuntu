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
   JsonHandler jhandle1;
   std::string building1_id = "NL.IMBAG.Pand.0503100000019695-0";
   jhandle1.read_certain_building(j, building1_id);
   jhandle1.message();

   JsonHandler jhandle2;
   std::string building2_id = "NL.IMBAG.Pand.0503100000018413-0"; // adjacent to building1
   jhandle2.read_certain_building(j, building2_id);
   jhandle2.message();
    
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
    

   // build a vector to store the nef polyhedra(if built successfully)
   std::vector<Nef_polyhedron> Nefs;


   // build polyhedron test
   BuildPolyhedron::build_one_polyhedron(jhandle1, Nefs);
   BuildPolyhedron::build_one_polyhedron(jhandle2, Nefs);


   // prompt Nefs
   std::cout<<"there are "<<Nefs.size()<<" Nef polyhedra now\n";


   // check if Nef is simple
   for(const auto& nef : Nefs)
      std::cout<<"is nef simple? "<<nef.is_simple()<<'\n';


   // big Nef
   Nef_polyhedron big_nef;
   for(const auto& nef : Nefs)
      big_nef += nef;


   // check if big Nef is simple - simple: no internal rooms, not simple: multiple rooms?
   std::cout<<"is bigNef simple? "<<big_nef.is_simple()<<'\n';


   // extract geometries 
	std::vector<Shell_explorer> shell_explorers;
	int volume_count = 0;
	int shell_count = 0;
	Nef_polyhedron::Volume_const_iterator current_volume;
	CGAL_forall_volumes(current_volume, big_nef) {
		std::cout << "volume: " << volume_count++ << " ";
		std::cout << "volume mark: " << current_volume->mark() << '\n';
		Nef_polyhedron::Shell_entry_const_iterator current_shell;
		CGAL_forall_shells_of(current_shell, current_volume) {
			Shell_explorer se;
			Nef_polyhedron::SFace_const_handle sface_in_shell(current_shell);
			big_nef.visit_shell_objects(sface_in_shell, se);

			//add the se to shell_explorers
			shell_explorers.push_back(se);
		}
	}
	std::cout << "after extracting geometries: " << '\n';
	std::cout << "shell explorers size: " << shell_explorers.size() << '\n';

   std::cout<<"info for each shell\n";
   for(const auto& se : shell_explorers){
      std::cout << "vertices size of this shell: " << se.vertices.size() << '\n';
		std::cout << "faces size of this shell: " << se.faces.size() << '\n';
		std::cout << '\n';
   }

   // write file
   //std::string writeFilename = "/SimpleBuildings.json";
   //jhandle.write_json_file(DATA_PATH + writeFilename, 0); // second argument: indicating which solid is going to be written to the file
                    

	return 0;
}