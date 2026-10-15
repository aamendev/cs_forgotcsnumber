#include "halfedge.h"
#include "lmath/lossy_conversions.h"
#include "lmath/mat3.h"
#include "lmath/vector2d.h"
#include "lmath/vector3d.h"
#include <algorithm>
#include <cfloat>
#include <cmath>
#include <iostream>
#include <iterator>
#include <set>
#include <stack>
#include <unordered_map>
#include <unordered_set>
#include <optional>
namespace lina { namespace mesh { namespace halfedge{

    void remesh(halfedge** e, f64 lambda, int iter)
    {
        for (int i = 0; i < iter; i++)
        {
            iso_remesh(e, lambda);
        }
    }
    void iso_remesh(halfedge **e, f64 lambda)
    {
        math::mat3<f64> id = math::mat3<f64>::get_identity();
        std::unordered_map<vertex*, f64> vertex_areas;
        std::unordered_map<vertex*, math::vector3d<f64>> vertex_normals;
        std::unordered_map<vertex*, math::vector3d<f64>> old_pos;
        std::unordered_set<halfedge*> newly_added;
        std::unordered_set<vertex*> newly_added_vertex;

        std::unordered_set<halfedge*> visited;
        std::unordered_set<vertex*> vertex_visited;

        std::stack<halfedge*> stack;
        std::stack<halfedge*> second_stack;
        std::set<halfedge*> to_process;
        stack.push(*e);

        f64 mean_length = 0.;
        int edge_count = 0;
        while (!stack.empty())
        {
            halfedge* he = stack.top();
            stack.pop();
            if (visited.count(he))
            {
                continue;
            }
            halfedge* curr = he;
            do 
            {
                visited.insert(curr);
                if (!visited.count(curr->twin))
                {
                    mean_length += (curr->tip->pos - curr->twin->tip->pos).norm();
                    edge_count++;
                    second_stack.push(curr);
                    stack.push(curr->twin);
                }
                curr = curr->next;
            }while (curr != he);
        }
        mean_length /= edge_count;
        visited.clear();
        halfedge* left_over = nullptr;
        while (!second_stack.empty())
        {
            halfedge* he = second_stack.top();
            second_stack.pop();
            f64 length = (he->tip->pos 
                    - he->twin->tip->pos).norm();
            if (length > 4./3. * mean_length)
            {
                split_edge(&he, newly_added, newly_added_vertex);
                visited.insert(he);
            }
            to_process.insert(he);
        }
        while (!to_process.empty())
        {
            auto* he = to_process.extract(to_process.begin()).value();
            if (visited.count(he) || newly_added.count(he) != 0)
            {
                continue;
            }
            f64 length = (he->tip->pos 
                    - he->twin->tip->pos).norm();
            if (length < 4./5. * mean_length)
            {
                halfedge* del[4] = 
                {
                    he->next->next,
                    he->next->next->twin,
                    he->twin->next,
                    he->twin->next->twin,
                };

                if(collapse_edge(&he))
                {
                    for (auto& d : del)
                    {
                        auto it = std::find_if(to_process.begin(), to_process.end(), 
                                [&](const auto& e)
                                {
                                return e == d;
                                });
                        if (it != to_process.end())
                        {
                            to_process.erase(it);
                        }
                    }
                }
            }

            left_over = he;
        }

        stack.push(left_over);
        visited.clear();
        while (!stack.empty())
        {
            halfedge* he = stack.top();
            stack.pop();
            if (visited.count(he))
            {
                continue;
            }
            halfedge* curr = he;
            do 
            {
                visited.insert(curr);
                if (!visited.count(curr->twin))
                {
                    to_process.insert(curr);
                    stack.push(curr->twin);
                }
                curr = curr->next;
            }while (curr != he);
        }
        visited.clear();
        while (!to_process.empty())
        {
            halfedge* he = to_process.extract(to_process.begin()).value();
            int n1 = get_degree(he->tip);
            int n2 = get_degree(he->twin->tip);
            int n3 = get_degree(he->next->tip);
            int n4 = get_degree(he->twin->next->tip);
            f64 var1 = 
                (n1 - 6)* (n1-6) + (n2 - 6) * (n2 - 6) + 
                (n3 - 6)* (n3-6) + (n4 - 6) * (n4 - 6);
            f64 var2 = 
                (n1 - 7) * (n1 - 7) + (n2 - 7)* (n2-7) + 
                (n3 - 5) * (n3 - 5) + (n4 - 5) * (n4 - 5);

            if (var1 > var2)
            {
                flip_edge(&he);
            }
            stack.push(he);
        }

        while (!stack.empty())
        {
            auto* he = stack.top();
            stack.pop();
            if (!vertex_areas.count(he->tip))
            {
                vertex_areas[he->tip] = get_area(he->tip);
                vertex_areas[he->twin->tip] = get_area(he->twin->tip);

                vertex_normals[he->tip] = get_normal(he->tip);
                vertex_normals[he->twin->tip] = get_normal(he->twin->tip);

                old_pos[he->tip] = he->tip->pos;
                old_pos[he->twin->tip] = he->twin->tip->pos;
            }
        }
        halfedge* curr;
        for (auto& [v, area] : vertex_areas)
        {
            auto nei = get_neighbours(v);
            math::vector3d<f64> centroid(0, 0, 0);
            f64 normalization_factor = 0.;
            for (auto* n : nei)
            {
                normalization_factor += vertex_areas[n]; 
                centroid += old_pos[n] * vertex_areas[n];
            }
            centroid = centroid * (1./normalization_factor);
            math::vector3d<f64> dir = centroid-v->pos;
            v->pos +=  vertex_normals[v] * ( 
                ((dir) - dir.dot(vertex_normals[v])) * lambda);
            curr = v->hedge;
        }
        stack.push(curr);
        visited.clear();
        *e = curr;
    }
    void simplify(halfedge** e, int iter)
    {
        quadratic_simplification(e, iter);
    }
    void subdivide(halfedge* e, int iter)
    {
        for (int i = 0; i < iter; i++)
        {
            loop_subdivision(&e);
        }
    }
    bool loop_subdivision(halfedge** e)
    {
        std::stack<halfedge*> to_process;
        std::unordered_set<halfedge*> visited;
        std::unordered_set<vertex*> visited_vertex;
        std::unordered_set<halfedge*> newly_added;
        std::unordered_set<vertex*> newly_added_vertex;
        std::unordered_map<vertex*, math::vector3d<f64>> new_pos;
        using vertexpair = std::pair<vertex*, vertex*>;
        std::unordered_map<halfedge*, std::pair<vertexpair, vertexpair>> old_surrounding;

        std::stack<halfedge*> stack;
        stack.push(*e);
        while (!stack.empty())
        {
            halfedge* he = stack.top();
            stack.pop();

            if (visited.count(he))
                continue;

            halfedge* curr = he;
            do 
            {
                visited.insert(curr);
                if (!visited_vertex.count(curr->tip))
                {
                    visited_vertex.insert(curr->tip);
                }
                if (!visited.count(curr->twin))
                {
                    old_surrounding[curr].first = {curr->tip, curr->twin->tip};
                    old_surrounding[curr].second = {curr->next->tip, curr->twin->next->tip};
                    old_surrounding[curr->twin].first = {curr->twin->tip, curr->tip};
                    old_surrounding[curr->twin].second = {curr->twin->next->tip, curr->next->tip};

                    to_process.push(curr);
                    stack.push(curr->twin);
                }
                curr = curr->next;
            }while (curr != he);
        }
        for (auto* ve : visited_vertex)
        {
            auto neighbours = get_neighbours(ve);

            f64 n = neighbours.size();
            f64 u = 3./16. * (n == 3) + 
                1./n * (5./8. - std::pow(3./8. + 1./4. * std::cos(2. * M_PI / n), 2)) * (n != 3);
            math::vector3d<f64> wsum(0, 0, 0);
            for (auto* v : neighbours)
            {
                wsum += v->pos;
            }
            new_pos[ve] =  ve->pos * (1. - n * u)  + wsum * u;
        }
        visited.clear();
        while (!to_process.empty())
        {
            halfedge* he = to_process.top();
            to_process.pop();

            if (visited.count(he) || newly_added.count(he) != 0)
                continue;
            math::vector3d<f64> new_pos = 
                (old_surrounding[he].first.first->pos + old_surrounding[he].first.second->pos) * 3./8.+
                (old_surrounding[he].second.first->pos + old_surrounding[he].second.second->pos) * 1./8.;
            split_edge(&he, 
                    newly_added, newly_added_vertex, 
                    {3./8., 3./8., 1./8., 1./8.});
            he->twin->tip->pos = new_pos;
            visited.insert(he);
        }
        visited.clear();
        stack.push(*e);
        while (!stack.empty())
        {
            halfedge* he = stack.top();
            stack.pop();

            if (visited.count(he))
                continue;

            halfedge* curr = he;
            do 
            {
                visited.insert(curr);
                to_process.push(curr);
                if (!visited.count(curr->twin))
                {
                    stack.push(curr->twin);
                }
                curr = curr->next;
            }while (curr != he);
        }
        visited.clear();
        while (!to_process.empty())
        {
            halfedge* he = to_process.top();
            to_process.pop();

            if (!visited.count(he) && 
                    newly_added.count(he) != 0 && 
                    newly_added_vertex.count(he->tip) != 0 && 
                    newly_added_vertex.count(he->twin->tip) == 0)
            {
                flip_edge(&he);
                visited.insert(he);
                visited.insert(he->twin);
            }
        }
        for (auto& [key, value] : new_pos)
        {
            key->pos = value;
        }
        return true;
    }


