/**
 * @file missiles.h
 *
 * Interface of missile functionality.
 */
#pragma once

#include <cstdint>
#include <list>
#include <optional>

#include "engine.h"
#include "engine/point.hpp"
#include "engine/world_tile.hpp"
#include "misdat.h"
#include "monster.h"
#include "player.h"
#include "spelldat.h"

namespace devilution {

constexpr WorldTilePosition GolemHoldingCell = Point { 1, 0 };

struct MissilePosition {
	/** Sprite's pixel offset from tile. */
	Displacement offset;
	/** Pixel velocity while moving */
	Displacement velocity;
	/** Pixels traveled as a numerator of 65,536. */
	Displacement traveled;

	WorldTilePosition tile;
	/** Start position */
	WorldTilePosition start;

	/**
	 * @brief Specifies the location (tile) while rendering
	 */
	WorldTilePosition tileForRendering;
	/**
	 * @brief Specifies the location (offset) while rendering
	 */
	Displacement offsetForRendering;

	/**
	 * @brief Stops the missile (set velocity to zero and set offset to last renderer location; shouldn't matter cause the missile don't move anymore)
	 */
	void StopMissile()
	{
		velocity = {};
		if (tileForRendering == tile)
			offset = offsetForRendering;
	}
};

/**
 * Represent a more fine-grained direction than the 8 value Direction enum.
 *
 * This is used when rendering projectiles like arrows which have additional sprites for "half-winds" on a 16-point compass.
 * The sprite sheets are typically 0-indexed and use the following layout (relative to the screen projection)
 *
 *      W  WSW   SW  SSW  S
 *               ^
 *     WNW       |       SSE
 *               |
 *     NW -------+------> SE
 *               |
 *     NNW       |       ESE
 *               |
 *      N  NNE   NE  ENE  E
 */
enum class Direction16 : uint8_t {
	South,
	South_SouthWest,
	SouthWest,
	West_SouthWest,
	West,
	West_NorthWest,
	NorthWest,
	North_NorthWest,
	North,
	North_NorthEast,
	NorthEast,
	East_NorthEast,
	East,
	East_SouthEast,
	SouthEast,
	South_SouthEast,
};

enum class MissileSource : uint8_t {
	Player,
	Monster,
	Trap,
};

struct Missile {
	/** Type of projectile */
	MissileID _mitype;
	MissilePosition position;
	int _mimfnum; // The direction of the missile (direction enum)
	int _mispllvl;
	bool _miDelFlag; // Indicate whether the missile should be deleted
	MissileGraphicID _miAnimType;
	MissileGraphicsFlags _miAnimFlags;
	OptionalClxSpriteList _miAnimData;
	int _miAnimDelay; // Tick length of each frame in the current animation
	int _miAnimLen;   // Number of frames in current animation

	// TODO: This field is no longer used and is always equal to
	// (*_miAnimData)[0].width()
	uint16_t _miAnimWidth;

	int16_t _miAnimWidth2;
	int _miAnimCnt; // Increases by one each game tick, counting how close we are to _pAnimDelay
	int _miAnimAdd;
	int _miAnimFrame; // Current frame of animation + 1.
	bool _miDrawFlag;
	bool _miLightFlag;
	bool _miPreFlag;
	uint32_t _miUniqTrans;
	int _mirange; // Time to live for the missile in game ticks, oncs 0 the missile will be marked for deletion via _miDelFlag
	int _misource;
	mienemy_type _micaster;
	int _midam;
	bool _miHitFlag;
	int _midist; // Used for arrows to measure distance travelled (increases by 1 each game tick). Higher value is a penalty for accuracy calculation when hitting enemy
	int _mlid;
	int _mirnd;
	int var1;
	int var2;
	int var3;
	int var4;
	int var5;
	int var6;
	int var7;
	bool limitReached;
	/**
	 * @brief For moving missiles lastCollisionTargetHash contains the last entity (player or monster) that was checked in CheckMissileCol (needed to avoid multiple hits for a entity at the same tile).
	 */
	int16_t lastCollisionTargetHash;

	/** @brief Was the missile generated by a trap? */
	[[nodiscard]] bool IsTrap() const
	{
		return _misource == -1;
	}

	[[nodiscard]] Player *sourcePlayer()
	{
		if (IsNoneOf(_micaster, TARGET_BOTH, TARGET_MONSTERS) || _misource == -1)
			return nullptr;
		return &Players[_misource];
	}

