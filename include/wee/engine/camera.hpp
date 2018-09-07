#pragma once

namespace wee {
    struct camera {
        float x, y;
        float zoom;
    };

    struct camera_shaker {
        camera* _camera;
        bool is_shaking;

        //void update(int dt) {
        //}
    };
}
