#pragma once

#include <wee.hpp>
#include <core/delegate.hpp>
#include <core/fsm.hpp>

namespace wee {

    class application;

    struct application_event {
        application* self;
    };

    class application { 
    public:
        virtual ~application() {}
        virtual int create() = 0;
        virtual int start() = 0;
        virtual int resume() = 0;
        virtual int pause() = 0;
        virtual int stop() = 0;
        virtual int destroy() = 0;
    public:
        event_handler<int(const application_event&)> on_create;
        event_handler<int(const application_event&)> on_start;
        event_handler<int(const application_event&)> on_resume;
        event_handler<int(const application_event&)> on_pause;
        event_handler<int(const application_event&)> on_stop;
        event_handler<int(const application_event&)> on_destroy;
        event_handler<int(const application_event&)> on_restart;
    };
}


APICALL int wee_create_application(wee::application**);
APICALL int wee_destroy_application(wee::application*);
