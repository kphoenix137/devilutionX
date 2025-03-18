#pragma once

#include <stdint.h>

namespace devilution {

struct Item;

bool CallLuaNoSpawnCondition(Item &item);
bool CallLuaRewardSpawnCondition(Item &item, uint8_t iCurs);
bool CallLuaVendorSpawnCondition(Item &item, int maxValue);
bool CallLuaSmithPremiumHfSpawnCondition(Item &item, int maxValue, int strength, int magic, int dexterity, int ivalue);
bool CallLuaBoyHfSpawnCondition(Item &item, int maxValue, int strength, int magic, int dexterity, int ivalue, int count, int maxCount);

} // namespace devilution
