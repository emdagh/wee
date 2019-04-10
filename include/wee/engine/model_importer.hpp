#pragma once

#include <iostream>
#include <string>

namespace wee {
    struct model;
    //class model_importer {
    //public:
        model* import_model(std::istream& is);
        model* import_model_from_file(const std::string& );
    //};
}

//model* m = assets<model>::instance().load(is);
