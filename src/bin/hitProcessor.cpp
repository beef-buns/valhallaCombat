#include "include/hitProcessor.h"
#include "include/attackHandler.h"
#include "include/stunHandler.h"
#include "include/balanceHandler.h"
#include "include/executionHandler.h"
#include "include/settings.h"
#include "include/reactionHandler.h"
#include "include/AI.h"
#include "include/Utils.h"
void hitProcessor::processHit(RE::Actor* a_aggressor, RE::Actor* a_victim, RE::HitData& a_hitData) {
	//offset damage from hitdata, based on player difficulty setting.
	float realDamage = a_hitData.totalDamage;
	Utils::offsetRealDamage(realDamage, a_aggressor, a_victim);
	int hitFlag = (int)a_hitData.flags;
	if (
		(hitFlag & (int)HITFLAG::kBlocked)) {
		if (blockHandler::GetSingleton()->processPhysicalBlock(a_victim, a_aggressor, hitFlag, a_hitData, realDamage)) {
			if (!a_victim->IsPlayerRef()) {
				AI::GetSingleton()->action_PerformEldenCounter(a_victim);
			}
			return; //if the hit is perfect blocked, no hit registration
		}
		// if not perfect blocked, regenerate stamina only if set so.
		if (settings::bBlockedHitRegenStamina && !(hitFlag & (int)HITFLAG::kBash)) {
			if (settings::bAttackStaminaToggle) {
				attackHandler::GetSingleton()->registerHit(a_aggressor);
				
			}
		}
		return;
	}

	//from this point on the hit is not blocked/

	//bash hit
	if (hitFlag & (int)HITFLAG::kBash) {
		if (hitFlag & (int)HITFLAG::kPowerAttack) {
			stunHandler::GetSingleton()->processStunDamage(stunHandler::STUNSOURCE::powerBash, nullptr, a_aggressor, a_victim, 0);
		}
		else {
			stunHandler::GetSingleton()->processStunDamage(stunHandler::STUNSOURCE::bash, nullptr, a_aggressor, a_victim, 0);
		}
		return;
	}

	//from this point on the hit can only be unblocked melee hit.
	if (settings::bAttackStaminaToggle) {
		attackHandler::GetSingleton()->registerHit(a_aggressor);
	}


	if (stunHandler::GetSingleton()->getIsStunned(a_victim) && a_hitData.weapon->IsMelee()) {
		if (a_aggressor->IsPlayerRef()) {
			if (settings::bAutoExecution) {//player only auto-execute if auto execution is on
				executionHandler::GetSingleton()->attemptExecute(a_aggressor, a_victim);
			}
		}
		else { //NPC auto-execute always.
			executionHandler::GetSingleton()->attemptExecute(a_aggressor, a_victim);
		}
	}
	//DEBUG("test execution");
	//executionHandler::GetSingleton()->playExecutionIdle(aggressor, victim, data::testIdle);
	if (hitFlag & (int)HITFLAG::kPowerAttack) {
		bool b;
		//TODO:redo stun damage for elden counter
		if (a_aggressor->GetGraphVariableBool(data::GraphBool_IsGuardCountering, b) && b) {
			realDamage *= 1.5;
		}
		stunHandler::GetSingleton()->processStunDamage(stunHandler::STUNSOURCE::powerAttack, a_hitData.weapon, a_aggressor, a_victim, realDamage);
		balanceHandler::GetSingleton()->processBalanceDamage(balanceHandler::DMGSOURCE::powerAttack, a_hitData.weapon, a_aggressor, a_victim, realDamage);
	}
	else {
		stunHandler::GetSingleton()->processStunDamage(stunHandler::STUNSOURCE::lightAttack, a_hitData.weapon, a_aggressor, a_victim, realDamage);
		balanceHandler::GetSingleton()->processBalanceDamage(balanceHandler::DMGSOURCE::lightAttack, a_hitData.weapon, a_aggressor, a_victim, realDamage);
	}

}
