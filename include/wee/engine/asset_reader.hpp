#pragma once

namespace wee {
    class asset_reader : binary_reader {
    public:
        vec3f read_vec3f();
        mat4f read_mat4f();
        quaternion read_quaternion();
    };
}
