#pragma once
/**
 * https://github.com/ephtracy/voxel-model/blob/master/MagicaVoxel-file-format-vox.txt
 * https://github.com/ephtracy/voxel-model/blob/master/MagicaVoxel-file-format-vox.txt
 */

namespace wee {

    struct mvid {
        union {
            uint32_t id;
            struct {
                uint8_t bytes[4];
            };
        };
    };

    constexpr static const unsigned int default_palette[256] = {
        0x00000000, 0xffffffff, 0xffccffff, 0xff99ffff, 0xff66ffff, 0xff33ffff, 0xff00ffff, 0xffffccff, 
        0xffccccff, 0xff99ccff, 0xff66ccff, 0xff33ccff, 0xff00ccff, 0xffff99ff, 0xffcc99ff, 0xff9999ff,
        0xff6699ff, 0xff3399ff, 0xff0099ff, 0xffff66ff, 0xffcc66ff, 0xff9966ff, 0xff6666ff, 0xff3366ff, 
        0xff0066ff, 0xffff33ff, 0xffcc33ff, 0xff9933ff, 0xff6633ff, 0xff3333ff, 0xff0033ff, 0xffff00ff,
        0xffcc00ff, 0xff9900ff, 0xff6600ff, 0xff3300ff, 0xff0000ff, 0xffffffcc, 0xffccffcc, 0xff99ffcc, 
        0xff66ffcc, 0xff33ffcc, 0xff00ffcc, 0xffffcccc, 0xffcccccc, 0xff99cccc, 0xff66cccc, 0xff33cccc,
        0xff00cccc, 0xffff99cc, 0xffcc99cc, 0xff9999cc, 0xff6699cc, 0xff3399cc, 0xff0099cc, 0xffff66cc, 
        0xffcc66cc, 0xff9966cc, 0xff6666cc, 0xff3366cc, 0xff0066cc, 0xffff33cc, 0xffcc33cc, 0xff9933cc,
        0xff6633cc, 0xff3333cc, 0xff0033cc, 0xffff00cc, 0xffcc00cc, 0xff9900cc, 0xff6600cc, 0xff3300cc, 
        0xff0000cc, 0xffffff99, 0xffccff99, 0xff99ff99, 0xff66ff99, 0xff33ff99, 0xff00ff99, 0xffffcc99,
        0xffcccc99, 0xff99cc99, 0xff66cc99, 0xff33cc99, 0xff00cc99, 0xffff9999, 0xffcc9999, 0xff999999, 
        0xff669999, 0xff339999, 0xff009999, 0xffff6699, 0xffcc6699, 0xff996699, 0xff666699, 0xff336699,
        0xff006699, 0xffff3399, 0xffcc3399, 0xff993399, 0xff663399, 0xff333399, 0xff003399, 0xffff0099, 
        0xffcc0099, 0xff990099, 0xff660099, 0xff330099, 0xff000099, 0xffffff66, 0xffccff66, 0xff99ff66,
        0xff66ff66, 0xff33ff66, 0xff00ff66, 0xffffcc66, 0xffcccc66, 0xff99cc66, 0xff66cc66, 0xff33cc66, 
        0xff00cc66, 0xffff9966, 0xffcc9966, 0xff999966, 0xff669966, 0xff339966, 0xff009966, 0xffff6666,
        0xffcc6666, 0xff996666, 0xff666666, 0xff336666, 0xff006666, 0xffff3366, 0xffcc3366, 0xff993366, 
        0xff663366, 0xff333366, 0xff003366, 0xffff0066, 0xffcc0066, 0xff990066, 0xff660066, 0xff330066,
        0xff000066, 0xffffff33, 0xffccff33, 0xff99ff33, 0xff66ff33, 0xff33ff33, 0xff00ff33, 0xffffcc33, 
        0xffcccc33, 0xff99cc33, 0xff66cc33, 0xff33cc33, 0xff00cc33, 0xffff9933, 0xffcc9933, 0xff999933,
        0xff669933, 0xff339933, 0xff009933, 0xffff6633, 0xffcc6633, 0xff996633, 0xff666633, 0xff336633, 
        0xff006633, 0xffff3333, 0xffcc3333, 0xff993333, 0xff663333, 0xff333333, 0xff003333, 0xffff0033,
        0xffcc0033, 0xff990033, 0xff660033, 0xff330033, 0xff000033, 0xffffff00, 0xffccff00, 0xff99ff00, 
        0xff66ff00, 0xff33ff00, 0xff00ff00, 0xffffcc00, 0xffcccc00, 0xff99cc00, 0xff66cc00, 0xff33cc00,
        0xff00cc00, 0xffff9900, 0xffcc9900, 0xff999900, 0xff669900, 0xff339900, 0xff009900, 0xffff6600, 
        0xffcc6600, 0xff996600, 0xff666600, 0xff336600, 0xff006600, 0xffff3300, 0xffcc3300, 0xff993300,
        0xff663300, 0xff333300, 0xff003300, 0xffff0000, 0xffcc0000, 0xff990000, 0xff660000, 0xff330000, 
        0xff0000ee, 0xff0000dd, 0xff0000bb, 0xff0000aa, 0xff000088, 0xff000077, 0xff000055, 0xff000044,
        0xff000022, 0xff000011, 0xff00ee00, 0xff00dd00, 0xff00bb00, 0xff00aa00, 0xff008800, 0xff007700, 
        0xff005500, 0xff004400, 0xff002200, 0xff001100, 0xffee0000, 0xffdd0000, 0xffbb0000, 0xffaa0000,
        0xff880000, 0xff770000, 0xff550000, 0xff440000, 0xff220000, 0xff110000, 0xffeeeeee, 0xffdddddd, 
        0xffbbbbbb, 0xffaaaaaa, 0xff888888, 0xff777777, 0xff555555, 0xff444444, 0xff222222, 0xff111111
    };

