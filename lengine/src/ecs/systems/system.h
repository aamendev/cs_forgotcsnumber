#ifndef ECS_SYSTEM_H
#define ECS_SYSTEM_H
namespace lina { namespace ecs{ namespace systems{
    class System 
    {
        public:
        virtual void update() = 0;
        virtual void enable() = 0;
        virtual void disable() = 0;
    };
}}}

#endif

