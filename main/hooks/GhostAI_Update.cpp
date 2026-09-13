#include "../Includes.h"
#include "../features/features_includes.h"

using namespace PhasmoCheatV;

void Hooks::hkGhostAI_Update(SDK::GhostAI* ghostAI, SDK::MethodInfo* methodInfo)
{
	LOG_CALL_UPDATE("Called GhostAI_Update");

	SDK::GhostAI_Update(ghostAI, methodInfo);

	// EXAMPLE CALL: CALL_METHOD(Visuals, Watermark, OnRender);

	CALL_METHOD(Ghost, GhostModifier, GhostModifierMain);
	CALL_METHOD(Ghost, GhostInteractor, GhostInteractorMain);
	CALL_METHOD_IF_ACTIVE_ARGS(Misc, GhostSpin, GhostSpinMain, ghostAI);
	CALL_METHOD_ARGS(Misc, GhostHandstand, GhostHandstandMain, ghostAI);
	CALL_METHOD(Players, Pickup, PickupMain);
	CALL_METHOD(Map, GrabKeys, GrabKeysMain);
	CALL_METHOD(Map, SaltModifier, SaltModifierMain);
	CALL_METHOD(Visuals, StatsPanel, StatsPanelCollectBone);
	CALL_METHOD(Misc, PhotoModifier, PhotoModifierAutoPhoto);
	CALL_METHOD(Map, FuseBoxModifier, FuseBoxModifierHandler);
	CALL_METHOD(Players, SkipLayerAnim, SkipLayerAnimHandler);
	CALL_METHOD(Misc, JournalModifier, JournalModifierHandler);

	if (ForTestsFlag && IsDebugging)
		Test::TestFeatures1();
}