    struct voxel_data {
        int8_t x, y, z, color;
    };

    using nlohmann::json;

    void to_json(json& j, const voxel_data& d) {
        j = {
            { "x", d.x },
            { "y", d.y },
            { "z", d.z },
            { "color", d.color }
        };
    }

    std::ostream& operator << (std::ostream& os, const voxel_data& d) {
        json j;
        to_json(j, d);
        return os << j;
    }

    voxel_data voxel_reader(binary_reader& b, bool subsample) {
        int8_t x[4];
        b.read<int8_t>(&x[0], 4);
        return voxel_data {
            subsample ? static_cast<int8_t>(static_cast<int>(x[0]) >> 1) : static_cast<int8_t>(x[0]),
            subsample ? static_cast<int8_t>(static_cast<int>(x[1]) >> 1) : static_cast<int8_t>(x[1]),
            subsample ? static_cast<int8_t>(static_cast<int>(x[2]) >> 1) : static_cast<int8_t>(x[2]),
            x[3] // color
        };
    }

    template <typename OutputIt>
    void from_magica_voxel(binary_reader& reader, OutputIt it) {
        DEBUG_METHOD();
        
        static const std::string kIsMagic   = "VOX ";
        static const std::string kIsSize    = "SIZE";
        static const std::string kIsXYZI    = "XYZI";
        static const std::string kIsRGBA    = "RGBA";
        static const std::string kIsMain    = "MAIN";
        static const std::string kIsMaterial= "MATT";
        static const std::string kIsPack    = "PACK";


        [[maybe_unused]] uint16_t data[32 * 32 * 128] = {0 };
        std::vector<voxel_data> all_data;
        std::string magic;
        int version;

        reader.read<char>(std::back_inserter(magic), 4);
        if(magic == kIsMagic) {
            reader.read<int>(&version);
            DEBUG_VALUE_OF(magic);
            DEBUG_VALUE_OF(version);
           

            while(reader.good()) {
                int chunk_size, chunk_children;
                std::string chunk_id;
                 
                reader.read<char>(std::back_inserter(chunk_id), 4);
                reader.read<int>(&chunk_size);
                reader.read<int>(&chunk_children);

                bool is_subsample = false;

                DEBUG_VALUE_OF(chunk_id);

                if(chunk_id == kIsSize) {
                    int dim[3];
                    reader.read<int>(&dim[0], 3);
                    is_subsample = dim[0] > 32 || dim[1] > 32;
                    reader.ignore(chunk_size - sizeof(dim));
                    DEBUG_VALUE_OF(dim);
                } else if(chunk_id == kIsXYZI) {
                    int n;
                    reader.read<int>(&n);
                    for([[maybe_unused]] auto i: range(n)) {
                        auto data = voxel_reader(reader, is_subsample);
                        //*it++ = data;
                        all_data.push_back(data);
                    }

                } else if(chunk_id == kIsRGBA) {
                    [[maybe_unused]] uint16_t palette[256] = { 0 };
                    for(auto i=0; i < 256; i++) {
                        int8_t c[4];
                        reader.read<int8_t>(&c[0], 4);
                        palette[i] = static_cast<uint16_t>(
                            ((c[0] & 0x1f) << 10) |
                            ((c[1] & 0x1f) << 5)  |
                             (c[2] & 0x1f)
                        );
                    }
                } else if(chunk_id == kIsPack) { // Chunk id 'PACK' : if it is absent, only one model in the file
                    int num_models;
                    reader.read<int>(&num_models);
                    DEBUG_VALUE_OF(num_models);
                } else {
                    reader.ignore(chunk_size);
                }
            }
        }

        DEBUG_VALUE_OF(all_data);
    }
}