    f64 compute_cost(
            std::unordered_map<vertex*, math::mat4<f64>>& error_matrices, halfedge** curr_ptr,
            std::unordered_map<halfedge*, std::pair<f64, f64>>& edge_weights
            )
    {

        halfedge* curr = *curr_ptr;
        math::mat4<f64> combined = error_matrices[curr->tip] + error_matrices[curr->twin->tip];
        f64 det = math::conversions::drop(std::move(combined)).det();
        std::array<f64, 2> weights = {0.5, 0.5};
        math::vector4d<f64> err_vec;
        f64 err = 0.;
        if (fabs(det) >= 1e-4)
        {
            math::vector3d<f64> new_vec = curr->tip->pos *0.5 + curr->twin->tip->pos * 0.5;

            err_vec = math::conversions::extend(std::move(curr->tip->pos), 1.);
            f64 err1 = (err_vec * combined).dot(err_vec);
            err_vec = math::conversions::extend(std::move(new_vec), 1.);
            f64 err2 = (err_vec * combined).dot(err_vec);
            err_vec = math::conversions::extend(std::move(curr->twin->tip->pos), 1.);
            f64 err3 = (err_vec * combined).dot(err_vec);

            std::array<std::pair<f64, f64>, 3> cands;
            cands[0] = {1., 0.};
            cands[1] = {0.5, 0.5};
            cands[2] = {0., 1.};
            std::array<f64, 3> cand_errs {err1, err2, err3};

            f64 min_val = FLT_MAX;

            for (int i = 0; i < cand_errs.size(); i++)
            {
                if (cand_errs[i] < min_val)
                {
                    min_val = cand_errs[i];
                    weights[0] = cands[i].first;
                    weights[1] = cands[i].second;
                }
            }
            new_vec = curr->tip->pos * weights[0] + curr->twin->tip->pos * weights[1];
            err = min_val;
            err_vec = math::conversions::extend(std::move(new_vec), 1.);
        }
        else
        {
            math::mat4<f64> opt = combined;
            opt.set_col(3, math::vector4d<f64>(0, 0, 0, 1));
            opt = opt.get_inverse();
            err_vec = opt * math::vector4d<f64>(0, 0, 0, 1);

            math::vector3d<f64> b = math::conversions::drop_last(std::move(err_vec));

            math::mat2<f64> Ata(
                    math::vector2d<f64>(curr->tip->pos.dot(curr->tip->pos), curr->tip->pos.dot(curr->twin->tip->pos)),
                    math::vector2d<f64>(curr->twin->tip->pos.dot(curr->tip->pos), curr->twin->tip->pos.dot(curr->twin->tip->pos))
                    );
            math::vector2d<f64> Atb(
                curr->tip->pos.dot(b),
                curr->twin->tip->pos.dot(b)
                    );
            math::vector2d<f64> coeffs = Ata.get_inverse() * Atb;

            weights[0] = coeffs.x;
            weights[1] = coeffs.y;
            math::vector3d<f64> new_vec = curr->tip->pos * weights[0] + curr->twin->tip->pos * weights[1];
            err_vec = math::conversions::extend(std::move(new_vec), 1.);
            err = (err_vec * combined).dot(err_vec);
        }
        edge_weights[curr] = {weights[0], weights[1]};
        edge_weights[curr->twin] = {weights[1], weights[0]};
        return err;
    }

