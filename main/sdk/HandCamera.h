#pragma once
#include "sdk.h"

namespace SDK
{
	struct HandCamera;

	struct SystemByteArray
	{
		void* klass;
		void* monitor;
		void* bounds;
		uint32_t max_length;
		uint8_t vector[1];
	};

	DEC_MET(HandCamera_UseNetworked, void(*)(HandCamera* handCamera, bool isOn, PhotonMessageInfo* info, MethodInfo* methodInfo), "Assembly-CSharp", "", "HandCamera", "UseNetworked", 2);
	DEC_MET(HandCamera_Start, void(*)(HandCamera* handCamera, MethodInfo* methodInfo), "Assembly-CSharp", "", "HandCamera", "Start", 0);
	DEC_MET(HandCamera_AddPhotoToJournal, nullptr_t(*)(HandCamera* handCamera, void* a1, int32_t a2, int32_t a3, int32_t a4, void* photonMessageInfo, MethodInfo* methodInfo), "Assembly-CSharp", "", "HandCamera", "AddPhotoToJournal", 5);
	DECLARE_PATTERN_POINTER(HandCamera_MoveNext, bool(*)(HandCamera* handCamera, MethodInfo* methodInfo), L"GameAssembly.dll", "il2cpp", "48 89 5C 24 ? 56 57 41 54 41 56 41 57 48 83 EC ? 48 8B F9 80 3D ? ? ? ? 00 0F 85");
	DEC_PATTERN_INSTR(HandCamera_AnimatePhotoCall, L"GameAssembly.dll", "il2cpp", "E8 ? ? ? ? 48 8B 0D ? ? ? ? 39 B1 ? ? ? ? 75 ? E8 ? ? ? ? 33 C9 E8 ? ? ? ? 48 85 C0 0F 84 ? ? ? ? 8B 40 ? 89 84 24 ? ? ? ? 48 8B 0D ? ? ? ? 83 B9 ? ? ? ? 00 75 ? E8 ? ? ? ? 33 C9 E8 ? ? ? ? 48 8B D8 33 D2 48 8D 8C 24 ? ? ? ? E8 ? ? ? ? 48 89 74 24 ? 4C 8B 0D ? ? ? ? 4C 8B C0 48 8B 15 ? ? ? ? 48 8B CB E8 ? ? ? ? 48 8B D8 33 D2"); // 6 in HowFindMethods
}