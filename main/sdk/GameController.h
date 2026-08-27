#pragma once
#include "sdk.h"

namespace SDK
{
	struct LevelController;

	struct GameControllerFields
	{
		MonoBehaviourPunCallbacksFields MonoBehaviourPunCallbacksFields;
		void* Field18;
		void* Field20;
		void* Field28;
		void* Field30;
		void* Field38;
		void* Field40;
		void* Field48;
		void* Field50;
		void* Field58;
		void* Field60;
		void* Field68;
		void* Field70;
		void* Field78;
		void* Field80;
		void* Field88;
		void* Field90;
		void* Field98;
		void* FieldA0;
		void* FieldA8;
		void* FieldB0;
		void* FieldB8;
		void* FieldC0;
		void* FieldC8;
		float FieldD0;
		char pad_000[0x4];
		void* FieldD8;
		LevelController* levelController;
		void* FieldE8;
		bool allPlayersAreConnected;
		bool FieldF1;
		char pad_001[0x2];
		int32_t FieldF4;
		void* FieldF8;
		bool Field100;
		char pad_002[0x7];
		void* Field108;
	};

	struct GameController
	{
		void* Clazz;
		void* Monitor;
		GameControllerFields Fields;
	};

	struct GameController_staticFields
	{
		GameController* instance;
	};

	DEC_MET(GameController_Exit, void(*)(GameController* gameController, void* photoMessageInfo, MethodInfo* methodInfo), "Assembly-CSharp", "", "GameController", "Exit", 1);
	DEC_MET(GameController_GetAveragePlayerInsanity, float(*)(GameController* gameController, MethodInfo* methodInfo), "Assembly-CSharp", "", "GameController", "GetAveragePlayerInsanity", 0);
	DEC_MET(GameController_PlayerDied, void(*)(GameController* gameController, Player* player, MethodInfo* methodInfo), "Assembly-CSharp", "", "GameController", "PlayerDied", 1);
	DEC_MET(GameController_Awake, void(*)(GameController* gameController, MethodInfo* methodInfo), "Assembly-CSharp", "", "GameController", "Awake", 0);
	DEC_SFIELD(GameController_StaticFields, GameController_staticFields, "Assembly-CSharp", "", "GameController");
}
