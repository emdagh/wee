#include <Box2D/Box2D.h>

class collisions : public b2ContactListener {
public:
    virtual ~collisions();
    virtual void BeginContact(b2Contact*);
    virtual void EndContact(b2Contact*);
};
