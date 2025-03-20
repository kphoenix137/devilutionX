#pragma once

#include <sol/forward.hpp>

#include "items.h"

namespace devilution {

void RegisterItemUsertype(sol::state &lua);
sol::object CreateItemObject(const Item &item);

} // namespace devilution
