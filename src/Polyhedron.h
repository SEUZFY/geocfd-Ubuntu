#pragma once

// JsonHandler
#include "JsonHandler.h"

// necessary include files from CGAL
#include <CGAL/Polyhedron_3.h>
#include <CGAL/Polyhedron_incremental_builder_3.h>
#include <CGAL/Nef_polyhedron_3.h>
#include <CGAL/convex_hull_3.h>
#include <CGAL/minkowski_sum_3.h>

// typedefs
typedef CGAL::Polyhedron_3<Kernel>                   Polyhedron;
typedef CGAL::Nef_polyhedron_3<Kernel>               Nef_polyhedron;
typedef Polyhedron::Facet_iterator                   Facet_iterator;
typedef Polyhedron::Halfedge_around_facet_circulator Halfedge_facet_circulator;


/*
load each building(or solid) into a CGAL Polyhedron_3 using the Polyhedron_incremental_builder_3. 
In order to use the Polyhedron_incremental_builder_3, you need to create a custom struct or class.
*/
template <class HDS>
struct Polyhedron_builder : public CGAL::Modifier_base<HDS> {
    std::vector<Point_3> vertices; // type: Kernel::Point_3
    std::vector<std::vector<unsigned long>> faces; // INDEX for vertices

    Polyhedron_builder() {}
    void operator()(HDS& hds) {
        CGAL::Polyhedron_incremental_builder_3<HDS> builder(hds, true);
        std::cout << "building surface with " << vertices.size() << " vertices and " << faces.size() << " faces" << '\n';

        builder.begin_surface(vertices.size(), faces.size());
        for (auto const& vertex : vertices) builder.add_vertex(vertex);
        for (auto const& face : faces) builder.add_facet(face.begin(), face.end());
        builder.end_surface();
    }
};


/*
use CGAL to build polyhedron
*/
class BuildPolyhedron
{
public:

    // build one polyhedron using vertices and faces from one shell (one building)
    // jhandle: A JsonHandler instance, contains all vertices and solids
    // index  : index of solids vector, indicating which solid is going to be built - ideally one building just contains one solid
    static void build_one_polyhedron(const JsonHandler& jhandle, std::vector<Nef_polyhedron>& Nefs, unsigned long index=0)
    {
        const auto& solid = jhandle.solids[index]; // get the solid
        if(solid.shells.size() != 1){
            std::cout<<"warning: this solid contains 0 or more than one shells\n";
            std::cout<<"please check build_one_polyhedron function and check the following solid:\n";
            std::cout<<"solid id: "<<solid.id<<'\n';
            std::cout<<"solid lod: "<<solid.lod<<'\n';
            std::cout<<"no polyhedron is built with this solid\n";
        }
        else{
            // create a polyhedron and a builder
            Polyhedron polyhedron;
            Polyhedron_builder<Polyhedron::HalfedgeDS> polyhedron_builder;

            // add vertices and faces to polyhedron_builder
            polyhedron_builder.vertices = jhandle.vertices; // now jhandle only handles one building(solid)
            for(auto const& shell : solid.shells)
                for(auto const& face : shell.faces)
                    for(auto const& ring : face.rings)
                        polyhedron_builder.faces.push_back(ring.indices);
            
            // call the delegate function
            polyhedron.delegate(polyhedron_builder);
            std::cout<<"polyhedron closed? "<<polyhedron.is_closed()<<'\n';


            /* test to write the polyhedron to .off file --------------------------------------------------------*/
            
            // Write polyhedron in Object File Format (OFF).
            // CGAL::set_ascii_mode(std::cout);
            // std::cout << "OFF" << std::endl << polyhedron.size_of_vertices() << ' '
            //     << polyhedron.size_of_facets() << " 0" << std::endl;
            // std::copy(polyhedron.points_begin(), polyhedron.points_end(),
            //     std::ostream_iterator<Point_3>( std::cout, "\n"));
            // for (Facet_iterator i = polyhedron.facets_begin(); i != polyhedron.facets_end(); ++i) 
            // {
            //     Halfedge_facet_circulator j = i->facet_begin();
            //     // Facets in polyhedral surfaces are at least triangles.
            //     std::cout << CGAL::circulator_size(j) << ' ';
            //     do {
            //         std::cout << ' ' << std::distance(polyhedron.vertices_begin(), j->vertex());
            //     } while ( ++j != i->facet_begin());
            //     std::cout << std::endl;
            // }

            // std::cout<<polyhedron<<std::endl;

            /* test to write the polyhedron to .off file --------------------------------------------------------*/


            if(polyhedron.is_closed()){
                Nef_polyhedron nef_polyhedron(polyhedron);
                Nefs.emplace_back();
                Nefs.back() = nef_polyhedron; // add the built nef_polyhedron to the Nefs vector
                std::cout<<"build nef polyhedron"<<'\n';
            }

            // visualize a polyhedron?
            
        }
       
    }
};



