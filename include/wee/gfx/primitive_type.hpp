#pragma once

namespace wee {

    enum struct primitive_type : uint8_t {
        kPoints,
        kTriangles,
        kTriangleFan,
        kTriangleStrip,
        kQuads,
        kQuadStrip,
        kLineList,
        kLineStrip,
        kLineLoop
    };
}
