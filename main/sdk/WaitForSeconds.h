#pragma once
#include "sdk.h"

namespace SDK
{
	struct WaitForSeconds;

	DEC_MET(WaitForSeconds_ctor, void(*)(WaitForSeconds* waitForSeconds, float seconds, MethodInfo* methodInfo), "UnityEngine.CoreModule", "UnityEngine", "WaitForSeconds", ".ctor", 1);
}