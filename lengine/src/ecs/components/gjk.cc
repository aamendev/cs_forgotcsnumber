#include "gjk.h"
#include <cmath>
namespace lina { namespace ecs { namespace components{ namespace gjk {

    math::vector3d<f32> support(collider* a, collider* b, const math::vector3d<f32>& d)
    {
        auto fa = a->furthest_point(d);
        auto fb = b->furthest_point(d);
       return a->furthest_point(d) - b->furthest_point(-d);
    }

    bool gjk(collider* a, collider* b)
    {
        Simplex m_simplex;
        math::vector3d<f32> sup = support(a, b, {0, 1, 0});
        m_simplex.push_front(sup); 
        math::vector3d<f32> new_dir = -sup;

        while(true)
        {
            sup = support(a, b, new_dir);
            if (sup.dot(new_dir) <= 0) {m_simplex.clear(); return false;}
            m_simplex.push_front(sup);
            if (check_next_simplex(m_simplex, new_dir)) 
            {
                return true;
            };
        }
    }

    Info gjk_and_info(collider* a, collider* b)
    {
        Simplex m_simplex;
        math::vector3d<f32> sup = support(a, b, {0, 1, 0});
        m_simplex.push_front(sup); 
        math::vector3d<f32> new_dir = -sup;
        int iter = 200;

        while(iter != 0)
        {
            sup = support(a, b, new_dir);
            if (std::isnan(sup.x)
                    || std::isnan(sup.y) || std::isnan(sup.z))
                return {.collided = false};
            if (!sup.same_direction(new_dir)) {return {.collided = false};}
            m_simplex.push_front(sup);
            if (check_next_simplex(m_simplex, new_dir)) 
            {
                return  epa(m_simplex, a, b);
            };
            iter--;
        }
        return {.collided = false};
    }


    bool check_next_simplex(Simplex& m_simplex, math::vector3d<f32>& d)
    {
        switch(m_simplex.size)
        {
            case 2:{
        //std::cerr << "Inline\n";
                       return check_line(m_simplex, d);}
            case 3:{
         //              std::cerr << "In_tri\n";
                       return check_tri(m_simplex, d);}
            case 4: {

          //             std::cerr << "In_tetra\n";
                        return check_tetra(m_simplex, d);} break;
            default:
                    {
                        std::cerr << "Error, Simplex with bad size: " << m_simplex.size;
                        std::cerr << "\n";
                        return false;
                    }
                    break;
        }
        return false;
    }

    bool check_line(Simplex& m_simplex, math::vector3d<f32>& d)
    {
        math::vector3d<f32> ab = m_simplex.at(1) - m_simplex.at(0); 
        math::vector3d<f32> ao = -m_simplex.at(0);
        if (ab.same_direction(ao))
        {
            d = (ab.cross(ao)).cross(ab);
        }
        else
        {
            d = ao;
            m_simplex.set({m_simplex.at(0)});
        }
        return false;
    }

    bool check_tri(Simplex& m_simplex, math::vector3d<f32>& d)
    {
        math::vector3d<f32> ab = m_simplex.at(1) - m_simplex.at(0);
        math::vector3d<f32> ac = m_simplex.at(2) - m_simplex.at(0);
        math::vector3d<f32> ao = -m_simplex.at(0);
        
        math::vector3d<f32> abc = ab.cross(ac);

        if (ao.same_direction(abc.cross(ac)))
        {
            if (ao.same_direction(ac))
            {
                m_simplex.set({m_simplex.at(0), m_simplex.at(2)});
                d = ac.cross(ao).cross(ac);
            }
            else 
            {
                m_simplex.set({m_simplex.at(0), m_simplex.at(1)});
                return check_line(m_simplex, d);
            }
        }
        else 
        {
            if (ao.same_direction(ab.cross(abc))) 
            {

                m_simplex.set({m_simplex.at(0), m_simplex.at(1)});
                return check_line(m_simplex, d);
            }
            else if (ao.same_direction(abc))
            {
                d = abc;
            }
            else
            {
                m_simplex.set({m_simplex.at(0), m_simplex.at(2), m_simplex.at(1)});
                d = -abc;
            }
        }

        return false;
    }

    bool check_tetra(Simplex& m_simplex, math::vector3d<f32>& d)
    {
        math::vector3d<f32> ab = m_simplex.at(1) - m_simplex.at(0);
        math::vector3d<f32> ac = m_simplex.at(2) - m_simplex.at(0);
        math::vector3d<f32> ad = m_simplex.at(3) - m_simplex.at(0);
        math::vector3d<f32> ao =  -m_simplex.at(0);

        math::vector3d<f32> abc = ab.cross(ac);
        math::vector3d<f32> acd = ac.cross(ad);
        math::vector3d<f32> adb = ad.cross(ab);

        if (ao.same_direction(abc))
        {
            m_simplex.set({m_simplex.at(0), m_simplex.at(1), m_simplex.at(2)});
            return check_tri(m_simplex, d);
        }

        if (ao.same_direction(acd))
        {
            m_simplex.set({m_simplex.at(0), m_simplex.at(2), m_simplex.at(3)});
            return check_tri(m_simplex, d);
        }

        if (ao.same_direction(adb))
        {
            m_simplex.set({m_simplex.at(0), m_simplex.at(3), m_simplex.at(1)});
            return check_tri(m_simplex, d);
        }
            return true;
    }


