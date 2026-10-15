#include "obj_parser.h"
#include "lmath/vector3d.h"
#include <algorithm>
#include <cstdio>
#include <fstream>
#include <ios>
#include <iostream>
#include <stack>
namespace lina { namespace mesh{
    template <typename T>
        ParseErr obj_parser<T>::parse(std::string&& path)
        {
            FILE* stream = fopen(path.c_str(), "r");
            if (!stream)
            {
                return ParseErr::FileNotFound;
            }
            char* buffer = NULL;
            size_t size = 0;
            while (getline(&buffer, &size, stream) != EOF)
            {
                std::string line(buffer);
                size_t pos = 0;
                if (line.find("#") == std::string::npos)
                {
                    if ((pos = line.find("v")) != std::string::npos)
                    {
                        if (line.find("t ") == pos + 1)
                        {
                            parse_line(m_tex_coords, line);
                            continue;
                        }
                        else if (line.find("n ") == pos + 1)
                        {
                            parse_line(m_normals, line);
                            continue;
                        }
                        else
                        {
                            parse_line(m_verts, line);
                            continue;
                        }
                    }
                    else if (line.find("f ") != std::string::npos)
                    {
                        parse_faces(line);            
                        continue;
                    }
                    else 
                    {
                        if (line.empty() || line == "\n")
                            continue;
                        //std::cerr << "Invalid Obj File!\n Line: " << line << '\n';
                        //clearAll();
                        continue;
                        break;
                    }
                }
            }
            fclose(stream);
            return ParseErr::None;
        }

    template <typename T>
        void obj_parser<T>::parse_line(std::vector<T>& vec, const std::string& line)
        {
            auto first_iter = (std::find_if(line.begin(), line.end(), [](char c){
                        return (c >= '0' && c <= '9') || (c == '-');}));
            auto end_iter = line.end();
            while (
                    first_iter != line.end()
                  )
            {
                end_iter = std::find(first_iter, line.end(), ' ');
                T f = std::atof(std::string(first_iter, end_iter).c_str());
                vec.push_back(f);
                first_iter = (std::find_if(end_iter, line.end(), [](char c){
                            return (c >= '0' && c <= '9') || (c == '-');}));
            }
        }
    template <typename T>
        void obj_parser<T>::parse_faces(const std::string& line)
        {
            std::vector<std::string> faces;
            faces.reserve(3);
            auto first_space = std::find_if(line.begin(), line.end(), [](char c){
                    return c == ' ' || c == '\n' || c == '\r' || c == '\t';
                    });
            auto second_space = line.end();
            while (first_space != line.end())
            {
                second_space = std::find_if(first_space, line.end(), [](char c){
                        return c == ' ' || c == '\n' || c == '\r' || c == '\t';
                        });
                faces.push_back(std::string(first_space, second_space));
                if (second_space  == line.end())
                {
                    break;
                }
                first_space = second_space + 1;
            }
            for (auto subline: faces)
            {
                auto first_iter = (std::find_if(subline.begin(), subline.end(), [](char c){
                            return (c >= '0' && c <= '9') || (c == '/');}));
                auto end_iter = subline.end();
                int curr = 0;
                auto sz = m_indices.size();
                while (
                        first_iter != subline.end()
                      )
                {
                    i64 f = -1;

                    if (*first_iter != '/')
                    {
                        end_iter = std::find(first_iter, subline.end(), '/');
                        f = std::atoi(std::string(first_iter, end_iter).c_str()) - 1;
                    }
                    else 
                    {
                        end_iter = first_iter;
                    }
                    if (end_iter == subline.end())
                    {
                        end_iter--;
                    }
                    if (curr == 0)
                    {
                        m_indices.push_back(
                                {
                                .position= f,
                                .texture = -1,
                                .normal = -1,
                                }
                                );
                    }
                    else if (curr == 1)
                    {
                        m_indices[sz].texture = f;
                    }
                    else if (curr == 2)
                    {
                        m_indices[sz].normal = f;
                    }
                    first_iter = (std::find_if(end_iter+1, subline.end(), [](char c){
                                return (c >= '0' && c <= '9') || (c == '/');}));
                    curr++;
                }
            }
        }

    template <typename T>
        std::string obj_parser<T>::err_string(const ParseErr& err)
        {
            switch (err)
            {
                case ParseErr::FileNotFound:
                    {
                        return "obj_parser_err: file not found";
                    }
                    break;
                default: return "obj_parser_err: unknown error";
            }
        }
        template <typename T>
    void obj_parser<T>::write_to_file(const std::vector<T>& verts, const std::vector<i32>& faces, std::string&& file_path)
    {
        std::ofstream outfile;
        outfile.open(file_path);
        // Write vertices
        for (size_t i = 0; i < verts.size(); i+=3)
        {
            outfile << std::fixed << "v " << verts[i + 0] << " " << verts[i + 1] << " " << verts[i + 2] << '\n';
        }

        // Write faces
        for (size_t i = 0; i < faces.size(); i+=3)
        {
            outfile << "f " << (faces[0 + i]+1) << " " << (faces[1 + i]+1) << " " << (faces[2 + i]+1) << '\n';
        }

        outfile.close();
    }

