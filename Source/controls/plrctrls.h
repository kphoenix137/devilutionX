#pragma once
// Controller actions implementation

#include <cstdint>

#include "controls/controller.h"
#include "player.h"

namespace devilution {

typedef enum belt_item_type : uint8_t {
	BLT_HEALING,
	BLT_MANA,
} belt_item_type;

enum class ControlTypes : uint8_t {
	None,
	KeyboardAndMouse,
	Gamepad,
	VirtualGamepad,
};

extern ControlTypes ControlMode;

// Runs every frame.
// Handles menu movement.
void plrctrls_every_frame();

// Run after every game logic iteration.
// Handles player movement.
void plrctrls_after_game_logic();

// Runs at the end of CheckCursMove()
// Handles item, object, and monster auto-aim.
void plrctrls_after_check_curs_move();

// Moves the map if active, the cursor otherwise.
void HandleRightStickMotion();

// Whether we're in a dialog menu that the game handles natively with keyboard controls.
bool InGameMenu();

void SetPointAndClick(bool value);

bool IsPointAndClick();

void DetectInputMethod(const SDL_Event &event, const ControllerButtonEvent &gamepadEvent);

// Whether the automap is being displayed.
bool IsAutomapActive();

void UseBeltItem(int type);

// Talk to towners, click on inv items, attack, etc.
void PerformPrimaryAction();

// Open chests, doors, pickup items.
void PerformSecondaryAction();
bool TryDropItem();
void InvalidateInventorySlot();
void FocusOnInventory();
void PerformSpellAction();
void QuickCast(int slot);

extern int speedspellcount;

} // namespace devilution
