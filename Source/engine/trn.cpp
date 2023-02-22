#include <fmt/format.h>
#include <unordered_map>

#ifdef _DEBUG
#include "debug.h"
#endif
#include "engine/load_file.hpp"
#include "engine/trn.hpp"
#include "lighting.h"

namespace devilution {

uint8_t *GetCustomTRN(uint8_t pal8blue, uint8_t pal8red, uint8_t pal8yellow, uint8_t pal8orange, uint8_t pal16beige, uint8_t pal16blue, uint8_t pal16yellow, uint8_t pal16orange, uint8_t pal16red, uint8_t pal16gray)
{
	std::unique_ptr<uint8_t[]> customTrn(new uint8_t[256]);

	// Fill town/dungeon palette as default
	for (uint8_t i = 0; i < 128; i++) {
		customTrn[i] = i;
	}

	for (uint8_t i = PAL8_BLUE; i < PAL8_RED; i++) {
		customTrn[i] = i - PAL8_BLUE + pal8blue;
	}

	for (uint8_t i = PAL8_RED; i < PAL8_YELLOW; i++) {
		customTrn[i] = i - PAL8_RED + pal8red;
	}

	for (uint8_t i = PAL8_YELLOW; i < PAL8_ORANGE; i++) {
		customTrn[i] = i - PAL8_YELLOW + pal8yellow;
	}

	for (uint8_t i = PAL8_ORANGE; i < PAL16_BEIGE; i++) {
		customTrn[i] = i - PAL8_ORANGE + pal8orange;
	}

	for (uint8_t i = PAL16_BEIGE; i < PAL16_BLUE; i++) {
		uint8_t adj = 0;
		if ((i == PAL16_BEIGE) && (pal16beige % 16 != 0))
			adj = 1;
		int value = i - PAL16_BEIGE + pal16beige + adj;
		value = std::clamp(value, 0, 254);
		customTrn[i] = static_cast<uint8_t>(value);
	}

	for (uint8_t i = PAL16_BLUE; i < PAL16_YELLOW; i++) {
		uint8_t adj = 0;
		if ((i == PAL16_BLUE) && (pal16blue % 16 != 0))
			adj = 1;
		int value = i - PAL16_BLUE + pal16blue + adj;
		value = std::clamp(value, 0, 254);
		customTrn[i] = static_cast<uint8_t>(value);
	}

	for (uint8_t i = PAL16_YELLOW; i < PAL16_ORANGE; i++) {
		uint8_t adj = 0;
		if ((i == PAL16_YELLOW) && (pal16yellow % 16 != 0))
			adj = 1;
		int value = i - PAL16_YELLOW + pal16yellow + adj;
		value = std::clamp(value, 0, 254);
		customTrn[i] = static_cast<uint8_t>(value);
	}

	for (uint8_t i = PAL16_ORANGE; i < PAL16_RED; i++) {
		uint8_t adj = 0;
		if ((i == PAL16_ORANGE) && (pal16orange % 16 != 0))
			adj = 1;
		int value = i - PAL16_ORANGE + pal16orange + adj;
		value = std::clamp(value, 0, 254);
		customTrn[i] = static_cast<uint8_t>(value);
	}

	for (uint8_t i = PAL16_RED; i < PAL16_GRAY; i++) {
		uint8_t adj = 0;
		if ((i == PAL16_RED) && (pal16red % 16 != 0))
			adj = 1;
		int value = i - PAL16_RED + pal16red + adj;
		value = std::clamp(value, 0, 254);
		customTrn[i] = static_cast<uint8_t>(value);
	}

	for (uint8_t i = PAL16_GRAY; i < PAL16_GRAY + 15; i++) {
		uint8_t adj = 0;
		if ((i == PAL16_GRAY) && (pal16gray % 16 != 0))
			adj = 1;
		int value = i - PAL16_GRAY + pal16gray + adj;
		value = std::clamp(value, 0, 254);
		customTrn[i] = static_cast<uint8_t>(value);
	}


	customTrn[255] = 255;

	return customTrn.release();
}

uint8_t *GetInfravisionTRN()
{
	return &LightTables[16 * 256];
}

uint8_t *GetStoneTRN()
{
	return &LightTables[17 * 256];
}

uint8_t *GetPauseTRN()
{
	return &LightTables[18 * 256];
}

std::optional<std::array<uint8_t, 256>> GetClassTRN(Player &player)
{
	std::array<uint8_t, 256> trn;
	const char *path;

	switch (player._pClass) {
	case HeroClass::Warrior:
		path = "plrgfx\\warrior.trn";
		break;
	case HeroClass::Rogue:
		path = "plrgfx\\rogue.trn";
		break;
	case HeroClass::Sorcerer:
		path = "plrgfx\\sorcerer.trn";
		break;
	case HeroClass::Monk:
		path = "plrgfx\\monk.trn";
		break;
	case HeroClass::Bard:
		path = "plrgfx\\bard.trn";
		break;
	case HeroClass::Barbarian:
		path = "plrgfx\\barbarian.trn";
		break;
	}

#ifdef _DEBUG
	if (!debugTRN.empty()) {
		path = debugTRN.c_str();
	}
#endif
	if (LoadOptionalFileInMem(path, &trn[0], 256)) {
		return trn;
	}
	return std::nullopt;
}

} // namespace devilution
