#include "mesh.h"
#include "halfedge.h"
#include "obj_parser.h"
namespace lina { namespace mesh {
    template <typename FloatPercision>
    void mesh<FloatPercision>::load(std::string&& path)
    {
        m_obj_parser.parse(std::string(path));
        m_reconvert_halfedge = true;
        m_halfedge = nullptr;
        m_write_edits = false;
        auto& faces = m_obj_parser.get_indices();
        m_indices.reserve(faces.size());
        for (const auto& f : faces)
        {
            m_indices.push_back(f.position);
        }
    }
    template <typename FloatPercision>
    void mesh<FloatPercision>::subdivide(int iter)
    {
        if (m_reconvert_halfedge)
        {
            m_halfedge = m_obj_parser.to_halfedges(); 
            m_reconvert_halfedge = false;
        }
        halfedge::subdivide(m_halfedge, iter);
        m_write_edits = true;
    }
    template <typename FloatPercision>
    void mesh<FloatPercision>::simplify(int iter)
    {
        if (m_reconvert_halfedge)
        {
            m_halfedge = m_obj_parser.to_halfedges(); 
            m_reconvert_halfedge = false;
        }
        halfedge::simplify(&m_halfedge, iter);
        m_write_edits = true;
    }
    template <typename FloatPercision>
    void mesh<FloatPercision>::remesh(f32 lambda, int iter)
    {
        if (m_reconvert_halfedge)
        {
            m_halfedge = m_obj_parser.to_halfedges(); 
            m_reconvert_halfedge = false;
        }
        halfedge::remesh(&m_halfedge, lambda, iter);
        m_write_edits = true;
    }
    template <typename FloatPercision>
    void mesh<FloatPercision>::submit_edits(std::string&& path)
    {
        std::vector<FloatPercision> verts;
        std::vector<i32> faces;
        obj_parser<FloatPercision>::from_halfedges(
                m_halfedge, 
                verts, 
                faces);
        obj_parser<FloatPercision>::write_to_file(verts, faces, std::string(path));
    }
    template <typename FloatPercision>
        const std::vector<FloatPercision>& mesh<FloatPercision>::get_positions()
    {
        if (m_write_edits)
        {
            m_obj_parser.from_halfedges(m_halfedge);
            m_write_edits = false;
            m_indices.clear();
        }
        return m_obj_parser.get_positions();
    }
    template <typename FloatPercision>
    const std::vector<u32>& mesh<FloatPercision>::get_position_indices()
    {
        if (m_write_edits)
        {
            m_obj_parser.from_halfedges(m_halfedge);
            m_write_edits = false;
            m_indices.clear();
        }
        if (m_indices.size() == 0)
        {
            auto& faces = m_obj_parser.get_indices();
            m_indices.reserve(faces.size());
            for (const auto& f : faces)
            {
                m_indices.push_back(f.position);
            }
        }
        return m_indices;
    }
    template class mesh<f32>;
    template class mesh<f64>;

}}
