#include "lua/modules/items.hpp"
#include "items.h"
#include "lua/lua.hpp"
#include "lua/metadoc.hpp"
#include <sol/sol.hpp>

namespace devilution {

namespace {

std::string_view ItemEquipTypeToString(item_equip_type equipType)
{
	switch (equipType) {
	case ILOC_NONE: return "ILOC_NONE";
	case ILOC_ONEHAND: return "ILOC_ONEHAND";
	case ILOC_TWOHAND: return "ILOC_TWOHAND";
	case ILOC_ARMOR: return "ILOC_ARMOR";
	case ILOC_HELM: return "ILOC_HELM";
	case ILOC_RING: return "ILOC_RING";
	case ILOC_AMULET: return "ILOC_AMULET";
	case ILOC_UNEQUIPABLE: return "ILOC_UNEQUIPABLE";
	case ILOC_BELT: return "ILOC_BELT";
	case ILOC_INVALID: return "ILOC_INVALID";
	default: return "Unknown";
	}
}

std::string_view ItemClassToString(item_class itemClass)
{
	switch (itemClass) {
	case ICLASS_NONE: return "ICLASS_NONE";
	case ICLASS_WEAPON: return "ICLASS_WEAPON";
	case ICLASS_ARMOR: return "ICLASS_ARMOR";
	case ICLASS_MISC: return "ICLASS_MISC";
	case ICLASS_GOLD: return "ICLASS_GOLD";
	case ICLASS_QUEST: return "ICLASS_QUEST";
	default: return "Unknown";
	}
}

std::string_view ItemMiscIdToString(item_misc_id miscId)
{
	switch (miscId) {
	case IMISC_NONE: return "IMISC_NONE";
	case IMISC_FULLHEAL: return "IMISC_FULLHEAL";
	case IMISC_HEAL: return "IMISC_HEAL";
	case IMISC_MANA: return "IMISC_MANA";
	case IMISC_FULLMANA: return "IMISC_FULLMANA";
	case IMISC_ELIXSTR: return "IMISC_ELIXSTR";
	case IMISC_ELIXMAG: return "IMISC_ELIXMAG";
	case IMISC_ELIXDEX: return "IMISC_ELIXDEX";
	case IMISC_ELIXVIT: return "IMISC_ELIXVIT";
	case IMISC_REJUV: return "IMISC_REJUV";
	case IMISC_FULLREJUV: return "IMISC_FULLREJUV";
	case IMISC_SCROLL: return "IMISC_SCROLL";
	case IMISC_SCROLLT: return "IMISC_SCROLLT";
	case IMISC_STAFF: return "IMISC_STAFF";
	case IMISC_BOOK: return "IMISC_BOOK";
	case IMISC_RING: return "IMISC_RING";
	case IMISC_AMULET: return "IMISC_AMULET";
	case IMISC_UNIQUE: return "IMISC_UNIQUE";
	default: return "Unknown";
	}
}

std::string_view ItemIndexToString(_item_indexes idx)
{
	switch (idx) {
	case IDI_GOLD: return "IDI_GOLD";
	case IDI_WARRIOR: return "IDI_WARRIOR";
	case IDI_WARRSHLD: return "IDI_WARRSHLD";
	case IDI_WARRCLUB: return "IDI_WARRCLUB";
	case IDI_ROGUE: return "IDI_ROGUE";
	case IDI_SORCERER: return "IDI_SORCERER";
	case IDI_CLEAVER: return "IDI_CLEAVER";
	case IDI_NONE: return "IDI_NONE";
	default: return "Unknown";
	}
}

std::string ItemSpecialEffectHfToString(ItemSpecialEffectHf flags)
{
	std::string result;
	if (flags == ItemSpecialEffectHf::None)
		return "None";
	if (static_cast<uint8_t>(flags) & static_cast<uint8_t>(ItemSpecialEffectHf::Devastation))
		result += "Devastation, ";
	if (static_cast<uint8_t>(flags) & static_cast<uint8_t>(ItemSpecialEffectHf::Decay))
		result += "Decay, ";
	if (static_cast<uint8_t>(flags) & static_cast<uint8_t>(ItemSpecialEffectHf::Peril))
		result += "Peril, ";
	if (static_cast<uint8_t>(flags) & static_cast<uint8_t>(ItemSpecialEffectHf::Jesters))
		result += "Jesters, ";
	if (static_cast<uint8_t>(flags) & static_cast<uint8_t>(ItemSpecialEffectHf::Doppelganger))
		result += "Doppelganger, ";
	if (static_cast<uint8_t>(flags) & static_cast<uint8_t>(ItemSpecialEffectHf::ACAgainstDemons))
		result += "ACAgainstDemons, ";
	if (static_cast<uint8_t>(flags) & static_cast<uint8_t>(ItemSpecialEffectHf::ACAgainstUndead))
		result += "ACAgainstUndead, ";
	if (!result.empty())
		result.erase(result.size() - 2); // remove trailing comma and space
	return result;
}

static std::string CreateSpellIDString(SpellID spell)
{
	switch (spell) {
	case SpellID::Null: return "Null";
	case SpellID::Firebolt: return "Firebolt";
	case SpellID::Healing: return "Healing";
	case SpellID::Lightning: return "Lightning";
	case SpellID::Flash: return "Flash";
	case SpellID::Identify: return "Identify";
	case SpellID::FireWall: return "FireWall";
	case SpellID::TownPortal: return "TownPortal";
	case SpellID::StoneCurse: return "StoneCurse";
	case SpellID::Infravision: return "Infravision";
	case SpellID::Phasing: return "Phasing";
	case SpellID::ManaShield: return "ManaShield";
	case SpellID::Fireball: return "Fireball";
	case SpellID::Guardian: return "Guardian";
	case SpellID::ChainLightning: return "ChainLightning";
	case SpellID::FlameWave: return "FlameWave";
	case SpellID::DoomSerpents: return "DoomSerpents";
	case SpellID::BloodRitual: return "BloodRitual";
	case SpellID::Nova: return "Nova";
	case SpellID::Invisibility: return "Invisibility";
	case SpellID::Inferno: return "Inferno";
	case SpellID::Golem: return "Golem";
	case SpellID::Rage: return "Rage";
	case SpellID::Teleport: return "Teleport";
	case SpellID::Apocalypse: return "Apocalypse";
	case SpellID::Etherealize: return "Etherealize";
	case SpellID::ItemRepair: return "ItemRepair";
	case SpellID::StaffRecharge: return "StaffRecharge";
	case SpellID::TrapDisarm: return "TrapDisarm";
	case SpellID::Elemental: return "Elemental";
	case SpellID::ChargedBolt: return "ChargedBolt";
	case SpellID::HolyBolt: return "HolyBolt";
	case SpellID::Resurrect: return "Resurrect";
	case SpellID::Telekinesis: return "Telekinesis";
	case SpellID::HealOther: return "HealOther";
	case SpellID::BloodStar: return "BloodStar";
	case SpellID::BoneSpirit: return "BoneSpirit";
	case SpellID::Mana: return "Mana";
	case SpellID::Magi: return "Magi";
	case SpellID::Jester: return "Jester";
	case SpellID::LightningWall: return "LightningWall";
	case SpellID::Immolation: return "Immolation";
	case SpellID::Warp: return "Warp";
	case SpellID::Reflect: return "Reflect";
	case SpellID::Berserk: return "Berserk";
	case SpellID::RingOfFire: return "RingOfFire";
	case SpellID::Search: return "Search";
	case SpellID::RuneOfFire: return "RuneOfFire";
	case SpellID::RuneOfLight: return "RuneOfLight";
	case SpellID::RuneOfNova: return "RuneOfNova";
	case SpellID::RuneOfImmolation: return "RuneOfImmolation";
	case SpellID::RuneOfStone: return "RuneOfStone";
	case SpellID::Invalid: return "Invalid";
	default: return "Unknown";
	}
}

sol::table CreateItemTable(const Item &item)
{
	sol::state &L = GetLuaState();
	sol::table tbl = L.create_table();

	tbl["_iSeed"] = item._iSeed;
	tbl["_iCreateInfo"] = item._iCreateInfo;
	tbl["_itype"] = ItemTypeToString(item._itype);
	tbl["_iAnimFlag"] = item._iAnimFlag;
	tbl["position"] = { item.position.x, item.position.y };
	tbl["_iDelFlag"] = item._iDelFlag;
	tbl["selectionRegion"] = static_cast<int>(item.selectionRegion);
	tbl["_iPostDraw"] = item._iPostDraw;
	tbl["_iIdentified"] = item._iIdentified;
	tbl["_iMagical"] = static_cast<int>(item._iMagical);
	tbl["_iName"] = item._iName;
	tbl["_iIName"] = item._iIName;
	tbl["_iLoc"] = static_cast<int>(item._iLoc);
	tbl["_iClass"] = static_cast<int>(item._iClass);
	tbl["_iCurs"] = item._iCurs;
	tbl["_ivalue"] = item._ivalue;
	tbl["_iIvalue"] = item._iIvalue;
	tbl["_iMinDam"] = item._iMinDam;
	tbl["_iMaxDam"] = item._iMaxDam;
	tbl["_iAC"] = item._iAC;
	tbl["_iFlags"] = static_cast<int>(item._iFlags);
	tbl["_iMiscId"] = static_cast<int>(item._iMiscId);
	tbl["spell"] = CreateSpellIDString(item._iSpell);
	tbl["IDidx"] = static_cast<int>(item.IDidx);
	tbl["_iCharges"] = item._iCharges;
	tbl["_iMaxCharges"] = item._iMaxCharges;
	tbl["_iDurability"] = item._iDurability;
	tbl["_iMaxDur"] = item._iMaxDur;
	tbl["_iPLDam"] = item._iPLDam;
	tbl["_iPLToHit"] = item._iPLToHit;
	tbl["_iPLAC"] = item._iPLAC;
	tbl["_iPLStr"] = item._iPLStr;
	tbl["_iPLMag"] = item._iPLMag;
	tbl["_iPLDex"] = item._iPLDex;
	tbl["_iPLVit"] = item._iPLVit;
	tbl["_iPLFR"] = item._iPLFR;
	tbl["_iPLLR"] = item._iPLLR;
	tbl["_iPLMR"] = item._iPLMR;
	tbl["_iPLMana"] = item._iPLMana;
	tbl["_iPLHP"] = item._iPLHP;
	tbl["_iPLDamMod"] = item._iPLDamMod;
	tbl["_iPLGetHit"] = item._iPLGetHit;
	tbl["_iPLLight"] = item._iPLLight;
	tbl["_iSplLvlAdd"] = item._iSplLvlAdd;
	tbl["_iRequest"] = item._iRequest;
	tbl["_iUid"] = item._iUid;
	tbl["_iFMinDam"] = item._iFMinDam;
	tbl["_iFMaxDam"] = item._iFMaxDam;
	tbl["_iLMinDam"] = item._iLMinDam;
	tbl["_iLMaxDam"] = item._iLMaxDam;
	tbl["_iPLEnAc"] = item._iPLEnAc;
	tbl["_iPrePower"] = static_cast<int>(item._iPrePower);
	tbl["_iSufPower"] = static_cast<int>(item._iSufPower);
	tbl["_iVAdd1"] = item._iVAdd1;
	tbl["_iVMult1"] = item._iVMult1;
	tbl["_iVAdd2"] = item._iVAdd2;
	tbl["_iVMult2"] = item._iVMult2;
	tbl["_iMinStr"] = item._iMinStr;
	tbl["_iMinMag"] = item._iMinMag;
	tbl["_iMinDex"] = item._iMinDex;
	tbl["_iStatFlag"] = item._iStatFlag;
	tbl["_iDamAcFlags"] = ItemSpecialEffectHfToString(item._iDamAcFlags);
	tbl["dwBuff"] = item.dwBuff;

	return tbl;
}

static sol::table CreateItemTable(const Item &item)
{
	sol::state &L = GetLuaState();
	sol::table tbl = L.create_table();

	tbl["_iSeed"] = item._iSeed;
	tbl["_iCreateInfo"] = item._iCreateInfo;
	tbl["_itype"] = static_cast<int>(item._itype);
	tbl["_iAnimFlag"] = item._iAnimFlag;
	tbl["position"] = { item.position.x, item.position.y };
	tbl["_iDelFlag"] = item._iDelFlag;
	tbl["selectionRegion"] = static_cast<int>(item.selectionRegion);
	tbl["_iPostDraw"] = item._iPostDraw;
	tbl["_iIdentified"] = item._iIdentified;
	tbl["_iMagical"] = static_cast<int>(item._iMagical);
	tbl["_iName"] = item._iName;
	tbl["_iIName"] = item._iIName;
	tbl["_iLoc"] = static_cast<int>(item._iLoc);
	tbl["_iClass"] = ItemClassToString(item._iClass);
	tbl["_iCurs"] = item._iCurs;
	tbl["_ivalue"] = item._ivalue;
	tbl["_iIvalue"] = item._iIvalue;
	tbl["_iMinDam"] = item._iMinDam;
	tbl["_iMaxDam"] = item._iMaxDam;
	tbl["_iAC"] = item._iAC;
	tbl["_iFlags"] = static_cast<int>(item._iFlags);
	tbl["_iMiscId"] = ItemMiscIdToString(item._iMiscId);
	tbl["spell"] = CreateSpellIDTable(item._iSpell);
	tbl["IDidx"] = ItemIndexToString(item.IDidx);
	tbl["_iCharges"] = item._iCharges;
	tbl["_iMaxCharges"] = item._iMaxCharges;
	tbl["_iDurability"] = item._iDurability;
	tbl["_iMaxDur"] = item._iMaxDur;
	tbl["_iPLDam"] = item._iPLDam;
	tbl["_iPLToHit"] = item._iPLToHit;
	tbl["_iPLAC"] = item._iPLAC;
	tbl["_iPLStr"] = item._iPLStr;
	tbl["_iPLMag"] = item._iPLMag;
	tbl["_iPLDex"] = item._iPLDex;
	tbl["_iPLVit"] = item._iPLVit;
	tbl["_iPLFR"] = item._iPLFR;
	tbl["_iPLLR"] = item._iPLLR;
	tbl["_iPLMR"] = item._iPLMR;
	tbl["_iPLMana"] = item._iPLMana;
	tbl["_iPLHP"] = item._iPLHP;
	tbl["_iPLDamMod"] = item._iPLDamMod;
	tbl["_iPLGetHit"] = item._iPLGetHit;
	tbl["_iPLLight"] = item._iPLLight;
	tbl["_iSplLvlAdd"] = item._iSplLvlAdd;
	tbl["_iRequest"] = item._iRequest;
	tbl["_iUid"] = item._iUid;
	tbl["_iFMinDam"] = item._iFMinDam;
	tbl["_iFMaxDam"] = item._iFMaxDam;
	tbl["_iLMinDam"] = item._iLMinDam;
	tbl["_iLMaxDam"] = item._iLMaxDam;
	tbl["_iPLEnAc"] = item._iPLEnAc;
	tbl["_iPrePower"] = static_cast<int>(item._iPrePower);
	tbl["_iSufPower"] = static_cast<int>(item._iSufPower);
	tbl["_iVAdd1"] = item._iVAdd1;
	tbl["_iVMult1"] = item._iVMult1;
	tbl["_iVAdd2"] = item._iVAdd2;
	tbl["_iVMult2"] = item._iVMult2;
	tbl["_iMinStr"] = item._iMinStr;
	tbl["_iMinMag"] = item._iMinMag;
	tbl["_iMinDex"] = item._iMinDex;
	tbl["_iStatFlag"] = item._iStatFlag;
	tbl["_iDamAcFlags"] = static_cast<int>(item._iDamAcFlags);
	tbl["dwBuff"] = item.dwBuff;
	return tbl;
}

bool CallLuaSkipIdxHelper(const std::string_view eventName, _item_indexes idx, bool defaultCondition)
{
	sol::table events = GetLuaEvents();
	sol::object eventObj = events[eventName];
	if (!eventObj.valid() || !eventObj.is<sol::table>())
		return defaultCondition;

	sol::table eventTable = eventObj;
	sol::object triggerObj = eventTable["trigger"];
	if (!triggerObj.valid() || !triggerObj.is<sol::function>())
		return defaultCondition;

	sol::function trigger = triggerObj;
	sol::protected_function_result result = trigger(static_cast<int>(idx));
	if (!result.valid())
		return defaultCondition;

	return result.get<bool>();
}

} // namespace

bool CallLuaNoSpawnCondition(Item &item)
{
	bool defaultCondition = false;

	sol::table events = GetLuaEvents();
	sol::object eventObj = events["NoSpawnCondition"];
	if (!eventObj.valid() || !eventObj.is<sol::table>())
		return defaultCondition;

	sol::table eventTable = eventObj;
	sol::object triggerObj = eventTable["trigger"];
	if (!triggerObj.valid() || !triggerObj.is<sol::function>())
		return defaultCondition;

	sol::function trigger = triggerObj;
	sol::table itemTable = CreateItemTable(item);
	sol::protected_function_result result = trigger(itemTable);
	if (!result.valid())
		return defaultCondition;

	return result.get<bool>();
}

bool CallLuaRewardSkipItem(Item &item, uint8_t iCurs)
{
	bool defaultCondition = item._iCurs != iCurs;

	sol::table events = GetLuaEvents();
	sol::object eventObj = events["RewardSpawnCondition"];
	if (!eventObj.valid() || !eventObj.is<sol::table>())
		return defaultCondition;

	sol::table eventTable = eventObj;
	sol::object triggerObj = eventTable["trigger"];
	if (!triggerObj.valid() || !triggerObj.is<sol::function>())
		return defaultCondition;

	sol::function trigger = triggerObj;
	sol::table itemTable = CreateItemTable(item);
	sol::protected_function_result result = trigger(itemTable, iCurs);
	if (!result.valid())
		return defaultCondition;

	return result.get<bool>();
}

bool CallLuaVendorSkipItem(Item &item, int maxValue)
{
	bool defaultCondition = item._iIvalue > maxValue;

	sol::table events = GetLuaEvents();
	sol::object eventObj = events["VendorSpawnCondition"];
	if (!eventObj.valid() || !eventObj.is<sol::table>())
		return defaultCondition;

	sol::table vendorEvent = eventObj;
	sol::object triggerObj = vendorEvent["trigger"];
	if (!triggerObj.valid() || !triggerObj.is<sol::function>())
		return defaultCondition;

	sol::function trigger = triggerObj;
	sol::table itemTable = GetLuaState().create_table();
	itemTable["iIvalue"] = item._iIvalue;
	sol::protected_function_result result = trigger(itemTable, maxValue);
	if (!result.valid())
		return defaultCondition;

	return result.get<bool>();
}

bool CallLuaSmithPremiumHfSkipItem(Item &item, int maxValue, int strength, int magic, int dexterity, int ivalue)
{
	bool defaultCondition = item._iIvalue > maxValue || item._iMinStr > strength || item._iMinMag > magic || item._iMinDex > dexterity || item._iIvalue < ivalue;

	sol::table events = GetLuaEvents();
	sol::object eventObj = events["SmithPremiumHfSpawnCondition"];
	if (!eventObj.valid() || !eventObj.is<sol::table>())
		return defaultCondition;

	sol::table eventTable = eventObj;
	sol::object triggerObj = eventTable["trigger"];
	if (!triggerObj.valid() || !triggerObj.is<sol::function>())
		return defaultCondition;

	sol::function trigger = triggerObj;
	sol::table itemTable = CreateItemTable(item);
	sol::protected_function_result result = trigger(itemTable, maxValue, strength, magic, dexterity, ivalue);
	if (!result.valid())
		return defaultCondition;

	return result.get<bool>();
}

bool CallLuaBoyHfSkipItem(Item &item, int maxValue, int strength, int magic, int dexterity, int ivalue, int count, int maxCount)
{
	bool defaultCondition = item._iIvalue > maxValue || item._iMinStr > strength || item._iMinMag > magic || item._iMinDex > dexterity || item._iIvalue < ivalue || count < maxCount;

	sol::table events = GetLuaEvents();
	sol::object eventObj = events["BoyHfSpawnCondition"];
	if (!eventObj.valid() || !eventObj.is<sol::table>())
		return defaultCondition;

	sol::table eventTable = eventObj;
	sol::object triggerObj = eventTable["trigger"];
	if (!triggerObj.valid() || !triggerObj.is<sol::function>())
		return defaultCondition;

	sol::function trigger = triggerObj;
	sol::table itemTable = CreateItemTable(item);
	sol::protected_function_result result = trigger(itemTable, maxValue, strength, magic, dexterity, ivalue, count, maxCount);
	if (!result.valid())
		return defaultCondition;

	return result.get<bool>();
}

bool CallLuaUniqueMonsterSkipIdx(_item_indexes idx)
{
	return CallLuaSkipIdxHelper("UniqueMonsterSkipIdx", idx, false);
}

bool CallLuaMonsterSkipIdx(_item_indexes idx)
{
	return CallLuaSkipIdxHelper("MonsterSkipIdx", idx, false);
}

bool CallLuaDungeonRndItemSkipIdx(_item_indexes idx)
{
	return CallLuaSkipIdxHelper("DungeonRndItemSkipIdx", idx, false);
}

bool CallLuaDungeonTypeItemSkipIdx(_item_indexes idx)
{
	return CallLuaSkipIdxHelper("DungeonTypeItemSkipIdx", idx, false);
}

bool CallLuaDungeonSpellBookSkipIdx(_item_indexes idx)
{
	return CallLuaSkipIdxHelper("DungeonSpellBookSkipIdx", idx, false);
}

} // namespace devilution
