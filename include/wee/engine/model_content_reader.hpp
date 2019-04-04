
#pragma once

#include <iostream>


namespace wee {

    template <typename T>
    struct content_reader {
        virtual T* read(std::istream&) const = 0;
    };

    struct model_content;

    class model_content_reader : content_reader<model_content> {
        void _import_meshes();
        void _import_bones();
        void _import_animations();
    public:
        model_content* read(std::istream&) const;
    };
}
