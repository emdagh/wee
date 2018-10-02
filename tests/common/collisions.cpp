#include "collisions.hpp"
#include "components.hpp"
#include <util/logstream.hpp>

collisions::~collisions() {
}
void collisions::BeginContact(b2Contact* contact) 
{
    DEBUG_METHOD();
    const b2Fixture* fA = contact->GetFixtureA();
    const b2Fixture* fB = contact->GetFixtureB();

    auto objA = (kult::type)fA->GetUserData(); 
    auto objB = (kult::type)fB->GetUserData();

    b2WorldManifold man;
    contact->GetWorldManifold(&man);
    if(kult::has<collider>(objA)) {
        kult::get<collider>(objA).enter({objA, objB, {man.normal.x, man.normal.y}, {man.points[0].x, man.points[0].y}});
    }

    if(kult::has<collider>(objB)) {
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

