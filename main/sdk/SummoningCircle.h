#pragma once
#include "sdk.h"

namespace SDK
{
	struct CandleArray
	{
		void* Clazz;
		void* Monitor;
		void* Bounds;
		int32_t MaxLength;
		Candle* Vector[1];
	};

	struct SummoningCircleFields
	{
		char pad_000[0x1C0];
		void* ritualParticles;
		void* spawnPoint;
		CandleArray* allCandles;
		void* onSummoningCircleUsed;
		void* ritualSequence;
		float insanityOfPlayerWhoLitCircle;
	};

	struct SummoningCircle
	{
		void* Clazz;
		void* Monitor;
		SummoningCircleFields Fields;
	};

	DECLARE_PATTERN_POINTER(SummoningCircle_BeginRitual, void(*)(SummoningCircle* summoningCircle, MethodInfo* methodInfo), L"GameAssembly.dll", "il2cpp", "40 56 48 83 EC 50 80 3D ? ? ? ? ? 48 8B F1 75 67 48 8D 0D ? ? ? ? E8 ? ? ? ? 48 8D 0D ? ? ? ? E8 ? ? ? ? 48 8D 0D ? ? ? ? E8 ? ? ? ? 48 8D 0D ? ? ? ? E8 ? ? ? ? 48 8D 0D ? ? ? ? E8 ? ? ? ? 48 8D 0D ? ? ? ? E8 ? ? ? ? 48 8D 0D ? ? ? ? E8 ? ? ? ? 48 8D 0D ? ? ? ? E8 ? ? ? ? C6 05 ? ? ? ? ? 48 8B 86"); // 8 in HowFindMethods
	DECLARE_PATTERN_POINTER(SummoningCircle_CandleLit, void(*)(SummoningCircle* summoningCircle, Player* player, Candle* candle, MethodInfo* methodInfo), L"GameAssembly.dll", "il2cpp", "48 89 5C 24 ? 48 89 74 24 ? 57 48 81 EC ? ? ? ? 80 3D ? ? ? ? 00 49 8B F0 48 8B FA 48 8B D9 75 ? 48 8D 0D ? ? ? ? E8 ? ? ? ? 48 8D 0D ? ? ? ? E8 ? ? ? ? 48 8D 0D ? ? ? ? E8 ? ? ? ? 48 8D 0D ? ? ? ? E8 ? ? ? ? 48 8D 0D ? ? ? ? E8 ? ? ? ? 48 8D 0D ? ? ? ? E8 ? ? ? ? 48 8D 0D ? ? ? ? E8 ? ? ? ? C6 05"); // 9 in HowFindMethods
}