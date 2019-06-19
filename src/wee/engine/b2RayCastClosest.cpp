#include <engine/b2RayCastImpl.hpp>
#include <engine/ecs.hpp>

using namespace wee;

void b2RayCastClosest::RayCast(b2World* world, const b2Vec2& p1, const b2Vec2& p2) {
    _fixture = NULL;
    _fraction = 1.0f;
    world->RayCast(this, p1, p2);
    if(_fixture) {
        kult::type self = reinterpret_cast<entity_t>(_fixture->GetUserData());
        raycast_t& r = kult::get<raycast>(self);
        r.hit   = true;
        r.point = {_point.x, _point.y};
        r.n     = {_normal.x, _normal.y};
    }

}
float32 b2RayCastClosest::ReportFixture(b2Fixture* fixture, 
        const b2Vec2& point, 
        const b2Vec2& normal, 
        float32 fraction) {
    _fixture    = fixture;
    _fraction   = fraction;
    _point      = point;
    _normal     = normal;

    return fraction;
}
