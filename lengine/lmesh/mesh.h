#ifndef MESH_H
#define MESH_H
#include "halfedge.h"
#include "obj_parser.h"
#include "types.h"
#include <vector>
namespace lina { namespace mesh {
    template <typename FloatPercision>
    class mesh 
    {
        public:
            mesh(): m_reconvert_halfedge(true), m_halfedge(nullptr), m_write_edits(false){}
            void load(std::string&& path);
            void subdivide(i32 iter = 1);
            void simplify(i32 iter = 1);
            void remesh(f32 lambda = 0.2f, i32 iter = 1);
            const std::vector<FloatPercision>& get_positions();
            const std::vector<u32>& get_position_indices();
            void submit_edits(std::string&& path);
        private:
            obj_parser<FloatPercision> m_obj_parser;
            bool m_reconvert_halfedge;
            bool m_write_edits;
            halfedge::halfedge* m_halfedge;
            std::vector<u32> m_indices;
    };
}}
#endif
