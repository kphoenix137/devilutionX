/**
 * @file stash.cpp
 *
 * Implementation of player stash.
 */
#include <utility>

#include <algorithm>
#include <fmt/format.h>

#include "controls/plrctrls.h"
#include "cursor.h"
#include "engine/cel_sprite.hpp"
#include "engine/load_cel.hpp"
#include "engine/points_in_rectangle_range.hpp"
#include "engine/rectangle.hpp"
#include "engine/render/cel_render.hpp"
#include "engine/render/text_render.hpp"
#include "engine/size.hpp"
#include "hwcursor.hpp"
#include "inv_iterators.hpp"
#include "loadsave.h"
#include "minitext.h"
#include "options.h"
#include "pfile.h"
#include "plrmsg.h"
#include "qol/stash.h"
#include "stores.h"
#include "town.h"
#include "towners.h"
#include "utils/language.h"
#include "utils/paths.h"
#include "utils/sdl_geometry.h"
#include "utils/stdcompat/optional.hpp"
#include <fstream>
#include <iostream>

namespace devilution {

bool IsStashOpen;
StashStruct Stash;

namespace {

// position for stash buttons
constexpr int StashNavY = 15;
constexpr int StashNavW = 21;
constexpr int StashNavH = 19;

/** Contains mappings for the buttons in the stash (2 navigation buttons, withdraw gold buttons, 2 navigation buttons) */
constexpr Rectangle StashButtonRect[] = {
	// clang-format off
	{ {  35, StashNavY }, { StashNavW, StashNavH } }, // 10 left
	{ {  63, StashNavY }, { StashNavW, StashNavH } }, // 1 left
	{ {  92,        16 }, {        27,        19 } }, // withdraw gold
	{ { 240, StashNavY }, { StashNavW, StashNavH } }, // 1 right
	{ { 268, StashNavY }, { StashNavW, StashNavH } }  // 10 right
	// clang-format on
};

constexpr int StashCelY = 33;

constexpr Point StashButtonCelRect[] = {
	// clang-format off
	{  35, StashCelY }, // 10 left
	{  63, StashCelY }, // 1 left
	{  92, StashCelY }, // withdraw gold
	{ 240, StashCelY }, // 1 right
	{ 268, StashCelY }, // 10 right
	// clang-format on
};

std::optional<OwnedCelSprite> StashPanelArt;
std::optional<OwnedCelSprite> StashNavButtonArt;
std::optional<OwnedCelSprite> StashGoldButtonArt;

/**
 * @param stashListIndex The item's StashList index
 * @param itemSize Size of item
 */
void AddItemToStashGrid(Point position, uint16_t stashListIndex, Size itemSize)
{
	for (auto point : PointsInRectangleRange({ { 0, 0 }, itemSize })) {
		Stash.stashGrids[Stash.page][position.x + point.x][position.y + point.y] = stashListIndex + 1;
	}
}

Point FindSlotUnderCursor(Point cursorPosition)
{
	if ((icursSize28.width & 1) == 0)
		cursorPosition.x -= INV_SLOT_HALF_SIZE_PX;
	if ((icursSize28.height & 1) == 0)
		cursorPosition.y -= INV_SLOT_HALF_SIZE_PX;
	cursorPosition.y -= (icursSize28.height - 1) / 2 * INV_SLOT_SIZE_PX;

	for (auto point : PointsInRectangleRange({ { 0, 0 }, { 10, 10 } })) {
		Rectangle cell {
			GetPanelPosition(UiPanels::Stash, StashCellPosition(point)),
			InventorySlotSizeInPixels
		};

		if (cell.Contains(cursorPosition)) {
			return point;
		}
	}

	return InvalidStashPoint;
}

void CheckStashPaste(Point cursorPosition)
{
	auto &player = Players[MyPlayerId];

	SetICursor(player.HoldItem._iCurs + CURSOR_FIRSTITEM);
	if (!IsHardwareCursor()) {
		cursorPosition += Displacement(icursSize / 2);
	}

	if (player.HoldItem._itype == ItemType::Gold) {
		Stash.gold += player.HoldItem._ivalue;
		player.HoldItem._itype = ItemType::None;
		if (!IsHardwareCursor())
			SetCursorPos(cursorPosition);
		NewCursor(CURSOR_HAND);
		return;
	}

	Point firstSlot = FindSlotUnderCursor(cursorPosition);
	if (firstSlot == InvalidStashPoint)
		return;

	const Size itemSize = icursSize28;

	if (firstSlot.x + itemSize.width > 10 || firstSlot.y + itemSize.height > 10) {
		return; // Item does not fit
	}

	// Check that no more then 1 item is replaced by the move
	uint16_t it = 0;
	for (auto point : PointsInRectangleRange({ { 0, 0 }, itemSize })) {
		uint16_t iv = Stash.stashGrids[Stash.page][firstSlot.x + point.x][firstSlot.y + point.y];
		if (iv == 0 || it == iv)
			continue;
		if (it == 0) {
			it = iv; // Found first item
			continue;
		}
		return; // Found a second item
	}

	PlaySFX(ItemInvSnds[ItemCAnimTbl[player.HoldItem._iCurs]]);

	player.HoldItem.position = firstSlot + Displacement { 0, itemSize.height - 1 };

	int cn = CURSOR_HAND;
	uint16_t stashIndex;
	if (it == 0) {
		Stash.stashList.push_back(player.HoldItem);
		stashIndex = Stash.stashList.size() - 1;
	} else {
		stashIndex = it - 1;
		cn = SwapItem(Stash.stashList[stashIndex], player.HoldItem);
		for (auto &row : Stash.stashGrids[Stash.page]) {
			for (auto &itemId : row) {
				if (itemId == it)
					itemId = 0;
			}
		}
	}

	AddItemToStashGrid(firstSlot, stashIndex, itemSize);

	if (cn == CURSOR_HAND && !IsHardwareCursor())
		SetCursorPos(cursorPosition);
	NewCursor(cn);
}

void CheckStashCut(Point cursorPosition, bool automaticMove, bool dropItem)
{
	auto &player = Players[MyPlayerId];

	if (withdrawGoldFlag) {
		withdrawGoldFlag = false;
		withdrawGoldValue = 0;
	}

	Point slot = InvalidStashPoint;

	for (auto point : PointsInRectangleRange({ { 0, 0 }, { 10, 10 } })) {
		Rectangle cell {
			GetPanelPosition(UiPanels::Stash, StashCellPosition(point)),
			{ InventorySlotSizeInPixels.width + 1, InventorySlotSizeInPixels.height + 1 }
		};

		// check which inventory rectangle the mouse is in, if any
		if (cell.Contains(cursorPosition)) {
			slot = point;
			break;
		}
	}

	if (slot == InvalidStashPoint) {
		return;
	}

	Item &holdItem = player.HoldItem;
	holdItem._itype = ItemType::None;

	bool automaticallyMoved = false;
	bool automaticallyEquipped = false;

	uint16_t ii = Stash.stashGrids[Stash.page][slot.x][slot.y];
	if (ii != 0) {
		uint16_t iv = ii - 1;

		holdItem = Stash.stashList[iv];
		if (automaticMove) {
			if (CanBePlacedOnBelt(holdItem)) {
				automaticallyMoved = AutoPlaceItemInBelt(player, holdItem, true);
			} else {
				automaticallyMoved = automaticallyEquipped = AutoEquip(MyPlayerId, holdItem);
			}
		}

		if (!automaticMove || automaticallyMoved) {
			Stash.RemoveStashItem(iv);
		}
	}

	if (!holdItem.isEmpty()) {
		CalcPlrInv(player, true);
		CheckItemStats(player);
		if (automaticallyEquipped) {
			PlaySFX(ItemInvSnds[ItemCAnimTbl[holdItem._iCurs]]);
		} else if (!automaticMove || automaticallyMoved) {
			PlaySFX(IS_IGRAB);
		}

		if (automaticMove) {
			if (!automaticallyMoved) {
				if (CanBePlacedOnBelt(holdItem)) {
					player.SaySpecific(HeroSpeech::IHaveNoRoom);
				} else {
					player.SaySpecific(HeroSpeech::ICantDoThat);
				}
			}

			holdItem._itype = ItemType::None;
		} else {
			NewCursor(holdItem._iCurs + CURSOR_FIRSTITEM);
			if (!IsHardwareCursor() && !dropItem) {
				// For a hardware cursor, we set the "hot point" to the center of the item instead.
				SetCursorPos(cursorPosition - Displacement(cursSize / 2));
			}
		}
	}

	if (dropItem) {
		TryDropItem();
	}
}

void StartGoldWithdraw()
{
	initialWithdrawGoldValue = Stash.gold;

	if (talkflag)
		control_reset_talk();

	Point start = GetPanelPosition(UiPanels::Stash, { 67, 128 });
	SDL_Rect rect = MakeSdlRect(start.x, start.y, 180, 20);
	SDL_SetTextInputRect(&rect);

	withdrawGoldFlag = true;
	withdrawGoldValue = 0;
	SDL_StartTextInput();
}

} // namespace

Point StashCellPosition(Point slot)
{
	constexpr int StashNextCell = INV_SLOT_SIZE_PX + 1; // spacing between each cell

	return slot * StashNextCell + Displacement { 17, 47 };
}

void FreeStashGFX()
{
	StashPanelArt = std::nullopt;
	StashNavButtonArt = std::nullopt;
	StashGoldButtonArt = std::nullopt;
}

void InitStash()
{
	StashPanelArt = LoadCel("data\\stash.cel", SPANEL_WIDTH);
	StashNavButtonArt = LoadCel("data\\stashnavbtns.cel", 21);
	StashGoldButtonArt = LoadCel("data\\stashgoldbtn.cel", 27);
}

bool StashButtons[5];
bool IsStashButtonsPressed;

void CheckStashBtnUp(Point mousePosition)
{
	Rectangle stashButton;
	IsStashButtonsPressed = false;

	for (int i = 0; i < 5; i++) {
		if (!StashButtons[i]) {
			continue;
		}
		StashButtons[i] = false;
		stashButton = StashButtonRect[i];
		stashButton.position = GetPanelPosition(UiPanels::Stash, stashButton.position);
		if (!stashButton.Contains(mousePosition)) {
			continue;
		}
		switch (i) {
		case 0:
			Stash.page -= 10;
			break;
		case 1:
			Stash.page -= 1;
			break;
		case 2:
			StartGoldWithdraw();
			break;
		case 3:
			Stash.page += 1;
			break;
		case 4:
			Stash.page += 10;
			break;
		}
	}
	if (Stash.page < 0)
		Stash.page = 0;
}

void DoStashBtn(Point mousePosition)
{
	Rectangle stashButton;

	for (int i = 0; i < 5; i++) {
		stashButton = StashButtonRect[i];
		stashButton.position = GetPanelPosition(UiPanels::Stash, stashButton.position);
		if (stashButton.Contains(mousePosition)) {
			StashButtons[i] = true;
			IsStashButtonsPressed = true;
		}
	}
}

void DrawStash(const Surface &out)
{
	CelDrawTo(out, GetPanelPosition(UiPanels::Stash, { 0, 351 }), *StashPanelArt, 1);
	for (unsigned buttonId = 0; buttonId < sizeof(StashButtons); buttonId++) {
		if (StashButtons[buttonId]) {
			Point stashButton = GetPanelPosition(UiPanels::Stash, StashButtonCelRect[buttonId]);
			switch (buttonId) {
			case 0:
				CelDrawTo(out, stashButton, *StashNavButtonArt, 1);
				break;
			case 1:
				CelDrawTo(out, stashButton, *StashNavButtonArt, 2);
				break;
			case 2:
				CelDrawTo(out, stashButton, *StashGoldButtonArt, 1);
				break;
			case 3:
				CelDrawTo(out, stashButton, *StashNavButtonArt, 3);
				break;
			case 4:
				CelDrawTo(out, stashButton, *StashNavButtonArt, 4);
				break;
			}
		}
	}

	for (auto point : PointsInRectangleRange({ { 0, 0 }, { 10, 10 } })) {
		if (Stash.stashGrids[Stash.page][point.x][point.y] != 0) {
			const Point position = GetPanelPosition(UiPanels::Stash, StashCellPosition(point)) + Displacement { 0, INV_SLOT_SIZE_PX - 1 };
			InvDrawSlotBack(out, position, InventorySlotSizeInPixels);
		}
	}

	for (auto point : PointsInRectangleRange({ { 0, 0 }, { 10, 10 } })) {
		if (Stash.stashGrids[Stash.page][point.x][point.y] == 0) {
			continue; // Not the first slot of an item
		}

		uint16_t ii = Stash.stashGrids[Stash.page][point.x][point.y] - 1;
		Item &item = Stash.stashList[ii];
		if (item.position != point) {
			continue;
		}
		int frame = item._iCurs + CURSOR_FIRSTITEM;

		const auto &cel = GetInvItemSprite(frame);
		const int celFrame = GetInvItemFrame(frame);
		const Point position = GetPanelPosition(UiPanels::Stash, StashCellPosition(point)) + Displacement { 0, INV_SLOT_SIZE_PX - 1 };
		if (pcursstashitem == ii) {
			CelBlitOutlineTo(
			    out,
			    GetOutlineColor(Stash.stashList[ii], true),
			    position,
			    cel, celFrame, false);
		}

		CelDrawItem(Stash.stashList[ii], out, position, cel, celFrame);
	}

	Point pos = GetPanelPosition(UiPanels::Stash, { 0, 0 });
	UiFlags style = UiFlags::AlignCenter | UiFlags::VerticalCenter | UiFlags::ColorWhite;

	DrawString(out, fmt::format("{:d}", Stash.page + 1), { pos + Displacement { 11, 14 }, { 16, 18 } }, style);
	DrawString(out, fmt::format("{:d}", Stash.gold), { pos + Displacement { 128, 14 }, { 99, 18 } }, style);
}

void CheckStashItem(Point mousePosition, bool isShiftHeld, bool isCtrlHeld)
{
	if (pcurs >= CURSOR_FIRSTITEM) {
		CheckStashPaste(mousePosition);
	} else {
		CheckStashCut(mousePosition, isShiftHeld, isCtrlHeld);
	}
	if (gbIsMultiplayer) {
		sfile_write_stash();
	}
}

uint16_t CheckStashHLight(Point mousePosition)
{
	Point slot = InvalidStashPoint;
	for (auto point : PointsInRectangleRange({ { 0, 0 }, { 10, 10 } })) {
		Rectangle cell {
			GetPanelPosition(UiPanels::Stash, StashCellPosition(point)),
			{ InventorySlotSizeInPixels.width + 1, InventorySlotSizeInPixels.height + 1 }
		};

		if (cell.Contains({ mousePosition })) {
			slot = point;
			break;
		}
	}

	if (slot == InvalidStashPoint)
		return -1;

	InfoColor = UiFlags::ColorWhite;

	ClearPanel();

	uint16_t itemId = abs(Stash.stashGrids[Stash.page][slot.x][slot.y]);
	if (itemId == 0)
		return -1;

	uint16_t ii = itemId - 1;
	Item &item = Stash.stashList[ii];

	if (item.isEmpty())
		return -1;

	InfoColor = item.getTextColor();
	if (item._iIdentified) {
		strcpy(infostr, item._iIName);
		PrintItemDetails(item);
	} else {
		strcpy(infostr, item._iName);
		PrintItemDur(item);
	}

	return ii;
}

bool UseStashItem(uint16_t c)
{
	if (MyPlayer->_pInvincible && MyPlayer->_pHitPoints == 0)
		return true;
	if (pcurs != CURSOR_HAND)
		return true;
	if (stextflag != STORE_NONE)
		return true;

	Item *item = &Stash.stashList[c];

	constexpr int SpeechDelay = 10;
	if (item->IDidx == IDI_MUSHROOM) {
		MyPlayer->Say(HeroSpeech::NowThatsOneBigMushroom, SpeechDelay);
		return true;
	}
	if (item->IDidx == IDI_FUNGALTM) {
		PlaySFX(IS_IBOOK);
		MyPlayer->Say(HeroSpeech::ThatDidntDoAnything, SpeechDelay);
		return true;
	}

	if (!AllItemsList[item->IDidx].iUsable)
		return false;

	if (!MyPlayer->CanUseItem(*item)) {
		MyPlayer->Say(HeroSpeech::ICantUseThisYet);
		return true;
	}

	if (withdrawGoldFlag) {
		withdrawGoldFlag = false;
		withdrawGoldValue = 0;
	}

	if (item->IsScroll() && currlevel == 0 && !spelldata[item->_iSpell].sTownSpell) {
		return true;
	}

	if (item->_iMiscId > IMISC_RUNEFIRST && item->_iMiscId < IMISC_RUNELAST && currlevel == 0) {
		return true;
	}

	if (item->_iMiscId == IMISC_BOOK)
		PlaySFX(IS_RBOOK);
	else
		PlaySFX(ItemInvSnds[ItemCAnimTbl[item->_iCurs]]);

	UseItem(MyPlayerId, item->_iMiscId, item->_iSpell);

	if (Stash.stashList[c]._iMiscId == IMISC_MAPOFDOOM)
		return true;
	if (Stash.stashList[c]._iMiscId == IMISC_NOTE) {
		InitQTextMsg(TEXT_BOOK9);
		IsStashOpen = false;
		return true;
	}
	Stash.RemoveStashItem(c);

	return true;
}

void StashStruct::RemoveStashItem(uint16_t iv)
{
	// Iterate through stashGrid and remove every reference to item
	for (auto &row : Stash.stashGrids[Stash.page]) {
		for (uint16_t &itemId : row) {
			if (itemId - 1 == iv) {
				itemId = 0;
			}
		}
	}

	if (stashList.empty()) {
		return;
	}

	// If the item at the end of stash array isn't the one we removed, we need to swap its position in the array with the removed item
	std::size_t lastItemIndex = stashList.size() - 1;
	if (lastItemIndex != iv) {
		stashList[iv] = stashList[lastItemIndex];

		for (auto &pair : Stash.stashGrids) {
			auto &grid = pair.second;
			for (auto &row : grid) {
				for (uint16_t &itemId : row) {
					if (itemId == lastItemIndex + 1) {
						itemId = iv + 1;
					}
				}
			}
		}
	}
	stashList.pop_back();
}

} // namespace devilution
