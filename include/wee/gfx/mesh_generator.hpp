#pragma once

#include <core/factory.hpp>

namespace wee {

    struct model;

    typedef factory<model> model_factory;

    struct icosphere_builder {

        typedef icosphere_builder self_type;

        self_type& subdivide();
        self_type& vertex();
        self_type& index();
    };

    struct mesh_generator {
        static model* ico_sphere(float, int);
    };
}
