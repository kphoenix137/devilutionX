/**
 * @file qol/stash.h
 *
 * Interface of player stash.
 */
#pragma once

#include <cstdint>

#include "engine/point.hpp"
#include "items.h"
#include "loadsave.h"
#include "palette.h"
#include "player.h"
#include <map>

namespace devilution {

struct StashStruct {
	void RemoveStashItem(uint16_t iv);
	std::map<int, std::array<std::array<uint16_t, 10>, 10>> stashGrids;
	std::vector<Item> stashList;
	int gold;
	/** Current Page */
	int page;
};

constexpr Point InvalidStashPoint { -1, -1 };

extern bool IsStashOpen;
extern bool IsStashButtonsPressed;
extern StashStruct Stash;

/** Calculate the cell position in the panel */
Point StashCellPosition(Point slot);
void InitStash();
void FreeStashGFX();
/**
 * @brief Render the inventory panel to the given buffer.
 */
void DrawStash(const Surface &out);
void CheckStashItem(Point mousePosition, bool isShiftHeld = false, bool isCtrlHeld = false);
bool UseStashItem(uint16_t cii);
uint16_t CheckStashHLight(Point mousePosition);
void CheckStashBtnUp(Point mousePosition);
void DoStashBtn(Point mousePosition);

} // namespace devilution
