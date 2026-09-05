#pragma once
#include "sdk.h"

namespace SDK
{
	struct CrucifixFields
	{
		char trash[0x3C0];
		void* photoEvidence;
		void* videoEvidence;
		void* burningClip;
		void* particles;
		Color glowColor;
		void* allModels;
		void* allRenderers;
		int32_t useCount;
		int32_t range;
		int32_t maxUses;
		bool hasBeenUsed;
		char pad_000[0x3];
		float dropDelay;
		char pad_001[0x4];
		void* burnSequence;
		bool stoppedCursedHunt;
		char pad_002[0x7];
	};

	struct Crucifix
	{
		void* Clazz;
		void* Monitor;
		CrucifixFields Fields;
	};

	DEC_MET(Crucifix_GhostUse, void(*)(Crucifix* crucifix, MethodInfo* methodInfo), "Assembly-CSharp", "", "Crucifix", "GhostUse", 0);
}