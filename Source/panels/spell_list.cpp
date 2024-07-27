#include "panels/spell_list.hpp"

#include <cstdint>

#include <fmt/format.h>

#include "control.h"
#include "controls/plrctrls.h"
#include "engine.h"
#include "engine/backbuffer_state.hpp"
#include "engine/palette.h"
#include "engine/render/text_render.hpp"
#include "inv_iterators.hpp"
#include "options.h"
#include "panels/spell_icons.hpp"
#include "player.h"
#include "spells.h"
#include "utils/algorithm/container.hpp"
#include "utils/language.h"
#include "utils/str_cat.hpp"
#include "utils/utf8.hpp"

#define SPLROWICONLS 10

namespace devilution {

namespace {

void PrintSBookSpellType(const Surface &out, Point position, std::string_view text, uint8_t rectColorIndex)
{
	DrawLargeSpellIconBorder(out, position, rectColorIndex);

	// Align the spell type text with bottom of spell icon
	position += Displacement { SPLICONLENGTH / 2 - GetLineWidth(text) / 2, (IsSmallFontTall() ? -19 : -15) };

	// Then draw the text over the top
	DrawString(out, text, position, { .flags = UiFlags::ColorWhite | UiFlags::Outlined });
}

void PrintSBookHotkey(const Surface &out, Point position, const std::string_view text)
{
	// Align the hot key text with the top-right corner of the spell icon
	position += Displacement { SPLICONLENGTH - (GetLineWidth(text.data()) + 5), 5 - SPLICONLENGTH };

	// Then draw the text over the top
	DrawString(out, text, position, { .flags = UiFlags::ColorWhite | UiFlags::Outlined });
}

bool GetSpellListSelection(SpellID &pSpell, SpellType &pSplType)
{
	pSpell = SpellID::Invalid;
	pSplType = SpellType::Invalid;
	Player &myPlayer = *MyPlayer;

	for (auto &spellListItem : GetSpellListItems()) {
		if (spellListItem.isSelected) {
			pSpell = spellListItem.id;
			pSplType = spellListItem.type;
			if (myPlayer.heroClass == HeroClass::Monk && spellListItem.id == SpellID::Search)
				pSplType = SpellType::Skill;
			return true;
		}
	}

	return false;
}

std::optional<std::string_view> GetHotkeyName(SpellID spellId, SpellType spellType, bool useShortName = false)
{
	Player &myPlayer = *MyPlayer;
	for (size_t t = 0; t < NumHotkeys; t++) {
		if (myPlayer.hotkeySpell[t] != spellId || myPlayer.hotkeySpellType[t] != spellType)
			continue;
		auto quickSpellActionKey = StrCat("QuickSpell", t + 1);
		if (ControlMode == ControlTypes::Gamepad)
			return sgOptions.Padmapper.InputNameForAction(quickSpellActionKey, useShortName);
		return sgOptions.Keymapper.KeyNameForAction(quickSpellActionKey);
	}
	return {};
}

} // namespace

void DrawSpell(const Surface &out)
{
	Player &myPlayer = *MyPlayer;
	SpellID spl = myPlayer.selectedSpell;
	SpellType st = myPlayer.selectedSpellType;

	if (!IsValidSpell(spl)) {
		st = SpellType::Invalid;
		spl = SpellID::Null;
	}

	if (st == SpellType::Spell) {
		int tlvl = myPlayer.GetSpellLevel(spl);
		if (CheckSpell(*MyPlayer, spl, st, true) != SpellCheckResult::Success)
			st = SpellType::Invalid;
		if (tlvl <= 0)
			st = SpellType::Invalid;
	}

	if (leveltype == DTYPE_TOWN && st != SpellType::Invalid && !GetSpellData(spl).isAllowedInTown())
		st = SpellType::Invalid;

	SetSpellTrans(st);
	const Point position = GetMainPanel().position + Displacement { 565, 119 };
	DrawLargeSpellIcon(out, position, spl);

	std::optional<std::string_view> hotkeyName = GetHotkeyName(spl, myPlayer.selectedSpellType, true);
	if (hotkeyName)
		PrintSBookHotkey(out, position, *hotkeyName);
}

void DrawSpellList(const Surface &out)
{
	InfoString = StringOrView {};

	const Player &myPlayer = *MyPlayer;

	for (auto &spellListItem : GetSpellListItems()) {
		const SpellID spellId = spellListItem.id;
		SpellType transType = spellListItem.type;
		int spellLevel = 0;
		const SpellData &spellDataItem = GetSpellData(spellListItem.id);
		if (leveltype == DTYPE_TOWN && !spellDataItem.isAllowedInTown()) {
			transType = SpellType::Invalid;
		}
		if (spellListItem.type == SpellType::Spell) {
			spellLevel = myPlayer.GetSpellLevel(spellListItem.id);
			if (spellLevel == 0)
				transType = SpellType::Invalid;
		}

		SetSpellTrans(transType);
		DrawLargeSpellIcon(out, spellListItem.location, spellId);

		std::optional<std::string_view> shortHotkeyName = GetHotkeyName(spellId, spellListItem.type, true);

		if (shortHotkeyName)
			PrintSBookHotkey(out, spellListItem.location, *shortHotkeyName);

		if (!spellListItem.isSelected)
			continue;

		uint8_t spellColor = PAL16_GRAY + 5;

		switch (spellListItem.type) {
		case SpellType::Skill:
			spellColor = PAL16_YELLOW - 46;
			PrintSBookSpellType(out, spellListItem.location, _("Skill"), spellColor);
			InfoString = fmt::format(fmt::runtime(_("{:s} Skill")), pgettext("spell", spellDataItem.sNameText));
			break;
		case SpellType::Spell:
			if (!myPlayer.isOnLevel(0)) {
				spellColor = PAL16_BLUE + 5;
			}
			PrintSBookSpellType(out, spellListItem.location, _("Spell"), spellColor);
			InfoString = fmt::format(fmt::runtime(_("{:s} Spell")), pgettext("spell", spellDataItem.sNameText));
			if (spellId == SpellID::HolyBolt) {
				AddPanelString(_("Damages undead only"));
			}
			if (spellLevel == 0)
				AddPanelString(_("Spell Level 0 - Unusable"));
			else
				AddPanelString(fmt::format(fmt::runtime(_("Spell Level {:d}")), spellLevel));
			break;
		case SpellType::Scroll: {
			if (!myPlayer.isOnLevel(0)) {
				spellColor = PAL16_RED - 59;
			}
			PrintSBookSpellType(out, spellListItem.location, _("Scroll"), spellColor);
			InfoString = fmt::format(fmt::runtime(_("Scroll of {:s}")), pgettext("spell", spellDataItem.sNameText));
			const int scrollCount = c_count_if(InventoryAndBeltPlayerItemsRange { myPlayer }, [spellId](const Item &item) {
				return item.isScrollOf(spellId);
			});
			AddPanelString(fmt::format(fmt::runtime(ngettext("{:d} Scroll", "{:d} Scrolls", scrollCount)), scrollCount));
		} break;
		case SpellType::Charges: {
			if (!myPlayer.isOnLevel(0)) {
				spellColor = PAL16_ORANGE + 5;
			}
			PrintSBookSpellType(out, spellListItem.location, _("Staff"), spellColor);
			InfoString = fmt::format(fmt::runtime(_("Staff of {:s}")), pgettext("spell", spellDataItem.sNameText));
			int charges = myPlayer.bodySlot[INVLOC_HAND_LEFT]._iCharges;
			AddPanelString(fmt::format(fmt::runtime(ngettext("{:d} Charge", "{:d} Charges", charges)), charges));
		} break;
		case SpellType::Invalid:
			break;
		}
		std::optional<std::string_view> fullHotkeyName = GetHotkeyName(spellId, spellListItem.type);
		if (fullHotkeyName) {
			AddPanelString(fmt::format(fmt::runtime(_("Spell Hotkey {:s}")), *fullHotkeyName));
		}
	}
}

std::vector<SpellListItem> GetSpellListItems()
{
	std::vector<SpellListItem> spellListItems;

	uint64_t mask;
	const Point mainPanelPosition = GetMainPanel().position;

	int x = mainPanelPosition.x + 12 + SPLICONLENGTH * SPLROWICONLS;
	int y = mainPanelPosition.y - 17;

	for (auto i : enum_values<SpellType>()) {
		Player &myPlayer = *MyPlayer;
		switch (static_cast<SpellType>(i)) {
		case SpellType::Skill:
			mask = myPlayer.skills;
			break;
		case SpellType::Spell:
			mask = myPlayer.learnedSpells;
			break;
		case SpellType::Scroll:
			mask = myPlayer.scrollSpells;
			break;
		case SpellType::Charges:
			mask = myPlayer.staffSpells;
			break;
		default:
			continue;
		}
		int8_t j = static_cast<int8_t>(SpellID::Firebolt);
		for (uint64_t spl = 1; j < MAX_SPELLS; spl <<= 1, j++) {
			if ((mask & spl) == 0)
				continue;
			int lx = x;
			int ly = y - SPLICONLENGTH;
			bool isSelected = (MousePosition.x >= lx && MousePosition.x < lx + SPLICONLENGTH && MousePosition.y >= ly && MousePosition.y < ly + SPLICONLENGTH);
			spellListItems.emplace_back(SpellListItem { { x, y }, static_cast<SpellType>(i), static_cast<SpellID>(j), isSelected });
			x -= SPLICONLENGTH;
			if (x == mainPanelPosition.x + 12 - SPLICONLENGTH) {
				x = mainPanelPosition.x + 12 + SPLICONLENGTH * SPLROWICONLS;
				y -= SPLICONLENGTH;
			}
		}
		if (mask != 0 && x != mainPanelPosition.x + 12 + SPLICONLENGTH * SPLROWICONLS)
			x -= SPLICONLENGTH;
		if (x == mainPanelPosition.x + 12 - SPLICONLENGTH) {
			x = mainPanelPosition.x + 12 + SPLICONLENGTH * SPLROWICONLS;
			y -= SPLICONLENGTH;
		}
	}

	return spellListItems;
}

void SetSpell()
{
	SpellID pSpell;
	SpellType pSplType;

	spselflag = false;
	if (!GetSpellListSelection(pSpell, pSplType)) {
		return;
	}

	Player &myPlayer = *MyPlayer;
	myPlayer.selectedSpell = pSpell;
	myPlayer.selectedSpellType = pSplType;

	RedrawEverything();
}

void SetSpeedSpell(size_t slot)
{
	SpellID pSpell;
	SpellType pSplType;

	if (!GetSpellListSelection(pSpell, pSplType)) {
		return;
	}
	Player &myPlayer = *MyPlayer;

	if (myPlayer.hotkeySpell[slot] == pSpell && myPlayer.hotkeySpellType[slot] == pSplType) {
		// Unset spell hotkey
		myPlayer.hotkeySpell[slot] = SpellID::Invalid;
		return;
	}

	for (size_t i = 0; i < NumHotkeys; ++i) {
		if (myPlayer.hotkeySpell[i] == pSpell && myPlayer.hotkeySpellType[i] == pSplType)
			myPlayer.hotkeySpell[i] = SpellID::Invalid;
	}
	myPlayer.hotkeySpell[slot] = pSpell;
	myPlayer.hotkeySpellType[slot] = pSplType;
}

bool IsValidSpeedSpell(size_t slot)
{
	uint64_t spells;

	Player &myPlayer = *MyPlayer;

	const SpellID spellId = myPlayer.hotkeySpell[slot];
	if (!IsValidSpell(spellId)) {
		return false;
	}

	switch (myPlayer.hotkeySpellType[slot]) {
	case SpellType::Skill:
		spells = myPlayer.skills;
		break;
	case SpellType::Spell:
		spells = myPlayer.learnedSpells;
		break;
	case SpellType::Scroll:
		spells = myPlayer.scrollSpells;
		break;
	case SpellType::Charges:
		spells = myPlayer.staffSpells;
		break;
	case SpellType::Invalid:
		return false;
	}

	return (spells & GetSpellBitmask(spellId)) != 0;
}

void ToggleSpell(size_t slot)
{
	if (IsValidSpeedSpell(slot)) {
		Player &myPlayer = *MyPlayer;
		myPlayer.selectedSpell = myPlayer.hotkeySpell[slot];
		myPlayer.selectedSpellType = myPlayer.hotkeySpellType[slot];
		RedrawEverything();
	}
}

void DoSpeedBook()
{
	spselflag = true;
	const Point mainPanelPosition = GetMainPanel().position;
	int xo = mainPanelPosition.x + 12 + SPLICONLENGTH * 10;
	int yo = mainPanelPosition.y - 17;
	int x = xo + SPLICONLENGTH / 2;
	int y = yo - SPLICONLENGTH / 2;

	Player &myPlayer = *MyPlayer;

	if (IsValidSpell(myPlayer.selectedSpell)) {
		for (auto i : enum_values<SpellType>()) {
			uint64_t spells;
			switch (static_cast<SpellType>(i)) {
			case SpellType::Skill:
				spells = myPlayer.skills;
				break;
			case SpellType::Spell:
				spells = myPlayer.learnedSpells;
				break;
			case SpellType::Scroll:
				spells = myPlayer.scrollSpells;
				break;
			case SpellType::Charges:
				spells = myPlayer.staffSpells;
				break;
			default:
				continue;
			}
			uint64_t spell = 1;
			for (int j = 1; j < MAX_SPELLS; j++) {
				if ((spell & spells) != 0) {
					if (j == static_cast<int8_t>(myPlayer.selectedSpell) && static_cast<SpellType>(i) == myPlayer.selectedSpellType) {
						x = xo + SPLICONLENGTH / 2;
						y = yo - SPLICONLENGTH / 2;
					}
					xo -= SPLICONLENGTH;
					if (xo == mainPanelPosition.x + 12 - SPLICONLENGTH) {
						xo = mainPanelPosition.x + 12 + SPLICONLENGTH * SPLROWICONLS;
						yo -= SPLICONLENGTH;
					}
				}
				spell <<= 1ULL;
			}
			if (spells != 0 && xo != mainPanelPosition.x + 12 + SPLICONLENGTH * SPLROWICONLS)
				xo -= SPLICONLENGTH;
			if (xo == mainPanelPosition.x + 12 - SPLICONLENGTH) {
				xo = mainPanelPosition.x + 12 + SPLICONLENGTH * SPLROWICONLS;
				yo -= SPLICONLENGTH;
			}
		}
	}

	SetCursorPos({ x, y });
}

} // namespace devilution
