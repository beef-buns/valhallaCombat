#pragma once

#include <boost/container/set.hpp>
#include <boost/unordered_map.hpp>
#include <boost/foreach.hpp>
#define foreach_         BOOST_FOREACH
/*Class handling actor debuffs and corresponding UI alerts.*/
class debuffHandler
{

public:
	/*Mapping of a set of actors currently in stamina debuff to their stamina blinking timer.*/
	boost::unordered_map<RE::Actor*, float> actorsInDebuff;

	static debuffHandler* GetSingleton()
	{
		static debuffHandler singleton;
		return  std::addressof(singleton);
	}

	/*called along with the main update hook.*/
	void update();

	inline void greyOutStaminaMeter(RE::Actor* actor);
	inline void revertStaminaMeter(RE::Actor* actor);

	void initStaminaDebuff(RE::Actor* actor);
	void stopStaminaDebuff(RE::Actor* actor);


private:

	void addDebuffPerk(RE::Actor* a_actor);

	void removeDebuffPerk(RE::Actor* a_actor);

	RE::BGSPerk* debuffPerk;

};


namespace Utils
{

	inline void safeApplySpell(RE::SpellItem* a_spell, RE::Actor* a_actor) {
		if (a_actor && a_spell) {
			a_actor->AddSpell(a_spell);
			DEBUG("spell {} applied to {}.", a_spell->GetName(), a_actor->GetName());
		}
	}

	inline void safeRemoveSpell(RE::SpellItem* a_spell, RE::Actor* a_actor) {
		if (a_actor && a_spell) {
			a_actor->RemoveSpell(a_spell);
			DEBUG("spell {} removed from {}.", a_spell->GetName(), a_actor->GetName());
		}
	}

	inline void safeApplyPerk(RE::BGSPerk* a_perk, RE::Actor* a_actor) {
		if (a_actor && a_perk && !a_actor->HasPerk(a_perk)) {
			a_actor->AddPerk(a_perk);
			DEBUG("perk {} added to {}", a_perk->GetName(), a_actor->GetName());
		}
	}

	inline void safeRemovePerk(RE::BGSPerk* a_perk, RE::Actor* a_actor) {
		if (a_actor && a_perk && a_actor->HasPerk(a_perk)) {
			a_actor->RemovePerk(a_perk);
			DEBUG("perk {} removed from {}", a_perk->GetName(), a_actor->GetName());
		}
	}
}