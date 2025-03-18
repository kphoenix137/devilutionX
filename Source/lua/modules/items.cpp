#include "lua/modules/items.hpp"

#include <sol/sol.hpp>

#include "items.h"
#include "lua/lua.hpp"
#include "lua/metadoc.hpp"

namespace devilution {

bool CallLuaNoSpawnCondition(Item &item)
{
	sol::state &lua = GetLuaState();

	// ✅ Check if Lua state exists
	if (!lua.lua_state() || !lua["NoSpawnCondition"].valid()) {
		return false; // Default behavior
	}

	// ✅ Create event data table
	sol::table eventData = lua.create_table();
	eventData["item"] = lua.create_table();
	eventData["item"]["_iCurs"] = item._iCurs;
	eventData["handled"] = false;

	// ✅ Dispatch event safely
	LuaEvent("NoSpawnCondition", eventData);

	return eventData["handled"].get_or(false);
}

bool CallLuaRewardSpawnCondition(Item &item, uint8_t iCurs)
{
	sol::state &lua = GetLuaState();

	// ✅ Check if Lua event exists
	if (!lua.lua_state() || !lua["RewardSpawnCondition"].valid()) {
		return item._iCurs == iCurs; // Default behavior
	}

	sol::table eventData = lua.create_table();
	eventData["item"] = lua.create_table();
	eventData["item"]["_iCurs"] = item._iCurs;
	eventData["iCurs"] = iCurs;
	eventData["handled"] = false;

	LuaEvent("RewardSpawnCondition", eventData);

	return eventData["handled"].get_or(item._iCurs == iCurs);
}

bool CallLuaVendorSpawnCondition(Item &item, int maxValue)
{
	sol::state &lua = GetLuaState();

	if (!lua.lua_state() || !lua["VendorSpawnCondition"].valid()) {
		return item._iIvalue > maxValue;
	}

	sol::table eventData = lua.create_table();
	eventData["item"] = lua.create_table();
	eventData["item"]["_iCurs"] = item._iCurs;
	eventData["item"]["_iIvalue"] = item._iIvalue;
	eventData["item"]["_iSpell"] = item._iSpell;
	eventData["item"]["_iMinStr"] = item._iMinStr;
	eventData["item"]["_iMinMag"] = item._iMinMag;
	eventData["item"]["_iMinDex"] = item._iMinDex;
	eventData["maxValue"] = maxValue;
	eventData["handled"] = false;

	LuaEvent("VendorSpawnCondition", eventData);

	return eventData["handled"].get_or(item._iIvalue > maxValue);
}

bool CallLuaSmithPremiumHfSpawnCondition(Item &item, int maxValue, int strength, int magic, int dexterity, int ivalue)
{
	sol::state &lua = GetLuaState();

	if (!lua.lua_state() || !lua["SmithPremiumHfSpawnCondition"].valid()) {
		return item._iIvalue <= maxValue
		    && item._iMinStr <= strength
		    && item._iMinMag <= magic
		    && item._iMinDex <= dexterity
		    && item._iIvalue >= ivalue;
	}

	sol::table eventData = lua.create_table();
	eventData["item"] = lua.create_table();
	eventData["item"]["_iIvalue"] = item._iIvalue;
	eventData["item"]["_iMinStr"] = item._iMinStr;
	eventData["item"]["_iMinMag"] = item._iMinMag;
	eventData["item"]["_iMinDex"] = item._iMinDex;
	eventData["maxValue"] = maxValue;
	eventData["strength"] = strength;
	eventData["magic"] = magic;
	eventData["dexterity"] = dexterity;
	eventData["ivalue"] = ivalue;
	eventData["handled"] = false;

	LuaEvent("SmithPremiumHfSpawnCondition", eventData);

	return eventData["handled"].get_or(item._iIvalue <= maxValue
	    && item._iMinStr <= strength
	    && item._iMinMag <= magic
	    && item._iMinDex <= dexterity
	    && item._iIvalue >= ivalue);
}

bool CallLuaBoyHfSpawnCondition(Item &item, int maxValue, int strength, int magic, int dexterity, int ivalue, int count, int maxCount)
{
	sol::state &lua = GetLuaState();

	if (!lua.lua_state() || !lua["BoyHfSpawnCondition"].valid()) {
		return item._iIvalue > maxValue
		    || item._iMinStr > strength
		    || item._iMinMag > magic
		    || item._iMinDex > dexterity
		    || item._iIvalue < ivalue
		    || count < maxCount;
	}

	sol::table eventData = lua.create_table();
	eventData["item"] = lua.create_table();
	eventData["item"]["_iIvalue"] = item._iIvalue;
	eventData["item"]["_iMinStr"] = item._iMinStr;
	eventData["item"]["_iMinMag"] = item._iMinMag;
	eventData["item"]["_iMinDex"] = item._iMinDex;
	eventData["maxValue"] = maxValue;
	eventData["strength"] = strength;
	eventData["magic"] = magic;
	eventData["dexterity"] = dexterity;
	eventData["ivalue"] = ivalue;
	eventData["count"] = count;
	eventData["maxCount"] = maxCount;
	eventData["handled"] = false;

	LuaEvent("BoyHfSpawnCondition", eventData);

	return eventData["handled"].get_or(item._iIvalue > maxValue
	    || item._iMinStr > strength
	    || item._iMinMag > magic
	    || item._iMinDex > dexterity
	    || item._iIvalue < ivalue
	    || count < maxCount);
}

} // namespace devilution
