/**
 * @file trn.hpp
 *
 * Contains most of trn logic
 */
#pragma once

#include "engine/palette.h"
#include "player.h"
#include "utils/stdcompat/optional.hpp"

namespace devilution {

uint8_t *GetCustomTRN(uint8_t pal8blue = PAL8_BLUE, uint8_t pal8red = PAL8_RED, uint8_t pal8yellow = PAL8_YELLOW, uint8_t pal8orange = PAL8_ORANGE, uint8_t pal16beige = PAL16_BEIGE, uint8_t pal16blue = PAL16_BLUE, uint8_t pal16yellow = PAL16_YELLOW, uint8_t pal16orange = PAL16_ORANGE, uint8_t pal16red = PAL16_RED, uint8_t pal16gray = PAL16_GRAY);
uint8_t *GetInfravisionTRN();
uint8_t *GetStoneTRN();
uint8_t *GetPauseTRN();
std::optional<std::array<uint8_t, 256>> GetClassTRN(Player &player);

} // namespace devilution
