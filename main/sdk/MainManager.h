#pragma once
#include "sdk.h"

namespace SDK
{
	struct MainManagerFields
	{
		MonoBehaviourPunCallbacksFields monoBehaviourPunCallbacksFields;
		void* sceneCamera;
		void* mainUICanvasGroup;
		void* spawns;
		void* psvr2EulaSpawn;
		bool ranOnce;
		char pad_000[0x7];
		ServerManager* serverManager;
		LevelSelectionManager* levelSelection;
		void* difficultyUIManager;
		void* ghostOS;
		void* errorManager;
		// im too lazy to write further
	};

	struct MainManager
	{
		void* Clazz;
		void* Monitor;
		MainManagerFields Fields;
	};

	struct MainManager_StaticFields
	{
		MainManager* instance;
	};

	DEC_SFIELD(MainManager_staticFields, MainManager_StaticFields, "Assembly-CSharp", "", "MainManager");
}