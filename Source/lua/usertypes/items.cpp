#include "lua/usertypes/items.hpp"

#include <sol/sol.hpp>

#include "lua/lua.hpp"

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

std::string SpellIDToString(SpellID spell)
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

} // namespace

void RegisterItemUsertype(sol::state &lua)
{
	lua.new_usertype<Item>("Item",
	    // Direct member access (if desired)
	    "_iSeed", &Item::_iSeed,
	    "_iCreateInfo", &Item::_iCreateInfo,
	    "_iAnimFlag", &Item::_iAnimFlag,
	    // For the position, you might need to register a usertype for Point separately.
	    "position", &Item::position,
	    "_iDelFlag", &Item::_iDelFlag,
	    "selectionRegion", sol::property([](const Item &i) { return static_cast<int>(i.selectionRegion); }),
	    "_iPostDraw", &Item::_iPostDraw,
	    "_iIdentified", &Item::_iIdentified,
	    "_iMagical", sol::property([](const Item &i) { return static_cast<int>(i._iMagical); }),
	    "_iName", &Item::_iName,
	    "_iIName", &Item::_iIName,
	    "_iLoc", sol::property([](const Item &i) { return ItemEquipTypeToString(i._iLoc); }),
	    "_iClass", sol::property([](const Item &i) { return ItemClassToString(i._iClass); }),
	    "_iCurs", &Item::_iCurs,
	    "_ivalue", &Item::_ivalue,
	    "_iIvalue", &Item::_iIvalue,
	    "_iMinDam", &Item::_iMinDam,
	    "_iMaxDam", &Item::_iMaxDam,
	    "_iAC", &Item::_iAC,
	    "_iFlags", sol::property([](const Item &i) { return static_cast<int>(i._iFlags); }),
	    "_iMiscId", sol::property([](const Item &i) { return ItemMiscIdToString(i._iMiscId); }),
	    "spell", sol::property([](const Item &i) { return SpellIDToString(i._iSpell); }),
	    "IDidx", sol::property([](const Item &i) { return ItemIndexToString(i.IDidx); }),
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
