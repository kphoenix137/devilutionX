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

uint8_t *GetCustomTRN(uint8_t pal8blue, int8_t pal8blueBrightness, uint8_t pal8red, int8_t pal8redBrightness, uint8_t pal8yellow, int8_t pal8yellowBrightness, uint8_t pal8orange, int8_t pal8orangeBrightness, uint8_t pal16beige, int8_t pal16beigeBrightness, uint8_t pal16blue, int8_t pal16blueBrightness, uint8_t pal16yellow, int8_t pal16yellowBrightness, uint8_t pal16orange, int8_t pal16orangeBrightness, uint8_t pal16red, int8_t pal16redBrightness, uint8_t pal16gray, int8_t pal16grayBrightness);
uint8_t *GetInfravisionTRN();
uint8_t *GetStoneTRN();
uint8_t *GetPauseTRN();
std::optional<std::array<uint8_t, 256>> GetClassTRN(Player &player);

} // namespace devilution
