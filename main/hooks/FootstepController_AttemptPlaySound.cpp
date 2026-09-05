#include "../Includes.h"
#include "../features/features_includes.h"

using namespace PhasmoCheatV;

void Hooks::hkFootstepController_AttemptPlaySound(SDK::FootstepController* footstepController, SDK::MethodInfo* methodInfo)
{
	LOG_CALL_UPDATE("Called FootstepController_AttemptPlaySound");

	if (GET_ACTIVE(Misc, AudioModifier) && 
		CALL_METHOD(Misc, AudioModifier, get_bMuteFootsteps)) //! CALL_METHOD_IF_ACTIVE dont return value
	{
		return;
	}

	SDK::FootstepController_AttemptPlaySound(footstepController, methodInfo);
}