/**
 * class to write intermediate representation of models to the final compressed form.
 */
#pragma once

#include <core/binary_writer.hpp>
#include <wee.hpp>

namespace wee {

    struct model_content;

    struct basic_content_writer : public binary_writer {

        explicit basic_content_writer(std::ostream& os) : binary_writer(os) 
        {
        }

        virtual int write_content() = 0;
        virtual int write_json() = 0;
    }; 

    class model_content_writer : public basic_content_writer {
        model_content* _content;
    public:
        model_content_writer(std::ostream& os, model_content* content) 
        : basic_content_writer(os)
        , _content(content)
        {
        }
        virtual int write_content() ; 
        virtual int write_json() {
            return throw not_implemented(), 0;
        }
    };
}
