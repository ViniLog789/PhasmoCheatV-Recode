#pragma once
#include "sdk.h"

namespace SDK
{
    DEC_MET(Time_Get_DeltaTime, float(*)(MethodInfo* methodInfo), "UnityEngine.CoreModule", "UnityEngine", "Time", "get_deltaTime", 0);
    DEC_MET(Time_Get_Time, float(*)(MethodInfo* methodInfo), "UnityEngine.CoreModule", "UnityEngine", "Time", "get_time", 0);
    DEC_MET(Time_Get_TimeScale, float(*)(MethodInfo* methodInfo), "UnityEngine.CoreModule", "UnityEngine", "Time", "get_timeScale", 0);
    DEC_MET(Time_Get_UnscaledDeltaTime, float(*)(MethodInfo* methodInfo), "UnityEngine.CoreModule", "UnityEngine", "Time", "get_unscaledDeltaTime", 0);
    DEC_MET(Time_Set_TimeScale, void(*)(float value, MethodInfo* methodInfo), "UnityEngine.CoreModule", "UnityEngine", "Time", "set_timeScale", 1);
}