    bool quadratic_simplification(halfedge** e, int iter)
    {
        std::stack<halfedge*> stack;
        std::unordered_set<halfedge*> visited;
        std::unordered_map<vertex*, math::mat4<f64>> error_matrices;
        std::unordered_map<halfedge*, std::pair<f64, f64>> edge_weights;
        struct pair_less
        {
            bool operator()(const std::pair<f64, halfedge*>& v1, const std::pair<f64, halfedge*>& v2) const
            {
                return v1.first < v2.first;
            }
        };
        std::multiset<std::pair<f64, halfedge*>, pair_less> vertex_container;
        stack.push(*e);
        while (!stack.empty())
        {
            halfedge* he = stack.top();
            stack.pop();

            if (visited.count(he))
                continue;

            halfedge* curr = he;
            do 
            {
                visited.insert(curr);
                if (!edge_weights.count(curr))
                {
                    if (!error_matrices.count(curr->tip))
                    {
                        error_matrices[curr->tip] = get_initial_plane_errors(curr->tip);
                    }
                    if (!error_matrices.count(curr->twin->tip))
                    {
                        error_matrices[curr->twin->tip] = get_initial_plane_errors(curr->twin->tip);
                    }
                    f64 err = compute_cost(error_matrices, &curr, edge_weights); 
                    vertex_container.insert({err, curr});
                }
                if (!visited.count(curr->twin))
                {
                    stack.push(curr->twin);
                }
                curr = curr->next;
            }while (curr != he);
        }
        visited.clear();
        int i = 0;
        while (!vertex_container.empty() && i < iter)
        {
            auto curr = vertex_container.extract(vertex_container.begin()).value();
            halfedge* he = curr.second;

            math::mat4<f64> err_mat = error_matrices[he->tip] 
                + error_matrices[he->twin->tip];
            auto* del1 = he->twin->next;
            auto* del1t = he->twin->next->twin;
            auto* del2 = he->next->next;
            auto* del2t = he->next->next->twin;
            halfedge* dels[4] = {del1, del2, del1t, del2t};
            if (collapse_edge(&he,
                        {edge_weights[he].first, edge_weights[he].second}))
            {
                i += 2;
                for (auto& del : dels)
                {
                    auto it = std::find_if(vertex_container.begin(), vertex_container.end(), 
                            [&](const std::pair<f64, halfedge*>& e){
                            return e.second == del;
                            });

                    if (it != vertex_container.end())
                    {
                        vertex_container.erase(it);
                    }
                }
                error_matrices[he->tip] = err_mat;
                auto neighbours = get_half_edges(he->tip);
                for (auto& n : neighbours)
                {
                    f64 err = compute_cost(error_matrices, &n, edge_weights);
                    auto it = std::find_if(vertex_container.begin(), vertex_container.end(), 
                            [&](const std::pair<f64, halfedge*>& e){
                            return e.second == n || e.second == n->twin;
                            });
                    if (it != vertex_container.end())
                    {
                        vertex_container.erase(it);
                        vertex_container.insert({err, n});
                    }
                }
            }
            *e = he;
        }
        return true;
    }

