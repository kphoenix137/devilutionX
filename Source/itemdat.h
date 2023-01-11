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
enum _item_indexes : int16_t { // TODO defines all indexes in AllItemsList
	IDI_GOLD,
	IDI_WARRIOR,
	IDI_WARRSHLD,
	IDI_WARRCLUB,
	IDI_ROGUE,
	IDI_SORCERER,
	IDI_CLEAVER,
	IDI_FIRSTQUEST = IDI_CLEAVER,
	IDI_SKCROWN,
	IDI_INFRARING,
	IDI_ROCK,
	IDI_OPTAMULET,
	IDI_TRING,
	IDI_BANNER,
	IDI_HARCREST,
	IDI_STEELVEIL,
	IDI_GLDNELIX,
	IDI_ANVIL,
	IDI_MUSHROOM,
	IDI_BRAIN,
	IDI_FUNGALTM,
	IDI_SPECELIX,
	IDI_BLDSTONE,
	IDI_MAPOFDOOM,
	IDI_LASTQUEST = IDI_MAPOFDOOM,
	IDI_EAR,
	IDI_HEAL,
	IDI_MANA,
	IDI_IDENTIFY,
	IDI_PORTAL,
	IDI_ARMOFVAL,
	IDI_FULLHEAL,
	IDI_FULLMANA,
	IDI_GRISWOLD,
	IDI_LGTFORGE,
	IDI_LAZSTAFF,
	IDI_RESURRECT,
	IDI_OIL,
	IDI_SHORTSTAFF,
	IDI_BARDSWORD,
	IDI_BARDDAGGER,
	IDI_RUNEBOMB,
	IDI_THEODORE,
	IDI_AURIC,
	IDI_NOTE1,
	IDI_NOTE2,
	IDI_NOTE3,
	IDI_FULLNOTE,
	IDI_BROWNSUIT,
	IDI_GREYSUIT,
	IDI_BOOK1 = 114,
	IDI_BOOK2,
	IDI_BOOK3,
	IDI_BOOK4,
	IDI_BARBARIAN = 139,
	IDI_RUNEOFSTONE = 165,
	IDI_SORCERER_DIABLO,

	IDI_LAST = IDI_SORCERER_DIABLO,
	IDI_NONE = -1,
};

enum item_drop_rate : uint8_t {
	IDROP_NEVER,
	IDROP_REGULAR,
	IDROP_DOUBLE,
};

enum item_class : uint8_t {
	ICLASS_NONE,
	ICLASS_WEAPON,
	ICLASS_ARMOR,
	ICLASS_MISC,
	ICLASS_GOLD,
	ICLASS_QUEST,
};

enum item_equip_type : int8_t {
	ILOC_NONE,
	ILOC_ONEHAND,
	ILOC_TWOHAND,
	ILOC_ARMOR,
	ILOC_HELM,
	ILOC_RING,
	ILOC_AMULET,
	ILOC_UNEQUIPABLE,
	ILOC_BELT,
	ILOC_INVALID = -1,
};

