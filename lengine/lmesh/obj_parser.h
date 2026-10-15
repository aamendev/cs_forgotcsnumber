#ifndef OBJ_PARSER_H
#define OBJ_PARSER_H
#include <string>
#include <vector>
#include "halfedge.h"
#include "types.h"
namespace lina { namespace mesh{
    enum class ParseErr
    {
        FileNotFound,
        Unknown,
        None,
    };
    struct FaceIndx 
    {
        i64 position = -1;
        i64 texture = -1;
        i64 normal = -1;
    };
    template <typename FloatPercision>
    class obj_parser 
    {
        public:
            ParseErr parse(std::string&& path);
            static std::string err_string(const ParseErr& err);
            inline void clear_data();
            static void parse_line(std::vector<FloatPercision>& vec, const std::string& line);
            void parse_faces(const std::string& line);
            const inline std::vector<FaceIndx>& get_indices() const;
            const inline std::vector<FloatPercision>& get_positions() const;
            static void write_to_file(const std::vector<FloatPercision>& verts, const std::vector<i32>& faces, std::string&& path);
            static halfedge::halfedge* to_halfedges(const std::vector<FloatPercision>& verts, const std::vector<i32>& faces);
            halfedge::halfedge* to_halfedges();
            static void from_halfedges(halfedge::halfedge* he, std::vector<FloatPercision>& verts, std::vector<i32>& faces);
            void from_halfedges(halfedge::halfedge* he);
            void write_to_file(std::string&& path);
        private:
            std::vector<FloatPercision> m_verts;
            std::vector<FloatPercision> m_normals;
            std::vector<FloatPercision> m_tex_coords;
            std::vector<FaceIndx> m_indices;
    };
    template <typename FloatPercision>
        inline void obj_parser<FloatPercision>::clear_data()
        {
            m_verts.clear();
            m_normals.clear();
            m_tex_coords.clear();
            m_indices.clear();
        }
    template <typename FloatPercision>
        inline const std::vector<FaceIndx>& 
        obj_parser<FloatPercision>::get_indices() const
        {
            return m_indices;
        }
    template <typename FloatPercision>
        inline const std::vector<FloatPercision>& 
        obj_parser<FloatPercision>::get_positions() const
        {
            return m_verts;
        }
}}
#endif
