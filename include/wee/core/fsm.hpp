#pragma once

#include <stack>

namespace wee {

    template <typename T>
        struct state {
            virtual void enter(T*) = 0;
            virtual void update(T*, int) = 0;
            virtual void leave(T*) = 0;
        };

        template <typename T>
        class fsm {
            std::stack<state<T>* > _s;
        public:

        void push(T* self, state<T>* s) {
            if(_s.size()) {
                _s.top()->leave(self);
               // _s.pop();
            }
            _s.push(s);
            _s.top()->enter(self);
        }

        void pop(T* self) {
            if(_s.size()) {
                _s.top()->leave(self);
                _s.pop();
            }

            if(_s.size())
                _s.top()->enter(self);
        }

        void update(T* self, int dt) {
            if(_s.size()) 
                _s.top()->update(self, dt);
        }
    };
}