    Info epa(Simplex& s, collider* a, collider* b)
    {
        std::vector<math::vector3d<f32>> verts(s.begin(), s.end());
        std::vector<u32> ind
        {
            0, 1, 2,
            0, 3, 1,
            0, 2, 3, 
            1, 3, 2
        };

        auto [normals, min_face] = normal_and_min(verts, ind);
        math::vector3d<f32> min_normal;
        f32 min_dist = FLT_MAX;
        f32 old_dist = 0.0f;
        u32 iterations = 100;
        int count = 0;
        while (iterations > 0)
        {
            min_normal = {normals[min_face].x, normals[min_face].y, normals[min_face].z};
            min_dist = normals[min_face].w;

            math::vector3d<f32> sup = support(a, b, min_normal);
            f32 dist = min_normal.dot(sup);

            if (fabs(dist - min_dist) > COLLISION_EPSILON)
            {
                //Statbility
                if (fabs(dist - old_dist) < COLLISION_EPSILON)
                {
                    iterations--;
                }
                else {
                    old_dist = dist;
                    iterations = 10;
                }
                // End Stavility

                std::vector<std::pair<u32, u32>> edges;
                for (u32 i = 0; i < normals.size(); i++)
                {
                    math::vector3d<f32> n3 = {normals[i].x, normals[i].y, normals[i].z};
                    u32 f = i * 3;
                    if (n3.same_direction(sup - verts[ind[f]]))
                    {
                        for (int j = 0; j < 3; j++)
                        {
                            add_unique_remove_dup(edges, ind, f + j, f + ((j + 1) % 3));
                        }
                        for (int j = 2; j > -1; j--)
                        {
                            ind[f + j] = ind.back(); ind.pop_back();
                        }
                        normals[i] = normals.back(); normals.pop_back();
                        i--;
                    }
                }
                std::vector<u32> new_ind;
                for (auto [v1, v2] : edges)
                {
                    new_ind.push_back(v1);
                    new_ind.push_back(v2);
                    new_ind.push_back(verts.size());
                }
                verts.push_back(sup);
                auto [new_normals, new_min_face] = normal_and_min(verts, new_ind);

                if (new_normals.empty()) break;

                f32 old_min = FLT_MAX;
                for (u32 i = 0; i < normals.size(); i++)
                {
                    if (normals[i].w < old_min)
                    {
                        old_min = normals[i].w;
                        min_face = i;
                    }
                }
                if (new_normals[new_min_face].w < old_min)
                {
                    min_face = new_min_face + normals.size();
                }
                ind.insert(ind.end(), new_ind.begin(), new_ind.end());
                normals.insert(normals.end(), new_normals.begin(), new_normals.end());
            }
            else
            {
                break;
            }
            count++;
            //std::cerr << "Still_in: " << count <<'\n';
        }
       // min_dist = min_dist * (iterations > 0) + old_dist * !(iterations > 0);
        Info inf {
            .depth = min_dist/* + COLLISION_EPSILON*/,
            .normal = min_normal,
            .collided = true,
        };
        return inf;
    }
    std::pair<std::vector<math::vector4d<f32>>, u32> normal_and_min(
            std::vector<math::vector3d<f32>>& verts,
            std::vector<u32>& indices)
    {
        std::vector<math::vector4d<f32>> normals;
        u32 min_face = 0;
        f32 min_dist = FLT_MAX;

        for (u32 i = 0; i < indices.size(); i+=3)
        {
            math::vector3d<f32> p0 =  verts[indices[i]];
            math::vector3d<f32> p1 =  verts[indices[i + 1]];
            math::vector3d<f32> p2 =  verts[indices[i + 2]];
            math::vector3d<f32> norm = ((p1 - p0).cross(p2 - p0)).normalized();
            f32 dist = norm.dot(p0);
            norm *= (-(dist < 0) + !(dist < 0));
            dist *= (-(dist < 0) + !(dist < 0));
            normals.emplace_back(norm.x, norm.y, norm.z, dist);

            min_face = i/3 * (dist < min_dist) + min_face * !(dist < min_dist);
            min_dist = dist * (dist < min_dist) + dist * !(dist < min_dist);
        }
        return {normals, min_face};
    }
    void add_unique_remove_dup(std::vector<std::pair<u32, u32>>& edges, 
            std::vector<u32>& ind,
            u32 a, u32 b)
    {
        auto e = std::find(edges.begin(), edges.end(), std::make_pair(ind[b], ind[a]));
        if (e != edges.end()) edges.erase(e);
        else edges.emplace_back(ind[a], ind[b]);
    }
}}}}

