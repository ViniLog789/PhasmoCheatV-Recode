#include "../Includes.h"
#include "../features/features_includes.h"

using namespace PhasmoCheatV;
using namespace PhasmoCheatV::Features::Ghost;

void Hooks::hkGhostInfo_SyncValuesNetworked(SDK::GhostInfo* instance, int ghostType, int ghostAge, bool isMale, int ghostFirstNameID, int ghostLastNameID, bool isShy, int deathLength, int favouriteRoomID, bool isWhisper, void* photon, SDK::MethodInfo* methodInfo)
{
    LOG_CALL("Called GhostInfo_SyncValuesNetworked");

    CALL_METHOD_IF_ACTIVE_ARGS(Ghost, GhostDesigner, ApplyGhostSettings, ghostType, ghostAge, isShy);

    SDK::GhostInfo_SyncValuesNetworked(instance, ghostType, ghostAge, isMale, ghostFirstNameID, ghostLastNameID, isShy, deathLength, favouriteRoomID, isWhisper, photon, methodInfo);
}