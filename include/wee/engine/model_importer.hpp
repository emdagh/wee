#pragma once

#include <iostream>


namespace wee {

    template <typename T>
    struct importer {
        virtual T* import(std::istream&) const = 0;
    };

    struct model;

    class model_importer : importer<model> {

    public:
        model* import(std::istream&) const;
        //std::ostream operator << (std::ostream&, model*);
    };
}
