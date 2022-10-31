#include <CGAL/Polyhedron_3.h>
#include "JsonWriter.h"
#include <CGAL/boost/graph/helpers.h>
#include "Polyhedron.h" 
#include <CGAL/OFF_to_nef_3.h>
#include <string>
#include <CGAL/Vector_3.h>
#include <CGAL/Exact_predicates_inexact_constructions_kernel.h>
#include <CGAL/Surface_mesh.h>
#include <CGAL/Polygon_mesh_processing/distance.h>
// #include "MultiThread.hpp"
// #include <CGAL/boost/graph/IO/STL.h>

// #define _ENABLE_MULTI_THREADING_
// #define DATA_PATH "/home/atherias/synthesis-NEF/geocfd-Ubuntu/data/"

#define _ENABLE_MINKOWSKI_SUM_ // switch on/off minkowski sum method -> active by default
#define _ENABLE_CONVEX_HULL_ // switch on/off convex hull method

typedef Kernel::Vector_3 Vector_3;
typedef Nef_polyhedron::Aff_transformation_3  Aff_transformation_3;

Nef_polyhedron minkowski_sum_custom(Nef_polyhedron&, double);
// Nef_polyhedron* get_aabb(Nef_polyhedron*);

int main(int argc, const char** argv)
{
	std::string DATA_PATH = "/home/atherias/synthesis-NEF/geocfd-Ubuntu/data/";
	std::string filename = argv[1];
    // std::cout << "current reading file is: " << DATA_PATH + filename << '\n';
    std::ifstream input(filename);
    json j; 
    input >> j;
    input.close();

	std::vector<double> LoDs{1.2,1.3,2.2};

	for (auto l : LoDs)
	{

		double lod = l;

		std::vector<JsonHandler> jhandlers;

		// read buildings directly from json (use all geometries found)
		for (auto& co : j["CityObjects"].items()) 
			{
				if (co.key().find("-0")!=-1)
				{
					std::string building_name = co.key();
					JsonHandler jhandler;
					jhandler.read_certain_building(j, building_name, lod);
					// jhandler.message();
					jhandlers.emplace_back(jhandler);
				}
			}
			

	// 	std::cout << "------------------------ building(part) info ------------------------\n";

	// 	for (auto& co : j["CityObjects"].items()) // iterate over buildings in file
	// 	{
	// 	// for (auto const& building_name : adjacency) // get each building
	// 			{
	// 				JsonHandler jhandler;
	// 				jhandler.read_building(j, building_name, lod); // read in the building, change if we are specifying based on list of adjacent buildings
	// 				jhandler.message();
	// 				jhandlers.emplace_back(jhandler); // add to the jhandlers vector
	// 			}
	// 		iterator +=1;
	// 	}

	// 	std::cout << "---------------------------------------------------------------------\n";

		// a vector to store the nef polyhedra(if built successfully)
		std::vector<Nef_polyhedron> nefs;

		// build Nef_polyhedron and store in Nefs vector
		for (const auto& jhdl : jhandlers) {
			Build::build_nef_polyhedron(jhdl, nefs); // triangulation tag can be passed as parameters, set to true by default
		}

		// prompt Nefs
		std::cout << "there are " << nefs.size() << " Nef polyhedra now\n";

		// big Nef
		std::cout << "building big nef...\n";
		Nef_polyhedron big_nef;
		for (const auto& nef : nefs) {
			big_nef += nef;
		}	
		std::cout << "build big nef done\n";

		// process the original big nef to make it available for output
		std::vector<Shell_explorer> shell_explorers; // store the extracted geometries
		NefProcessing::extract_nef_geometries(big_nef, shell_explorers); // extract geometries of the bignef
		NefProcessing::process_shells_for_cityjson(shell_explorers); // process shells for writing to cityjson


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
		double mink_value = 0.01;

		for (auto& nef : nefs)
		{
			Nef_polyhedron merged_nef = minkowski_sum_custom(nef, mink_value); // cube size is 1.0 by default, can be altered
			merged_big_nef += merged_nef;
			// original_big_nef += nef;
		}
		std::cout << "performing minkowski sum done\n";

	// 	// // erode Nef back to original building (credits to Hugo Ledoux)
	// 	// Nef_polyhedron* merged_nef_pointer = &merged_big_nef;

	// 	// Nef_polyhedron* output;
	//   	// Nef_polyhedron* bbox = get_aabb(merged_nef_pointer);
	//   	// Nef_polyhedron complement = *bbox - *merged_nef_pointer;
	//   	// Nef_polyhedron tmp = minkowski_sum_custom(complement, mink_value);
	//   	// *output = *merged_nef_pointer - tmp;
	// 	// // Fengyan: please try running with regularization enabled to see if it prevents loss of faces
	// 	// // output->regularization();

		// process the merged big nef to make it available for output
		std::vector<Shell_explorer> merged_shell_explorers;
		NefProcessing::extract_nef_geometries(merged_big_nef, merged_shell_explorers);
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

		std::vector<Shell_explorer> original_shell_explorers;
		NefProcessing::extract_nef_geometries(big_nef, original_shell_explorers);
		NefProcessing::process_shells_for_cityjson(original_shell_explorers);
	#endif

		// write file json original geometry
		JsonWriter jwrite_original;
		std::ostringstream oss_original;
		oss_original << "/dataset_3_original_" << std::to_string(lod)[0] << "_" << std::to_string(lod)[2] << ".city.json";
		std::string fileString_original = oss_original.str();
		std::string writeFilename_original = fileString_original;
		// const Shell_explorer& shell = shell_explorers[0]; // which shell is going to be written to the file, 0 - exterior, 1 - interior
		const Shell_explorer& shell_original = original_shell_explorers[0]; // enable this instead when using MINK
		std::cout << "writing the result to cityjson file...\n";
		jwrite_original.write_json_file(DATA_PATH + writeFilename_original, shell_original, lod);

	// 	// // write file json bbox
	// 	// JsonWriter jwrite_bbox;
	// 	// std::string writeFilename_bbox= "/bbox.city.json";
	// 	// // const Shell_explorer& shell = shell_explorers[0]; // which shell is going to be written to the file, 0 - exterior, 1 - interior
	// 	// const Shell_explorer& shell_bbox = bbox_shell_explorers[0]; // enable this instead when using MINK
	// 	// std::cout << "writing the result to cityjson file...\n";
	// 	// jwrite_bbox.write_json_file(DATA_PATH + writeFilename_bbox, shell_bbox, lod);

	// 	// // write file json complement
	// 	// JsonWriter jwrite_complement;
	// 	// std::string writeFilename_complement= "/complement.city.json";
	// 	// // const Shell_explorer& shell = shell_explorers[0]; // which shell is going to be written to the file, 0 - exterior, 1 - interior
	// 	// const Shell_explorer& shell_complement = complement_shell_explorers[0]; // enable this instead when using MINK
	// 	// std::cout << "writing the result to cityjson file...\n";
	// 	// jwrite_complement.write_json_file(DATA_PATH + writeFilename_complement, shell_complement, lod);

	// 	// // write file json complement + mink
	// 	// JsonWriter jwrite_complement_mink;
	// 	// std::string writeFilename_complement_mink= "/complement_mink.city.json";
	// 	// // const Shell_explorer& shell = shell_explorers[0]; // which shell is going to be written to the file, 0 - exterior, 1 - interior
	// 	// const Shell_explorer& shell_complement_mink = complement_mink_shell_explorers[1]; // enable this instead when using MINK
	// 	// std::cout << "writing the result to cityjson file...\n";
	// 	// jwrite_complement_mink.write_json_file(DATA_PATH + writeFilename_complement_mink, shell_complement_mink, lod);

		// calculate hausdorf distance
		// make polygon meshes, one for original and one for merged: Shell_explorer.cleaned_vertices, Shell_explorer.cleaned_faces
		// CGAL::Polygon_mesh_processing::approximate_Hausdorff_distance(tm1, tm2)
		// write value to txt file

		// write file json
		JsonWriter jwrite;
		std::ostringstream oss;
		oss << "/dataset_3_" << std::to_string(lod)[0] << "_" << std::to_string(lod)[2] << ".city.json";
		std::string fileString = oss.str();
		std::string writeFilename = fileString;
		// const Shell_explorer& shell = shell_explorers[0]; // which shell is going to be written to the file, 0 - exterior, 1 - interior
		const Shell_explorer& shell = merged_shell_explorers[0]; // enable this instead when using MINK
		std::cout << "writing the result to cityjson file...\n";
		jwrite.write_json_file(DATA_PATH + writeFilename, shell, lod);

	//    // // write file .stl
	//    // std::ostream "model.stl"
	//    // write_STL(stl_file, shell)	
                    
	}
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