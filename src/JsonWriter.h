#pragma once

#include "JsonHandler.h"
#include "Polyhedron.h"


/*
* class for writing big nef_polyhedron to cityjson file
*/
class JsonWriter : public JsonHandler
{
public:
	/*
	* write the selected building to cityjson file
	* index: index of solids, indicating which solid is going to be written to the json file
	* this needs to be altered to write the big nef to cityjson
	*/
	void write_json_file(const std::string& filename, const Nef_polyhedron& big_nef)
	{
		// basic info ---------------------------------------------------------------
		json js;
		js["type"] = "CityJSON";
		js["version"] = "1.1";
		js["transform"] = json::object();
		js["transform"]["scale"] = json::array({ 1.0, 1.0, 1.0 });
		js["transform"]["translate"] = json::array({ 0.0, 0.0, 0.0 });
		js["vertices"] = json::array({}); // vertices

		// all vertices(including repeated vertices)-----------------------------------		
		for (auto const& v : vertices) {
			double x = CGAL::to_double(v.x()); // warning: may have precision loss
			double y = CGAL::to_double(v.y());
			double z = CGAL::to_double(v.z());
			js["vertices"].push_back({ x, y, z });
		}

		// names
		// std::string bp_name = solids[index].id; // BuildingPart's name, i.e. "NL.IMBAG.Pand.0503100000019695-0"
		// std::string b_name = bp_name.substr(0, bp_name.length() - 2); // Building's name, i.e. "NL.IMBAG.Pand.0503100000019695"

		// // Building info -------------------------------------------------------------------
		// js["CityObjects"] = json::object();
		// js["CityObjects"][b_name]["type"] = "Building";
		// js["CityObjects"][b_name]["attributes"] = json({});
		// js["CityObjects"][b_name]["children"] = json::array({ bp_name });
		// js["CityObjects"][b_name]["geometry"] = json::array({});

		// // BuildingPart info ---------------------------------------------------------------
		// js["CityObjects"][bp_name]["type"] = "BuildingPart";
		// js["CityObjects"][bp_name]["attributes"] = json({});
		// js["CityObjects"][bp_name]["parents"] = json::array({ b_name });

		// // geometry
		// js["CityObjects"][bp_name]["geometry"] = json::array();
		// js["CityObjects"][bp_name]["geometry"][0]["type"] = "Solid";
		// js["CityObjects"][bp_name]["geometry"][0]["lod"] = "1.3"; // lod must be string, otherwise invalid file
		// js["CityObjects"][bp_name]["geometry"][0]["boundaries"] = json::array({}); // indices	

		// // boundaries
		// auto& boundaries = js["CityObjects"][bp_name]["geometry"][0]["boundaries"][0];
		// const auto& so = solids[index];
		// for (const auto& se : so.shells)
		// 	for (const auto& f : se.faces)
		// 		for(const auto& r : f.rings)
		// 			boundaries.push_back({ r.indices });
	
		// write to file
		std::string json_string = js.dump(2);
		std::ofstream out_stream(filename);
		out_stream << json_string;
		out_stream.close();
		std::cout << "file saved at: " << filename << '\n';
	}

};