// extract geometries
// guidance: https://3d.bk.tudelft.nl/courses/geo1004//hw/3/#4-extracting-geometries
// credit: Ken Ohori
struct Shell_explorer {
    std::vector<Point_3> vertices; // store the vertices for one shell when extracting geometries from a Nef polyhedron
    std::vector<std::vector<unsigned long>> faces; // store faces for one shell when extracting geometries from a  Nef polyhedron

    std::vector<Point_3> cleaned_vertices; // after extracting geometries, process the vertices and store cleaned vertices for one shell
    std::vector<std::vector<unsigned long>> cleaned_faces; // after extracting geometries, process the face indices and store cleaned faces for one shell

    void visit(Nef_polyhedron::Vertex_const_handle v) {}
    void visit(Nef_polyhedron::Halfedge_const_handle he) {}
    void visit(Nef_polyhedron::SHalfedge_const_handle she) {}
    void visit(Nef_polyhedron::SHalfloop_const_handle shl) {}
    void visit(Nef_polyhedron::SFace_const_handle sf) {}

    void visit(Nef_polyhedron::Halffacet_const_handle hf) {
        // do something to each half-facet of a shell
        for (Nef_polyhedron::Halffacet_cycle_const_iterator it = hf->facet_cycles_begin(); it != hf->facet_cycles_end(); it++) {
            
            //std::cout << it.is_shalfedge() << " " << it.is_shalfloop() << '\n';
            Nef_polyhedron::SHalfedge_const_handle she = Nef_polyhedron::SHalfedge_const_handle(it);
            CGAL_assertion(she != 0);
            Nef_polyhedron::SHalfedge_around_facet_const_circulator hc_start = she;
            Nef_polyhedron::SHalfedge_around_facet_const_circulator hc_end = hc_start;
            //std::cout << "hc_start = hc_end? " << (hc_start == hc_end) << '\n';

            faces.emplace_back();
            unsigned long index = 0; // index for each half-facet
            CGAL_For_all(hc_start, hc_end) // each vertex of one halffacet
            {
                Nef_polyhedron::SVertex_const_handle svert = hc_start->source();
                Point_3 vpoint = svert->center_vertex()->point();
                //std::cout << "v: " << "(" << vpoint.x() << ", " << vpoint.y() << ", " << vpoint.z() << ")" << '\n';
                vertices.push_back(vpoint);
                faces.back().push_back(index++); 
            }
            //std::cout << '\n';
         
        }

    }
};



/*
* class to extract geometries of a nef polyhedron
*/
class NefProcessing
{
public:
    /*
    * Extract geometries from a Nef polyhedron
    * @params:
    * nef: a Nef_Polyhedron
    * shell_explorers: store all the shells of one Nef_Polyhedron
    */
    static void extract_nef_geometries(const Nef_polyhedron& nef, std::vector<Shell_explorer>& shell_explorers)
    {
        int volume_count = 0; // for counting volumes
        Nef_polyhedron::Volume_const_iterator current_volume;
        CGAL_forall_volumes(current_volume, nef)
        { 
            std::cout << "volume: " << volume_count++ << " ";
            std::cout << "volume mark: " << current_volume->mark() << '\n';
            Nef_polyhedron::Shell_entry_const_iterator current_shell;
            CGAL_forall_shells_of(current_shell, current_volume)
            {
                Shell_explorer se;
                Nef_polyhedron::SFace_const_handle sface_in_shell(current_shell);
                nef.visit_shell_objects(sface_in_shell, se);

                // add the se to shell_explorers
                shell_explorers.push_back(se);
            }
        }

        // prompt some info
        std::cout << "after extracting geometries: " << '\n';
        std::cout << "shell explorers size: " << shell_explorers.size() << '\n';
        std::cout << "info for each shell\n";
        for (const auto &se : shell_explorers)
        {
            std::cout << "vertices size of this shell: " << se.vertices.size() << '\n';
            std::cout << "faces size of this shell: " << se.faces.size() << '\n';
            std::cout << '\n';
        }
    }