	[[nodiscard]] Monster *sourceMonster()
	{
		if (_micaster != TARGET_PLAYERS || _misource == -1)
			return nullptr;
		return &Monsters[_misource];
	}

	[[nodiscard]] bool isSameSource(Missile &missile)
	{
		return sourceType() == missile.sourceType() && _misource == missile._misource;
	}

	MissileSource sourceType()
	{
		if (_misource == -1)
			return MissileSource::Trap;
		if (_micaster == TARGET_PLAYERS)
			return MissileSource::Monster;
		return MissileSource::Player;
	}
};

extern std::list<Missile> Missiles;
extern bool MissilePreFlag;

struct DamageRange {
	int min;
	int max;
};
DamageRange GetDamageAmt(SpellID spell, int spellLevel);

/**
 * @brief Returns the direction a vector from p1(x1, y1) to p2(x2, y2) is pointing to.
 *
 * @code{.unparsed}
 *      W  sW  SW   Sw  S
 *              ^
 *     nW       |       Se
 *              |
 *     NW ------+-----> SE
 *              |
 *     Nw       |       sE
 *              |
 *      N  Ne  NE   nE  E
 * @endcode
 *
 * @param p1 The point from which the vector starts.
 * @param p2 The point from which the vector ends.
 * @return the direction of the p1->p2 vector
 */
Direction16 GetDirection16(Point p1, Point p2);
bool MonsterTrapHit(int monsterId, int mindam, int maxdam, int dist, MissileID t, DamageType damageType, bool shift);
bool PlayerMHit(Player &player, Monster *monster, int dist, int mind, int maxd, MissileID mtype, DamageType damageType, bool shift, DeathReason deathReason, bool *blocked);

/**
 * @brief Could the missile collide with solid objects? (like walls or closed doors)
 */
bool IsMissileBlockedByTile(Point position);

/**
 * @brief Sets the missile sprite to the given sheet frame
 * @param missile this object
 * @param dir Sprite frame, typically representing a direction but there are some exceptions (arrows being 1 indexed, directionless spells)
 */
void SetMissDir(Missile &missile, int dir);

/**
 * @brief Sets the sprite for this missile so it matches the given Direction
 * @param missile this object
 * @param dir Desired facing
 */
inline void SetMissDir(Missile &missile, Direction dir)
{
	SetMissDir(missile, static_cast<int>(dir));
}

/**
 * @brief Sets the sprite for this missile so it matches the given Direction16
 * @param missile this object
 * @param dir Desired facing at a 22.8125 degree resolution
 */
inline void SetMissDir(Missile &missile, Direction16 dir)
{
	SetMissDir(missile, static_cast<int>(dir));
}

void InitMissiles();

struct AddMissileParameter {
	WorldTilePosition dst;
	Direction midir;
	Missile *pParent;
	bool spellFizzled;
};

void AddOpenNest(Missile &missile, AddMissileParameter &parameter);
void AddRuneOfFire(Missile &missile, AddMissileParameter &parameter);
void AddRuneOfLight(Missile &missile, AddMissileParameter &parameter);
void AddRuneOfNova(Missile &missile, AddMissileParameter &parameter);
void AddRuneOfImmolation(Missile &missile, AddMissileParameter &parameter);
void AddRuneOfStone(Missile &missile, AddMissileParameter &parameter);
void AddReflect(Missile &missile, AddMissileParameter &parameter);
void AddBerserk(Missile &missile, AddMissileParameter &parameter);

/**
 * var1: Direction to place the spawn
 */
void AddHorkSpawn(Missile &missile, AddMissileParameter &parameter);
void AddJester(Missile &missile, AddMissileParameter &parameter);
void AddStealPotions(Missile &missile, AddMissileParameter &parameter);
void AddStealMana(Missile &missile, AddMissileParameter &parameter);
void AddSpectralArrow(Missile &missile, AddMissileParameter &parameter);
void AddWarp(Missile &missile, AddMissileParameter &parameter);
void AddLightningWall(Missile &missile, AddMissileParameter &parameter);
void AddBigExplosion(Missile &missile, AddMissileParameter &parameter);
void AddImmolation(Missile &missile, AddMissileParameter &parameter);
void AddLightningBow(Missile &missile, AddMissileParameter &parameter);
void AddMana(Missile &missile, AddMissileParameter &parameter);
void AddMagi(Missile &missile, AddMissileParameter &parameter);
void AddRingOfFire(Missile &missile, AddMissileParameter &parameter);
void AddSearch(Missile &missile, AddMissileParameter &parameter);
void AddChargedBoltBow(Missile &missile, AddMissileParameter &parameter);
void AddElementalArrow(Missile &missile, AddMissileParameter &parameter);
void AddArrow(Missile &missile, AddMissileParameter &parameter);
void AddPhasing(Missile &missile, AddMissileParameter &parameter);
void AddFirebolt(Missile &missile, AddMissileParameter &parameter);
void AddMagmaBall(Missile &missile, AddMissileParameter &parameter);
void AddTeleport(Missile &missile, AddMissileParameter &parameter);
void AddNovaBall(Missile &missile, AddMissileParameter &parameter);
void AddFireWall(Missile &missile, AddMissileParameter &parameter);

/**
 * var1: X coordinate of the missile-light
 * var2: Y coordinate of the missile-light
 * var4: X coordinate of the missile-light
 * var5: Y coordinate of the missile-light
 */
void AddFireball(Missile &missile, AddMissileParameter &parameter);

/**
 * var1: X coordinate of the missile
 * var2: Y coordinate of the missile
 */
void AddLightningControl(Missile &missile, AddMissileParameter &parameter);
void AddLightning(Missile &missile, AddMissileParameter &parameter);
void AddMissileExplosion(Missile &missile, AddMissileParameter &parameter);
void AddWeaponExplosion(Missile &missile, AddMissileParameter &parameter);

/**
 * var1: Animation
 */
void AddTownPortal(Missile &missile, AddMissileParameter &parameter);
void AddFlashBottom(Missile &missile, AddMissileParameter &parameter);
void AddFlashTop(Missile &missile, AddMissileParameter &parameter);
void AddManaShield(Missile &missile, AddMissileParameter &parameter);
void AddFlameWave(Missile &missile, AddMissileParameter &parameter);

/**
 * var1: Animation
 * var3: Light strength
 */
void AddGuardian(Missile &missile, AddMissileParameter &parameter);

/**
 * var1: X coordinate of the destination
 * var2: Y coordinate of the destination
 */
void AddChainLightning(Missile &missile, AddMissileParameter &parameter);
void AddRhino(Missile &missile, AddMissileParameter &parameter);

/**
 * var1: X coordinate of the missile-light
 * var2: Y coordinate of the missile-light
 */
void AddGenericMagicMissile(Missile &missile, AddMissileParameter &parameter);

/**
 * var1: X coordinate of the missile-light
 * var2: Y coordinate of the missile-light
 */
void AddAcid(Missile &missile, AddMissileParameter &parameter);
void AddAcidPuddle(Missile &missile, AddMissileParameter &parameter);

/**
 * var1: mmode of the monster
 * var2: mnum of the monster
 */
void AddStoneCurse(Missile &missile, AddMissileParameter &parameter);
void AddGolem(Missile &missile, AddMissileParameter &parameter);
void AddApocalypseBoom(Missile &missile, AddMissileParameter &parameter);
void AddHealing(Missile &missile, AddMissileParameter &parameter);
void AddHealOther(Missile &missile, AddMissileParameter &parameter);

/**
 * var1: X coordinate of the missile-light
 * var2: Y coordinate of the missile-light
 * var4: X coordinate of the destination
 * var5: Y coordinate of the destination
 */
void AddElemental(Missile &missile, AddMissileParameter &parameter);
void AddIdentify(Missile &missile, AddMissileParameter &parameter);

/**
 * var1: X coordinate of the first wave
 * var2: Y coordinate of the first wave
 * var3: Direction of the first wave
 * var4: Direction of the second wave
 * var5: X coordinate of the second wave
 * var6: Y coordinate of the second wave
 */
void AddFireWallControl(Missile &missile, AddMissileParameter &parameter);
void AddInfravision(Missile &missile, AddMissileParameter &parameter);

/**
 * var1: X coordinate of the destination
 * var2: Y coordinate of the destination
 */
void AddFlameWaveControl(Missile &missile, AddMissileParameter &parameter);
void AddNova(Missile &missile, AddMissileParameter &parameter);
void AddRage(Missile &missile, AddMissileParameter &parameter);
void AddItemRepair(Missile &missile, AddMissileParameter &parameter);
void AddStaffRecharge(Missile &missile, AddMissileParameter &parameter);
void AddTrapDisarm(Missile &missile, AddMissileParameter &parameter);
void AddApocalypse(Missile &missile, AddMissileParameter &parameter);
void AddInferno(Missile &missile, AddMissileParameter &parameter);
void AddInfernoControl(Missile &missile, AddMissileParameter &parameter);

/**
 * var1: Light strength
 * var2: Base direction
 */
void AddChargedBolt(Missile &missile, AddMissileParameter &parameter);
void AddHolyBolt(Missile &missile, AddMissileParameter &parameter);
void AddResurrect(Missile &missile, AddMissileParameter &parameter);
void AddResurrectBeam(Missile &missile, AddMissileParameter &parameter);
void AddTelekinesis(Missile &missile, AddMissileParameter &parameter);
void AddBoneSpirit(Missile &missile, AddMissileParameter &parameter);
void AddRedPortal(Missile &missile, AddMissileParameter &parameter);
void AddDiabloApocalypse(Missile &missile, AddMissileParameter &parameter);
Missile *AddMissile(WorldTilePosition src, WorldTilePosition dst, Direction midir, MissileID mitype,
    mienemy_type micaster, int id, int midam, int spllvl,
    Missile *parent = nullptr, std::optional<SfxID> lSFX = std::nullopt);
inline Missile *AddMissile(WorldTilePosition src, WorldTilePosition dst, Direction midir, MissileID mitype,
    mienemy_type micaster, const Player &player, int midam, int spllvl,
    Missile *parent = nullptr, std::optional<SfxID> lSFX = std::nullopt)
{
	return AddMissile(src, dst, midir, mitype, micaster, player.getId(), midam, spllvl, parent, lSFX);
}
inline Missile *AddMissile(WorldTilePosition src, WorldTilePosition dst, Direction midir, MissileID mitype,
    mienemy_type micaster, const Monster &monster, int midam, int spllvl,
    Missile *parent = nullptr, std::optional<SfxID> lSFX = std::nullopt)
{
	return AddMissile(src, dst, midir, mitype, micaster, static_cast<int>(monster.getId()), midam, spllvl, parent, lSFX);
}
void ProcessElementalArrow(Missile &missile);
void ProcessArrow(Missile &missile);
void ProcessGenericProjectile(Missile &missile);
void ProcessNovaBall(Missile &missilei);
void ProcessAcidPuddle(Missile &missile);
void ProcessFireWall(Missile &missile);
void ProcessFireball(Missile &missile);
void ProcessHorkSpawn(Missile &missile);
void ProcessRune(Missile &missile);
void ProcessLightningWall(Missile &missile);
void ProcessBigExplosion(Missile &missile);
void ProcessLightningBow(Missile &missile);
void ProcessRingOfFire(Missile &missile);
void ProcessSearch(Missile &missile);
void ProcessLightningWallControl(Missile &missile);
void ProcessImmolation(Missile &missile);
void ProcessSpectralArrow(Missile &missile);
void ProcessLightningControl(Missile &missile);
void ProcessLightning(Missile &missile);
void ProcessTownPortal(Missile &missile);
void ProcessFlashBottom(Missile &missile);
void ProcessFlashTop(Missile &missile);
void ProcessFlameWave(Missile &missile);
void ProcessGuardian(Missile &missile);
void ProcessChainLightning(Missile &missile);
void ProcessWeaponExplosion(Missile &missile);
void ProcessMissileExplosion(Missile &missile);
void ProcessAcidSplate(Missile &missile);
void ProcessTeleport(Missile &missile);
void ProcessStoneCurse(Missile &missile);
void ProcessApocalypseBoom(Missile &missile);
void ProcessRhino(Missile &missile);
void ProcessFireWallControl(Missile &missile);
void ProcessInfravision(Missile &missile);
void ProcessApocalypse(Missile &missile);
void ProcessFlameWaveControl(Missile &missile);
void ProcessNova(Missile &missile);
void ProcessRage(Missile &missile);
void ProcessInferno(Missile &missile);
void ProcessInfernoControl(Missile &missile);
void ProcessChargedBolt(Missile &missile);
void ProcessHolyBolt(Missile &missile);
void ProcessElemental(Missile &missile);
void ProcessBoneSpirit(Missile &missile);
void ProcessResurrectBeam(Missile &missile);
void ProcessRedPortal(Missile &missile);
void ProcessMissiles();
void SetUpMissileanimationData();
void RedoMissileFlags();

#ifdef BUILD_TESTING
void TestRotateBlockedMissile(Missile &missile);
#endif

} // namespace devilution
