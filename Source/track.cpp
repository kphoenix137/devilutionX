/**
 * @file track.cpp
 *
 * Implementation of functionality tracking what the mouse cursor is pointing at.
 */
#include "track.h"

#include <SDL.h>

#include "cursor.h"
#include "engine/point.hpp"
#include "player.h"
#include "stores.h"

namespace devilution {

namespace {

void RepeatWalk(Player &player)
{
	if (!InDungeonBounds(cursPosition))
		return;

	if (player._pmode != PM_STAND && !(player.IsWalking() && player.AnimInfo.GetFrameToUseForRendering() > 6))
		return;

	const Point target = player.GetTargetPosition();
	if (cursPosition == target)
		return;

	NetSendCmdLoc(MyPlayerId, true, CMD_WALKXY, cursPosition);
}

} // namespace

void RepeatMouseAction()
{
	if (pcurs != CURSOR_HAND)
		return;

	if (sgbMouseDown == CLICK_NONE)
		return;

	if (stextflag != STORE_NONE)
		return;

	if (LastMouseButtonAction == MouseActionType::None)
		return;

	auto &myPlayer = Players[MyPlayerId];
	if (myPlayer.destAction != ACTION_NONE)
		return;
	if (myPlayer._pInvincible)
		return;
	if (!myPlayer.CanChangeAction())
		return;

	bool rangedAttack = myPlayer.UsesRangedWeapon();
	switch (LastMouseButtonAction) {
	case MouseActionType::Attack:
		if (InDungeonBounds(cursPosition))
			NetSendCmdLoc(MyPlayerId, true, rangedAttack ? CMD_RATTACKXY : CMD_SATTACKXY, cursPosition);
		break;
	case MouseActionType::AttackMonsterTarget:
		if (pcursmonst != -1)
			NetSendCmdParam1(true, rangedAttack ? CMD_RATTACKID : CMD_ATTACKID, pcursmonst);
		break;
	case MouseActionType::AttackPlayerTarget:
		if (pcursplr != -1 && !gbFriendlyMode)
			NetSendCmdParam1(true, rangedAttack ? CMD_RATTACKPID : CMD_ATTACKPID, pcursplr);
		break;
	case MouseActionType::Spell:
		CheckPlrSpell(true);
		break;
	case MouseActionType::SpellMonsterTarget:
		if (pcursmonst != -1)
			CheckPlrSpell(false);
		break;
	case MouseActionType::SpellPlayerTarget:
		if (pcursplr != -1 && !gbFriendlyMode)
			CheckPlrSpell(false);
		break;
	case MouseActionType::OperateObject:
		if (pcursobj != -1) {
			auto &object = Objects[pcursobj];
			if (object.IsDoor())
				break;
			NetSendCmdLocParam1(true, CMD_OPOBJXY, object.position, pcursobj);
		}
		break;
	case MouseActionType::Walk:
		RepeatWalk(myPlayer);
		break;
	case MouseActionType::None:
		break;
	}
}

bool track_isscrolling()
{
	return LastMouseButtonAction == MouseActionType::Walk;
}

} // namespace devilution
