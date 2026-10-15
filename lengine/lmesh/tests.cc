#include "tests.h"
#include "halfedge.h"
#include "obj_parser.h"
#include <cassert>
#include <stack>
#include <vector>
#define eps (1e-7)
namespace lina { namespace mesh { namespace tests {
    void line_parse()
    {
        BEGIN_TEST("line parse");

        std::vector<std::string> strings;
        strings.push_back("v 0.01 -0.02232 20.5\n");
        strings.push_back("vn 0.01 -0.02232 20.5\n");
        strings.push_back("vn 0.01 -0.02232 20.5 \n");
        strings.push_back("   vn   0.01   -0.02232   20.5 \n");



        std::vector<f64> data;
        for (auto s : strings)
        {
            obj_parser<f64>::parse_line(data, s);
            assert(data.size() == 3);
            assert(data[0] - 0.01 < eps);
            assert(data[1] - -0.02232 < eps);
            assert(data[2] - 20.5 < eps);
            data.clear();
        }

        std::string s = "vt 0.01 -0.02232\n";

        obj_parser<f64>::parse_line(data, s);
        assert(data.size() == 2);
        assert(data[0] - 0.01 < eps);
        assert(data[1] - -0.02232 < eps);
        END_TEST("line parse");
    }
    void face_parse()
    {
        BEGIN_TEST("faces parse");

        std::vector<std::string> strings;
        strings.push_back("f 1 2 3");
        strings.push_back("f 1/2/3 2/3/4 3/4/3\n");
        strings.push_back("f 1//2 2//3 3//5");
        strings.push_back("f 1//2 2/6/3 3/8/5");



        obj_parser<f64> parser;
        {
            parser.parse_faces(strings[0]);
            auto data = parser.get_indices();
            assert(data.size() == 3);
            assert(data[0].position == 0);
            assert(data[0].texture == -1);
            assert(data[0].normal == -1);

            assert(data[1].position == 1);
            assert(data[1].texture == -1);
            assert(data[1].normal == -1);

            assert(data[2].position == 2);
            assert(data[2].texture == -1);
            assert(data[2].normal == -1);
            parser.clear_data();
        }

        {
            parser.parse_faces(strings[1]);
            auto data = parser.get_indices();
            assert(data.size() == 3);
            assert(data[0].position == 0);
            assert(data[0].texture == 1);
            assert(data[0].normal == 2);

            assert(data[1].position == 1);
            assert(data[1].texture == 2);
            assert(data[1].normal == 3);

            assert(data[2].position == 2);
            assert(data[2].texture == 3);
            assert(data[2].normal == 2);
            parser.clear_data();
        }
        {
            parser.parse_faces(strings[2]);
            auto data = parser.get_indices();
            assert(data.size() == 3);
            assert(data[0].position == 0);
            assert(data[0].texture == -1);
            assert(data[0].normal == 1);

            assert(data[1].position == 1);
            assert(data[1].texture == -1);
            assert(data[1].normal == 2);

            assert(data[2].position == 2);
            assert(data[2].texture == -1);
            assert(data[2].normal == 4);
            parser.clear_data();
        }

        {
            parser.parse_faces(strings[3]);
            auto data = parser.get_indices();
            assert(data.size() == 3);
            assert(data[0].position == 0);
            assert(data[0].texture == -1);
            assert(data[0].normal == 1);

            assert(data[1].position == 1);
            assert(data[1].texture == 5);
            assert(data[1].normal == 2);

            assert(data[2].position == 2);
            assert(data[2].texture == 7);
            assert(data[2].normal == 4);
            parser.clear_data();
        }

        END_TEST("faces parse");
    }
    void flip()
    {
        math::vector3d<f64> p1(0.0f, 0.0f, 0.0f);
        math::vector3d<f64> p2(-2.0f, 1.0f, 0.0f);
        math::vector3d<f64> p3(0.0f, 2.0f, 0.0f);
        math::vector3d<f64> p4(2.0f, 1.0f, 0.0f);

        std::vector<f64> verts{
            p1.x, p1.y, p1.z,
            p2.x, p2.y, p2.z,
            p3.x, p3.y, p3.z,
            p4.x, p4.y, p4.z,
        };
        std::vector<i32> faces{
            0, 1, 3, 
            1, 2, 3};
#if OUT_OBJ
        obj_parser<f64>::write_to_file(verts, faces, "./test_objs/before_flip.obj");
#endif
        halfedge::halfedge* h = 
            obj_parser<f64>::to_halfedges(verts, faces);

        int count = 0;
        global_validation(&h, &count, true);
        assert(count == 2);
        assert(h->next->twin != h->next);

        halfedge::halfedge* fe = h->next;
        halfedge::halfedge* fe_n = fe->next;
        halfedge::halfedge* fe_nn = fe->next->next;

        halfedge::halfedge* fet = fe->twin;
        halfedge::halfedge* fet_n = fet->next;
        halfedge::halfedge* fet_nn = fet->next->next;

        flip_edge(&fe);
        global_validation(&h, &count, true);
        assert(count == 2);

        assert(fe->next == fe_nn);
        assert(fe->next->next == fet_n);
        assert(fet->next == fet_nn);
        assert(fet->next->next == fe_n);
        assert(fet->f == fe_n->f);
        assert(fet->f != fe->f);

#if OUT_OBJ
        obj_parser<f64>::from_halfedges(fe, verts, faces);
        obj_parser<f64>::write_to_file(verts, faces, "./test_objs/after_flip.obj");
#endif
    }
    void split()
    {
        math::vector3d<f64> p1(0.0f, 0.0f, 0.0f);
        math::vector3d<f64> p2(-2.0f, 1.0f, 0.0f);
        math::vector3d<f64> p3(0.0f, 2.0f, 0.0f);
        math::vector3d<f64> p4(2.0f, 1.0f, 0.0f);

        std::vector<f64> verts{
            p1.x, p1.y, p1.z,
            p2.x, p2.y, p2.z,
            p3.x, p3.y, p3.z,
            p4.x, p4.y, p4.z,
        };
        std::vector<i32> faces{
            0, 1, 3, 
            1, 2, 3};
#if OUT_OBJ
        obj_parser<f64>::write_to_file(verts, faces, "./test_objs/before_split.obj");
#endif
        halfedge::halfedge* h = 
            obj_parser<f64>::to_halfedges(verts, faces);

        int count = 0;
        global_validation(&h, &count, true);
        assert(count == 2);
        assert(h->next->twin != h->next);

        halfedge::halfedge* fe = h->next;
        halfedge::halfedge* fe_n = fe->next;
        halfedge::halfedge* fe_nn = fe->next->next;

        halfedge::halfedge* fet = fe->twin;
        halfedge::halfedge* fet_n = fet->next;
        halfedge::halfedge* fet_nn = fet->next->next;

        split_edge(&fe);
        global_validation(&h, &count, true);
        assert(count == 4);

#if OUT_OBJ
        obj_parser<f64>::from_halfedges(h, verts, faces);
        obj_parser<f64>::write_to_file(verts, faces, "./test_objs/after_split.obj");
#endif
        assert(fe->next == fe_n);
        assert(fe->next->next != fet_nn);
        assert(fe->twin->next->next == fet_nn);
        assert(fe->twin->next != fet_n);
    }
    void collapse()
    {
        std::vector<i32> faces;
        math::vector3d<f64> p0(0.0f, 1.0f, 0.0f);
        math::vector3d<f64> p1(-1.0f, 0.0f, 0.0f);
        math::vector3d<f64> p2(1.0f, 0.0f, 0.0f);

        faces.push_back(1);
        faces.push_back(2);
        faces.push_back(0);

        math::vector3d<f64> p3(0.0f, -1.0f, 0.0f);
        faces.push_back(1);
        faces.push_back(3);
        faces.push_back(2);

        math::vector3d<f64> p4(-2.0f, 1.0f, 0.0f);
        faces.push_back(4);
        faces.push_back(1);
        faces.push_back(0);

        math::vector3d<f64> p5(-3.0f, 0.0f, 0.0f);
        faces.push_back(4);
        faces.push_back(5);
        faces.push_back(1);

        math::vector3d<f64> p6(-2.0f, -1.0f, 0.0f);
        faces.push_back(5);
        faces.push_back(6);
        faces.push_back(1);

        faces.push_back(1);
        faces.push_back(6);
        faces.push_back(3);

        math::vector3d<f64> p7(2.0f, -1.0f, 0.0f);
        faces.push_back(2);
        faces.push_back(3);
        faces.push_back(7);

        math::vector3d<f64> p8(3.0f, 0.0f, 0.0f);
        faces.push_back(2);
        faces.push_back(7);
        faces.push_back(8);

        math::vector3d<f64> p9(2.0f, 1.0f, 0.0f);
        faces.push_back(9);
        faces.push_back(2);
        faces.push_back(8);

        faces.push_back(0);
        faces.push_back(2);
        faces.push_back(9);

        std::vector<f64> verts{
            p0.x, p0.y, p0.z, 
                p1.x, p1.y, p1.z, 
                p2.x, p2.y, p2.z,
                p3.x, p3.y, p3.z,
                p4.x, p4.y, p4.z,
                p5.x, p5.y, p5.z,
                p6.x, p6.y, p6.z,
                p7.x, p7.y, p7.z,
                p8.x, p8.y, p8.z,
                p9.x, p9.y, p9.z
        };
#if OUT_OBJ
        obj_parser<f64>::write_to_file(verts, faces, "./test_objs/before_collapse.obj");
#endif
        halfedge::halfedge* h = obj_parser<f64>::to_halfedges(verts, faces);
        int count = 0;
        global_validation(&h, &count, true);
        assert(count == faces.size()/3);
        assert(h != h->twin);
        collapse_edge(&h);
        global_validation(&h, &count, true);
        assert(count = faces.size()/3 - 2);

        int old_size = verts.size()/3;
        obj_parser<f64>::from_halfedges(h, verts, faces);
        assert(verts.size()/3 == old_size - 1);

#if OUT_OBJ
        obj_parser<f64>::write_to_file(verts, faces, "./test_objs/after_collapse.obj");
#endif
    }
    void loop_subdivision()
    {
        std::vector<f64> verts;
        std::vector<i32> faces;
        obj_parser<f64> pars;
        pars.parse("./test_objs/icos.obj");
        verts = pars.get_positions();
        auto ff = pars.get_indices();
        faces.reserve(ff.size());
        for (auto idx : ff)
        {
            faces.push_back(idx.position);
        }
        auto* h = obj_parser<f64>::to_halfedges(verts, faces);
        global_validation(&h);
        halfedge::subdivide(h, 5);
        global_validation(&h);

        // out stuff
#if OUT_OBJ
        obj_parser<f64>::from_halfedges(h, verts, faces);
        obj_parser<f64>::write_to_file(verts, faces, "./test_objs/icos_divide_5.obj");
#endif
    }
    void quadratic_simplification()
    {
        std::vector<f64> verts;
        std::vector<i32> faces;
        obj_parser<f64> pars;
        pars.parse("./test_objs/sphere.obj");
        verts = pars.get_positions();
        auto ff = pars.get_indices();
        faces.reserve(ff.size());
        for (auto idx : ff)
        {
            faces.push_back(idx.position);
        }
        auto* h = obj_parser<f64>::to_halfedges(verts, faces);
        global_validation(&h);
        halfedge::simplify(&h, 5000);
        global_validation(&h);

        // out stuff
#if OUT_OBJ
        std::vector<f64> verts2;
        std::vector<i32> faces2;
        obj_parser<f64>::from_halfedges(h, verts, faces);
        obj_parser<f64>::write_to_file(verts, faces, "./test_objs/sphere_simplify_5000.obj");
#endif
    }
    void remesh()
    {
    }

