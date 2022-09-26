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

            std::cout<<polyhedron<<std::endl;

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
    std::vector<Point_3> vertices;
    std::vector<std::vector<unsigned long>> faces;

    void visit(Nef_polyhedron::Vertex_const_handle v) {}
    void visit(Nef_polyhedron::Halfedge_const_handle he) {}
    void visit(Nef_polyhedron::SHalfedge_const_handle she) {}
    void visit(Nef_polyhedron::SHalfloop_const_handle shl) {}
    void visit(Nef_polyhedron::SFace_const_handle sf) {}

    void visit(Nef_polyhedron::Halffacet_const_handle hf) {
        for (Nef_polyhedron::Halffacet_cycle_const_iterator it = hf->facet_cycles_begin(); it != hf->facet_cycles_end(); it++) {
            
            //std::cout << it.is_shalfedge() << " " << it.is_shalfloop() << '\n';
            Nef_polyhedron::SHalfedge_const_handle she = Nef_polyhedron::SHalfedge_const_handle(it);
            CGAL_assertion(she != 0);
            Nef_polyhedron::SHalfedge_around_facet_const_circulator hc_start = she;
            Nef_polyhedron::SHalfedge_around_facet_const_circulator hc_end = hc_start;
            //std::cout << "hc_start = hc_end? " << (hc_start == hc_end) << '\n';

            faces.emplace_back();
            int index = 0;
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



// post process of shell explorers
