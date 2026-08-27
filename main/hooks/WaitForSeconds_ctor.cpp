#include "../Includes.h"
#include "../features/features_includes.h"

using namespace PhasmoCheatV;

void Hooks::hkWaitForSeconds_ctor(SDK::WaitForSeconds* waitForSeconds, float seconds, SDK::MethodInfo* methodInfo)
{
	LOG_CALL_UPDATE("Called WaitForSeconds_ctor");

	if (CheatWork)
	{
		if (seconds == 20 && Utils::GetGhostTypeInt() == 9 || 
			seconds == 25 && Utils::GetGhostTypeInt() != 9)
		{
			CALL_METHOD_IF_ACTIVE_ARGS(Visuals, GhostPanel, GhostTimerHook, seconds, 2);
		}
		else if (Utils::GetGhostAI() && 
			Utils::GetGhostAI()->Fields.delayedBySmudgeStick && 
			seconds == 60 && Utils::GetGhostTypeInt() == 9   || 
			seconds == 180 && Utils::GetGhostTypeInt() == 0  || 
			seconds == 90)
		{
			CALL_METHOD_IF_ACTIVE_ARGS(Visuals, GhostPanel, GhostTimerHook, seconds, 1);
		}
	}

	SDK::WaitForSeconds_ctor(waitForSeconds, seconds, methodInfo);
}