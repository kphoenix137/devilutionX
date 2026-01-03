#pragma once

#include <cstdint>

#include "engine/clx_sprite.hpp"

namespace devilution {

enum class UiPanels : uint8_t {
	Main,
	Quest,
	Character,
	Spell,
	Inventory,
	Stash,
};

extern OptionalOwnedClxSpriteList PanelButton;
extern OptionalOwnedClxSpriteList PanelButtonGrime;
extern OptionalOwnedClxSpriteList PanelButtonDownGrime;

} // namespace devilution
