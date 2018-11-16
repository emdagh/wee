#pragma once

class b2Contact;

namespace wee {
    class b2ContactListenerImpl: public b2ContactListener {
    public:
        virtual ~b2ContactListenerImpl();
        virtual void BeginContact(b2Contact*);
        virtual void EndContact(b2Contact*);
    };
}
