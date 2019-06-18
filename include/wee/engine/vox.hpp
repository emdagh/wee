#pragma once

#include <core/binary_reader.hpp>
#include <core/binary_writer.hpp>
#include <core/range.hpp>
#include <core/factory.hpp>
#include <core/logstream.hpp>
#include <nlohmann/json.hpp>
#include <map>

/**
 * LEARNING MOMENTS:
 *
 * std::istream.good() will *NOT* check if the stream is currently at EOF, but if it has read *past* eof. Slight nuance...
 *
 * << with aggregate inheritance, a parent is a subobject of the aggregate! no
 * clue how multiple inheritance would work,
 * << but this works!
 *
 * struct a {}; // <- not lack of polymorphism (aggregate only)
 * struct b { int i; };
 *
 * a* obj = new b { {}, 0 };
 *
 */
//
// we can't dynamic_cast aggregates (non-polymporphic...) and we like aggregates
// better than polymorhism (at least, I do...), so; an old-school type-id (int)
// would sort of work.. then again; we can't use any data members, because we
// can't cast.. catch-22 there. No worries, there is no requirement to write
// (yet).
//
// Well.. we probaby want to write any nice models we get from WFC to the
// magicavoxel format.
//
//
/**
 * static classes will only be initialized as top-level (namespace) classes. Nested statics won't be initialized on program start.
 */

#define TAG(a, b, c, d) (a | (b << 8) | (c << 16) | (d << 24))
namespace wee {

    class model;

    using wee::binary_reader;
    using wee::range;
    using nlohmann::json;

    static const int VOX_= TAG('V', 'O', 'X', ' ');
    static const int MAIN= TAG('M', 'A', 'I', 'N');
    static const int PACK= TAG('P', 'A', 'C', 'K');
    static const int SIZE= TAG('S', 'I', 'Z', 'E');
    static const int XYZI= TAG('X', 'Y', 'Z', 'I');
    static const int RGBA= TAG('R', 'G', 'B', 'A');

