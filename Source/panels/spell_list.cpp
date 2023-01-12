#include "panels/spell_list.hpp"

#include <fmt/format.h>

#include "control.h"
#include "engine.h"
#include "engine/backbuffer_state.hpp"
#include "engine/palette.h"
#include "engine/render/text_render.hpp"
#include "inv_iterators.hpp"
#include "miniwin/misc_msg.h"
#include "options.h"
#include "panels/spell_icons.hpp"
#include "player.h"
#include "spells.h"
#include "utils/language.h"
#include "utils/str_cat.hpp"
#include "utils/utf8.hpp"

#define SPLROWICONLS 10

namespace devilution {

namespace {

void PrintSBookSpellType(const Surface &out, Point position, string_view text, uint8_t rectColorIndex)
{
	DrawLargeSpellIconBorder(out, position, rectColorIndex);

	// Align the spell type text with bottom of spell icon
	position += Displacement { SPLICONLENGTH / 2 - GetLineWidth(text) / 2, (IsSmallFontTall() ? -19 : -15) };

	// Then draw the text over the top
	DrawString(out, text, position, UiFlags::ColorWhite | UiFlags::Outlined);
}

void PrintSBookHotkey(const Surface &out, Point position, const string_view text)
{
	// Align the hot key text with the top-right corner of the spell icon
	position += Displacement { SPLICONLENGTH - (GetLineWidth(text.data()) + 5), 5 - SPLICONLENGTH };

	// Then draw the text over the top
	DrawString(out, text, position, UiFlags::ColorWhite | UiFlags::Outlined);
}

bool GetSpellListSelection(SpellID &pSpell, spell_type &pSplType)
{
	pSpell = SpellID::Invalid;
	pSplType = spell_type::RSPLTYPE_INVALID;
	Player &myPlayer = *MyPlayer;

	for (auto &spellListItem : GetSpellListItems()) {
		if (spellListItem.isSelected) {
			pSpell = spellListItem.id;
			pSplType = spellListItem.type;
			if (myPlayer._pClass == HeroClass::Monk && spellListItem.id == SpellID::Search)
				pSplType = RSPLTYPE_SKILL;
			return true;
		}
	}

	return false;
}

std::optional<string_view> GetHotkeyName(SpellID spellId, spell_type spellType)
{
	Player &myPlayer = *MyPlayer;
	for (size_t t = 0; t < NumHotkeys; t++) {
		if (myPlayer._pSplHotKey[t] != spellId || myPlayer._pSplTHotKey[t] != spellType)
			continue;
		auto quickSpellActionKey = StrCat("QuickSpell", t + 1);
		return sgOptions.Keymapper.KeyNameForAction(quickSpellActionKey);
	}
	return {};
}

} // namespace

void DrawSpell(const Surface &out)
{
	Player &myPlayer = *MyPlayer;
	SpellID spl = myPlayer._pRSpell;
	spell_type st = myPlayer._pRSplType;

	if (!IsValidSpell(spl)) {
		st = RSPLTYPE_INVALID;
		spl = SpellID::Null;
	}

	if (st == RSPLTYPE_SPELL) {
		int tlvl = myPlayer.GetSpellLevel(spl);
		if (CheckSpell(*MyPlayer, spl, st, true) != SpellCheckResult::Success)
			st = RSPLTYPE_INVALID;
		if (tlvl <= 0)
			st = RSPLTYPE_INVALID;
	}

	if (leveltype == DTYPE_TOWN && st != RSPLTYPE_INVALID && !spelldata[static_cast<int8_t>(spl)].sTownSpell)
		st = RSPLTYPE_INVALID;

	SetSpellTrans(st);
	const Point position = GetMainPanel().position + Displacement { 565, 119 };
	DrawLargeSpellIcon(out, position, spl);

	std::optional<string_view> hotkeyName = GetHotkeyName(spl, myPlayer._pRSplType);
	if (hotkeyName)
		PrintSBookHotkey(out, position, *hotkeyName);
}

void DrawSpellList(const Surface &out)
{
	InfoString = {};

	Player &myPlayer = *MyPlayer;

	for (auto &spellListItem : GetSpellListItems()) {
		const SpellID spellId = spellListItem.id;
		spell_type transType = spellListItem.type;
		int spellLevel = 0;
		const SpellData &spellDataItem = spelldata[static_cast<size_t>(spellListItem.id)];
		if (leveltype == DTYPE_TOWN && !spellDataItem.sTownSpell) {
			transType = RSPLTYPE_INVALID;
		}
		if (spellListItem.type == RSPLTYPE_SPELL) {
			spellLevel = myPlayer.GetSpellLevel(spellListItem.id);
			if (spellLevel == 0)
				transType = RSPLTYPE_INVALID;
		}

		SetSpellTrans(transType);
		DrawLargeSpellIcon(out, spellListItem.location, spellId);

		std::optional<string_view> hotkeyName = GetHotkeyName(spellId, spellListItem.type);

		if (hotkeyName)
			PrintSBookHotkey(out, spellListItem.location, *hotkeyName);

		if (!spellListItem.isSelected)
			continue;

		uint8_t spellColor = PAL16_GRAY + 5;

		switch (spellListItem.type) {
		case RSPLTYPE_SKILL:
			spellColor = PAL16_YELLOW - 46;
			PrintSBookSpellType(out, spellListItem.location, _("Skill"), spellColor);
			InfoString = fmt::format(fmt::runtime(_("{:s} Skill")), pgettext("spell", spellDataItem.sNameText));
			break;
		case RSPLTYPE_SPELL:
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
		case RSPLTYPE_SCROLL: {
			if (!myPlayer.isOnLevel(0)) {
				spellColor = PAL16_RED - 59;
			}
			PrintSBookSpellType(out, spellListItem.location, _("Scroll"), spellColor);
			InfoString = fmt::format(fmt::runtime(_("Scroll of {:s}")), pgettext("spell", spellDataItem.sNameText));
			const InventoryAndBeltPlayerItemsRange items { myPlayer };
			const int scrollCount = std::count_if(items.begin(), items.end(), [spellId](const Item &item) {
				return item.isScrollOf(spellId);
			});
			AddPanelString(fmt::format(fmt::runtime(ngettext("{:d} Scroll", "{:d} Scrolls", scrollCount)), scrollCount));
		} break;
		case RSPLTYPE_CHARGES: {
			if (!myPlayer.isOnLevel(0)) {
				spellColor = PAL16_ORANGE + 5;
			}
			PrintSBookSpellType(out, spellListItem.location, _("Staff"), spellColor);
			InfoString = fmt::format(fmt::runtime(_("Staff of {:s}")), pgettext("spell", spellDataItem.sNameText));
			int charges = myPlayer.InvBody[INVLOC_HAND_LEFT]._iCharges;
			AddPanelString(fmt::format(fmt::runtime(ngettext("{:d} Charge", "{:d} Charges", charges)), charges));
		} break;
		case RSPLTYPE_INVALID:
			break;
		}
		if (hotkeyName) {
			AddPanelString(fmt::format(fmt::runtime(_("Spell Hotkey {:s}")), *hotkeyName));
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

	for (int i = RSPLTYPE_SKILL; i < RSPLTYPE_INVALID; i++) {
		Player &myPlayer = *MyPlayer;
		switch ((spell_type)i) {
		case RSPLTYPE_SKILL:
			mask = myPlayer._pAblSpells;
			break;
		case RSPLTYPE_SPELL:
			mask = myPlayer._pMemSpells;
			break;
		case RSPLTYPE_SCROLL:
			mask = myPlayer._pScrlSpells;
			break;
		case RSPLTYPE_CHARGES:
			mask = myPlayer._pISpells;
			break;
		case RSPLTYPE_INVALID:
			break;
		}
		int8_t j = static_cast<int8_t>(SpellID::Firebolt);
		for (uint64_t spl = 1; j < MAX_SPELLS; spl <<= 1, j++) {
			if ((mask & spl) == 0)
				continue;
			int lx = x;
			int ly = y - SPLICONLENGTH;
			bool isSelected = (MousePosition.x >= lx && MousePosition.x < lx + SPLICONLENGTH && MousePosition.y >= ly && MousePosition.y < ly + SPLICONLENGTH);
			spellListItems.emplace_back(SpellListItem { { x, y }, (spell_type)i, (SpellID)j, isSelected });
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
	spell_type pSplType;

	spselflag = false;
	if (!GetSpellListSelection(pSpell, pSplType)) {
		return;
	}

	Player &myPlayer = *MyPlayer;
	myPlayer._pRSpell = pSpell;
	myPlayer._pRSplType = pSplType;

	RedrawEverything();
}

void SetSpeedSpell(size_t slot)
{
	SpellID pSpell;
	spell_type pSplType;

	if (!GetSpellListSelection(pSpell, pSplType)) {
		return;
	}
	Player &myPlayer = *MyPlayer;
	for (size_t i = 0; i < NumHotkeys; ++i) {
		if (myPlayer._pSplHotKey[i] == pSpell && myPlayer._pSplTHotKey[i] == pSplType)
			myPlayer._pSplHotKey[i] = SpellID::Invalid;
	}
	myPlayer._pSplHotKey[slot] = pSpell;
	myPlayer._pSplTHotKey[slot] = pSplType;
}

void ToggleSpell(size_t slot)
{
	uint64_t spells;

	Player &myPlayer = *MyPlayer;

	const SpellID spellId = myPlayer._pSplHotKey[slot];
	if (!IsValidSpell(spellId)) {
		return;
	}

	switch (myPlayer._pSplTHotKey[slot]) {
	case RSPLTYPE_SKILL:
		spells = myPlayer._pAblSpells;
		break;
	case RSPLTYPE_SPELL:
		spells = myPlayer._pMemSpells;
		break;
	case RSPLTYPE_SCROLL:
		spells = myPlayer._pScrlSpells;
		break;
	case RSPLTYPE_CHARGES:
		spells = myPlayer._pISpells;
		break;
	case RSPLTYPE_INVALID:
		return;
	}

	if ((spells & GetSpellBitmask(spellId)) != 0) {
		myPlayer._pRSpell = spellId;
		myPlayer._pRSplType = myPlayer._pSplTHotKey[slot];
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

	if (IsValidSpell(myPlayer._pRSpell)) {
		for (int i = RSPLTYPE_SKILL; i <= RSPLTYPE_CHARGES; i++) {
			uint64_t spells;
			switch (i) {
			case RSPLTYPE_SKILL:
				spells = myPlayer._pAblSpells;
				break;
			case RSPLTYPE_SPELL:
				spells = myPlayer._pMemSpells;
				break;
			case RSPLTYPE_SCROLL:
				spells = myPlayer._pScrlSpells;
				break;
			case RSPLTYPE_CHARGES:
				spells = myPlayer._pISpells;
				break;
			}
			uint64_t spell = 1;
			for (int j = 1; j < MAX_SPELLS; j++) {
				if ((spell & spells) != 0) {
					if (j == static_cast<int8_t>(myPlayer._pRSpell) && i == myPlayer._pRSplType) {
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
