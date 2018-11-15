#include <classes/tmx.hpp>
#include <Box2D/Box2D.h>
#include <classes/factories.hpp>
#include <tmxlite/Map.hpp>
#include <tmxlite/Layer.hpp>
#include <tmxlite/TileLayer.hpp>
#include <cxx-prettyprint/prettyprint.hpp>
#include <SDL.h>
#include <core/logstream.hpp>

void parse_tmx(b2World* world, const std::string& pt, SDL_Point* spawnPoint) {
    tmx::Map map;
    if(map.load(pt)) {
        DEBUG_LOG("loaded map version: {}.{}", 
                map.getVersion().upper,
                map.getVersion().lower);
        const auto& properties = map.getProperties();
        DEBUG_LOG("map has {} properties", properties.size());
        for(const auto& p : properties) {
            DEBUG_VALUE_OF(p.getName());
            DEBUG_VALUE_OF(p.getStringValue());
            DEBUG_VALUE_OF((int)p.getType());

        }

        const auto& layers = map.getLayers();
        DEBUG_LOG("map has {} layers", layers.size());
        for(const auto& layer : layers) {

            if(layer->getType() == tmx::Layer::Type::Object) {
                DEBUG_LOG("found object layer {}",
                        layer->getName());

                const auto& objects = dynamic_cast<tmx::ObjectGroup*>(layer.get())->getObjects();

                for(const auto& object : objects) {
                    DEBUG_LOG("object {}", object.getName());

                    const auto& position = object.getPosition();
                    //const auto& type = object.getType();
                    //const auto& shape = object.getShape();

                    //b2Body* body = b2BodyBuilder::instance().build(world, object);
                    //
                    //
                    //
                    auto in = object.getProperties();

                    /*auto props = wee::zip(
                      wee::map(in, [] (const tmx::Property& p) { return p.getName(); }), 
                      wee::map(in, [] (const tmx::Property& p) { return p.getStringValue(); })
                      );*/

                    //std::vector<std::tuple<std::string, std::string> > props;

                    std::map<std::string, std::string> props;
                    for(const auto& p : object.getProperties()) {
                        props.emplace(p.getName(), p.getStringValue());
                    }

                    DEBUG_VALUE_AND_TYPE_OF(props);

                    if(props.count("class")) {
                        DEBUG_LOG("creating object at {}, {}", object.getPosition().x, object.getPosition().y);
                        entity_type e;
                        object_factory::instance().create(props["class"], world, object, e);
                    }
                    const auto& object_properties = object.getProperties();
                    DEBUG_LOG("object has {} properties", 
                            object_properties.size());
                    for(const auto& object_property : object_properties) {
                        if(object_property.getName() == "class") {
                            if(object_property.getStringValue() == "spawn") {
                                spawnPoint->x = (int)position.x;
                                spawnPoint->y = (int)position.y;
                            }
                        }
                    }
                }
            }
            const auto& layer_properties = layer->getProperties();
            DEBUG_LOG("layer has {} properties", layer_properties.size());
            for(const auto& p : layer_properties) {
                DEBUG_LOG("property: {} ({})", 
                        p.getName(),
                        (int)p.getType()
                        );

            }
        }
    }
}
