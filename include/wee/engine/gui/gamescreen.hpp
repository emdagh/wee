#pragma once

#include <vector>
#include <algorithm>
#include <functional>

struct SDL_Renderer;

namespace wee {

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
		gamescreen_manager* _manager;
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
		void set_owner(gamescreen_manager* mgr) {
			_manager = mgr;
		}

		gamescreen_manager* get_owner() {
			return _manager;
		}

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

		void on_exit(const callback_fn& fn) { _on_exit = fn; }

        //static const std::vector<gamescreen*> all();
        static void update_all(int);
        static void draw_all(SDL_Renderer*);

	};

	class gamescreen_manager
	{
		std::vector<gamescreen*> _screens;
		public:
		gamescreen_manager();
		virtual ~gamescreen_manager();
		void add(gamescreen*);
		void remove(gamescreen*);
		void update(int, int);
		void draw(SDL_Renderer*);
	};
}
