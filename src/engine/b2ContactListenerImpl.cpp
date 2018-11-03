#include <engine/b2Adapters.hpp>
#include <engine/ecs.hpp>

using namespace wee;
b2ContactListenerImpl::~b2ContactListenerImpl() {
}
void b2ContactListenerImpl::BeginContact(b2Contact* contact) 
{
    const b2Fixture* fA = contact->GetFixtureA();
    const b2Fixture* fB = contact->GetFixtureB();

    auto objA = (entity_t)fA->GetUserData(); 
    auto objB = (entity_t)fB->GetUserData();

    b2WorldManifold man;
    contact->GetWorldManifold(&man);
    if(kult::has<physics>(objA)) {
        if(fA->IsSensor()) {

            kult::get<physics>(objA).on_trigger_enter({
                objA, 
                objB, {
                    man.normal.x, 
                    man.normal.y
                }, {
                    man.points[0].x, 
                    man.points[0].y
                }
            });
        }
        kult::get<physics>(objA).on_collision_enter({
            objA, 
            objB, {
                man.normal.x, 
                man.normal.y
            }, {
                man.points[0].x, 
                man.points[0].y
            }
        });
    }

    if(kult::has<physics>(objB)) {
        if(fB->IsSensor()) {
            kult::get<physics>(objB).on_trigger_enter({objB, objA, {man.normal.x, man.normal.y}, {man.points[0].x, man.points[0].y}});
        }
        kult::get<physics>(objB).on_collision_enter({objB, objA, {man.normal.x, man.normal.y}, {man.points[0].x, man.points[0].y}});
    }


}
void b2ContactListenerImpl::EndContact(b2Contact* contact) 
{
    const b2Fixture* fA = contact->GetFixtureA();
    const b2Fixture* fB = contact->GetFixtureB();

    auto objA = (entity_t)fA->GetUserData(); 
    auto objB = (entity_t)fB->GetUserData();

    b2WorldManifold man;
    contact->GetWorldManifold(&man);
    if(kult::has<physics>(objA)) {
        kult::get<physics>(objA).on_collision_leave({objA, objB, {man.normal.x, man.normal.y}, {man.points[0].x, man.points[0].y}});
    }

    if(kult::has<physics>(objB)) {
        kult::get<physics>(objB).on_collision_leave({objB, objA, {man.normal.x, man.normal.y}, {man.points[0].x, man.points[0].y}});
    }
}

