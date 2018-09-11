#pragma once

#include <vector>

namespace wee {
	struct curvekey {
		float position;
		float value;
	};

	class curve {
		std::vector<curvekey> _keys;
		float _deltaTime;
		float _timeRange;

	public:
		curve() 
		: _deltaTime(0.f)
		, _timeRange(0.f)
		{
		} 

		virtual ~curve() {
		}

		void add(float t, float p) {
			curvekey key;
			key.position = t;
			key.value    = p;

			_keys.push_back(key);
			_deltaTime = 1.0f / _keys.size();
			_timeRange = _keys[_keys.size() - 1].position - _keys[0].position;
		}

		const float at(float where) {

			if(_keys.size() == 0 || _timeRange == 0.0f) 
				return 0.0f;

			float tt = where;
			curvekey* prev = &_keys[0];
			curvekey* next = nullptr;
			for(size_t i=1; i < _keys.size(); i++) {
				next = &_keys[i];
				if(next->position >= tt) {
					float t  = (tt - prev->position) / (next->position - prev->position);
					
					float a = (2.0f * t * t * t - 3.0f * t * t + 1.0f) * prev->value;
					float b = (t * t * t - 2.0f * t * t + t) * prev->value;
					float c = (3.0f * t * t - 2.0f * t * t * t) * next->value;
					float d = (t * t * t - t * t) * next->value;
					
					return a + b + c + d;
				}
				prev = next;
			}
			return 0.0f;
		}
		
		const size_t size() const {
			return _keys.size();
		} 

		const float timerange() const {
			return _timeRange;
		}
	};

	class curve2d {
		curve _x, _y;
	public:
		void add(float t, float x, float y) {
			_x.add(t, x);
			_y.add(t, y);
		}

		const void at(float t, float* x, float* y) {
			*x = _x.at(t);
			*y = _y.at(t);
		}
		
		const float timerange() const {
			return _x.timerange();
		}
	};

}