    bool flip_edge(halfedge** h0_ptr)
    {
        halfedge* h0 = *h0_ptr;
        if (get_degree(h0->tip) == 3 || get_degree(h0->twin->tip) == 3)
        {
            return false;
        }
        halfedge* h1 = h0->next;
        halfedge* h2 = h1->next;

        halfedge* ht0 = h0->twin;
        halfedge* ht1 = ht0->next;
        halfedge* ht2 = ht1->next;


        vertex* vh = h0->tip;
        vertex* vt = ht0->tip;

        vertex* vh1 = h1->tip;
        vertex* vt1 = ht1->tip;

        vh1->hedge = h1;
        vt1->hedge = ht1;

        h0->tip = vh1;
        h0->next = h2;
        h2->next = ht1;
        ht1->next = h0;
        ht1->f = h0->f;
        h0->f->hedge = h0;

        ht0->tip = vt1;
        ht0->next = ht2;
        ht2->next = h1;
        h1->next = ht0;
        h1->f = ht0->f;
        ht0->f->hedge = ht0;

        vh->hedge = ht2;
        vt->hedge = h2;

        return true;
    }
    bool split_edge(halfedge** h0_ptr,  std::array<f64, 4> arr)
    {
        halfedge* h0 = *h0_ptr;
        halfedge* h1 = h0->next;
        halfedge* h2 = h1->next;

        halfedge* ht0 = h0->twin;
        halfedge* ht1 = ht0->next;
        halfedge* ht2 = ht1->next;

        vertex* vh = h0->tip;
        vertex* vt = ht0->tip;
        vertex* vh1 = h1->tip;
        vertex* vt1 = ht1->tip;

        delete h0->f;
        h0->f = nullptr;
        delete ht0->f;
        ht0->f = nullptr;

        vertex* vm = new vertex;
        vm->pos = vh->pos * arr[0] + vt->pos * arr[1] + vh1->pos * arr[2] + vt1->pos * arr[3];

        halfedge* mid_vh = h0;
        halfedge* vh_mid = ht0;
        halfedge* mid_vt = new halfedge;
        halfedge* vt_mid = new halfedge;

        halfedge* mid_vh1 = new halfedge;
        halfedge* vh1_mid = new halfedge;
        halfedge* mid_vt1 = new halfedge;
        halfedge* vt1_mid = new halfedge;

        face* f0 = new face;
        face* f1 = new face;
        face* f2 = new face;
        face* f3 = new face;

        // face 1
        vh1_mid->next = mid_vh;
        mid_vh->next = h1;
        h1->next = vh1_mid;

        vh1_mid->tip = vm;
        vh1_mid->tip->hedge = vh1_mid;

        mid_vh->tip = vh;
        mid_vh->tip->hedge = mid_vh;


        f0->hedge = vh1_mid;
        vh1_mid->f = f0;
        mid_vh->f = f0;
        h1->f = f0;

        // face 2
        mid_vt1->next = ht2;
        ht2->next = vh_mid;
        vh_mid->next = mid_vt1;

        vh_mid->tip = vm;
        vh_mid->tip->hedge = vh_mid;

        mid_vt1->tip = vt1;
        mid_vt1->tip->hedge = mid_vt1;


        mid_vt1->f = f1;
        ht2->f = f1;
        vh_mid->f = f1;
        f1->hedge = mid_vt1;

        // face 3
        mid_vh1->next = h2;
        h2->next = vt_mid;
        vt_mid->next = mid_vh1;

        vt_mid->tip = vm;
        vm->hedge = vt_mid;

        mid_vh1->tip = vh1;
        vh1->hedge = mid_vh1;


        f2->hedge = mid_vh1;
        mid_vh1->f = f2;
        vt_mid->f = f2;
        h2->f = f2;

        // face 4
        vt1_mid->next = mid_vt;
        mid_vt->next = ht1;
        ht1->next = vt1_mid;

        vt1_mid->tip = vm;
        vm->hedge = vt1_mid;

        mid_vt->tip = vt;
        vt->hedge = mid_vt;

        vt1_mid->f = f3;
        mid_vt->f = f3;
        ht1->f = f3;
        f3->hedge = vt1_mid;

        vh1_mid->twin = mid_vh1;
        mid_vh1->twin = vh1_mid;

        vt1_mid->twin = mid_vt1;
        mid_vt1->twin = vt1_mid;

        vt_mid->twin = mid_vt;
        mid_vt->twin = vt_mid;

        mid_vh->twin = vh_mid;
        vh_mid->twin = mid_vh;

        vm->hedge = vh_mid;
        *h0_ptr = mid_vh;


        return true;
    }
    bool split_edge(halfedge** h0_ptr,  std::unordered_set<halfedge*>& newly_added, std::unordered_set<vertex*>& newly_added_vertex, std::array<f64, 4> arr)
    {
        halfedge* h0 = *h0_ptr;
        halfedge* h1 = h0->next;
        halfedge* h2 = h1->next;

        halfedge* ht0 = h0->twin;
        halfedge* ht1 = ht0->next;
        halfedge* ht2 = ht1->next;

        vertex* vh = h0->tip;
        vertex* vt = ht0->tip;
        vertex* vh1 = h1->tip;
        vertex* vt1 = ht1->tip;

        delete h0->f;
        h0->f = nullptr;
        delete ht0->f;
        ht0->f = nullptr;

        vertex* vm = new vertex;
        newly_added_vertex.insert(vm);
        vm->pos = vh->pos * arr[0] + vt->pos * arr[1] + vh1->pos * arr[2] + vt1->pos * arr[3];

        halfedge* mid_vh = h0;
        halfedge* vh_mid = ht0;
        halfedge* mid_vt = new halfedge;
        halfedge* vt_mid = new halfedge;

        halfedge* mid_vh1 = new halfedge;
        halfedge* vh1_mid = new halfedge;
        halfedge* mid_vt1 = new halfedge;
        halfedge* vt1_mid = new halfedge;

        face* f0 = new face;
        face* f1 = new face;
        face* f2 = new face;
        face* f3 = new face;

        // face 1
        vh1_mid->next = mid_vh;
        mid_vh->next = h1;
        h1->next = vh1_mid;

        vh1_mid->tip = vm;
        vh1_mid->tip->hedge = vh1_mid;

        mid_vh->tip = vh;
        mid_vh->tip->hedge = mid_vh;


        f0->hedge = vh1_mid;
        vh1_mid->f = f0;
        mid_vh->f = f0;
        h1->f = f0;

        // face 2
        mid_vt1->next = ht2;
        ht2->next = vh_mid;
        vh_mid->next = mid_vt1;

        vh_mid->tip = vm;
        vh_mid->tip->hedge = vh_mid;

        mid_vt1->tip = vt1;
        mid_vt1->tip->hedge = mid_vt1;


        mid_vt1->f = f1;
        ht2->f = f1;
        vh_mid->f = f1;
        f1->hedge = mid_vt1;

        // face 3
        mid_vh1->next = h2;
        h2->next = vt_mid;
        vt_mid->next = mid_vh1;

        vt_mid->tip = vm;
        vm->hedge = vt_mid;

        mid_vh1->tip = vh1;
        vh1->hedge = mid_vh1;


        f2->hedge = mid_vh1;
        mid_vh1->f = f2;
        vt_mid->f = f2;
        h2->f = f2;

        // face 4
        vt1_mid->next = mid_vt;
        mid_vt->next = ht1;
        ht1->next = vt1_mid;

        vt1_mid->tip = vm;
        vm->hedge = vt1_mid;

        mid_vt->tip = vt;
        vt->hedge = mid_vt;

        vt1_mid->f = f3;
        mid_vt->f = f3;
        ht1->f = f3;
        f3->hedge = vt1_mid;

        vh1_mid->twin = mid_vh1;
        mid_vh1->twin = vh1_mid;
        newly_added.insert(vh1_mid);
        newly_added.insert(mid_vh1);

        vt1_mid->twin = mid_vt1;
        mid_vt1->twin = vt1_mid;
        newly_added.insert(vt1_mid);
        newly_added.insert(mid_vt1);

        vt_mid->twin = mid_vt;
        mid_vt->twin = vt_mid;

        mid_vh->twin = vh_mid;
        vh_mid->twin = mid_vh;

        vm->hedge = vh_mid;
        *h0_ptr = mid_vh;


        return true;
    }

