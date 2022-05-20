#pragma once
#include "include/debuffHandler.h"
#include "include/blockHandler.h"
#include "include/stunHandler.h"
#include "include/balanceHandler.h"
#include "include/lib/TrueHUDAPI.h"
#include "include/settings.h"
/*Combat tweaks to make Skyrim's melee combat feel like AC:Valhalla.*/
class ValhallaCombat
{
public:

	TRUEHUD_API::IVTrueHUD3* ersh = nullptr;
	
	static ValhallaCombat* GetSingleton()
	{
		static ValhallaCombat singleton;
		return  std::addressof(singleton);
	}


	enum HANDLER {
		debuffHandler = 0,
		blockHandler,
		stunHandler,
		balanceHandler,
	};

	/*Start updating a handler per tick.
	@param handlerToActivate: the handler which will start updating per tick.*/
	void activateUpdate(HANDLER a_ValhallaHandler) {
		switch (a_ValhallaHandler)
		{
		case ValhallaCombat::debuffHandler: update_DebuffHandler = true;
			break;
		case ValhallaCombat::blockHandler: update_BlockHandler = true;
			break;
		case ValhallaCombat::stunHandler: update_StunHandler = true;
			break;
		case ValhallaCombat::balanceHandler: update_balanceHandler = true;
		}
	}
	/*Stop updating a handler per tick.
	@param handlerToDeactivate: the handler which will stop updating per tick.*/
	void deactivateUpdate(HANDLER a_ValallaHandler) {
		switch (a_ValallaHandler) {
		case ValhallaCombat::debuffHandler: update_DebuffHandler = false;
			break;
		case ValhallaCombat::blockHandler: update_BlockHandler = false;
			break;
		case ValhallaCombat::stunHandler: update_StunHandler = false;
			break;
		case ValhallaCombat::balanceHandler: update_balanceHandler = false;
		}
	}

	/*Runs every frame*/
	void update() {
		if (RE::UI::GetSingleton()->GameIsPaused()) {
			return;
		}
		if (update_DebuffHandler) {
			debuffHandler::GetSingleton()->update();
		}
		if (update_BlockHandler) {
			blockHandler::GetSingleton()->update();
		}
		if (update_StunHandler) {
			stunHandler::GetSingleton()->update();
		}
		if (update_balanceHandler) {
			balanceHandler::GetSingleton()->update();
		}
	}

	static void queueGarbageCollection() {
		INFO("Initializing garbage collection...");
		if (settings::bStunToggle) {
			stunHandler::GetSingleton()->queueGarbageCollection();
		}
		if (settings::bBalanceToggle) {
			balanceHandler::GetSingleton()->queueGarbageCollection();
		}
		INFO("...done");
	}


	void launchCleanUpThread() {
		INFO("Launch clean up thread...");
		auto cleanUpThreadFunc = []() {
			while (true) {
				std::this_thread::sleep_for(std::chrono::minutes(30));
				queueGarbageCollection();
			}
		};
		std::jthread cleanUpThread(cleanUpThreadFunc);
		cleanUpThread.detach();
		INFO("..done");
	}

	/*Request special bar control from truehud API. 
	If successful, set the truehud specialmeter global value to true.*/
	void requestTrueHudSpecialBarControl() {
		INFO("Request trueHUD API special bar control...");
		if (ersh) {
			if (ersh->RequestSpecialResourceBarsControl(SKSE::GetPluginHandle()) == TRUEHUD_API::APIResult::OK) {
				ersh->RegisterSpecialResourceFunctions(SKSE::GetPluginHandle(), stunHandler::getCurrentStun_static, stunHandler::getMaxStun_static , true, false);
				settings::TrueHudAPI_HasSpecialBarControl = true;
				settings::updateGlobals();
				INFO("...Success");
			}
			else {
				//INFO("...Failure");
			}
		}
	}

	void releaseTrueHudSpecialBarControl() {
		INFO("Release trueHUD API special bar control...");
		if (ValhallaCombat::GetSingleton()->ersh
			->ReleaseSpecialResourceBarControl(SKSE::GetPluginHandle()) == TRUEHUD_API::APIResult::OK) {
			settings::TrueHudAPI_HasSpecialBarControl = false;
			settings::updateGlobals();
			INFO("...Success");
		}
		else {
			//INFO("...Failure");
		}
	}

private:
	bool update_DebuffHandler;
	bool update_BlockHandler;
	bool update_StunHandler;
	bool update_balanceHandler;
};