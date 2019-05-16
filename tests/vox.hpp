#pragma once

#include <core/binary_reader.hpp>
#include <core/binary_writer.hpp>
#include <core/factory.hpp>
#include <nlohmann/json.hpp>

#define TAG(a, b, c, d) \
    (a | (b << 8) | (c << 16) | (d << 24))
/**
 * << with aggregate inheritance, a parent is a subobject of the aggregate! no clue how multiple inheritance would work,
 * << but this works! 
 *
 * struct a {}; // <- not lack of polymorphism (aggregate only)
 * struct b { int i; };
 *
 * a* obj = new b { {}, 0 };
 *
 */
//
// we can't dynamic_cast aggregates (non-polymporphic...) and we like aggregates better than polymorhism (at least, I do...), so; an old-school type-id (int) would sort of work.. then again; we can't use any data members, because we can't cast..
// catch-22 there. No worries, there is no requirement to write (yet).
//
// Well.. we probaby want to write any nice models we get from WFC to the magicavoxel format.
//
//

namespace vox {

    using wee::binary_reader;

        struct chunk {
#if 1
            virtual ~chunk() = default;
#endif
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
            char colors[255];
        };

        struct unknown : chunk {
            int id;
            std::vector<char> data;
        };

        struct vox {
            int version;
            std::vector<chunk*> chunks;
        };


    struct vox_reader {
        static const int VOX_ = TAG('V','O','X',' '); 
        static const int MAIN = TAG('M','A','I','N');
        static const int PACK = TAG('P','A','C','K');
        static const int SIZE = TAG('S','I','Z','E');
        static const int XYZI = TAG('X','Y','Z','I');
        static const int RGBA = TAG('R','G','B','A');

        typedef wee::factory<chunk, int> factory_t;
        typedef std::function<chunk*(chunk*, binary_reader&)> builder;

        static std::map<int, builder> builders;



        static struct register_factories_and_builders {
            register_factories_and_builders() {
                wee::register_factory<chunk, pack>(PACK);
                wee::register_factory<chunk, size>(SIZE);
                wee::register_factory<chunk, rgba>(RGBA);
                wee::register_factory<chunk, xyzi>(XYZI);
                
                vox_reader::builders[PACK] = [] (chunk* in, binary_reader& reader) {
                    if(auto* a = dynamic_cast<pack*>(in); a != nullptr) {
                        a->num_models = reader.read_object<int>();
                    }
                    return in;
                };

                vox_reader::builders[SIZE] = [] (chunk* in, binary_reader& reader) {
                    if(auto* a = dynamic_cast<size*>(in); a != nullptr) {
                        a->x = reader.read_object<int>();
                        a->y = reader.read_object<int>();
                        a->z = reader.read_object<int>();
                    }
                    return in;
                };
                vox_reader::builders[RGBA] = [] (chunk* in, binary_reader& reader) {
                    if(auto* a = dynamic_cast<rgba*>(in); a != nullptr) {
                        reader.read<char>(&a->colors[0], 255);
                    }
                    return in;
                };
                vox_reader::builders[XYZI] = [] (chunk* in, binary_reader& reader) {
                    if(auto* a = dynamic_cast<xyzi*>(in); a != nullptr) {
                        a->voxels = vox_reader::read_voxels(reader);
                    }
                    return in;
                };
            }
        } _;


        static std::vector<char> read_unkown(binary_reader& reader, int n) {
            std::vector<char> res;
            reader.read<char>(std::back_inserter(res), 255);
            return res;
        }

        static std::vector<voxel> read_voxels(binary_reader& reader) {
            std::vector<voxel> res;
            int num_voxels = reader.read_object<int>();
            for(int i=0; i < num_voxels; i++) {
                res.push_back(voxel { 
                    reader.read_object<char>(), // x
                    reader.read_object<char>(), // y
                    reader.read_object<char>(), // z
                    reader.read_object<char>(), // i (color index)
                });
            }
            return res;
        }

        static chunk* read_chunk(binary_reader& reader) {
            int chunk_id = reader.read_object<int>();
            [[maybe_unused]] int content_size = reader.read_object<int>();
            int num_children = reader.read_object<int>();

            if(num_children != 0) 
                throw std::runtime_error("recursive chunks not supported!");


            return builders[chunk_id](factory_t::instance().create(chunk_id), reader);
        }

        static std::vector<chunk*> read_chunks(binary_reader& reader) {
            std::vector<chunk*> chunks;

            while(reader.good()) {
                chunks.push_back(read_chunk(reader));
            }
            return chunks;
        }

        static vox* read(binary_reader& reader) {
            int magic = reader.read_object<int>();
            if(VOX_ != magic)
                throw std::runtime_error("VOX_ not found in file");
            [[maybe_unused]] int version         = reader.read_object<int>();
            [[maybe_unused]] int chunkid         = reader.read_object<int>();
            [[maybe_unused]] int content_size    = reader.read_object<int>();
            [[maybe_unused]] int num_children    = reader.read_object<int>();
            
            return new vox {
                version, 
                read_chunks(reader)
            };
        }

        static void write_chunk(const chunk* ptr, binary_writer& writer) {
            /**
             * TODO: create writers as we did for readers.
             */
            if(const auto* a = dynamic_cast<const pack*>(ptr); a != nullptr) {
                writer.write(PACK); // chunk_id
                writer.write(4);    // content_size
                writer.write(0);    // number_of_children
                writer.write(a->num_models);
            } else if(const auto* a = dynamic_cast<const size*>(ptr); a != nullptr) {
                writer.write(SIZE);
                writer.write(12);
                writer.write(0);
                writer.write(a->x);
                writer.write(a->y);
                writer.write(a->z);
            } else if(const auto* a = dynamic_cast<const xyzi*>(ptr); a != nullptr) {
                writer.write(XYZI);
                writer.write(static_cast<int>(4 * 4 * a->voxels.size()));
                writer.write(0);
                writer.write(static_cast<int>(a->voxels.size()));
                for(const auto& v: a->voxels) {
                    writer.write(v.x);
                    writer.write(v.y);
                    writer.write(v.z);
                    writer.write(v.i);
                }
            } else if(const auto* a = dynamic_cast<const rgba*>(ptr); a != nullptr) {
                writer.write(RGBA);
                writer.write(4 + 4 * 255);
                writer.write(0);
                for(int i=0; i < 255; i++)
                    writer.write(a->colors[i]);
            } else {
                throw std::runtime_error("unkown chunk!");
            }
        }

        static void write(const vox& v, binary_writer& writer) {
            writer.write(VOX_);
            writer.write(v.version);
            writer.write(MAIN);
            writer.write(0);
            
            for(const auto* ch : v.chunks) {
                write_chunk(ch, writer);
            }
        }
    };

}