/// Item graphic IDs; frame_num-11 of objcurs.cel.
enum class ItemCursorGraphic : uint8_t {
	// clang-format off
	PotionFullMana,
	ScrollRed,
	ScrollBrown, // unused
	ScrollBlue, // unused
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

enum unique_base_item : int8_t {
	UITYPE_NONE,
	UITYPE_SHORTBOW,
	UITYPE_LONGBOW,
	UITYPE_HUNTBOW,
	UITYPE_COMPBOW,
	UITYPE_WARBOW,
	UITYPE_BATTLEBOW,
	UITYPE_DAGGER,
	UITYPE_FALCHION,
	UITYPE_CLAYMORE,
	UITYPE_BROADSWR,
	UITYPE_SABRE,
	UITYPE_SCIMITAR,
	UITYPE_LONGSWR,
	UITYPE_BASTARDSWR,
	UITYPE_TWOHANDSWR,
	UITYPE_GREATSWR,
	UITYPE_CLEAVER,
	UITYPE_LARGEAXE,
	UITYPE_BROADAXE,
	UITYPE_SMALLAXE,
	UITYPE_BATTLEAXE,
	UITYPE_GREATAXE,
	UITYPE_MACE,
	UITYPE_MORNSTAR,
	UITYPE_SPIKCLUB,
	UITYPE_MAUL,
	UITYPE_WARHAMMER,
	UITYPE_FLAIL,
	UITYPE_LONGSTAFF,
	UITYPE_SHORTSTAFF,
	UITYPE_COMPSTAFF,
	UITYPE_QUARSTAFF,
	UITYPE_WARSTAFF,
	UITYPE_SKULLCAP,
	UITYPE_HELM,
	UITYPE_GREATHELM,
	UITYPE_CROWN,
	UITYPE_38,
	UITYPE_RAGS,
	UITYPE_STUDARMOR,
	UITYPE_CLOAK,
	UITYPE_ROBE,
	UITYPE_CHAINMAIL,
	UITYPE_LEATHARMOR,
	UITYPE_BREASTPLATE,
	UITYPE_CAPE,
	UITYPE_PLATEMAIL,
	UITYPE_FULLPLATE,
	UITYPE_BUCKLER,
	UITYPE_SMALLSHIELD,
	UITYPE_LARGESHIELD,
	UITYPE_KITESHIELD,
	UITYPE_GOTHSHIELD,
	UITYPE_RING,
	UITYPE_55,
	UITYPE_AMULET,
	UITYPE_SKCROWN,
	UITYPE_INFRARING,
	UITYPE_OPTAMULET,
	UITYPE_TRING,
	UITYPE_HARCREST,
	UITYPE_MAPOFDOOM,
	UITYPE_ELIXIR,
	UITYPE_ARMOFVAL,
	UITYPE_STEELVEIL,
	UITYPE_GRISWOLD,
	UITYPE_LGTFORGE,
	UITYPE_LAZSTAFF,
	UITYPE_BOVINE,
	UITYPE_INVALID = -1,
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
	ACAgainstDemons    = 1 << 5,
	ACAgainstUndead    = 1 << 6,
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
	enum item_drop_rate iRnd;
	enum item_class iClass;
	enum item_equip_type iLoc;
	enum ItemCursorGraphic iCurs;
	enum ItemType itype;
	enum unique_base_item iItemId;
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

enum item_effect_type : int8_t {
	IPL_TOHIT,
	IPL_TOHIT_CURSE,
	IPL_DAMP,
	IPL_DAMP_CURSE,
	IPL_TOHIT_DAMP,
	IPL_TOHIT_DAMP_CURSE,
	IPL_ACP,
	IPL_ACP_CURSE,
	IPL_FIRERES,
	IPL_LIGHTRES,
	IPL_MAGICRES,
	IPL_ALLRES,
	IPL_SPLLVLADD = 14,
	IPL_CHARGES,
	IPL_FIREDAM,
	IPL_LIGHTDAM,
	IPL_STR = 19,
	IPL_STR_CURSE,
	IPL_MAG,
	IPL_MAG_CURSE,
	IPL_DEX,
	IPL_DEX_CURSE,
	IPL_VIT,
	IPL_VIT_CURSE,
	IPL_ATTRIBS,
	IPL_ATTRIBS_CURSE,
	IPL_GETHIT_CURSE,
	IPL_GETHIT,
	IPL_LIFE,
	IPL_LIFE_CURSE,
	IPL_MANA,
	IPL_MANA_CURSE,
	IPL_DUR,
	IPL_DUR_CURSE,
	IPL_INDESTRUCTIBLE,
	IPL_LIGHT,
	IPL_LIGHT_CURSE,
	IPL_MULT_ARROWS = 41, /* only used in hellfire */
	IPL_FIRE_ARROWS,
	IPL_LIGHT_ARROWS,
	IPL_INVCURS,
	IPL_THORNS,
	IPL_NOMANA,
	IPL_FIREBALL = 50, /* only used in hellfire */
	IPL_ABSHALFTRAP = 52,
	IPL_KNOCKBACK,
	IPL_STEALMANA = 55,
	IPL_STEALLIFE,
	IPL_TARGAC,
	IPL_FASTATTACK,
	IPL_FASTRECOVER,
	IPL_FASTBLOCK,
	IPL_DAMMOD,
	IPL_RNDARROWVEL,
	IPL_SETDAM,
	IPL_SETDUR,
	IPL_NOMINSTR,
	IPL_SPELL,
	IPL_ONEHAND = 68,
	IPL_3XDAMVDEM,
	IPL_ALLRESZERO,
	IPL_DRAINLIFE = 72,
	IPL_RNDSTEALLIFE,
	IPL_SETAC = 75,
	IPL_ADDACLIFE,
	IPL_ADDMANAAC,
	IPL_AC_CURSE = 79,
	IPL_LASTDIABLO = IPL_AC_CURSE,
	IPL_FIRERES_CURSE,
	IPL_LIGHTRES_CURSE,
	IPL_MAGICRES_CURSE,
	IPL_DEVASTATION = 84,
	IPL_DECAY,
	IPL_PERIL,
	IPL_JESTERS,
	IPL_CRYSTALLINE,
	IPL_DOPPELGANGER,
	IPL_ACDEMON,
	IPL_ACUNDEAD,
	IPL_MANATOLIFE,
	IPL_LIFETOMANA,
	IPL_INVALID = -1,
};

enum goodorevil : uint8_t {
	GOE_ANY,
	GOE_EVIL,
	GOE_GOOD,
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
	item_effect_type type = IPL_INVALID;
	int param1 = 0;
	int param2 = 0;
};

struct PLStruct {
	const char *PLName;
	ItemPower power;
	int8_t PLMinLvl;
	AffixItemType PLIType; // AffixItemType as bit flags
	enum goodorevil PLGOE;
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
