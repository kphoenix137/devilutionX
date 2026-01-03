#include "panels/charpanel.hpp"

#include <cstdint>

#include <algorithm>
#include <string>

#include <expected.hpp>
#include <fmt/format.h>
#include <function_ref.hpp>

#include "control/control.hpp"
#include "engine/load_clx.hpp"
#include "engine/render/clx_render.hpp"
#include "engine/render/text_render.hpp"
#include "panels/ui_panels.hpp"
#include "player.h"
#include "playerdat.hpp"
#include "utils/algorithm/container.hpp"
#include "utils/display.h"
#include "utils/enum_traits.h"
#include "utils/format_int.hpp"
#include "utils/language.h"
#include "utils/status_macros.hpp"
#include "utils/str_cat.hpp"
#include "utils/surface_to_clx.hpp"

namespace devilution {

OptionalOwnedClxSpriteList pChrButtons;

namespace {

struct StyledText {
	UiFlags style;
	std::string text;
	int spacing = 1;
};

struct PanelEntry {
	std::string label;
	Point position;
	int length;
	int labelLength;                                               // max label's length - used for line wrapping
	std::optional<tl::function_ref<StyledText()>> statDisplayFunc; // function responsible for displaying stat
};

UiFlags GetBaseStatColor(CharacterAttribute attr)
{
	UiFlags style = UiFlags::ColorWhite;
	if (InspectPlayer->GetBaseAttributeValue(attr) == InspectPlayer->GetMaximumAttributeValue(attr))
		style = UiFlags::ColorWhitegold;
	return style;
}

UiFlags GetCurrentStatColor(CharacterAttribute attr)
{
	UiFlags style = UiFlags::ColorWhite;
	const int current = InspectPlayer->GetCurrentAttributeValue(attr);
	const int base = InspectPlayer->GetBaseAttributeValue(attr);
	if (current > base)
		style = UiFlags::ColorBlue;
	if (current < base)
		style = UiFlags::ColorRed;
	return style;
}

UiFlags GetValueColor(int value, bool flip = false)
{
	UiFlags style = UiFlags::ColorWhite;
	if (value > 0)
		style = (flip ? UiFlags::ColorRed : UiFlags::ColorBlue);
	if (value < 0)
		style = (flip ? UiFlags::ColorBlue : UiFlags::ColorRed);
	return style;
}

UiFlags GetMaxManaColor()
{
	if (HasAnyOf(InspectPlayer->_pIFlags, ItemSpecialEffect::NoMana))
		return UiFlags::ColorRed;
	return InspectPlayer->_pMaxMana > InspectPlayer->_pMaxManaBase ? UiFlags::ColorBlue : UiFlags::ColorWhite;
}

UiFlags GetMaxHealthColor()
{
	return InspectPlayer->_pMaxHP > InspectPlayer->_pMaxHPBase ? UiFlags::ColorBlue : UiFlags::ColorWhite;
}

std::pair<int, int> GetDamage()
{
	int damageMod = InspectPlayer->_pIBonusDamMod;
	if (InspectPlayer->InvBody[INVLOC_HAND_LEFT]._itype == ItemType::Bow && InspectPlayer->_pClass != HeroClass::Rogue) {
		damageMod += InspectPlayer->_pDamageMod / 2;
	} else {
		damageMod += InspectPlayer->_pDamageMod;
	}
	const int mindam = InspectPlayer->_pIMinDam + InspectPlayer->_pIBonusDam * InspectPlayer->_pIMinDam / 100 + damageMod;
	const int maxdam = InspectPlayer->_pIMaxDam + InspectPlayer->_pIBonusDam * InspectPlayer->_pIMaxDam / 100 + damageMod;
	return { mindam, maxdam };
}

StyledText GetResistInfo(int8_t resist)
{
	UiFlags style = UiFlags::ColorBlue;
	if (resist == 0)
		style = UiFlags::ColorWhite;
	else if (resist < 0)
		style = UiFlags::ColorRed;
	else if (resist >= MaxResistance)
		style = UiFlags::ColorWhitegold;

	return { style, StrCat(resist, "%") };
}

constexpr int LeftColumnLabelX = 88;
constexpr int TopRightLabelX = 211;
constexpr int RightColumnLabelX = 253;

constexpr int LeftColumnLabelWidth = 76;
constexpr int RightColumnLabelWidth = 68;

// Indices in `panelEntries`.
constexpr unsigned AttributeHeaderEntryIndices[2] = { 5, 6 };
constexpr unsigned GoldHeaderEntryIndex = 16;

constexpr Point PageButtonPos { 57, 80 };
constexpr int PageButtonLen = 71;

PanelEntry panelEntries[] = {
	{ "", { 9, 14 }, 150, 0,
	    []() { return StyledText { UiFlags::ColorWhite, InspectPlayer->_pName }; } },
	{ "", { 161, 14 }, 149, 0,
	    []() { return StyledText { UiFlags::ColorWhite, std::string(InspectPlayer->getClassName()) }; } },

	{ N_("Level"), { 57, 52 }, 57, 45,
	    []() { return StyledText { UiFlags::ColorWhite, StrCat(InspectPlayer->getCharacterLevel()) }; } },
	{ N_("Experience"), { TopRightLabelX, 52 }, 99, 91,
	    []() {
	        return StyledText { UiFlags::ColorWhite, FormatInteger(InspectPlayer->_pExperience) };
	    } },
	{ N_("Next level"), { TopRightLabelX, 80 }, 99, 198,
	    []() {
	        if (InspectPlayer->isMaxCharacterLevel()) {
		        return StyledText { UiFlags::ColorWhitegold, std::string(_("None")) };
	        }
	        const uint32_t nextExperienceThreshold = InspectPlayer->getNextExperienceThreshold();
	        return StyledText { UiFlags::ColorWhite, FormatInteger(nextExperienceThreshold) };
	    } },

	{ N_("Base"), { LeftColumnLabelX, /* set dynamically */ 0 }, 0, 44, {} },
	{ N_("Now"), { 135, /* set dynamically */ 0 }, 0, 44, {} },
	{ N_("Strength"), { LeftColumnLabelX, 135 }, 45, LeftColumnLabelWidth,
	    []() { return StyledText { GetBaseStatColor(CharacterAttribute::Strength), StrCat(InspectPlayer->_pBaseStr) }; } },
	{ "", { 135, 135 }, 45, 0,
	    []() { return StyledText { GetCurrentStatColor(CharacterAttribute::Strength), StrCat(InspectPlayer->_pStrength) }; } },
	{ N_("Magic"), { LeftColumnLabelX, 163 }, 45, LeftColumnLabelWidth,
	    []() { return StyledText { GetBaseStatColor(CharacterAttribute::Magic), StrCat(InspectPlayer->_pBaseMag) }; } },
	{ "", { 135, 163 }, 45, 0,
	    []() { return StyledText { GetCurrentStatColor(CharacterAttribute::Magic), StrCat(InspectPlayer->_pMagic) }; } },
	{ N_("Dexterity"), { LeftColumnLabelX, 191 }, 45, LeftColumnLabelWidth, []() { return StyledText { GetBaseStatColor(CharacterAttribute::Dexterity), StrCat(InspectPlayer->_pBaseDex) }; } },
	{ "", { 135, 191 }, 45, 0,
	    []() { return StyledText { GetCurrentStatColor(CharacterAttribute::Dexterity), StrCat(InspectPlayer->_pDexterity) }; } },
	{ N_("Vitality"), { LeftColumnLabelX, 219 }, 45, LeftColumnLabelWidth, []() { return StyledText { GetBaseStatColor(CharacterAttribute::Vitality), StrCat(InspectPlayer->_pBaseVit) }; } },
	{ "", { 135, 219 }, 45, 0,
	    []() { return StyledText { GetCurrentStatColor(CharacterAttribute::Vitality), StrCat(InspectPlayer->_pVitality) }; } },
	{ N_("Points to distribute"), { LeftColumnLabelX, 248 }, 45, LeftColumnLabelWidth,
	    []() {
	        InspectPlayer->_pStatPts = std::min(CalcStatDiff(*InspectPlayer), InspectPlayer->_pStatPts);
	        return StyledText { UiFlags::ColorRed, (InspectPlayer->_pStatPts > 0 ? StrCat(InspectPlayer->_pStatPts) : "") };
	    } },

	{ N_("Gold"), { TopRightLabelX, /* set dynamically */ 0 }, 0, 98, {} },
	{ "", { TopRightLabelX, 127 }, 99, 0,
	    []() { return StyledText { UiFlags::ColorWhite, FormatInteger(InspectPlayer->_pGold) }; } },

	{ N_("Armor class"), { RightColumnLabelX, 163 }, 57, RightColumnLabelWidth,
	    []() { return StyledText { GetValueColor(InspectPlayer->_pIBonusAC), StrCat(InspectPlayer->GetArmor() + InspectPlayer->getCharacterLevel() * 2) }; } },
	{ N_("Chance To Hit"), { RightColumnLabelX, 191 }, 57, RightColumnLabelWidth,
	    []() { return StyledText { GetValueColor(InspectPlayer->_pIBonusToHit), StrCat(InspectPlayer->InvBody[INVLOC_HAND_LEFT]._itype == ItemType::Bow ? InspectPlayer->GetRangedToHit() : InspectPlayer->GetMeleeToHit(), "%") }; } },
	{ N_("Damage"), { RightColumnLabelX, 219 }, 57, RightColumnLabelWidth,
	    []() {
	        const auto [dmgMin, dmgMax] = GetDamage();
	        return StyledText { GetValueColor(InspectPlayer->_pIBonusDam), StrCat(dmgMin, "-", dmgMax) };
	    } },

	{ N_("Life"), { LeftColumnLabelX, 284 }, 45, LeftColumnLabelWidth,
	    []() { return StyledText { GetMaxHealthColor(), StrCat(InspectPlayer->_pMaxHP >> 6) }; } },
	{ "", { 135, 284 }, 45, 0,
	    []() { return StyledText { (InspectPlayer->_pHitPoints != InspectPlayer->_pMaxHP ? UiFlags::ColorRed : GetMaxHealthColor()), StrCat(InspectPlayer->_pHitPoints >> 6) }; } },
	{ N_("Mana"), { LeftColumnLabelX, 312 }, 45, LeftColumnLabelWidth,
	    []() { return StyledText { GetMaxManaColor(), StrCat(HasAnyOf(InspectPlayer->_pIFlags, ItemSpecialEffect::NoMana) ? 0 : InspectPlayer->_pMaxMana >> 6) }; } },
	{ "", { 135, 312 }, 45, 0,
	    []() { return StyledText { (InspectPlayer->_pMana != InspectPlayer->_pMaxMana ? UiFlags::ColorRed : GetMaxManaColor()), StrCat((HasAnyOf(InspectPlayer->_pIFlags, ItemSpecialEffect::NoMana) || InspectPlayer->hasNoMana()) ? 0 : InspectPlayer->_pMana >> 6) }; } },

	{ N_("Resist magic"), { RightColumnLabelX, 256 }, 57, RightColumnLabelWidth,
	    []() { return GetResistInfo(InspectPlayer->_pMagResist); } },
	{ N_("Resist fire"), { RightColumnLabelX, 284 }, 57, RightColumnLabelWidth,
	    []() { return GetResistInfo(InspectPlayer->_pFireResist); } },
	{ N_("Resist lightning"), { RightColumnLabelX, 313 }, 57, RightColumnLabelWidth,
	    []() { return GetResistInfo(InspectPlayer->_pLghtResist); } },
};

PanelEntry panelEntries2[] = {
	{ "", { 9, 14 }, 150, 0,
	    []() { return StyledText { UiFlags::ColorWhite, InspectPlayer->_pName }; } },
	{ "", { 161, 14 }, 149, 0,
	    []() { return StyledText { UiFlags::ColorWhite, std::string(InspectPlayer->getClassName()) }; } },

	{ N_("Level"), { 57, 52 }, 57, 45,
	    []() { return StyledText { UiFlags::ColorWhite, StrCat(InspectPlayer->getCharacterLevel()) }; } },
	{ N_("Experience"), { TopRightLabelX, 52 }, 99, 91,
	    []() {
	        return StyledText { UiFlags::ColorWhite, FormatInteger(InspectPlayer->_pExperience) };
	    } },
	{ N_("Next level"), { TopRightLabelX, 80 }, 99, 198,
	    []() {
	        if (InspectPlayer->isMaxCharacterLevel()) {
		        return StyledText { UiFlags::ColorWhitegold, std::string(_("None")) };
	        }
	        const uint32_t nextExperienceThreshold = InspectPlayer->getNextExperienceThreshold();
	        return StyledText { UiFlags::ColorWhite, FormatInteger(nextExperienceThreshold) };
	    } },

	{ N_("Base"), { LeftColumnLabelX, /* set dynamically */ 0 }, 0, 44, {} },
	{ N_("Now"), { 135, /* set dynamically */ 0 }, 0, 44, {} },
	{ N_("Strength"), { LeftColumnLabelX, 135 }, 45, LeftColumnLabelWidth,
	    []() { return StyledText { GetBaseStatColor(CharacterAttribute::Strength), StrCat(InspectPlayer->_pBaseStr) }; } },
	{ "", { 135, 135 }, 45, 0,
	    []() { return StyledText { GetCurrentStatColor(CharacterAttribute::Strength), StrCat(InspectPlayer->_pStrength) }; } },
	{ N_("Magic"), { LeftColumnLabelX, 163 }, 45, LeftColumnLabelWidth,
	    []() { return StyledText { GetBaseStatColor(CharacterAttribute::Magic), StrCat(InspectPlayer->_pBaseMag) }; } },
	{ "", { 135, 163 }, 45, 0,
	    []() { return StyledText { GetCurrentStatColor(CharacterAttribute::Magic), StrCat(InspectPlayer->_pMagic) }; } },
	{ N_("Dexterity"), { LeftColumnLabelX, 191 }, 45, LeftColumnLabelWidth, []() { return StyledText { GetBaseStatColor(CharacterAttribute::Dexterity), StrCat(InspectPlayer->_pBaseDex) }; } },
	{ "", { 135, 191 }, 45, 0,
	    []() { return StyledText { GetCurrentStatColor(CharacterAttribute::Dexterity), StrCat(InspectPlayer->_pDexterity) }; } },
	{ N_("Vitality"), { LeftColumnLabelX, 219 }, 45, LeftColumnLabelWidth, []() { return StyledText { GetBaseStatColor(CharacterAttribute::Vitality), StrCat(InspectPlayer->_pBaseVit) }; } },
	{ "", { 135, 219 }, 45, 0,
	    []() { return StyledText { GetCurrentStatColor(CharacterAttribute::Vitality), StrCat(InspectPlayer->_pVitality) }; } },
	{ N_("Points to distribute"), { LeftColumnLabelX, 248 }, 45, LeftColumnLabelWidth,
	    []() {
	        InspectPlayer->_pStatPts = std::min(CalcStatDiff(*InspectPlayer), InspectPlayer->_pStatPts);
	        return StyledText { UiFlags::ColorRed, (InspectPlayer->_pStatPts > 0 ? StrCat(InspectPlayer->_pStatPts) : "") };
	    } },

	{ N_("Gold"), { TopRightLabelX, /* set dynamically */ 0 }, 0, 98, {} },
	{ "", { TopRightLabelX, 127 }, 99, 0,
	    []() { return StyledText { UiFlags::ColorWhite, FormatInteger(InspectPlayer->_pGold) }; } },

	{ N_("Chance To Block"), { RightColumnLabelX, 163 }, 57, RightColumnLabelWidth,
	    []() { return StyledText { GetValueColor(InspectPlayer->_pBlockFlag ? InspectPlayer->GetBlockChance() : -1), StrCat(InspectPlayer->_pBlockFlag ? InspectPlayer->GetBlockChance() : 0, "%") }; } },
	{ N_("Magic To Hit"), { RightColumnLabelX, 191 }, 57, RightColumnLabelWidth,
	    []() { return StyledText { GetValueColor(InspectPlayer->GetMagicToHit()), StrCat(InspectPlayer->GetMagicToHit(), "%") }; } },
	{ N_("Critical Hit Chance"), { RightColumnLabelX, 219 }, 57, RightColumnLabelWidth,
	    []() { return StyledText { GetValueColor(IsAnyOf(InspectPlayer->_pClass, HeroClass::Warrior, HeroClass::Barbarian) ? (InspectPlayer->getCharacterLevel()) : -1), StrCat(IsAnyOf(InspectPlayer->_pClass, HeroClass::Warrior, HeroClass::Barbarian) ? InspectPlayer->getCharacterLevel() : 0, "%") }; } },

	{ N_("Life"), { LeftColumnLabelX, 284 }, 45, LeftColumnLabelWidth,
	    []() { return StyledText { GetMaxHealthColor(), StrCat(InspectPlayer->_pMaxHP >> 6) }; } },
	{ "", { 135, 284 }, 45, 0,
	    []() { return StyledText { (InspectPlayer->_pHitPoints != InspectPlayer->_pMaxHP ? UiFlags::ColorRed : GetMaxHealthColor()), StrCat(InspectPlayer->_pHitPoints >> 6) }; } },
	{ N_("Mana"), { LeftColumnLabelX, 312 }, 45, LeftColumnLabelWidth,
	    []() { return StyledText { GetMaxManaColor(), StrCat(HasAnyOf(InspectPlayer->_pIFlags, ItemSpecialEffect::NoMana) ? 0 : InspectPlayer->_pMaxMana >> 6) }; } },
	{ "", { 135, 312 }, 45, 0,
	    []() { return StyledText { (InspectPlayer->_pMana != InspectPlayer->_pMaxMana ? UiFlags::ColorRed : GetMaxManaColor()), StrCat((HasAnyOf(InspectPlayer->_pIFlags, ItemSpecialEffect::NoMana) || InspectPlayer->hasNoMana()) ? 0 : InspectPlayer->_pMana >> 6) }; } },

};

OptionalOwnedClxSpriteList Panel[2];

constexpr int PanelFieldHeight = 24;
constexpr int PanelFieldPaddingTop = 3;
constexpr int PanelFieldPaddingBottom = 3;
constexpr int PanelFieldPaddingSide = 5;
constexpr int PanelFieldInnerHeight = PanelFieldHeight - PanelFieldPaddingTop - PanelFieldPaddingBottom;

void DrawPanelField(const Surface &out, Point pos, int len, ClxSprite left, ClxSprite middle, ClxSprite right)
{
	RenderClxSprite(out, left, pos);
	pos.x += left.width();
	len -= left.width() + right.width();
	RenderClxSprite(out.subregion(pos.x, pos.y, len, middle.height()), middle, Point { 0, 0 });
	pos.x += len;
	RenderClxSprite(out, right, pos);
}

void DrawPanelString(const Surface &out, const PanelEntry &entry, const Point pos)
{
	if (entry.statDisplayFunc) {
		const StyledText tmp = (*entry.statDisplayFunc)();
		DrawString(
		    out,
		    tmp.text,
		    { entry.position + Displacement { pos.x + PanelFieldPaddingSide, pos.y + PanelFieldPaddingTop }, { entry.length - (PanelFieldPaddingSide * 2), PanelFieldInnerHeight } },
		    { .flags = UiFlags::KerningFitSpacing | UiFlags::AlignCenter | UiFlags::VerticalCenter | tmp.style });
	}
}

void DrawShadowString(const Surface &out, const PanelEntry &entry)
{
	if (entry.label.empty())
		return;

	constexpr int Spacing = 0;
	const std::string_view textStr = LanguageTranslate(entry.label);
	std::string_view text;
	std::string wrapped;
	if (entry.labelLength > 0) {
		wrapped = WordWrapString(textStr, entry.labelLength, GameFont12, Spacing);
		text = wrapped;
	} else {
		text = textStr;
	}

	UiFlags style = UiFlags::VerticalCenter;

	Point labelPosition = entry.position;

	if (entry.length == 0) {
		style |= UiFlags::AlignCenter;
	} else {
		style |= UiFlags::AlignRight;
		labelPosition += Displacement { -entry.labelLength - (IsSmallFontTall() ? 2 : 3), 0 };
	}

	// If the text is less tall than the field, we center it vertically relative to the field.
	// Otherwise, we draw from the top of the field.
	const int textHeight = static_cast<int>((c_count(wrapped, '\n') + 1) * GetLineHeight(wrapped, GameFont12));
	const int labelHeight = std::max(PanelFieldHeight, textHeight);

	DrawString(out, text, { labelPosition + Displacement { -2, 2 }, { entry.labelLength, labelHeight } },
	    { .flags = style | UiFlags::ColorBlack, .spacing = Spacing });
	DrawString(out, text, { labelPosition, { entry.labelLength, labelHeight } },
	    { .flags = style | UiFlags::ColorWhite, .spacing = Spacing });
}

void DrawStatButtons(const Surface &out)
{
	if (InspectPlayer->_pStatPts > 0 && !IsInspectingPlayer()) {
		if (InspectPlayer->_pBaseStr < InspectPlayer->GetMaximumAttributeValue(CharacterAttribute::Strength))
			ClxDraw(out, GetPanelPosition(UiPanels::Character, { 137, 157 }), (*pChrButtons)[CharPanelButton[static_cast<size_t>(CharacterAttribute::Strength)] ? 2 : 1]);
		if (InspectPlayer->_pBaseMag < InspectPlayer->GetMaximumAttributeValue(CharacterAttribute::Magic))
			ClxDraw(out, GetPanelPosition(UiPanels::Character, { 137, 185 }), (*pChrButtons)[CharPanelButton[static_cast<size_t>(CharacterAttribute::Magic)] ? 4 : 3]);
		if (InspectPlayer->_pBaseDex < InspectPlayer->GetMaximumAttributeValue(CharacterAttribute::Dexterity))
			ClxDraw(out, GetPanelPosition(UiPanels::Character, { 137, 214 }), (*pChrButtons)[CharPanelButton[static_cast<size_t>(CharacterAttribute::Dexterity)] ? 6 : 5]);
		if (InspectPlayer->_pBaseVit < InspectPlayer->GetMaximumAttributeValue(CharacterAttribute::Vitality))
			ClxDraw(out, GetPanelPosition(UiPanels::Character, { 137, 242 }), (*pChrButtons)[CharPanelButton[static_cast<size_t>(CharacterAttribute::Vitality)] ? 8 : 7]);
	}
}

void DrawBakedMainPanelButton(const Surface &out, Point dstPos)
{
	if (!BottomBuffer)
		return;

	constexpr int BakedBtnSrcX = 6;
	constexpr int BakedBtnSrcY = 32;
	constexpr int BakedBtnW = 77;
	constexpr int BakedBtnH = 25;

	const int panel8Y = (GetMainPanel().size.height + PanelPaddingHeight) - 1;

	const SDL_Rect src = MakeSdlRect(BakedBtnSrcX, panel8Y + BakedBtnSrcY, BakedBtnW, BakedBtnH);
	out.BlitFrom(*BottomBuffer, src, dstPos);
}

} // namespace

tl::expected<void, std::string> LoadCharPanel()
{
	ASSIGN_OR_RETURN(OptionalOwnedClxSpriteList background, LoadClxWithStatus("data\\charbg.clx"));
	const OwnedSurface outPrimary((*background)[0].width(), (*background)[0].height());
	RenderClxSprite(outPrimary, (*background)[0], { 0, 0 });
	const OwnedSurface outSecondary((*background)[0].width(), (*background)[0].height());
	RenderClxSprite(outSecondary, (*background)[0], { 0, 0 });
	background = std::nullopt;

	{
		ASSIGN_OR_RETURN(OwnedClxSpriteList boxLeft, LoadClxWithStatus("data\\boxleftend.clx"));
		ASSIGN_OR_RETURN(OwnedClxSpriteList boxMiddle, LoadClxWithStatus("data\\boxmiddle.clx"));
		ASSIGN_OR_RETURN(OwnedClxSpriteList boxRight, LoadClxWithStatus("data\\boxrightend.clx"));

		const bool isSmallFontTall = IsSmallFontTall();
		const int attributeHeadersY = isSmallFontTall ? 112 : 114;

		for (const unsigned i : AttributeHeaderEntryIndices) {
			panelEntries[i].position.y = attributeHeadersY;
			panelEntries2[i].position.y = attributeHeadersY;
		}

		panelEntries[GoldHeaderEntryIndex].position.y = isSmallFontTall ? 105 : 106;

		for (auto &entry : panelEntries) {
			if (entry.statDisplayFunc) {
				DrawPanelField(outPrimary, entry.position, entry.length, boxLeft[0], boxMiddle[0], boxRight[0]);
			}
			DrawShadowString(outPrimary, entry);
		}
		Panel[0] = SurfaceToClx(outPrimary);

		panelEntries2[GoldHeaderEntryIndex].position.y = isSmallFontTall ? 105 : 106;

		for (auto &entry : panelEntries2) {
			if (entry.statDisplayFunc) {
				DrawPanelField(outSecondary, entry.position, entry.length, boxLeft[0], boxMiddle[0], boxRight[0]);
			}
			DrawShadowString(outSecondary, entry);
		}
		Panel[1] = SurfaceToClx(outSecondary);
	}
	return {};
}

void FreeCharPanel()
{
	Panel[0] = std::nullopt;
	Panel[1] = std::nullopt;
}

namespace {

void DrawMainPanelStyleButton(const Surface &out, Point pos, std::string_view text, bool pressed)
{
	// if (!PanelButton || !PanelButtonGrime || !PanelButtonDownGrime)
	//	return;

	const int frame = 0; // main panel uses frame indices for different buttons; 0 is fine for a generic one
	RenderClxSprite(out, (*PanelButton)[frame], pos);

	// Text spacing behavior similar to mainpanel.cpp (optional but closer to identical)
	int spacing = 2;
	int width = std::min<int>(GetLineWidth(text, GameFont12, spacing), (*PanelButton)[frame].width());
	if (width > 38) {
		spacing = 1;
		width = std::min<int>(GetLineWidth(text, GameFont12, spacing), (*PanelButton)[frame].width());
	}

	const ClxSprite grime = pressed ? (*PanelButtonDownGrime)[frame] : (*PanelButtonGrime)[frame];

	// Same grime placement style: center a subregion and draw grime into it
	const int x = pos.x + ((*PanelButton)[frame].width() - width) / 2;
	const int y = pos.y + 7;
	RenderClxSprite(out.subregion(x, y, width, grime.height()), grime, { 0, 0 });

	const Displacement pressOffset = pressed ? Displacement { 0, 2 } : Displacement { 0, 0 };
	const UiFlags style = pressed ? UiFlags::ColorButtonpushed : UiFlags::ColorButtonface;

	// Shadow + face, matching main panel behavior
	DrawString(out, text,
	    { pos + pressOffset + Displacement { 0, 1 }, { (*PanelButton)[frame].width(), 0 } },
	    { .flags = UiFlags::AlignCenter | UiFlags::KerningFitSpacing | UiFlags::ColorBlack, .spacing = spacing });

	DrawString(out, text,
	    { pos + pressOffset, { (*PanelButton)[frame].width(), 0 } },
	    { .flags = UiFlags::AlignCenter | UiFlags::KerningFitSpacing | style, .spacing = spacing });
}

} // namespace

void DrawChr(const Surface &out)
{
	const Point pos = GetPanelPosition(UiPanels::Character, { 0, 0 });

	if (CharPanelDetailsPage) {
		RenderClxSprite(out, (*Panel[1])[0], pos);
		for (auto &entry : panelEntries2)
			DrawPanelString(out, entry, pos);
	} else {
		RenderClxSprite(out, (*Panel[0])[0], pos);
		for (auto &entry : panelEntries)
			DrawPanelString(out, entry, pos);
	}

	DrawStatButtons(out);

	const Point btnPos = GetPanelPosition(UiPanels::Character, CharPanelPageButtonRect.position);
	const std::string_view label = CharPanelDetailsPage ? _("Back") : _("Next");

	DrawBakedMainPanelButton(out, btnPos);
	DrawMainPanelStyleButton(out, btnPos, label, CharPanelPageButtonDown);
}

} // namespace devilution