    struct WEE_EXPORT vox {
        constexpr static const unsigned int default_palette[256] = {
            0x00000000, 0xffffffff, 0xffccffff, 0xff99ffff, 0xff66ffff, 0xff33ffff,
            0xff00ffff, 0xffffccff, 0xffccccff, 0xff99ccff, 0xff66ccff, 0xff33ccff,
            0xff00ccff, 0xffff99ff, 0xffcc99ff, 0xff9999ff, 0xff6699ff, 0xff3399ff,
            0xff0099ff, 0xffff66ff, 0xffcc66ff, 0xff9966ff, 0xff6666ff, 0xff3366ff,
            0xff0066ff, 0xffff33ff, 0xffcc33ff, 0xff9933ff, 0xff6633ff, 0xff3333ff,
            0xff0033ff, 0xffff00ff, 0xffcc00ff, 0xff9900ff, 0xff6600ff, 0xff3300ff,
            0xff0000ff, 0xffffffcc, 0xffccffcc, 0xff99ffcc, 0xff66ffcc, 0xff33ffcc,
            0xff00ffcc, 0xffffcccc, 0xffcccccc, 0xff99cccc, 0xff66cccc, 0xff33cccc,
            0xff00cccc, 0xffff99cc, 0xffcc99cc, 0xff9999cc, 0xff6699cc, 0xff3399cc,
            0xff0099cc, 0xffff66cc, 0xffcc66cc, 0xff9966cc, 0xff6666cc, 0xff3366cc,
            0xff0066cc, 0xffff33cc, 0xffcc33cc, 0xff9933cc, 0xff6633cc, 0xff3333cc,
            0xff0033cc, 0xffff00cc, 0xffcc00cc, 0xff9900cc, 0xff6600cc, 0xff3300cc,
            0xff0000cc, 0xffffff99, 0xffccff99, 0xff99ff99, 0xff66ff99, 0xff33ff99,
            0xff00ff99, 0xffffcc99, 0xffcccc99, 0xff99cc99, 0xff66cc99, 0xff33cc99,
            0xff00cc99, 0xffff9999, 0xffcc9999, 0xff999999, 0xff669999, 0xff339999,
            0xff009999, 0xffff6699, 0xffcc6699, 0xff996699, 0xff666699, 0xff336699,
            0xff006699, 0xffff3399, 0xffcc3399, 0xff993399, 0xff663399, 0xff333399,
            0xff003399, 0xffff0099, 0xffcc0099, 0xff990099, 0xff660099, 0xff330099,
            0xff000099, 0xffffff66, 0xffccff66, 0xff99ff66, 0xff66ff66, 0xff33ff66,
            0xff00ff66, 0xffffcc66, 0xffcccc66, 0xff99cc66, 0xff66cc66, 0xff33cc66,
            0xff00cc66, 0xffff9966, 0xffcc9966, 0xff999966, 0xff669966, 0xff339966,
            0xff009966, 0xffff6666, 0xffcc6666, 0xff996666, 0xff666666, 0xff336666,
            0xff006666, 0xffff3366, 0xffcc3366, 0xff993366, 0xff663366, 0xff333366,
            0xff003366, 0xffff0066, 0xffcc0066, 0xff990066, 0xff660066, 0xff330066,
            0xff000066, 0xffffff33, 0xffccff33, 0xff99ff33, 0xff66ff33, 0xff33ff33,
            0xff00ff33, 0xffffcc33, 0xffcccc33, 0xff99cc33, 0xff66cc33, 0xff33cc33,
            0xff00cc33, 0xffff9933, 0xffcc9933, 0xff999933, 0xff669933, 0xff339933,
            0xff009933, 0xffff6633, 0xffcc6633, 0xff996633, 0xff666633, 0xff336633,
            0xff006633, 0xffff3333, 0xffcc3333, 0xff993333, 0xff663333, 0xff333333,
            0xff003333, 0xffff0033, 0xffcc0033, 0xff990033, 0xff660033, 0xff330033,
            0xff000033, 0xffffff00, 0xffccff00, 0xff99ff00, 0xff66ff00, 0xff33ff00,
            0xff00ff00, 0xffffcc00, 0xffcccc00, 0xff99cc00, 0xff66cc00, 0xff33cc00,
            0xff00cc00, 0xffff9900, 0xffcc9900, 0xff999900, 0xff669900, 0xff339900,
            0xff009900, 0xffff6600, 0xffcc6600, 0xff996600, 0xff666600, 0xff336600,
            0xff006600, 0xffff3300, 0xffcc3300, 0xff993300, 0xff663300, 0xff333300,
            0xff003300, 0xffff0000, 0xffcc0000, 0xff990000, 0xff660000, 0xff330000,
            0xff0000ee, 0xff0000dd, 0xff0000bb, 0xff0000aa, 0xff000088, 0xff000077,
            0xff000055, 0xff000044, 0xff000022, 0xff000011, 0xff00ee00, 0xff00dd00,
            0xff00bb00, 0xff00aa00, 0xff008800, 0xff007700, 0xff005500, 0xff004400,
            0xff002200, 0xff001100, 0xffee0000, 0xffdd0000, 0xffbb0000, 0xffaa0000,
            0xff880000, 0xff770000, 0xff550000, 0xff440000, 0xff220000, 0xff110000,
            0xffeeeeee, 0xffdddddd, 0xffbbbbbb, 0xffaaaaaa, 0xff888888, 0xff777777,
            0xff555555, 0xff444444, 0xff222222, 0xff111111};

        struct chunk {
            virtual ~chunk() = default;
        };
        struct voxel {
            char x, y, z, i;
        };

        struct pack : chunk {
            int num_models;
        };
        struct size : chunk {
            int x, y, z;
        };
        struct xyzi : chunk {
            std::vector<voxel> voxels;
        };
        struct rgba : chunk {
            unsigned int colors[256];
        };

        struct unknown : chunk {
            int id;
            std::vector<char> data;
        };
        typedef wee::factory<chunk, int> factory_t;
        
