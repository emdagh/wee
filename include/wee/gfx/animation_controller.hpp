#pragma once

#include <cstdint>
#include <vector>
#include <unordered_map>
#include <core/vec3.hpp>
#include <core/quaternion.hpp>
#include <core/mat4.hpp>

namespace wee {
    struct mat4;
    struct bone;

    struct animation_transform {
        vec3f       _position;
        quaternion  _orientation;
        vec3f       _scale;

        static animation_transform lerp(const animation_transform& a, 
                const animation_transform& b, 
                float c) {
                
            return animation_transform {
                vec3f::lerp(a._position, b._position, c),
                quaternion::slerp(a._orientation, b._orientation, c),
                vec3f::lerp(a._scale, b._scale, c)
            };

        }
    };
    
    struct animation_keyframe {
        size_t              _time;
        animation_transform _pose;
    };
    /*struct animation_channel {
        std::string                     _name;
        std::vector<animation_keyframe> _keyframes;
    };*/
    typedef std::vector<animation_keyframe> animation_channel;

    struct animation_clip {
        std::string _name;
        size_t  _duration;
        std::unordered_map<std::string, animation_channel*> _channels;
    };


    

    class animation_controller {
        std::vector<bone*> _bones; // <- should these be pointers?? what about instancing?
        std::vector<animation_transform> _local_bone_poses;

        float _time;
        float _crossfade_elapsed_time;
        float _crossfade_time;
        float _crossfade_lerp;

        animation_clip* _animation;
        animation_clip* _crossfade_animation;

        bool _is_crossfade_enabled;
        bool _is_finished;
        bool _is_loop_enabled;
        bool _is_playing;

    public:
        void update(int);
        void start(animation_clip*);
        void crossfade(animation_clip*, float);
        void copy_absolute_bone_transforms_to(mat4[]);
    protected:
        void update_animation_time(float);
        void update_crossfade_time(float);
        void update_channel_poses();
        void update_absolute_bone_transforms();
        auto interpolate_channel_pose(animation_channel*, float);
    };
}
