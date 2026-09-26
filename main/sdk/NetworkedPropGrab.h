#pragma once
#include "sdk.h"

namespace SDK
{
	struct NetworkedPropGrab;

	DEC_MET(NetworkedPropGrab_EnableOrDisableObjectRPC, void(*)(NetworkedPropGrab* networkedPropGrab, int32_t photon_objId, bool enabled, PhotonMessageInfo* photonMessageInfo, MethodInfo* methodInfo),
		"Assembly-CSharp", "", "NetworkedPropGrab", "EnableOrDisableObjectRPC", 3);
}