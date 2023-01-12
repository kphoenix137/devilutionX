/**
 * @file itemdat.h
 *
 * Interface of all item data.
 */
#pragma once

#include <cstdint>

#include "objdat.h"
#include "spelldat.h"
#include "utils/stdcompat/string_view.hpp"

namespace devilution {

/** @todo add missing values and apply */
enum class ItemIndex : int16_t { // TODO defines all indexes in AllItemsList
	Gold,
	WarriorSword,
	WarriorShield,
	WarriorClub,
	RogueBow,
	SorcererStaff,
	Cleaver,
	FirstQuest = Cleaver,
	UndeadCrown,
	EmpyreanBand,
	MagicRock,
	OpticAmulet,
	RingTruth,
	TavernSign,
	HarlequinCrest,
	VeilSteel,
	GoldenElixir,
	AnvilFury,
	BlackMushroom,
	Brain,
	FungalTome,
	SpectralElixir,
	BloodStone,
	Map,
	LastQuest = Map,
	Ear,
	PotionHealing,
	PotionMana,
	ScrollIdentify,
	ScrollTownPortal,
	ArkainesValor,
	PotionFullHealing,
	PotionFullMana,
	GriswoldsEdge,
	BovinePlate,
	StaffLazarus,
	ScrollResurrect,
	Oil,
	MonkShortStaff,
	BardSword,
	BardDagger,
	RuneBomb,
	Theodore,
	AuricAmulet,
	TornNote1,
	TornNote2,
	TornNote3,
	ReconstructedNote,
	BrownSuit,
	GreySuit,
	Book1 = 114,
	Book2,
	Book3,
	Book4,
	BarbarianClub = 139,
	RuneStone = 165,
	SorcererStaffDiablo,

