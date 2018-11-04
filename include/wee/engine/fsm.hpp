#pragma once

#include <stack>

namespace wee {

    template <typename T>
    class fsm {
        std::stack<state<T>* > _q;
    public:
        void push(T& self, state<T>* st) {
            if(_q.size()) {
                _q.top()->leave(self);
            }
            _q.push(st);
            _q.top()->enter(self);
        }

        void pop(T& self) {
            if(_q.size()) {
                _q.top()->leave(self);
                _q.pop();
            }

            if(_q.size()) {
                _q.top()->enter(self);
            }
        }

        void update(T& self, int dt) {
            if(_q.size()) {
                _q.top()->update(self, dt);
            }
        }

        /**
         * relies on states being singletons
         */
        bool is_current(state<T>* which) const {
            return _q.size() ? which == _q.top() : false;
        }
    };

    template <typename T>
    struct state {
        virtual void enter(T&) = 0;
        virtual void leave(T&) = 0;
        virtual void update(T&, int) = 0;
    };
};