    bool collapse_edge(halfedge** he_ptr, std::array<f64, 2> arr)
    {
        halfedge* he = *he_ptr;
        halfedge* twin = he->twin;
        if (!shared_neighbour_condition(he->tip, twin->tip))
        {
            return false;
        }
        vertex* v = he->tip;
        vertex* v2 = twin->tip;
        auto halfedges = get_half_edges(v2);

        update_helf_edges_vertex(v2, v);

        v->pos = v2->pos * arr[1] + v->pos * arr[0];

        halfedge* hen = he->next;
        halfedge* hen2 = he->next->next;

        halfedge* new_next = hen2->twin->next;
        hen->next = new_next;
        new_next->next->next = hen;
        hen->tip->hedge = hen;
        hen->f = hen->next->f;
        hen->f->hedge = hen;


        halfedge* htn = twin->next->next;
        halfedge* htn2 = twin->next;
        htn2->tip->hedge = htn->twin;

        halfedge* newt_next = htn2->twin->next;
        htn->next = newt_next;
        newt_next->next->next = htn;
        htn->f = htn->next->f;
        htn->f->hedge = htn;


        v->hedge = hen->twin;

        delete hen2->twin;
        delete hen2;

        delete he->f;
        delete he;

        delete htn2->twin;
        delete htn2;

        delete twin->f;
        delete twin;

        delete v2;
        *he_ptr = hen->twin;
        return true;
    }
    std::set<vertex*> get_neighbours(vertex* v)
    {
        std::set<vertex*> s;
        halfedge* vhe = v->hedge;
        do 
        {
            vhe = vhe->next; // its tip is a neighbour of current v;
            s.insert(vhe->tip);
            vhe = vhe->twin;
        }
        while (vhe != v->hedge);
        return s;
    }