        template <typename T>
    halfedge::halfedge* obj_parser<T>::to_halfedges(const std::vector<T>& verts, const std::vector<i32>& faces)
    {
        std::unordered_map<std::pair<int, int>, halfedge::halfedge*, pairhash> halfedge_pairs;
        std::unordered_map<int, halfedge::halfedge*> verts_hash;
        halfedge::halfedge* ret = nullptr;
        for (int i = 0; i < faces.size(); i+=3)
        {
            math::vector3d<f64> v[3] = {
                math::vector3d<f64>(verts[3 * faces[i+0]], verts[3 * faces[i+0] + 1], verts[3 * faces[i+0] + 2]),
                math::vector3d<f64>(verts[3 * faces[i+1]], verts[3 * faces[i+1] + 1], verts[3 * faces[i+1] + 2]),
                math::vector3d<f64>(verts[3 * faces[i+2]], verts[3 * faces[i+2] + 1], verts[3 * faces[i+2] + 2]),
                };
                halfedge::face* f = new halfedge::face;
            halfedge::halfedge* prev = nullptr;
            for (int j = 0; j < 3; j++)
            {
                halfedge::halfedge* he = new halfedge::halfedge;
                he->f = f;


                int it = verts_hash.count(3*faces[i + (j + 1) % 3]);
                if (it == 0)
                {
                    he->tip = new halfedge::vertex;
                    verts_hash[3*faces[i + (j + 1) % 3]] = he;
                    he->tip->hedge = he;
                    he->tip->pos = v[(j+1) % 3];
                }
                else 
                {
                    he->tip = verts_hash[3*faces[i + (j+1) % 3]]->tip;
                }
                if (prev != nullptr)
                {
                    prev->next = he;
                }
                else 
                {
                    f->hedge = he;
                }
                prev = he;
                halfedge_pairs[std::make_pair(3*faces[i + j], 3*faces[i + (j + 1) % 3])] = he;
                if (i == 0 && j == 0)[[unlikely]]
                {
                    ret = he;
                }
            }
            prev->next = f->hedge;
        }
        for (auto& [k, v] : halfedge_pairs)
        {
            std::pair<int, int> twin_pair = {k.second, k.first};
            int it = halfedge_pairs.count(twin_pair);
            if (it == 0)
            {
                v->twin = v;
                halfedge_pairs[twin_pair] = v;
                continue;
            }
            v->twin = halfedge_pairs[twin_pair];
            halfedge_pairs[twin_pair]->twin = v;
        }
        return ret;
    }
        template <typename T>
    halfedge::halfedge* obj_parser<T>::to_halfedges()
    {
        const auto& verts = m_verts;
        const auto& faces = m_indices;
        std::unordered_map<std::pair<int, int>, halfedge::halfedge*, pairhash> halfedge_pairs;
        std::unordered_map<int, halfedge::halfedge*> verts_hash;
        halfedge::halfedge* ret = nullptr;
        for (int i = 0; i < faces.size(); i+=3)
        {
            math::vector3d<f64> v[3] = {
                math::vector3d<f64>(verts[3 * faces[i+0].position], verts[3 * faces[i+0].position + 1], verts[3 * faces[i+0].position + 2]),
                math::vector3d<f64>(verts[3 * faces[i+1].position], verts[3 * faces[i+1].position + 1], verts[3 * faces[i+1].position + 2]),
                math::vector3d<f64>(verts[3 * faces[i+2].position], verts[3 * faces[i+2].position + 1], verts[3 * faces[i+2].position + 2]),
                };
                halfedge::face* f = new halfedge::face;
            halfedge::halfedge* prev = nullptr;
            for (int j = 0; j < 3; j++)
            {
                halfedge::halfedge* he = new halfedge::halfedge;
                he->f = f;


                int it = verts_hash.count(3*faces[i + (j + 1) % 3].position);
                if (it == 0)
                {
                    he->tip = new halfedge::vertex;
                    verts_hash[3*faces[i + (j + 1) % 3].position] = he;
                    he->tip->hedge = he;
                    he->tip->pos = v[(j+1) % 3];
                }
                else 
                {
                    he->tip = verts_hash[3*faces[i + (j+1) % 3].position]->tip;
                }
                if (prev != nullptr)
                {
                    prev->next = he;
                }
                else 
                {
                    f->hedge = he;
                }
                prev = he;
                halfedge_pairs[std::make_pair(3*faces[i + j].position, 3*faces[i + (j + 1) % 3].position)] = he;
                if (i == 0 && j == 0)[[unlikely]]
                {
                    ret = he;
                }
            }
            prev->next = f->hedge;
        }
        for (auto& [k, v] : halfedge_pairs)
        {
            std::pair<int, int> twin_pair = {k.second, k.first};
            int it = halfedge_pairs.count(twin_pair);
            if (it == 0)
            {
                v->twin = v;
                halfedge_pairs[twin_pair] = v;
                continue;
            }
            v->twin = halfedge_pairs[twin_pair];
            halfedge_pairs[twin_pair]->twin = v;
        }
        return ret;
    }
        template <typename T>
    void obj_parser<T>::from_halfedges(halfedge::halfedge* e, std::vector<T>& verts, std::vector<i32>& faces)
    {
        verts.clear();
        faces.clear();
        std::stack<halfedge::halfedge*> stack;
        std::unordered_set<halfedge::halfedge*> visited;
        std::unordered_map<halfedge::vertex*, int> idx_map;

        int curr_idx = 0;
        stack.push(e);
        int fc = 0;
        while (!stack.empty())
        {
            halfedge::halfedge* he = stack.top();
            stack.pop();

            if (visited.count(he))
                continue;

            halfedge::halfedge* curr  = he;
            int count = 0;
            i32 face[3] = {-1, -1, -1};
            do
            {
                int it = idx_map.count(curr->tip);
                visited.insert(curr);
                if (it == 0)
                {
                    verts.push_back(curr->tip->pos.x);
                    verts.push_back(curr->tip->pos.y);
                    verts.push_back(curr->tip->pos.z);
                    idx_map[curr->tip] = curr_idx++;
                }
                face[count++] = idx_map[curr->tip];
                if (!visited.count(curr->twin))
                {
                    stack.push(curr->twin);
                }
                curr = curr->next;
            } while (curr != he);
            for (auto f : face)
            {
                faces.push_back(f);
            }
        }

    }
        template <typename T>
    void obj_parser<T>::from_halfedges(halfedge::halfedge* e)
    {
        auto& verts = m_verts;
        auto& faces = m_indices;
        verts.clear();
        faces.clear();
        std::stack<halfedge::halfedge*> stack;
        std::unordered_set<halfedge::halfedge*> visited;
        std::unordered_map<halfedge::vertex*, int> idx_map;

        int curr_idx = 0;
        stack.push(e);
        int fc = 0;
        while (!stack.empty())
        {
            halfedge::halfedge* he = stack.top();
            stack.pop();

            if (visited.count(he))
                continue;

            halfedge::halfedge* curr  = he;
            int count = 0;
            i32 face[3] = {-1, -1, -1};
            do
            {
                int it = idx_map.count(curr->tip);
                visited.insert(curr);
                if (it == 0)
                {
                    verts.push_back(curr->tip->pos.x);
                    verts.push_back(curr->tip->pos.y);
                    verts.push_back(curr->tip->pos.z);
                    idx_map[curr->tip] = curr_idx++;
                }
                face[count++] = idx_map[curr->tip];
                if (!visited.count(curr->twin))
                {
                    stack.push(curr->twin);
                }
                curr = curr->next;
            } while (curr != he);
            for (auto f : face)
            {
                faces.push_back({f, -1, -1});
            }
        }
    }
        template <typename T>
    void obj_parser<T>::write_to_file(std::string&& file_path)
    {
        std::ofstream outfile;
        outfile.open(file_path);
        auto& verts = m_verts;
        auto& faces = m_indices;
        // Write vertices
        for (size_t i = 0; i < verts.size(); i+=3)
        {
            outfile << std::fixed << "v " << verts[i + 0] << " " << verts[i + 1] << " " << verts[i + 2] << '\n';
        }

        // Write faces
        for (size_t i = 0; i < faces.size(); i+=3)
        {
            outfile << "f " << (faces[0 + i].position+1); 
            if (faces[0 + i].normal != -1)
            {
                outfile << '/';
                if (faces[0 +i].texture != -1)
                {
                    outfile << (faces[0 + i].texture+1) << '/' << (faces[0 + i].normal+1);
                }
                else 
                {
                    outfile << '/' << (faces[0 + i].normal+1);
                }
            }
            else if (faces[0 + i].texture != -1)
            {
                    outfile << '/' << (faces[0 + i].texture+1);
            }
            outfile << " " << (faces[1 + i].position+1); 
            if (faces[1 + i].normal != -1)
            {
                outfile << '/';
                if (faces[1 +i].texture != -1)
                {
                    outfile << (faces[1 + i].texture+1) << '/' << (faces[1 + i].normal+1);
                }
                else 
                {
                    outfile << '/' << (faces[1 + i].normal+1);
                }
            }
            else if (faces[1 + i].texture != -1)
            {
                    outfile << '/' << (faces[1 + i].texture+1);
            }
            outfile << " " << (faces[2 + i].position+1); 
            if (faces[2 + i].normal != -1)
            {
                outfile << '/';
                if (faces[2 +i].texture != -1)
                {
                    outfile << (faces[2 + i].texture+1) << '/' << (faces[2 + i].normal+1);
                }
                else 
                {
                    outfile << '/' << (faces[2 + i].normal+1);
                }
            }
            else if (faces[2 + i].texture != -1)
            {
                    outfile << '/' << (faces[2 + i].texture+1);
            }
                outfile << '\n';
        }

        outfile.close();
    }
    template class obj_parser<f64>;
    template class obj_parser<f32>;
}}
