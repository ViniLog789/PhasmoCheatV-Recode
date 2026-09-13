#include "../Includes.h"
#include "../features/features_includes.h"
#include "../discordrpc/discordrpc.h"

using namespace PhasmoCheatV;

void Hooks::hkScriptableRenderContext_Submit(SDK::ScriptableRenderContext* context, SDK::MethodInfo* methodInfo)
{
	LOG_CALL_UPDATE("Called ScriptableRenderContext_Submit");

	if (CheatWork)
		Discord::DiscordRPCStatusHandler();

	CALL_METHOD_IF_ACTIVE(Auto, AutoGame, AutoGameHandler);
	CALL_METHOD_IF_ACTIVE(Auto, AutoMap, AutoMapHandler);

	SDK::ScriptableRenderContext_Submit(context, methodInfo);
}