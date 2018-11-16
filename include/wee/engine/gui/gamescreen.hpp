#pragma once

#include <vector>
#include <algorithm>
#include <functional>
#include <core/fsm.hpp>
#include <nlohmann/json.hpp>

struct SDL_Renderer;

namespace wee {

    using nlohmann::json;

	enum gamescreen_state {
		E_STATE_HIDDEN,
		E_STATE_TRANSITION_ON,
		E_STATE_TRANSITION_OFF,
		E_STATE_ACTIVE
	};

	class gamescreen_manager;
	class gamescreen {
	protected:
		typedef std::function<void(const gamescreen&)> callback_fn;
		int   _state;
		bool  _exiting;
		int   _time_on;
		int   _time_off;
		float _pos;
		bool  _popup;
        bool _other_screen_has_focus;
		//gamescreen_manager* _manager;
		callback_fn _on_exit;
        static std::vector<gamescreen*> _all;

	protected:
		bool update_transition(int dt, int time, int direction);
	public:
		gamescreen();
		virtual ~gamescreen();
		virtual void load_content();
		virtual void handle_input();
		virtual void update(int dt, bool otherScreenHasFocus, bool coveredByOtherScreen);
		virtual void draw(SDL_Renderer*);
		virtual void unload_content();
		virtual void quit();


		inline float transition() const {
			return _pos;
		}

		inline bool exiting() {
			return _exiting != false;
		}

		inline int state() const
		{
			return _state;
		}
		inline bool is_popup() 
		{
			return _popup != false;
		}

        bool is_active() {
            return !_other_screen_has_focus && (
                _state == E_STATE_TRANSITION_ON || 
                _state == E_STATE_ACTIVE
            );
        }

        virtual void from_json(const json&);

		void on_exit(const callback_fn& fn) { _on_exit = fn; }

        //static const std::vector<gamescreen*> all();
        static void add(gamescreen*);
        static void update_all(int);
        static void draw_all(SDL_Renderer*);
        static std::vector<gamescreen*>& all() { return _all; }

	};

}
