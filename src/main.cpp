#include <CGAL/Polyhedron_3.h>
#include "JsonWriter.h"
#include <CGAL/boost/graph/helpers.h>
#include "Polyhedron.h" 
#include <CGAL/OFF_to_nef_3.h>
// #include <CGAL/boost/graph/IO/STL.h
typedef Nef_polyhedron::Aff_transformation_3  Aff_transformation_3;
Nef_polyhedron minkowski_sum_custom(Nef_polyhedron&, double);
#include <CGAL/Vector_3.h>
typedef Kernel::Vector_3 Vector_3;
Nef_polyhedron* get_aabb(Nef_polyhedron*);

//#define DATA_PATH ""
#define _ENABLE_MINKOWSKI_SUM_ // switch on/off minkowski sum method -> active by default

int main(int argc, const char** argv)
{

   std::cout << "-- activated data folder: " << DATA_PATH << '\n';
	std::cout << "This is: " << argv[0] << '\n';

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


	double lod = 1.3; // specify the lod level

	// get ids of adjacent buildings
	const char* adjacency[] = { "NL.IMBAG.Pand.0503100000019695-0",
								"NL.IMBAG.Pand.0503100000018413-0",
								"NL.IMBAG.Pand.0503100000018423-0",
								"NL.IMBAG.Pand.0503100000018419-0",
								"NL.IMBAG.Pand.0503100000018408-0",
								"NL.IMBAG.Pand.0503100000018412-0",
								"NL.IMBAG.Pand.0503100000018407-0",
								"NL.IMBAG.Pand.0503100000018411-0", 
								"NL.IMBAG.Pand.0503100000018425-0",
								"NL.IMBAG.Pand.0503100000018422-0",
								"NL.IMBAG.Pand.0503100000018427-0",
								"NL.IMBAG.Pand.0503100000018409-0",
								"NL.IMBAG.Pand.0503100000004564-0",
								"NL.IMBAG.Pand.0503100000032517-0",
								"NL.IMBAG.Pand.0503100000019797-0",
								"NL.IMBAG.Pand.0503100000019796-0",
								"NL.IMBAG.Pand.0503100000004566-0",
								"NL.IMBAG.Pand.0503100000004565-0",
								"NL.IMBAG.Pand.0503100000031928-0",
								"NL.IMBAG.Pand.0503100000017031-0",
								"NL.IMBAG.Pand.0503100000027802-0",
								"NL.IMBAG.Pand.0503100000027801-0",
								"NL.IMBAG.Pand.0503100000018586-0" };


	//read certain building, stores in jhandlers vector
	std::vector<JsonHandler> jhandlers;
	//jhandlers.reserve(size); -> if we know the length of adjacency lsit, we can use reserve()
	std::cout << "------------------------ building(part) info ------------------------\n";
	
	for (auto const& building_name : adjacency) // get each building
	{
		JsonHandler jhandler;
		jhandler.read_certain_building(j, building_name, lod); // read in the building
		jhandler.message();
		jhandlers.emplace_back(jhandler); // add to the jhandlers vector
	}

	std::cout << "---------------------------------------------------------------------\n";

	// a vector to store the nef polyhedra(if built successfully)
	std::vector<Nef_polyhedron> Nefs;

	// build Nef_polyhedron and sotres in Nefs vector
	for (auto const& jhandler : jhandlers)
	{
		BuildPolyhedron::build_nef_polyhedron(jhandler, Nefs);
	}

	// prompt Nefs
	std::cout << "there are " << Nefs.size() << " Nef polyhedra now\n";


	// check if Nef is simple and convert it to polyhedron 3 for visualise
	// for(const auto& nef : Nefs)
	// {
	//    std::cout<<"is nef simple? "<<nef.is_simple()<<'\n';
	//    if(nef.is_simple())
	//    {
	//       Polyhedron p;
	//       nef.convert_to_Polyhedron(p);
	//       std::cout<<p;
	//    }
	// }


	// big Nef
	std::cout << "building big nef...\n";
	Nef_polyhedron big_nef;
	for (const auto& nef : Nefs) {
		big_nef += nef;
	}	
	std::cout << "build big nef done\n";


	// check if big Nef is simple - simple: no internal rooms, not simple: multiple rooms?
	std::cout << "is bigNef simple? " << big_nef.is_simple() << '\n';

	// process the big nef to make it available for output
	std::vector<Shell_explorer> shell_explorers; // store the extracted geometries
	NefProcessing::extract_nef_geometries(big_nef, shell_explorers); // extract geometries of the bignef
	NefProcessing::process_shells_for_cityjson(shell_explorers); // process shells for writing to cityjson

	// prompt some info after cleaning operation
	/*std::cout << "info about shells after cleaning operations\n";
	std::cout << "shell explorers size: " << shell_explorers.size() << '\n';
	std::cout << "info for each shell\n";
	for (const auto& se : shell_explorers)
	{
		std::cout << "cleaned vertices size of this shell: " << se.cleaned_vertices.size() << '\n';
		std::cout << "cleaned faces size of this shell: " << se.cleaned_faces.size() << '\n';
		std::cout << '\n';
	}*/

#ifdef _ENABLE_CONVEX_HULL_
	/* get the convex hull of the big_nef, use all cleaned vertices of all shells */
	// get cleaned vertices of shell_explorers[0] - the shell indicating the exterior of the big nef
	std::vector<Point_3>& convex_vertices = shell_explorers[0].cleaned_vertices;

	// build convex hull of the big nef
	Polyhedron convex_polyhedron; // define polyhedron to hold convex hull
	Nef_polyhedron convex_big_nef;
	CGAL::convex_hull_3(convex_vertices.begin(), convex_vertices.end(), convex_polyhedron);
	std::cout << "is convex closed? " << convex_polyhedron.is_closed() << '\n';
	if (convex_polyhedron.is_closed()) {
		std::cout << "build convex hull for the big nef...\n";
		Nef_polyhedron convex_nef(convex_polyhedron);
		convex_big_nef = convex_nef;
		std::cout << "build convex hull for the big nef done\n";
	}

	// process the convex big nef to make it available for output
	std::vector<Shell_explorer> convex_shell_explorers;
	NefProcessing::extract_nef_geometries(convex_big_nef, convex_shell_explorers);
	NefProcessing::process_shells_for_cityjson(convex_shell_explorers);
#endif


#ifdef _ENABLE_MINKOWSKI_SUM_
	// minkowski_sum_3 -> add a "buffer" for each nef in Nefs
	std::cout << "performing minkowski sum...\n";
	// Nef_polyhedron original_big_nef; 
	Nef_polyhedron merged_big_nef; 
	double mink_value = 0.2;

	for (auto& nef : Nefs)
	{
		Nef_polyhedron merged_nef = minkowski_sum_custom(nef, mink_value); // cube size is 1.0 by default, can be altered
		merged_big_nef += merged_nef;
		// original_big_nef += nef;
	}
	std::cout << "performing minkowski sum done\n";

	// erode Nef back to original building (credits to Hugo Ledoux)
	Nef_polyhedron* merged_nef_pointer = &merged_big_nef;

	Nef_polyhedron* output;
  	Nef_polyhedron* bbox = get_aabb(merged_nef_pointer);
  	Nef_polyhedron complement = *bbox - *merged_nef_pointer;
  	Nef_polyhedron tmp = minkowski_sum_custom(complement, mink_value);
  	*output = *merged_nef_pointer - tmp;
	// Fengyan: please try running with regularization enabled to see if it prevents loss of faces
	// output->regularization();

	// process the merged big nef to make it available for output
	std::vector<Shell_explorer> merged_shell_explorers;
	NefProcessing::extract_nef_geometries(*output, merged_shell_explorers);
	NefProcessing::process_shells_for_cityjson(merged_shell_explorers);

	// // process the bbox nef to make it available for output
	// std::vector<Shell_explorer> bbox_shell_explorers;
	// NefProcessing::extract_nef_geometries(*bbox, bbox_shell_explorers);
	// NefProcessing::process_shells_for_cityjson(bbox_shell_explorers);

	// // process the complement nef to make it available for output
	// std::vector<Shell_explorer> complement_shell_explorers;
	// NefProcessing::extract_nef_geometries(complement, complement_shell_explorers);
	// NefProcessing::process_shells_for_cityjson(complement_shell_explorers);

	// // process the complement nef with mink to make it available for output
	// std::vector<Shell_explorer> complement_mink_shell_explorers;
	// NefProcessing::extract_nef_geometries(tmp, complement_mink_shell_explorers);
	// NefProcessing::process_shells_for_cityjson(complement_mink_shell_explorers);

	// std::vector<Shell_explorer> original_shell_explorers;
	// NefProcessing::extract_nef_geometries(merged_big_nef, original_shell_explorers);
	// NefProcessing::process_shells_for_cityjson(original_shell_explorers);
#endif

	// // write file json original geometry
	// JsonWriter jwrite_original;
	// std::string writeFilename_original = "/original.city.json";
	// // const Shell_explorer& shell = shell_explorers[0]; // which shell is going to be written to the file, 0 - exterior, 1 - interior
	// const Shell_explorer& shell_original = original_shell_explorers[1]; // enable this instead when using MINK
	// std::cout << "writing the result to cityjson file...\n";
	// jwrite_original.write_json_file(DATA_PATH + writeFilename_original, shell_original, lod);

	// // write file json bbox
	// JsonWriter jwrite_bbox;
	// std::string writeFilename_bbox= "/bbox.city.json";
	// // const Shell_explorer& shell = shell_explorers[0]; // which shell is going to be written to the file, 0 - exterior, 1 - interior
	// const Shell_explorer& shell_bbox = bbox_shell_explorers[0]; // enable this instead when using MINK
	// std::cout << "writing the result to cityjson file...\n";
	// jwrite_bbox.write_json_file(DATA_PATH + writeFilename_bbox, shell_bbox, lod);

	// // write file json complement
	// JsonWriter jwrite_complement;
	// std::string writeFilename_complement= "/complement.city.json";
	// // const Shell_explorer& shell = shell_explorers[0]; // which shell is going to be written to the file, 0 - exterior, 1 - interior
	// const Shell_explorer& shell_complement = complement_shell_explorers[0]; // enable this instead when using MINK
	// std::cout << "writing the result to cityjson file...\n";
	// jwrite_complement.write_json_file(DATA_PATH + writeFilename_complement, shell_complement, lod);

	// // write file json complement + mink
	// JsonWriter jwrite_complement_mink;
	// std::string writeFilename_complement_mink= "/complement_mink.city.json";
	// // const Shell_explorer& shell = shell_explorers[0]; // which shell is going to be written to the file, 0 - exterior, 1 - interior
	// const Shell_explorer& shell_complement_mink = complement_mink_shell_explorers[1]; // enable this instead when using MINK
	// std::cout << "writing the result to cityjson file...\n";
	// jwrite_complement_mink.write_json_file(DATA_PATH + writeFilename_complement_mink, shell_complement_mink, lod);

    // write file json
	JsonWriter jwrite;
	std::string writeFilename = "/eroded-exterior.city.json";
	// const Shell_explorer& shell = shell_explorers[0]; // which shell is going to be written to the file, 0 - exterior, 1 - interior
	const Shell_explorer& shell = merged_shell_explorers[0]; // enable this instead when using MINK
	std::cout << "writing the result to cityjson file...\n";
	jwrite.write_json_file(DATA_PATH + writeFilename, shell, lod);

   // // write file .stl
   // std::ostream "model.stl"
   // write_STL(stl_file, shell)	

	return 0;
                    
	return 0;
}