        int version;
        std::vector<chunk*> chunks;

        static void set_palette(vox* vx, const unsigned int colors[256]) {
            rgba* dst = nullptr;
            if(dst = const_cast<rgba*>(get<rgba>(const_cast<const vox*>(vx))); dst == nullptr) {
                dst = new rgba();
                vx->chunks.push_back(dst);
            }
            std::copy(&colors[0], &colors[255], &dst->colors[0]);
        }

        static void set_pack(vox* vx, int n) {

            pack* dst = nullptr;
            if(dst = const_cast<pack*>(get<pack>(const_cast<const vox*>(vx))); dst == nullptr) {
                dst = new pack();
                vx->chunks.push_back(dst);
            }
            dst->num_models = n;
        }


        static void set_size(vox* vx, int x, int y, int z) {
            size* dst = nullptr;
            if(dst = const_cast<size*>(get<size>(const_cast<const vox*>(vx))); dst == nullptr) {
                dst = new size();
                vx->chunks.push_back(dst);
            }
            dst->x = x;
            dst->y = y;
            dst->z = z;
        }

        template <typename T>
        static const T* get(const vox* v) {
            for(const auto* ptr: v->chunks) {
                if(const auto* a = dynamic_cast<const T*>(ptr); a != nullptr) {
                    return a;
                }
            }
            return nullptr;
        }

        static void to_model(const vox*, model**);

    };
    void to_json(json& j, const vox::voxel& v) {
        j = {
            { "x", v.x },
            { "y", v.y },
            { "z", v.z },
            { "i", v.i },
        };
    }
    std::ostream& operator << (std::ostream& os, const vox::voxel& v) {
        json j;
        to_json(j, v);
        return os << j;
    }
    void to_json(json& j, const vox::size& s) {
        j = {
            { "x", s.x },
            { "y", s.y },
            { "z", s.z },
        };
    }
    std::ostream& operator << (std::ostream& os, const vox::size& s) {
        json j;
        to_json(j, s);
        return os << j;
    }
    struct vox_reader {
        using chunk = vox::chunk;
        using pack  = vox::pack;
        using rgba  = vox::rgba;
        using xyzi  = vox::xyzi;
        using voxel = vox::voxel;
        using size  = vox::size;

        typedef std::function<chunk*(chunk*, binary_reader&)> builder;
        static std::vector<voxel> read_voxels(binary_reader& reader) {
            std::vector<voxel> res;
            int num_voxels = reader.read_object<int>();
            for (int i = 0; i < num_voxels; i++) {
                res.push_back(voxel{
                        reader.read_object<char>(),  // x
                        reader.read_object<char>(),  // y
                        reader.read_object<char>(),  // z
                        reader.read_object<char>(),  // i (color index)
                        });
            }
            return res;
        }

        static const std::map<int, builder> readers;


        static std::vector<char> read_unkown(binary_reader& reader, int n) {
            std::vector<char> res;
            reader.read<char>(std::back_inserter(res), n);
            return res;
        }


        template <typename OutputIt>
            static void read_chunk(binary_reader& reader, OutputIt it) {
                int chunk_id     = reader.read_object<int>();
                int content_size = reader.read_object<int>();
                int num_children = reader.read_object<int>();

                if(readers.count(chunk_id)) {
                    *it++ = readers.at(chunk_id)(vox::factory_t::instance().create(chunk_id), reader);
                    if(num_children) {
                        throw std::runtime_error("nested chunks not supported!");
                    }
                } else {
                    reader.ignore(content_size);
                }
            }

        static std::vector<vox::chunk*> read_chunks(binary_reader& reader) {
            std::vector<chunk*> chunks;

            while (reader.good() && reader.peek() != EOF) {
                read_chunk(reader, std::back_inserter(chunks));
            }
            return chunks;
        }

