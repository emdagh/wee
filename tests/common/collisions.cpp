#include "collisions.hpp"
#include "components.hpp"
#include <core/logstream.hpp>

collisions::~collisions() {
}
void collisions::BeginContact(b2Contact* contact) 
{
    const b2Fixture* fA = contact->GetFixtureA();
    const b2Fixture* fB = contact->GetFixtureB();

    auto objA = (kult::type)fA->GetUserData(); 
    auto objB = (kult::type)fB->GetUserData();

    b2WorldManifold man;
    contact->GetWorldManifold(&man);
    if(kult::has<collider>(objA)) {
        if(fA->IsSensor()) {
            kult::get<collider>(objA).on_trigger_enter(objA);
        }
        kult::get<collider>(objA).enter({objA, objB, {man.normal.x, man.normal.y}, {man.points[0].x, man.points[0].y}});
    }

    if(kult::has<collider>(objB)) {
        if(fB->IsSensor()) {
            kult::get<collider>(objB).on_trigger_enter(objB);
        }
        kult::get<collider>(objB).enter({objB, objA, {man.normal.x, man.normal.y}, {man.points[0].x, man.points[0].y}});
    }


}
void collisions::EndContact(b2Contact* contact) 
{
    const b2Fixture* fA = contact->GetFixtureA();
    const b2Fixture* fB = contact->GetFixtureB();

    auto objA = (kult::type)fA->GetUserData(); 
    auto objB = (kult::type)fB->GetUserData();

    b2WorldManifold man;
    contact->GetWorldManifold(&man);
    if(kult::has<collider>(objA)) {
        kult::get<collider>(objA).leave({objA, objB, {man.normal.x, man.normal.y}, {man.points[0].x, man.points[0].y}});
    }

    if(kult::has<collider>(objB)) {
        kult::get<collider>(objB).leave({objB, objA, {man.normal.x, man.normal.y}, {man.points[0].x, man.points[0].y}});
    }
}