	Last = SorcererStaffDiablo,
	None = -1,
};

enum class ItemDropRate : uint8_t {
	Never,
	Regular,
	Double,
};

enum class ItemClass : uint8_t {
	None,
	Weapon,
	Armor,
	Misc,
	Gold,
	Quest,
};

enum class ItemEquipType : int8_t {
	None,
	OneHand,
	TwoHand,
	Armor,
	Helm,
	Ring,
	Amulet,
	Unequipable,
	Belt,
	Invalid = -1,
};

/// Item graphic IDs; frame_num-11 of objcurs.cel.
enum class ItemCursorGraphic : uint8_t {
	// clang-format off
	PotionFullMana,
	Scroll,
	Scroll2, // unused
	Scroll3, // unused
	GoldSmall,
	GoldMedium,
	GoldLarge,
	GoldRing, // unused
	Bleeder,
	Bramble,
	RingTruth,
	RingRegha,
	Ring,
	RingEngagement,
	ConstrictingRing,
	SpectralElixir,
	ThreeColorPot, // unused
	GoldenElixir,
	EmpyreanBand,
	EarSorcerer,
	EarWarrior,
	EarRogue,
	Sphere, // unused
	Cube, // unused
	Pyramid, // unused
	BloodStone,
	JSphere, // unused
	JCube, // unused
	JPyramid, // unused
	Vial, // unused
	Oil,
	ElixirVitality,
	PotionHealing,
	PotionFullRejuvenation,
	ElixirMagic,
	PotionFullHealing,
	ElixirDexterity,
	PotionRejuvenation,
	ElixirStrength,
	PotionMana,
	Brain,
	Claw, // unused
	Fang, // unused
	Bread, // unused
	OpticAmulet,
	Amulet,
	Amulet2, // unused
	Amulet3, // unused
	Amulet4, // unused
	Pouch1, // unused
	Wizardspike,
	Dagger,
	BigBottle, // unused
	BlackRazor,
	GonnagalsDirk,
	Dagger5, // unused
	Blade,
	BastardSword,
	ExecutionersBlade,
	Mace,
	LongSword,
	BroadSword,
	Falchion,
	MorningStar,
	ShortSword,
	Claymore,
	Club,
	Sabre,
	FalconsTalon,
	Club1, // unused
	SpikedClub,
	Club3, // unused
	Scimitar,
	MagSword, // unused
	SkulSword, // unused
	FullHelm, // unused
	MagicRock,
	HelmSpirits,
	UndeadCrown,
	RoyalCirclet,
	FoolsCrest,
	HarlequinCrest,
	Helm,
	Buckler,
	FHelm2, // unused
	VeilSteel,
	BookGrey,
	BookRed,
	BookBlue,
	BlackMushroom,
	SkullCap,
	Cap,
	TornFleshSouls,
	ThinkingCap,
	Clothes, // unused
	Crown,
	Map,
	FungalTome,
	GreatHelm,
	OverlordsHelm,
	CompShld, // unused
	BattleAxe,
	HuntersBow,
	FieldPlate,
	Stonecutter,
	SmallShield,
	Cleaver,
	StuddedLeatherArmor,
	Eaglehorn,
	ShortStaff,
	TwoHandedSword,
	ChainMail,
	SmallAxe,
	KiteShield,
	ScaleMail,
	SmlShld, // unused
	SplitSkullShield,
	DragonsBreach,
	ShortBow,
	LongBattleBow,
	LongWarBow,
	WarHammer,
	Maul,
	LongStaff,
	WarStaff,
	LongStaffUnused, // unused
	TavernSign,
	HardLeatherArmor,
	Rags,
	QuiltedArmor,
	BallNChn, // unused
	Flail,
	TowerShield,
	CompositeBow,
	GreatSword,
	LeatherArmor,
	SplintMail,
	Robe,
	Nightscape,
	RingArmor, // unused
	AnvilFury,
	BroadAxe,
	LargeAxe,
	GreatAxe,
	Axe,
	GreatAxeUnused, // unused
	HolyDefender,
	LargeShield,
	GothicShield,
	Cloak,
	Cape,
	FullPlateMail,
	GothicPlate,
	BreastPlate,
	RingMail,
	StaffLazarus,
	GemGrtAxe, // unused
	ArkainesValor,
	Needler,
	NajsLightPlate,
	Grizzly,
	Grandfather,
	Protector,
	MesserschmidtsReaver,
	Windforce,
	ShortWarBow,
	CompositeStaff,
	ShortBattleBow,
	Gold, // MerlinRing in Hellfire?
	ManaRing, // unused
	AmuletWarding,
	NecMagic, // unused
	NecHealth, // unused
	KariksRing,
	RingGround, // unused
	AmulProt, // unused
	MercurialRing,
	RingThunder,
	NecTruth, // unused
	GiantsKnuckle,
	AuricAmulet,
	RingMystics,
	RingCopper, // unused
	AcolytesAmulet,
	RingMagma,
	NecPurify, // unused
	GladiatorsRing,
	RuneBomb,
	Theodore,
	TornNote1,
	TornNote2,
	TornNote3,
	ReconstructedNote,
	RuneFire,
	GreaterRuneFire,
	RuneLightning,
	GreaterRuneLightning,
	RuneStone,
	GreySuit,
	BrownSuit,
	EaterSouls,
	SwordGlam, // unused
	SwordSerr, // unused
	ArmorGloom,
	BoneChainArmor,
	Thunderclap,
	Diamondedge,
	StafJester, // unused
	StafMana, // unused
	Flambeau,
	GnatSting,
	AxeAncient, // unused
	ClubCarnag, // unused
	MaceDark, // unused
	ClubDecay, // unused
	AxeDecay, // unused
	SwrdDecay, // unused
	MaceDecay, // unused
	StafDecay, // unused
	Blitzen,
	ClubOuch, // unused
	SwrdDevast, // unused
	AxeDevast, // unused
	MornDevast, // unused
	MaceDevast, // unused
	DemonPlateArmor,
	BovinePlate,
	// clang-format on
};

enum class ItemType : int8_t {
	Misc,
	Sword,
	Axe,
	Bow,
	Mace,
	Shield,
	LightArmor,
	Helm,
	MediumArmor,
	HeavyArmor,
	Staff,
	Gold,
	Ring,
	Amulet,
	None = -1,
};

string_view ItemTypeToString(ItemType itemType);

enum class UniqueBaseItem : int8_t {
	None,
	ShortBow,
	LongBow,
	HuntersBow,
	CompoundBow,
	LongWarBow,
	LongBattleBow,
	Dagger,
	Falchion,
	Claymore,
	Broadsword,
	Sabre,
	Scimitar,
	Longsword,
	BastardSword,
	TwoHandedSword,
	GreatSword,
	Cleaver,
	LargeAxe,
	BroadAxe,
	SmallAxe,
	BattleAxe,
	GreatAxe,
	Mace,
	MorningStar,
	SpikedClub,
	Maul,
	WarHammer,
	Flail,
	LongStaff,
	ShortStaff,
	CompositeStaff,
	QuarterStaff,
	WarStaff,
	SkullCap,
	Helm,
	GreatHelm,
	Crown,
	UITYPE_38,
	UniqueBaseItem::Rags,
	UniqueBaseItem::StuddedArmor,
	UniqueBaseItem::Cloak,
	UniqueBaseItem::Robe,
	UniqueBaseItem::ChainMail,
	UniqueBaseItem::LeatherArmor,
	UniqueBaseItem::BreastPlate,
	UniqueBaseItem::Cape,
	UniqueBaseItem::PlateMail,
	UniqueBaseItem::FullPlateMail,
	UniqueBaseItem::Buckler,
	UniqueBaseItem::SmallShield,
	UniqueBaseItem::LargeShield,
	UniqueBaseItem::KiteShield,
	UniqueBaseItem::GothicShield,
	UniqueBaseItem::Ring,
	UITYPE_55,
	UniqueBaseItem::Amulet,
	UniqueBaseItem::UndeadCrown,
	UniqueBaseItem::EmpyreanBand,
	UniqueBaseItem::OpticAmulet,
	UniqueBaseItem::RingTruth,
	UniqueBaseItem::HarlequinCrest,
	UniqueBaseItem::Map,
	UniqueBaseItem::Elixir,
	UniqueBaseItem::ArkainesValor,
	UniqueBaseItem::VeilSteel,
	UniqueBaseItem::GriswoldsEdge,
	UniqueBaseItem::Lightforge,
	UniqueBaseItem::StaffLazarus,
	UniqueBaseItem::StaffLazarus,
	UniqueBaseItem::Invalid = -1,
};

enum class ItemSpecialEffect : uint32_t {
	// clang-format off
	None                   = 0,
	RandomStealLife        = 1 << 1,
	RandomArrowVelocity    = 1 << 2,
	FireArrows             = 1 << 3,
	FireDamage             = 1 << 4,
	LightningDamage        = 1 << 5,
	DrainLife              = 1 << 6,
	MultipleArrows         = 1 << 9,
	Knockback              = 1 << 11,
	StealMana3             = 1 << 13,
	StealMana5             = 1 << 14,
	StealLife3             = 1 << 15,
	StealLife5             = 1 << 16,
	QuickAttack            = 1 << 17,
	FastAttack             = 1 << 18,
	FasterAttack           = 1 << 19,
	FastestAttack          = 1 << 20,
	FastHitRecovery        = 1 << 21,
	FasterHitRecovery      = 1 << 22,
	FastestHitRecovery     = 1 << 23,
	FastBlock              = 1 << 24,
	LightningArrows        = 1 << 25,
	Thorns                 = 1 << 26,
	NoMana                 = 1 << 27,
	HalfTrapDamage         = 1 << 28,
	TripleDemonDamage      = 1 << 30,
	ZeroResistance         = 1U << 31,
	// clang-format on
};
use_enum_as_flags(ItemSpecialEffect);

enum class ItemSpecialEffectHf : uint8_t {
	// clang-format off
	None               = 0,
	Devastation        = 1 << 0,
	Decay              = 1 << 1,
	Peril              = 1 << 2,
	Jesters            = 1 << 3,
	Doppelganger       = 1 << 4,
	ArmorClassVsDemons = 1 << 5,
	ArmorClassVsUndead = 1 << 6,
	// clang-format on
};
use_enum_as_flags(ItemSpecialEffectHf);

enum item_misc_id : int8_t {
	IMISC_NONE,
	IMISC_USEFIRST,
	IMISC_FULLHEAL,
	IMISC_HEAL,
	IMISC_0x4, // Unused
	IMISC_0x5, // Unused
	IMISC_MANA,
	IMISC_FULLMANA,
	IMISC_0x8, // Unused
	IMISC_0x9, // Unused
	IMISC_ELIXSTR,
	IMISC_ELIXMAG,
	IMISC_ELIXDEX,
	IMISC_ELIXVIT,
	IMISC_0xE,  // Unused
	IMISC_0xF,  // Unused
	IMISC_0x10, // Unused
	IMISC_0x11, // Unused
	IMISC_REJUV,
	IMISC_FULLREJUV,
	IMISC_USELAST,
	IMISC_SCROLL,
	IMISC_SCROLLT,
	IMISC_STAFF,
	IMISC_BOOK,
	IMISC_RING,
	IMISC_AMULET,
	IMISC_UNIQUE,
	IMISC_0x1C, // Unused
	IMISC_OILFIRST,
	IMISC_OILOF, /* oils are beta or hellfire only */
	IMISC_OILACC,
	IMISC_OILMAST,
	IMISC_OILSHARP,
	IMISC_OILDEATH,
	IMISC_OILSKILL,
	IMISC_OILBSMTH,
	IMISC_OILFORT,
	IMISC_OILPERM,
	IMISC_OILHARD,
	IMISC_OILIMP,
	IMISC_OILLAST,
	IMISC_MAPOFDOOM,
	IMISC_EAR,
	IMISC_SPECELIX,
	IMISC_0x2D, // Unused
	IMISC_RUNEFIRST,
	IMISC_RUNEF,
	IMISC_RUNEL,
	IMISC_GR_RUNEL,
	IMISC_GR_RUNEF,
	IMISC_RUNES,
	IMISC_RUNELAST,
	IMISC_AURIC,
	IMISC_NOTE,
	IMISC_INVALID = -1,
};

struct ItemData {
	enum ItemDropRate iRnd;
	enum ItemClass iClass;
	enum ItemEquipType iLoc;
	enum ItemCursorGraphic iCurs;
	enum ItemType itype;
	enum UniqueBaseItem iItemId;
	const char *iName;
	const char *iSName;
	uint8_t iMinMLvl;
	uint8_t iDurability;
	uint8_t iMinDam;
	uint8_t iMaxDam;
	uint8_t iMinAC;
	uint8_t iMaxAC;
	uint8_t iMinStr;
	uint8_t iMinMag;
	uint8_t iMinDex;
	ItemSpecialEffect iFlags; // ItemSpecialEffect as bit flags
	enum item_misc_id iMiscId;
	enum spell_id iSpell;
	bool iUsable;
	uint16_t iValue;
};

enum class ItemEffectType : int8_t {
	ToHit,
	ToHitCurse,
	DamagePercent,
	DamagePercentCurse,
	ToHitDamagePercent,
	ToHitDamagePercentCurse,
	ArmorClassPercent,
	ArmorClassPercentCurse,
	FireResistance,
	LightningResistance,
	MagicResistance,
	AllResistances,
	SpellLevelAdd = 14,
	Charges,
	FireDamage,
	LightningDamage,
	Strength = 19,
	StrengthCurse,
	Magic,
	MagicCurse,
	Dexterity,
	DexterityCurse,
	Vitality,
	VitalityCurse,
	AllAttributes,
	AllAttributesCurse,
	GetHitCurse,
	GetHit,
	Life,
	LifeCurse,
	Mana,
	ManaCurse,
	Durability,
	DurabilityCurse,
	Indestructible,
	LightRadius,
	LightRadiusCurse,
	MultipleArrows = 41, /* only used in hellfire */
	FireArrows,
	LightningArrows,
	InventoryCursorGraphic,
	Thorns,
	NoMana,
	Fireball = 50, /* only used in hellfire */
	HalfTrapDamage = 52,
	Knockback,
	StealMana = 55,
	StealLife,
	TargetArmorClass,
	FastAttack,
	FastHitRecovery,
	FastBlock,
	DamageModifier,
	RandomArrowVelocity,
	SetDamage,
	SetDurability,
	NoMinimumStrength,
	Spell,
	OneHanded = 68,
	TripleDemonDamage,
	ZeroResistances,
	DrainLife = 72,
	RandomLifeSteal,
	SetArmorClass = 75,
	Lightning,
	ChargedBolt,
	SetArmorClassCurse = 79,
	LastDiablo = SetArmorClassCurse,
	FireResistanceCurse,
	LightningResistanceCurse,
	MagicResistanceCurse,
	Devastation = 84,
	Decay,
	Peril,
	Jesters,
	Crystalline,
	Doppelganger,
	ArmorClassVsDemons,
	ArmorClassVsUndead,
	ManaToLife,
	LifeToMana,
	Invalid = -1,
};

enum class GoodOrEvil : uint8_t {
	Any,
	Evil,
	Good,
};

enum class AffixItemType : uint8_t {
	// clang-format off
	None      = 0,
	Misc      = 1 << 0,
	Bow       = 1 << 1,
	Staff     = 1 << 2,
	Weapon    = 1 << 3,
	Shield    = 1 << 4,
	Armor     = 1 << 5,
	// clang-format on
};
use_enum_as_flags(AffixItemType);

struct ItemPower {
	ItemEffectType type = ItemEffectType::Invalid;
	int param1 = 0;
	int param2 = 0;
};

struct PLStruct {
	const char *PLName;
	ItemPower power;
	int8_t PLMinLvl;
	AffixItemType PLIType; // AffixItemType as bit flags
	enum GoodOrEvil PLGOE;
	bool PLDouble;
	bool PLOk;
	int minVal;
	int maxVal;
	int multVal;
};

struct UniqueItem {
	const char *UIName;
	enum unique_base_item UIItemId;
	int8_t UIMinLvl;
	uint8_t UINumPL;
	int UIValue;
	ItemPower powers[6];
};

extern ItemData AllItemsList[];
extern const PLStruct ItemPrefixes[];
extern const PLStruct ItemSuffixes[];
extern const UniqueItem UniqueItems[];

} // namespace devilution