Nef_polyhedron minkowski_sum_custom(Nef_polyhedron& nef, double size)
    {
        Nef_polyhedron cube = NefProcessing::make_cube(size);
		// translate cube back to be around the origin
		Vector_3 v = Vector_3(-size/2, -size/2, -size/2);
		Aff_transformation_3 transl(CGAL::TRANSLATION, v);
		cube.transform(transl);
        return CGAL::minkowski_sum_3(nef, cube);     
    }

Nef_polyhedron* get_aabb(Nef_polyhedron* mynef) 
{
  double xmin =  1e12; 
  double ymin =  1e12; 
  double zmin =  1e12; 
  double xmax = -1e12;
  double ymax = -1e12;
  double zmax = -1e12;;
  Nef_polyhedron::Vertex_const_iterator v;
  for (v = mynef->vertices_begin(); v != mynef->vertices_end(); v++) 
  {
    if ( CGAL::to_double(v->point().x()) < xmin )
      xmin = CGAL::to_double(v->point().x());
    if ( CGAL::to_double(v->point().y()) < ymin )
      ymin = CGAL::to_double(v->point().y());
    if ( CGAL::to_double(v->point().z()) < zmin )
      zmin = CGAL::to_double(v->point().z());
    if ( CGAL::to_double(v->point().x()) > xmax )
      xmax = CGAL::to_double(v->point().x());
    if ( CGAL::to_double(v->point().y()) > ymax )
      ymax = CGAL::to_double(v->point().y());
    if ( CGAL::to_double(v->point().z()) > zmax )
      zmax = CGAL::to_double(v->point().z());
  }
   //-- expand the bbox by 10units
  xmin -= 50;
  ymin -= 50;
  zmin -= 50;
  xmax += 50;
  ymax += 50;
  zmax += 50;
  //-- write an OFF file and convert Nef, simplest (and fastest?) solution
  std::stringstream ss;
  ss << "OFF" << std::endl
     << "8 6 0" << std::endl
     << xmin << " " << ymin << " " << zmin << std::endl
     << xmax << " " << ymin << " " << zmin << std::endl
     << xmax << " " << ymax << " " << zmin << std::endl
     << xmin << " " << ymax << " " << zmin << std::endl
     << xmin << " " << ymin << " " << zmax << std::endl
     << xmax << " " << ymin << " " << zmax << std::endl
     << xmax << " " << ymax << " " << zmax << std::endl
     << xmin << " " << ymax << " " << zmax << std::endl
     << "4 0 3 2 1" << std::endl
     << "4 0 1 5 4"    << std::endl
     << "4 1 2 6 5"    << std::endl
     << "4 2 3 7 6"    << std::endl
     << "4 0 4 7 3"    << std::endl
     << "4 4 5 6 7"    << std::endl;
  Nef_polyhedron* nefbbox = new Nef_polyhedron;
  CGAL::OFF_to_nef_3(ss, *nefbbox);
  return nefbbox;
}