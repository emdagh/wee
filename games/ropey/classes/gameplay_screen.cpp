#include <classes/gameplay_screen.hpp>
#include <classes/systems.hpp>
#include <classes/entities.hpp>
#include <classes/factories.hpp>
#include <classes/components.hpp>
#include <base/application.hpp>
#include <core/logstream.hpp>
#include <engine/b2RayCastImpl.hpp>
#include <engine/assets.hpp>
#include <engine/ecs.hpp>
#include <gfx/SDL_RendererEXT.hpp>
#include <gfx/SDL_ColorEXT.hpp>
#include <classes/tmx.hpp>
#include <classes/input.hpp>
#include <classes/level.hpp>

using namespace wee;
//typedef factory<entity_type, std::string,        b2World*, const tmx::Object&> object_factory;
//typedef factory<b2Shape*,   tmx::Object::Shape, const tmx::Object&> b2ShapeFactory;

gameplay_screen::gameplay_screen() {
    _debugdraw.SetFlags(
            b2Draw::e_shapeBit          | //= 0x0001, 
            b2Draw::e_jointBit          | //= 0x0002, 
            //b2Draw::e_aabbBit           | //= 0x0004, 
            //b2Draw::e_pairBit           | //= 0x0008, 
            b2Draw::e_centerOfMassBit   | //= 0x0010, 
            //e_particleBit// = 0x0020 
            0
            ) ;
    _world = new b2World({0.0f, 9.8f});
    _world->SetDebugDraw(&_debugdraw);
    _world->SetContactListener(&_contacts);
    _camera = { 0, 0, 0, 0 };
    this->_time_on = 4500;
}
gameplay_screen::~gameplay_screen() {
}
void gameplay_screen::load_content() {
    //std::string pt = wee::get_resource_path("assets/levels") + "level.tmx";
    //
    std::vector<entity_type> beats;
    std::ifstream ifs = wee::open_ifstream("assets/levels.json");
    json j = json::parse(ifs);
    for(const auto& i : j) {
        std::string abs_path = get_resource_path(dirname(i)) + basename(i);
        tmx::Map tiled_map;
        tiled_map.load(abs_path);

        level builder;
        beats.push_back(
            builder.source(&tiled_map)
                .world(_world)
                .build()
        );
    }
    vec2f spawnPoint = kult::get<beat>(beats[0]).spawn;
    p  = create_player(_world, spawnPoint);

    /**
     * TODO: 
     *   - level beats should be entities
     *   - all entitites within a beat should have the nested component to indicate a parent
     *   - a `getCurrentBeat` function should be written for the player
     */

#if 0
    SDL_Point spawnPoint;
    parse_tmx(_world, pt, &spawnPoint);

    p  = create_player(_world, spawnPoint);
    b1 = create_block(_world, { -16, -300, 32, 32 });

    b2Vec2 pa = kult::get<physics>(p).body->GetPosition();
    b2Vec2 temp = { 0.0f, -1000.0f };
    b2Vec2 pb = pa + SCREEN_TO_WORLD(temp);

    b2RayCastClosest rc;
    rc.RayCast(_world, pa, pb);


    tmx::Map tiled_map;
    tiled_map.load(pt);
    auto map_dimensions = tiled_map.getTileCount();
    int rows = map_dimensions.y;
    int cols = map_dimensions.x;

    auto tile_size = tiled_map.getTileSize();
    int tile_w = tile_size.x;
    int tile_h = tile_size.y;

    std::map<gid, SDL_Texture*> tilesets;

    auto& map_tilesets = tiled_map.getTilesets();
    for(auto& tset : map_tilesets) {
        auto first_gid = tset.getFirstGID();

        auto* tex = wee::assets<SDL_Texture>::instance().load(
                tset.getImagePath(),
                ::as_lvalue(
                    std::ifstream(tset.getImagePath(), std::ios::binary)
                    )
                );
        tilesets.insert(std::pair<gid, SDL_Texture*>(first_gid, tex));
    }

    const auto& map_layers = tiled_map.getLayers();
    for(const auto& layer : map_layers) {
        if(layer->getType() != tmx::Layer::Type::Tile) {
            continue;
        }

        auto* tile_layer = dynamic_cast<const tmx::TileLayer*>(layer.get());
        auto& layer_tiles = tile_layer->getTiles();

        for(int y=0; y < rows; y++) {
            for(int x=0; x < cols; x++) {
                int ix = x + y * cols;
                gid cur_gid = layer_tiles[ix].ID;
                if(0 == cur_gid) {
                    continue;
                }
                auto tset_gid = -1;
                for(auto& ts : tilesets) {
                    if(ts.first <= cur_gid) {
                        tset_gid = ts.first;
                        break;
                    }
                }

                if(tset_gid == -1) 
                    continue;

                cur_gid -= tset_gid;

                int ts_w = 0;
                int ts_h = 0;
                SDL_QueryTexture(tilesets[tset_gid],
                        NULL, NULL, &ts_w, &ts_h);

                int set_width = ts_w / tile_w;

                int region_x = (cur_gid % set_width) * tile_w;
                int region_y = std::floor(cur_gid / set_width) * tile_h;

                int x_pos = x * tile_w;
                int y_pos = y * tile_h;

                auto flipFlags = layer_tiles[ix].flipFlags;
                int flip = SDL_FLIP_NONE;
                flip |= (flipFlags & tmx::TileLayer::Horizontal)  ? static_cast<int>(SDL_FLIP_HORIZONTAL)  : static_cast<int>(SDL_FLIP_NONE);
                flip |= (flipFlags & tmx::TileLayer::Vertical)    ? static_cast<int>(SDL_FLIP_VERTICAL)    : static_cast<int>(SDL_FLIP_NONE);

                float theta = 0.0f;
                if(flipFlags & tmx::TileLayer::Diagonal) {
                    theta = 90.0f;
                }

                /*tile(tilesets[tset_gid], 
                        {x_pos, y_pos}, 
                        {region_x, region_y, tile_w, tile_h}, 
                        static_cast<SDL_RendererFlip>(flip), 
                        theta
                    );*/

            }
        }
    }
#endif
    copy_physics_to_transform();
}

