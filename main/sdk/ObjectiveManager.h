#pragma once
#include "sdk.h"

namespace SDK
{
	struct ObjectiveManager;

	enum class ObjectivesType : int32_t
	{
		BlowoutCandle,
		DetectParabolicSound,
		EMFGhost,
		EscapeHunt,
		GhostPhoto,
		IdentifyGhost,
		MotionSensorGhost,
		PreventHunt,
		SanityBelow25,
		SmudgeGhost,
		SmudgeHunt,
		WitnessEvent,
		DnaEvidence,
		CompleteInTimeLimit,
		GhostVideo,
		UniquePhotos,
		UniqueVideos,
		UniqueSounds
	};

	struct ObjectiveManager_staticFields
	{
		ObjectiveManager* instance;
	};

	DEC_MET(ObjectiveManager_Start, void(*)(ObjectiveManager* objectiveManager, MethodInfo* methodInfo), "Assembly-CSharp", "", "ObjectiveManager", "Start", 0);
	DEC_MET(ObjectiveManager_CompleteObjectiveSynced, void(*)(ObjectiveManager* objectiveManager, ObjectivesType objectiveType, MethodInfo* methodInfo), "Assembly-CSharp", "", "ObjectiveManager", "CompleteObjectiveSynced", 1);
	DEC_MET(ObjectiveManager_Update, void(*)(SDK::ObjectiveManager* objectiveManager, SDK::MethodInfo* methodInfo), "Assembly-CSharp", "", "ObjectiveManager", "Update", 0);
	DEC_SFIELD(ObjectiveManager_StaticFields, ObjectiveManager_staticFields, "Assembly-CSharp", "", "ObjectiveManager");
}