    /*
    * in Shell_explorer, the index is calculated for each half-facet
    * thus we need further processing the indices for writing to json file
    * i.e.
    * for one shell, its faces vector can be: [[0,1,2,3], [0,1,2,3], [0,1,2,3], [0,1,2,3]] after extracting geometries
    * since in visit(Nef_polyhedron::Halffacet_const_handle hf) function, we store the indices for each half-facet
    * thus when we start to visit a new half-facet, the index starts from 0, that means the indices are not accumulated for the whole shell
    * so after the process, the faces vector should be: [[0,1,2,3], [4,5,6,7],...] - this kind of indices are "desired" for writing cityjson file
    * and what's more, the repeatness needs to be taken into consideration
    * 
    * @param:
    * shell_explorers: contains all shells for one building / solid
    * @output of this function:
    * if this function is called -> 
    * Shell_explorer.cleaned_vertices will be filled with cleaned vertices (ideally no repeated vertices)
    * Shell_explorer.cleaned_faces will be filled with corresponding indices in cleaned_vertices vector (0-based)
    * cleaned_vertices and cleaned_faces are used for writing to json file without no repeated vertices warnings
    */
    static void process_shells_for_cityjson(std::vector<Shell_explorer>& shell_explorers)
    {
        // step 1
        // get all vertices of all shells and corresponding face indices of each shell ----------------------
        // first store all the vertices in a vector
        std::vector<Point_3> all_vertices; // contains repeated vertices - for all shells in shell_explorers
        for (auto const &se : shell_explorers){
            for (auto const &v : se.vertices){
                all_vertices.push_back(v);
            }
        }

        // next store the face indices(accumulated from 0)
        unsigned long index_in_all_vertices = 0;
        for (auto &se : shell_explorers){
            for (auto &face : se.faces){
                for (auto &index : face){
                    index = index_in_all_vertices++;
                }
            }
        }
        // now we have the all_vertices and face indices to write to cityjson -------------------------------


        // step 2
        // assume there are no repeated faces (repeated faces can exist principally)
        // cope with repeatness, get the cleaned_vertices and cleaned_faces ---------------------------------
        for(auto& se : shell_explorers){
            unsigned long cleaned_index = 0; // indices of cleaned_faces, accumulated from 0, starts from each shell
            for(auto const& face : se.faces){
                se.cleaned_faces.emplace_back(); // add a new empty face
                for(auto const& index : face){
                    Point_3& vertex = all_vertices[index]; // get the vertex according to the index
                    if(!vertex_exist_check(se.cleaned_vertices, vertex)){ // if the vertex is not in cleaned_vertices vector
                        se.cleaned_vertices.push_back(vertex);
                        se.cleaned_faces.back().push_back(cleaned_index++); // add the cleaned_index to the newest added face, and increment the index
                    }
                    else{ // if the vertex is already in the cleaned_vertices vector
                        unsigned long exist_vertex_index = find_vertex_index(se.cleaned_vertices, vertex);
                        se.cleaned_faces.back().push_back(exist_vertex_index ); // add the found index to the newest added face
                    }
                }
            }
        }
        // now we have cleaned_vertices and cleaned_faces to write to cityjson ------------------------------
    }


protected:
    /*
    * The vertex_exist_check() and find_vertex_index() fucntions are also in JsonHandler class
    * there may be other ways to avoid code repeatness
    * for now, let's just define and use them
    */


	/*
	* check if a vertex already exists in a vertices vector
	* use coordinates to compare whether two vertices are the same
	* return: False - not exist, True - already exist
	*/
	static bool vertex_exist_check(std::vector<Point_3>& vertices, const Point_3& vertex) {
		bool flag(false);
		for (const auto& v : vertices) {
			if (
				abs(CGAL::to_double(vertex.x()) - CGAL::to_double(v.x())) < epsilon &&
				abs(CGAL::to_double(vertex.y()) - CGAL::to_double(v.y())) < epsilon &&
				abs(CGAL::to_double(vertex.z()) - CGAL::to_double(v.z())) < epsilon) {
				flag = true;
			}
		}
		return flag;
	}


	/*
	* if a vertex is repeated, find the index in vertices vector and return the index
	*/
	static unsigned long find_vertex_index(std::vector<Point_3>& vertices, Point_3& vertex) {
		for (std::size_t i = 0; i != vertices.size(); ++i) {
			if (
				abs(CGAL::to_double(vertex.x()) - CGAL::to_double(vertices[i].x())) < epsilon &&
				abs(CGAL::to_double(vertex.y()) - CGAL::to_double(vertices[i].y())) < epsilon &&
				abs(CGAL::to_double(vertex.z()) - CGAL::to_double(vertices[i].z())) < epsilon) {
				return (unsigned long)i;
			}
		}
		std::cout << "warning: please check find_vertex_index function, no index found" << '\n';
		return 0;
	}

    
};

