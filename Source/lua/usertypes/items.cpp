#include "lua/usertypes/items.hpp"

#include <sol/sol.hpp>

#include "lua/lua.hpp"

namespace devilution {

namespace {

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
		result.erase(result.size() - 2); // Remove trailing comma and space
	return result;
}

} // namespace

void RegisterItemUsertype(sol::state &lua)
{
	// Register enums using sol.new_enum:
	lua.new_enum("ItemEquipType",
	    "ILOC_NONE", ILOC_NONE,
	    "ILOC_ONEHAND", ILOC_ONEHAND,
	    "ILOC_TWOHAND", ILOC_TWOHAND,
	    "ILOC_ARMOR", ILOC_ARMOR,
	    "ILOC_HELM", ILOC_HELM,
	    "ILOC_RING", ILOC_RING,
	    "ILOC_AMULET", ILOC_AMULET,
	    "ILOC_UNEQUIPABLE", ILOC_UNEQUIPABLE,
	    "ILOC_BELT", ILOC_BELT,
	    "ILOC_INVALID", ILOC_INVALID);
	lua.new_enum("ItemClass",
	    "ICLASS_NONE", ICLASS_NONE,
	    "ICLASS_WEAPON", ICLASS_WEAPON,
	    "ICLASS_ARMOR", ICLASS_ARMOR,
	    "ICLASS_MISC", ICLASS_MISC,
	    "ICLASS_GOLD", ICLASS_GOLD,
	    "ICLASS_QUEST", ICLASS_QUEST);
	lua.new_enum("ItemMiscId",
	    "IMISC_NONE", IMISC_NONE,
	    "IMISC_FULLHEAL", IMISC_FULLHEAL,
	    "IMISC_HEAL", IMISC_HEAL,
	    "IMISC_MANA", IMISC_MANA,
	    "IMISC_FULLMANA", IMISC_FULLMANA,
	    "IMISC_ELIXSTR", IMISC_ELIXSTR,
	    "IMISC_ELIXMAG", IMISC_ELIXMAG,
	    "IMISC_ELIXDEX", IMISC_ELIXDEX,
	    "IMISC_ELIXVIT", IMISC_ELIXVIT,
	    "IMISC_REJUV", IMISC_REJUV,
	    "IMISC_FULLREJUV", IMISC_FULLREJUV,
	    "IMISC_SCROLL", IMISC_SCROLL,
	    "IMISC_SCROLLT", IMISC_SCROLLT,
	    "IMISC_STAFF", IMISC_STAFF,
	    "IMISC_BOOK", IMISC_BOOK,
	    "IMISC_RING", IMISC_RING,
	    "IMISC_AMULET", IMISC_AMULET,
	    "IMISC_UNIQUE", IMISC_UNIQUE);
	lua.new_enum("SpellID",
	    "Null", SpellID::Null,
	    "Firebolt", SpellID::Firebolt,
	    "Healing", SpellID::Healing,
	    "Lightning", SpellID::Lightning,
	    "Flash", SpellID::Flash,
	    "Identify", SpellID::Identify,
	    "FireWall", SpellID::FireWall,
	    "TownPortal", SpellID::TownPortal,
	    "StoneCurse", SpellID::StoneCurse,
	    "Infravision", SpellID::Infravision,
	    "Phasing", SpellID::Phasing,
	    "ManaShield", SpellID::ManaShield,
	    "Fireball", SpellID::Fireball,
	    "Guardian", SpellID::Guardian,
	    "ChainLightning", SpellID::ChainLightning,
	    "FlameWave", SpellID::FlameWave,
	    "DoomSerpents", SpellID::DoomSerpents,
	    "BloodRitual", SpellID::BloodRitual,
	    "Nova", SpellID::Nova,
	    "Invisibility", SpellID::Invisibility,
	    "Inferno", SpellID::Inferno,
	    "Golem", SpellID::Golem,
	    "Rage", SpellID::Rage,
	    "Teleport", SpellID::Teleport,
	    "Apocalypse", SpellID::Apocalypse,
	    "Etherealize", SpellID::Etherealize,
	    "ItemRepair", SpellID::ItemRepair,
	    "StaffRecharge", SpellID::StaffRecharge,
	    "TrapDisarm", SpellID::TrapDisarm,
	    "Elemental", SpellID::Elemental,
	    "ChargedBolt", SpellID::ChargedBolt,
	    "HolyBolt", SpellID::HolyBolt,
	    "Resurrect", SpellID::Resurrect,
	    "Telekinesis", SpellID::Telekinesis,
	    "HealOther", SpellID::HealOther,
	    "BloodStar", SpellID::BloodStar,
	    "BoneSpirit", SpellID::BoneSpirit,
	    "Mana", SpellID::Mana,
	    "Magi", SpellID::Magi,
	    "Jester", SpellID::Jester,
	    "LightningWall", SpellID::LightningWall,
	    "Immolation", SpellID::Immolation,
	    "Warp", SpellID::Warp,
	    "Reflect", SpellID::Reflect,
	    "Berserk", SpellID::Berserk,
	    "RingOfFire", SpellID::RingOfFire,
	    "Search", SpellID::Search,
	    "RuneOfFire", SpellID::RuneOfFire,
	    "RuneOfLight", SpellID::RuneOfLight,
	    "RuneOfNova", SpellID::RuneOfNova,
	    "RuneOfImmolation", SpellID::RuneOfImmolation,
	    "RuneOfStone", SpellID::RuneOfStone,
	    "Invalid", SpellID::Invalid);

	lua.new_usertype<Item>("Item",
	    "_iSeed", &Item::_iSeed,
	    "_iCreateInfo", &Item::_iCreateInfo,
	    "_iAnimFlag", &Item::_iAnimFlag,
	    "position", &Item::position,
	    "_iDelFlag", &Item::_iDelFlag,
	    "selectionRegion", sol::property([](const Item &i) { return static_cast<int>(i.selectionRegion); }),
	    "_iPostDraw", &Item::_iPostDraw,
	    "_iIdentified", &Item::_iIdentified,
	    "_iMagical", sol::property([](const Item &i) { return static_cast<int>(i._iMagical); }),
	    "_iName", &Item::_iName,
	    "_iIName", &Item::_iIName,
	    "_iLoc", &Item::_iLoc,
	    "_iClass", &Item::_iClass,
	    "_iCurs", &Item::_iCurs,
	    "_ivalue", &Item::_ivalue,
	    "_iIvalue", &Item::_iIvalue,
	    "_iMinDam", &Item::_iMinDam,
	    "_iMaxDam", &Item::_iMaxDam,
	    "_iAC", &Item::_iAC,
	    "_iFlags", sol::property([](const Item &i) { return static_cast<int>(i._iFlags); }),
	    "_iMiscId", &Item::_iMiscId,
	    "_iSpell", &Item::_iSpell,
	    "IDidx", &Item::IDidx,
	    "_iCharges", &Item::_iCharges,
	    "_iMaxCharges", &Item::_iMaxCharges,
	    "_iDurability", &Item::_iDurability,
	    "_iMaxDur", &Item::_iMaxDur,
	    "_iPLDam", &Item::_iPLDam,
	    "_iPLToHit", &Item::_iPLToHit,
	    "_iPLAC", &Item::_iPLAC,
	    "_iPLStr", &Item::_iPLStr,
	    "_iPLMag", &Item::_iPLMag,
	    "_iPLDex", &Item::_iPLDex,
	    "_iPLVit", &Item::_iPLVit,
	    "_iPLFR", &Item::_iPLFR,
	    "_iPLLR", &Item::_iPLLR,
	    "_iPLMR", &Item::_iPLMR,
	    "_iPLMana", &Item::_iPLMana,
	    "_iPLHP", &Item::_iPLHP,
	    "_iPLDamMod", &Item::_iPLDamMod,
	    "_iPLGetHit", &Item::_iPLGetHit,
	    "_iPLLight", &Item::_iPLLight,
	    "_iSplLvlAdd", &Item::_iSplLvlAdd,
	    "_iRequest", &Item::_iRequest,
	    "_iUid", &Item::_iUid,
	    "_iFMinDam", &Item::_iFMinDam,
	    "_iFMaxDam", &Item::_iFMaxDam,
	    "_iLMinDam", &Item::_iLMinDam,
	    "_iLMaxDam", &Item::_iLMaxDam,
	    "_iPLEnAc", &Item::_iPLEnAc,
	    "_iPrePower", sol::property([](const Item &i) { return static_cast<int>(i._iPrePower); }),
	    "_iSufPower", sol::property([](const Item &i) { return static_cast<int>(i._iSufPower); }),
	    "_iVAdd1", &Item::_iVAdd1,
	    "_iVMult1", &Item::_iVMult1,
	    "_iVAdd2", &Item::_iVAdd2,
	    "_iVMult2", &Item::_iVMult2,
	    "_iMinStr", &Item::_iMinStr,
	    "_iMinMag", &Item::_iMinMag,
	    "_iMinDex", &Item::_iMinDex,
	    "_iStatFlag", &Item::_iStatFlag,
	    "_iDamAcFlags", sol::property([](const Item &i) { return ItemSpecialEffectHfToString(i._iDamAcFlags); }),
	    "dwBuff", &Item::dwBuff);
}

sol::object CreateItemObject(const Item &item)
{
	sol::state &L = GetLuaState();

	if (!L["Item"].valid())
		RegisterItemUsertype(L);

	return sol::make_object(L, item);
}

} // namespace devilution
