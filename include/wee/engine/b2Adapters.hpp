#pragma once

#include <Box2D/Box2D.h>

namespace wee {

    class b2ContactListenerImpl: public b2ContactListener {
    public:
        virtual ~b2ContactListenerImpl();
        virtual void BeginContact(b2Contact*);
        virtual void EndContact(b2Contact*);
    };

    class b2RayCastClosest : public b2RayCastCallback {
        b2Fixture*  _fixture;
        b2Vec2      _point;
        b2Vec2      _normal;
        float       _fraction= 1.f;
    public:
        void RayCast(b2World* world, const b2Vec2& p1, const b2Vec2& p2);
        float32 ReportFixture(b2Fixture* fixture, const b2Vec2& point, const b2Vec2& normal, float32 fraction);
    };
}
