#ifndef ECS_COLLIDER_H
#define ECS_COLLIDER_H
#include <functional>
#include <unordered_map>
#include <lmath/all.h>
#include <lmath/transformations.h>
#include <lmath/types.h>
#include "component.h"
namespace lina{ namespace ecs { namespace components {
    enum class CollisionStatus
    {
        None,
        Enter,
        Exit,
        Persist,
    };

    class collider : public component
    {
        public:
            typedef struct BVH
            {
                collider* root = nullptr;
                BVH* left = nullptr;
                BVH* right = nullptr;
            } BVH;
            virtual ~collider() = default;
        public:
            virtual bool check_collision(collider* c) = 0;
            virtual math::vector3d<f32> furthest_point(const math::vector3d<f32>& d) = 0;
            virtual void compute_BVH() = 0;
            virtual void compute_bounding_box() = 0;
            virtual void on_collision_enter(collider* c) {m_collision_enter_callback(this, c);}
            virtual void on_collision_exit(collider* c) {m_collision_exit_callback(this, c);}
            virtual void on_collision_persist(collider*c) {m_collision_persist_callback(this, c);}
            virtual void on_resolve(collider* c) {m_resolve_callback(this, c);}

            public:
            inline virtual void set_on_collision_enter(std::function<void (collider*, collider*)> f)
            {m_collision_enter_callback = f;};

            inline virtual void set_on_resolve(std::function<void(collider*, collider*)> f)
            {m_resolve_callback = f;}

            inline virtual void set_on_collision_exit(std::function<void(collider*, collider*)>f)
            {m_collision_exit_callback = f;}

            inline virtual void set_on_collision_persist(std::function<void(collider*, collider*)>f)
            {m_collision_persist_callback = f;}

            inline void set_position(const math::vector3d<f32>& new_pos) {
                m_transformed = ((m_center - new_pos).sq_norm() > 0.001f);
                m_center = new_pos;
            }
            inline void set_scale(const math::vector3d<f32>& s) {
                m_transformed = true;
                m_scale = s;
            }
            inline void set_scale(f32 s) {m_scale  = {s, s, s};}
            inline void scale(f32 s) {m_scale *= s;}
            inline virtual void set_status(
                    std::string s, CollisionStatus stat) {m_recent_collisions[s] = stat;}

            inline void set_rotation(math::vector3d<f32> a) {m_rotation = a;}
            inline virtual void remove_status(std::string s) {m_recent_collisions.erase(s);}

            inline virtual void set_call_defaults(bool c) {m_call_defaults = c;}
            inline void set_bounding_box_id(u32 id) {m_bounding_box_id = id;}
            inline void cohere() {m_transformed = false;}

        public:
            bool check_moved() const {return m_transformed;}
            const math::vector3d<f32>& get_position() const {return m_center;}
            const math::vector3d<f32> get_rotation() const {return m_rotation;}
            const math::vector3d<f32>& get_scale() const {return m_scale;}
            //collider_geometry get_collider_geometry() const {return m_geometry;}

            CollisionStatus get_status(std::string s) const {
                auto it = m_recent_collisions.find(s);
                if (it ==  m_recent_collisions.end())
                    return CollisionStatus::None;
            return it->second;}

            const std::string& get_tag() const {return m_tag;}
            bool is_call_default() const {return m_call_defaults;}
            inline virtual const std::pair<math::vector3d<f32>, math::vector3d<f32>>& 
                get_bounding_box() const {return m_bounding_box_extents;}
            BVH* get_BVH_pointer() {return &m_bvh;}
        protected:
        protected:
            std::string m_tag;
            //collider_geometry m_geometry;
            math::vector3d<f32> m_center;                
            math::vector3d<f32> m_scale;
            math::vector3d<f32> m_rotation;
            bool m_transformed;

            std::function<void (collider* c, collider* c2)> m_collision_enter_callback;
            std::function<void (collider* c, collider* c2)> m_resolve_callback;
            std::function<void (collider* c, collider* c2)> m_collision_exit_callback;
            std::function<void (collider* c, collider* c2)> m_collision_persist_callback;
            bool m_call_defaults;

            std::unordered_map<std::string, CollisionStatus> m_recent_collisions;
            u32 m_bounding_box_id = 0;
            std::pair<math::vector3d<f32>, math::vector3d<f32>> m_bounding_box_extents;
            BVH m_bvh;
    };
}}}
#endif