void gameplay_screen::handle_input() {
    static const int clickTimeout = 500;
    static int mouseWasDown = 0;
    static int mouseDownTime = 0;
    if(input::instance().mouse_down) {
        if(mouseWasDown == 0) {
            mouseDownTime = SDL_GetTicks();
        }
        mouseWasDown = 1;
    } else {
        if(mouseWasDown) {
            mouseWasDown = 0;
            auto delta = SDL_GetTicks() - mouseDownTime;
            DEBUG_VALUE_OF(delta);
            if((delta) < clickTimeout) {
                on_click();
            }
        }
    }
}

void gameplay_screen::update(int dt, bool a, bool b) {
    copy_transform_to_physics();
    _world->Step(1.0f / (float)60, 4, 3);
    copy_physics_to_transform();

    for(auto& e : kult::join<raycast>()) {
        raycast_t& r = kult::get<raycast>(e);
        if(r.hit) {
            r.hit = false;
            DEBUG_LOG("create new joint");
            _rope = create_rope(_world, p, e, b2Vec2{r.point.x, r.point.y});
            break;
        }
    }



    b2Vec2 pos = WORLD_TO_SCREEN(kult::get<physics>(p).body->GetPosition());
    _cam.set_position(pos.x, pos.y);
    _cam.update(dt);
    _debugdraw.SetCameraTransform(_cam.get_transform());
    clean_physics(_world);

    gamescreen::update(dt, a, b);
}



void gameplay_screen::draw(SDL_Renderer* renderer) {
    _debugdraw.SetRenderer(renderer);

    SDL_RenderGetLogicalSize(renderer, &_camera.w, &_camera.h);

    _cam.set_viewport(_camera.w, _camera.h);
    {
        /*std::vector<kult::type> entities;*/

        for(const auto& e : kult::join<transform, visual, nested>()) {
            const visual_t& v = kult::get<visual>(e);
            const transform_t& t = kult::get<transform>(e);
            const nested_t& n = kult::get<nested>(e);

            vec3 position = { 
                t.position.x + n.offset.x,
                t.position.y + n.offset.y,
                0.0f
            };

            vec3 positionCS = vec3::transform(position, _cam.get_transform());


            SDL_Rect dst = {
                (int)(positionCS.x + 0.5f), //cx + (int)(t.p.x + v.offset.x + .5f), 
                (int)(positionCS.y + 0.5f), //cy + (int)(t.p.y + v.offset.y + .5f),
                v.src.w, 
                v.src.h
            };
            SDL_RenderCopyEx(renderer, 
                    v.texture,
                    &v.src,
                    &dst,
                    t.rotation,
                    NULL,
                    v.flip
                    );
        }

        /*std::sort(entities.begin(), entities.end(), [&] (const kult::type& a, const kult::type& b) {
          int la = kult::get<visual>(a).layer;
          int lb = kult::get<visual>(b).layer;
          return la < lb;
          });*/
    }
    //b2DebugDrawEXT(_world, renderer, _camera);
    //
    _world->DrawDebugData();

    SDL_SetRenderDrawColorEXT(renderer, SDL_ColorPresetEXT::Black);

    vec2 pa = kult::get<transform>(p).position;
    vec3 playerPos = vec3::transform ({pa.x, pa.y, 0.0f}, _cam.get_transform());
    pa.x = playerPos.x;
    pa.y = playerPos.y;

    vec2 pb = { 
        (float)input::instance().mouse_x, 
        (float)input::instance().mouse_y 
    };


    SDL_RenderDrawLine(renderer, 
            (int)pa.x, 
            (int)pa.y, 
            (int)pb.x, 
            (int)pb.y
            );

    SDL_SetRenderDrawColorEXT(renderer, SDL_ColorPresetEXT::CornflowerBlue);
    gamescreen::draw(renderer);

}

int gameplay_screen::on_click() {

    joint_t& a = kult::get<joint>(_rope);
    if(a.joint) {
        DEBUG_LOG("destroy old joint");
        _world->DestroyJoint(a.joint);
        a.joint = NULL;
    }

    for(auto& e : kult::join<raycast>()) {
        kult::get<raycast>(e).hit = false;
    }

    vec2 playerPosition = kult::get<transform>(p).position;

    vec3 mousePosition = {
        (float)input::instance().mouse_x,
        (float)input::instance().mouse_y,// - _camera.h / 2, 
        0.0f
    };

    mousePosition = vec3::transform(mousePosition, mat4::inverted(_cam.get_transform()));

    b2Vec2 pa = SCREEN_TO_WORLD(b2Vec2(playerPosition.x, playerPosition.y));
    b2Vec2 pb = SCREEN_TO_WORLD(b2Vec2(mousePosition.x, mousePosition.y));


    b2RayCastClosest rc;
    rc.RayCast(_world, pa, pb);
    _cam.shake(1000, false);
    return 0;
}