    std::vector<vertex*> get_neighbours_vector(vertex* v)
    {
        std::unordered_set<vertex*> visited;
        std::vector<vertex*> s;
        s.reserve(10);
        halfedge* vhe = v->hedge;
        do 
        {
            vhe = vhe->next; // its tip is a neighbour of current v;
            if (!visited.count(vhe->tip))
            {
                s.push_back(vhe->tip);
                visited.insert(vhe->tip);
            }
            vhe = vhe->twin;
        }
        while (vhe != v->hedge);
        return s;
    }
    int get_degree(vertex* v)
    {
        int counter = 0;
        halfedge* vhe = v->hedge;
        do 
        {
            vhe = vhe->next->twin; // its tip is a neighbour of current v;
            counter++;
        }
        while (vhe != v->hedge);
        return counter;
    }
    int highest_neighbour_degree(vertex* v)
    {
        int max_deg = 0;
        halfedge* vhe = v->hedge;
        do 
        {
            vhe = vhe->next; // its tip is a neighbour of current v;
            max_deg = std::max(max_deg, get_degree(vhe->tip));
            vhe = vhe->twin;
        }
        while (vhe != v->hedge);
        return max_deg;
    }

    bool shared_neighbour_condition(vertex* v1, vertex* v2)
    {
        auto s1 = get_neighbours(v1);
        auto s2 = get_neighbours(v2);
        std::set<vertex*>inter;
        std::set_intersection(
                s1.begin(), s1.end(), 
                s2.begin(), s2.end(),
                std::inserter(inter, inter.end()));
        if (inter.size() > 2) return false;
        for (auto* v : inter)
        {
            if (get_degree(v) <= 3) return false;
        }
        return true;
    }

