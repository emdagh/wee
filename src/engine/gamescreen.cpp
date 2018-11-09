#include <engine/gui/gamescreen.hpp>
#include <SDL.h>
#include <algorithm>
#include <iostream>
#include <cassert>

using namespace wee;

bool gamescreen::update_transition(int dt, int time, int direction)
{
    float td = time == 0 ? 1.0f : (float)dt / (float)time;
    _pos += td * (float)direction;
    if((_pos <= 0.f) || (_pos >= 1.0f)) {
        _pos = std::min(1.f, std::max(0.f, _pos));
        return false;
    }
    return true;
}

gamescreen::gamescreen()
    : _state(E_STATE_TRANSITION_ON)
    , _exiting(false)
    , _time_on(1000)
    , _time_off(1000)
    , _pos(1.0f)
    , _popup(false)
    , _manager(NULL)
    , _on_exit(nullptr)
{
}

gamescreen::~gamescreen()
{
}

void gamescreen::load_content()
{
}

void gamescreen::handle_input()
{
}

void gamescreen::update(int dt, bool, bool coveredByOtherScreen)
{
    assert(_manager != NULL);
    if(_exiting)
    {
        _state = E_STATE_TRANSITION_OFF;
        if(!update_transition(dt, _time_off, 1))
        {
            _manager->remove(this);
//            _exiting = false;
            if(_on_exit!= nullptr)
                _on_exit(*this);
        }
    }
    else if(coveredByOtherScreen)
    {
        if(update_transition(dt, _time_off, 1))
        {
            _state = E_STATE_TRANSITION_OFF;
        } else {
            _state = E_STATE_HIDDEN;
        }
    }
    else
    {
        if(update_transition(dt, _time_on, -1))
        {
            _state = E_STATE_TRANSITION_ON;
        } else {
            _state = E_STATE_ACTIVE;
        }
    }
}

void gamescreen::draw(SDL_Renderer* ren)
{
    if(_popup) {
        return;
    }
    int rw, rh;
    SDL_GetRendererOutputSize(ren, &rw, &rh);
    uint8_t a = (uint8_t)((transition()) * 255.f);
    SDL_Rect rc = { 0, 0, rw, rh };
    SDL_SetRenderDrawColor(ren, 0, 0, 0, a);
    SDL_RenderFillRect(ren, &rc);

}

void gamescreen::unload_content()
{
}

void gamescreen::quit()
{
    std::wcout << "exiting screen" << std::endl;
    _exiting = true;
}


void gamescreen::update_all(int dt)
{
    if(_screens.size() == 0) {
        return;
    }
    /**
     * Make a copy of the master screen list, to avoid confusion if
     * the process of updating one screen adds or removes others
     * (or it happens on another thread)
     */
    static std::vector<gamescreen*> screensToUpdate;
    screensToUpdate.clear();

    for (size_t i = 0; i < _screens.size(); i++) {
        screensToUpdate.push_back(_screens[i]);
    }

    bool otherScreenHasFocus = false; //!Game.IsActive;
    bool coveredByOtherScreen = false;

    // Loop as long as there are screens waiting to be updated.
    while (screensToUpdate.size() > 0) {
        // Pop the topmost screen off the waiting list.
        gamescreen* screen = screensToUpdate.back();
        screensToUpdate.pop_back();
        // Update the screen.
        screen->update(dt, otherScreenHasFocus, coveredByOtherScreen);

        const int state = screen->state();

        if (state == E_STATE_TRANSITION_ON || state == E_STATE_ACTIVE || state == E_STATE_TRANSITION_OFF)
        {
            // If this is the first active screen we came across,
            // give it a chance to handle input and update presence.
            if (!otherScreenHasFocus) {
                screen->handle_input();
                otherScreenHasFocus = true;
            }
            // If this is an active non-popup, inform any subsequent
            // screens that they are covered by it.
            if (!screen->is_popup()) {
                coveredByOtherScreen = true;
            }
        }
    }
}

void gamescreen_manager::draw(SDL_Renderer* ren)
{
    static std::vector<gamescreen*> screensToDraw;
    screensToDraw.clear();

    for(auto i : _screens)
        screensToDraw.emplace_back(i);

    for(auto i : screensToDraw)
    {
        if(i->state() == E_STATE_HIDDEN)
            continue;

        i->draw(ren);
    }
}
