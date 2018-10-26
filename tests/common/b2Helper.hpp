namespace {
    struct b2Helper {
        b2Body* createCircle(b2World*, const b2Vec2&, float radius);
        b2Body* createBox(b2World*, const b2Vec2&, float w, float h);
        b2Body* createPolygon(b2World*, const b2Vec2*, size_t);
        b2Body* createPolyline(b2World*, const b2Vec2*, size_t);
    };
}