    std::vector<halfedge*> get_half_edges(vertex* v)
    {
        std::vector<halfedge*> hes;
        halfedge* vhe = v->hedge;
        do 
        {
            hes.push_back(vhe);
            vhe = vhe->next->twin; // its tip is v
        }
        while (vhe != v->hedge);
        return hes;
    }
    math::mat4<f64> get_initial_plane_errors(vertex* v)
    {
        math::mat4<f64> ret = math::mat4<f64>::get_zero();
        std::vector<halfedge*> hes;
        halfedge* vhe = v->hedge;
        do 
        {
            math::vector3d<f64> normal;
            normal = (vhe->tip->pos - vhe->next->tip->pos)
                .cross(vhe->tip->pos - vhe->next->next->tip->pos);
            normal.normalize();
            f64 d = -normal.dot(v->pos);
            math::mat4<f64> local;
            auto v2 = math::vector4d<f64>(normal.x, normal.y, normal.z, d);
            local.set_row(0, v2 * normal.x);
            local.set_row(1, v2 * normal.y);
            local.set_row(2, v2 * normal.z);
            local.set_row(3, v2 * d);
            ret = ret + local;
            vhe = vhe->next->twin; // its tip is v
        }
        while (vhe != v->hedge);
        return ret;
    }
    void update_helf_edges_vertex(vertex* vold, vertex* vnew)
    {
        halfedge* vhe = vold->hedge;
        do 
        {
            vhe->tip = vnew;
            vhe = vhe->next->twin; // its tip is v;
        }
        while (vhe != vold->hedge);
    }
    math::vector3d<f64> get_normal(vertex* v)
    {
        math::vector3d<f64> normal;
        halfedge* vhe = v->hedge;
        do 
        {
            math::vector3d<f64> local_normal;
            local_normal = (vhe->tip->pos - vhe->next->tip->pos)
                .cross(vhe->tip->pos - vhe->next->next->tip->pos);
            normal += local_normal * 0.5;
            vhe = vhe->next->twin; // its tip is v
        }
        while (vhe != v->hedge);
        normal.normalize();
        return normal;
    }
    f64 get_area(vertex* v)
    {
        auto iter = get_neighbours_vector(v);
        int n = iter.size();
        f64 area = 0.;
        for (int i = 0; i < n; i++)
        {
            math::vector3d<f64> va = v->pos - iter[i]->pos;
            math::vector3d<f64> van = va.normalized(); 

            math::vector3d<f64> ab = iter[i]->pos - iter[(i+1) % n]->pos;
            math::vector3d<f64> abn = ab.normalized(); 

            math::vector3d<f64> vb = iter[(i+1) % n]->pos - v->pos;
            math::vector3d<f64> vbn = vb.normalized(); 


            if (-(van.dot(vbn)) < 0)
            {
                area+= 0.5 *  0.5 * va.cross(vb).norm();
            }
            else if (-(van.dot(abn)) < 0 || -(abn.dot(vbn)) < 0)
            {
                area+= 0.25 *  0.5 * va.cross(vb).norm();
            }
            else 
            {

                f64 calpha = -van.dot(abn);
                f64 cbeta = -vbn.dot(abn);

                //assert(!std::isnan(calpha));
                //assert(!std::isnan(cbeta));
                if (fabs(calpha -1) < 1e-4)[[unlikely]]
                {
                    calpha = 0;
                }
                else 
                {
                    calpha = calpha / (sqrt(1. - calpha * calpha)); // cot formula
                }

                if (fabs(cbeta -1) < 1e-4)[[unlikely]]
                {
                    cbeta = 0;
                }
                else 
                {
                    cbeta = cbeta / (sqrt(1. - cbeta * cbeta)); // cot formula
                }
                area += 1./8. * (cbeta * va.sq_norm() +calpha * vb.sq_norm());
            }
        }
        return area;
    }

}}}
