#pragma once

#ifdef __vita__

#include <SDL.h>

#include "engine/point.hpp"

namespace devilution {

void HandleTouchEvent(SDL_Event *event, Point mousePosition);
void finish_simulated_mouse_clicks(Point mousePosition);

} // namespace devilution

#endif
