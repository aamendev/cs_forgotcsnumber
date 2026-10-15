#ifndef ECS_COMPONENT_H
#define ECS_COMPONENT_H
#include <string>
namespace lina { namespace ecs { namespace components {
    enum class ComponentType
    {
        None,
        Transform,
        Material,
        AI,

        Sphere_collider,
        Cylinder_collider,
        Plane_collider,
        Mesh_collider,

        Health,
    };
    class component 
    {
        public:
            inline const std::string& get_tag() {return m_tag;}
            inline virtual ComponentType get_type() {return ComponentType::None;}
            inline virtual void enable() {m_enabled = true;}
            inline virtual void disable() {m_enabled = false;}
            inline virtual bool is_enabled() {return m_enabled;}
            inline virtual void set_tag(std::string&& s) {m_tag = s;}
        protected:
            std::string m_tag;
            bool m_enabled = true;

    };
}}}
#endif
