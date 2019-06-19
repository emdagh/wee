#include <gfx/animation_controller.hpp>
#include <gfx/model.hpp>
#include <core/range.hpp>
#include <core/mat4.hpp>

using namespace wee;

int get_keyframe_index_by_time(animation_channel* c, float t) {
    if(c->size() == 0) 
        return -1;

    int index = 0;
    int a = 0;
    int b = c->size() - 1;

    while(b >= a) {
        index = (a + b) / 2;
        if(c->at(index)._time < t) {
            a = index + 1;
        } else if(c->at(index)._time > t) {
            b = index - 1;
        } else {
            break;
        }
    }

    if(c->at(index)._time > t)
        index--;
    return index;
}

const animation_keyframe& get_keyframe_by_time(animation_channel* c, float t) {
    return c->at(get_keyframe_index_by_time(c, t));
}

void animation_controller::start(animation_clip* a) {
    _animation = a;
    _is_finished = false;
    _is_playing = true;
}

void animation_controller::crossfade(animation_clip* a, float t) {
    if(_is_crossfade_enabled) {
        _crossfade_lerp         = 0.0f;
        _crossfade_time         = 0.0f;
        _crossfade_elapsed_time = 0.0f;
        start(a);
    }
    _crossfade_animation    = a;
    _crossfade_time         = t;
    _crossfade_elapsed_time = 0.0f;
    _is_crossfade_enabled   = true;
}

auto animation_controller::interpolate_channel_pose(animation_channel* channel, float t) 
{ //-> animation_transform {
    size_t i0 = (size_t)get_keyframe_index_by_time(channel, t);
    size_t i1 = (i0 + 1) % channel->size();

    auto& keyframe_a = channel->at(i0);
    auto& keyframe_b = channel->at(i1);
    auto keyframeDuration = i0 == channel->size() - 1 
        ? _animation->_duration - keyframe_a._time
        : keyframe_b._time - keyframe_a._time;

    if(keyframeDuration > 0) { 
        auto elapsedKeyframeTime = _time - keyframe_a._time;
        float flerp = elapsedKeyframeTime / keyframeDuration;
        return animation_transform::lerp(keyframe_a._pose, keyframe_b._pose, flerp); 
    } 
    return keyframe_a._pose;

}

void animation_controller::update_channel_poses() {
    animation_channel* channel  = nullptr;
    animation_transform pose;

    for(auto i: range(_bones.size())) {
        std::string name = _bones[i]->_name;
        if(_animation->_channels.count(name)) {
            channel = _animation->_channels[name];
            pose    = interpolate_channel_pose(channel, _time);
            _local_bone_poses[i] = pose;
        }

        if(_is_crossfade_enabled) {
            if(_crossfade_animation->_channels.count(name)) {
                channel = _crossfade_animation->_channels[name];
                pose = interpolate_channel_pose(channel, 0);
            }
            _local_bone_poses[i] = animation_transform::lerp(_local_bone_poses[i], pose, _crossfade_lerp);

        }
    }
}

void animation_controller::update_absolute_bone_transforms() {
    //for(const auto& pose: _local_bone_poses) {
    for(auto i: range(_local_bone_poses.size())) {
        const auto& pose = _local_bone_poses[i];
        mat4 scale      = mat4::create_scale(pose._scale);
        mat4 rotation   = mat4::create_from_quaternion(pose._orientation);
        mat4 position   = mat4::create_translation(pose._position);
        mat4 frame      = mat4::mul(scale, mat4::mul(rotation, position));
        _bones[i]->_local= frame;
    }
}

void animation_controller::update_crossfade_time(float dt) {
    _crossfade_elapsed_time += dt;
    if(_crossfade_elapsed_time > _crossfade_time) {
        _is_crossfade_enabled = false;
        _crossfade_lerp = 0.0f;
        _crossfade_time = 0.0f;
        _crossfade_elapsed_time = 0.0f;
        start(_crossfade_animation);
    } else {
        _crossfade_lerp = _crossfade_elapsed_time / _crossfade_time;
    }
}

void animation_controller::update_animation_time(float dt) {
    _time += dt; // -= for reverse play
    if(_time > (float)_animation->_duration) {
        if(_is_loop_enabled) {
            while(_time > _animation->_duration) {
                _time -= _animation->_duration;
            }
        } else {
            _time           = static_cast<float>(_animation->_duration);
            _is_playing     = false;
            _is_finished    = true;
        }

    }
}

void animation_controller::update(int dt) {
    if(_is_finished) 
        return;

    float fDt = 0.001f * dt; // ?
    if(_animation != nullptr) {
        update_animation_time(fDt);
        if(_is_crossfade_enabled) {
            update_crossfade_time(fDt);
        }
        update_channel_poses();
    }
    update_absolute_bone_transforms();
}

void animation_controller::copy_absolute_bone_transforms_to(mat4 arr[]) {
    for(auto i: range(_bones.size())) {
        auto* bone = _bones[i];
        if(bone->_parent < 0) {
            arr[i] = _bones[i]->_local;
            continue;
        }
        auto parent = _bones[i]->_parent;
        arr[i] = _bones[i]->_local * arr[parent];
    }

    for(auto i: range(_bones.size())) {
        arr[i] = _bones[i]->_bindPose * arr[i];
    }
}