        static vox* read(binary_reader& reader) {
            int magic = reader.read_object<int>();
            if (VOX_ != magic) throw std::runtime_error("VOX_ not found in file");
            [[maybe_unused]] int version = reader.read_object<int>();
            [[maybe_unused]] int chunkid = reader.read_object<int>();
            [[maybe_unused]] int content_size = reader.read_object<int>();
            [[maybe_unused]] int num_children = reader.read_object<int>();

            return new vox{version, read_chunks(reader)};
        }


    };

    struct vox_writer {
        using chunk = vox::chunk;
        using pack  = vox::pack;
        using rgba  = vox::rgba;
        using xyzi  = vox::xyzi;
        using voxel = vox::voxel;
        using size  = vox::size;
        static void write_chunk(const chunk* ptr, binary_writer& writer) {
            /**
             * TODO: create writers as we did for readers.
             */
            if (const auto* a = dynamic_cast<const pack*>(ptr); a != nullptr) {
                writer.write(PACK);  // chunk_id
                writer.write(4);     // content_size
                writer.write(0);     // number_of_children
                writer.write(a->num_models);
            } else if (const auto* a = dynamic_cast<const size*>(ptr);
                    a != nullptr) {
                writer.write(SIZE);
                writer.write(12);
                writer.write(0);
                writer.write(a->x);
                writer.write(a->y);
                writer.write(a->z);
            } else if (const auto* a = dynamic_cast<const xyzi*>(ptr); a != nullptr) {
                writer.write(XYZI);
                writer.write(static_cast<int>(4 * 4 * a->voxels.size()));
                writer.write(0);
                writer.write(static_cast<int>(a->voxels.size()));
                for (const auto& v : a->voxels) {
                    writer.write(v.x);
                    writer.write(v.y);
                    writer.write(v.z);
                    writer.write(v.i);
                }
            } else if (const auto* a = dynamic_cast<const rgba*>(ptr); a != nullptr) {
                writer.write(RGBA);
                writer.write(4 + 4 * 256);
                writer.write(0);
                for (int i = 0; i < 256; i++) writer.write(a->colors[i]);
            } else {
                throw std::runtime_error("unkown chunk!");
            }
        }
        static void write(const vox* v, binary_writer& writer) {
            writer.write(VOX_);
            writer.write(v->version);
            writer.write(MAIN);
            writer.write(0);

            for (const auto* ch : v->chunks) {
                write_chunk(ch, writer);
            }
        }
    };
    const std::map<int, vox_reader::builder> vox_reader::readers = {
        {PACK,
            [](vox::chunk* in, binary_reader& reader) {
                if (auto* a = dynamic_cast<vox::pack*>(in); a != nullptr) {
                    a->num_models = reader.read_object<int>();
                }
                return in;
            }},
        {SIZE,
            [](vox::chunk* in, binary_reader& reader) {
                if (auto* a = dynamic_cast<vox::size*>(in); a != nullptr) {
                    a->x = reader.read_object<int>();
                    a->y = reader.read_object<int>();
                    a->z = reader.read_object<int>();
                }
                return in;
            }},
        {RGBA,
            [](vox::chunk* in, binary_reader& reader) {
                if (auto* a = dynamic_cast<vox::rgba*>(in); a != nullptr) {
                    reader.read<int>(&a->colors[0], 256);
                }
                return in;
            }},
        {XYZI, 
            [](vox::chunk* in, binary_reader& reader) {
                if (auto* a = dynamic_cast<vox::xyzi*>(in); a != nullptr) {
                    a->voxels = vox_reader::read_voxels(reader);
                }
                return in;
            }}
    };
    static class register_vox_factories_and_readers {
        public:
            register_vox_factories_and_readers() {
                DEBUG_METHOD();
                wee::register_factory<vox::chunk, vox::pack>(PACK);
                wee::register_factory<vox::chunk, vox::size>(SIZE);
                wee::register_factory<vox::chunk, vox::rgba>(RGBA);
                wee::register_factory<vox::chunk, vox::xyzi>(XYZI);

            }
            virtual ~register_vox_factories_and_readers() = default;
    } _g_register_vox_factories_and_readers;

}  // namespace vox