    void global_validation(halfedge::halfedge** e, int* faces_count, bool allow_twin)
    {
        std::stack<halfedge::halfedge*> stack;
        std::unordered_set<halfedge::halfedge*> visited;

        stack.push(*e);
        int fc = 0;
        while (!stack.empty())
        {
            halfedge::halfedge* he = stack.top();
            stack.pop();

            if (visited.count(he))
                continue;

            halfedge::halfedge* curr  = he;
            int count = 0;
            fc++;
            do
            {
                assert(curr != nullptr);
                count++;
                visited.insert(curr);
                assert(curr->twin != nullptr);
                assert(curr->next != nullptr);
                assert(curr->f != nullptr);
                assert(curr->f->hedge != nullptr);
                assert(curr->tip->hedge != nullptr);
                assert(curr->tip != nullptr);
                assert(curr->twin != curr || allow_twin);
                assert(curr->tip == curr->twin->next->next->tip || (curr->twin == curr && allow_twin));
                assert(curr == curr->next->next->next);
                assert(curr->tip == curr->next->twin->tip || (curr->next->twin == curr->next && allow_twin));
                if (!visited.count(curr->twin))
                {
                    stack.push(curr->twin);
                }
                curr = curr->next;
            } while (curr != he);
            assert(count == 3);
        }
        if (faces_count != nullptr)
        {
            *faces_count = fc;
        }
    }
    void all()
    {
        line_parse();
        face_parse();

        flip();
        split();
        collapse();

        loop_subdivision();
        quadratic_simplification();
        remesh();
    }

}}}
