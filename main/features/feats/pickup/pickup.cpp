#include "pickup.h"

using namespace PhasmoCheatV::Features::Players;

Pickup::Pickup() : FeatureCore(LANG("Pickup_Header"), TYPE_PLAYERS)
{
	DECLARE_CONFIG(GetConfigManager(), "CustomThrow", float, 3.f);
	DECLARE_CONFIG(GetConfigManager(), "ThrowMultiplier", bool, false);
	DECLARE_CONFIG(GetConfigManager(), "PickupEverything", bool, false);
	DECLARE_CONFIG(GetConfigManager(), "CustomGrabDistance", float, 3.f);
	DECLARE_CONFIG(GetConfigManager(), "GrabDistanceMultiplier", bool, false);
	DECLARE_CONFIG(GetConfigManager(), "PocketEverything", bool, false);
}

void Pickup::OnMenuRender()
{
	ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(8, 6));

	bool enabled = IsActive();

	if (BCheckBox(LANG("EnablePickup"), &enabled, "b_EnablePickup"))
	{
		SET_CONFIG_VALUE(GetConfigManager(), "Enabled", bool, enabled);
		if (enabled) OnActivate();
		else OnDeactivate();
	}

	if (enabled)
	{
		float CustomThrow = CONFIG_FLOAT(GetConfigManager(), "CustomThrow");
		bool ThrowMultiplier = CONFIG_BOOL(GetConfigManager(), "ThrowMultiplier");
		float CustomGrabDistance = CONFIG_FLOAT(GetConfigManager(), "CustomGrabDistance");
		bool GrabDistanceMultiplier = CONFIG_BOOL(GetConfigManager(), "GrabDistanceMultiplier");
		bool PocketEverything = CONFIG_BOOL(GetConfigManager(), "PocketEverything");
		bool PickupEverything = CONFIG_BOOL(GetConfigManager(), "PickupEverything");

		if (ImGui::Checkbox(LANG("CustomThrow"), &ThrowMultiplier))
			SET_CONFIG_VALUE(GetConfigManager(), "ThrowMultiplier", bool, ThrowMultiplier);

		if (ImGui::SliderFloat(LANG("ThrowMultiplier"), &CustomThrow, 1.f, 100.0f, "%.1fx"))
			SET_CONFIG_VALUE(GetConfigManager(), "CustomThrow", float, CustomThrow);

		if (ImGui::Checkbox(LANG("CustomGrabDistance"), &GrabDistanceMultiplier))
			SET_CONFIG_VALUE(GetConfigManager(), "GrabDistanceMultiplier", bool, GrabDistanceMultiplier);

		if (ImGui::SliderFloat(LANG("GrabDistance"), &CustomGrabDistance, 1.f, 50.0f, "%.1f "))
			SET_CONFIG_VALUE(GetConfigManager(), "CustomGrabDistance", float, CustomGrabDistance);

		wImGui::TempDisabled(1);
		if (WD(ImGui::Checkbox(LANG("PickupEverything"), &PickupEverything)))
			SET_CONFIG_VALUE(GetConfigManager(), "PickupEverything", bool, PickupEverything);

		if (ImGui::Checkbox(LANG("PocketEverything"), &PocketEverything))
			SET_CONFIG_VALUE(GetConfigManager(), "PocketEverything", bool, PocketEverything);
	}

	ImGui::PopStyleVar();
}

void Pickup::PickupMain()
{
	if (InGame::photonInstances.empty())
		return;

	if (!IsActive()) 
		return;

	if (CONFIG_BOOL(GetConfigManager(), "GrabDistanceMultiplier"))
	{
		float grabDistance = CONFIG_FLOAT(GetConfigManager(), "CustomGrabDistance");
		if (SDK::PickupDistance_rd() != grabDistance)
			SDK::PickupDistance_wr(grabDistance);

	}

	if (CONFIG_BOOL(GetConfigManager(), "PickupEverything"))
	{
		for (auto* obj : InGame::photonInstances)
		{
			if (!obj) return;

			obj->Fields.isProp = true;
			obj->Fields.isItem = true;
			obj->Fields.isDroppable = true;
			obj->Fields.isUsable = true;
			obj->Fields.isGrabbable = true;
		}
	}

	if (CONFIG_BOOL(GetConfigManager(), "ThrowMultiplier"))
	{
		for (auto* obj : InGame::photonInstances)
		{
			if (!obj) return;

			obj->Fields.throwMultiplier = CONFIG_FLOAT(GetConfigManager(), "CustomThrow");
		}
	}
}

/* have fun :)
* (C#, UnityExplorer, Multiplayer not tested)
inf slots:

int targetSlots = 50;

var allPc = UnityEngine.Object.FindObjectsOfType<Il2Cpp.PCPropGrab>();
for (int i = 0; i < allPc.Length; i++)
{
	var list = allPc[i].field_Public_List_1_PhotonObjectInteract_0;
	if (list != null)
	{
		Log("PCPropGrab[" + i + "] 1" + list.Count);
		while (list.Count < targetSlots)
			list.Add(null);
		Log("PCPropGrab[" + i + "] 2" + list.Count);
	}
}

var allNet = UnityEngine.Object.FindObjectsOfType<Il2Cpp.NetworkedPropGrab>();
for (int i = 0; i < allNet.Length; i++)
{
	var list = allNet[i].field_Public_List_1_PhotonObjectInteract_0;
	if (list != null)
	{
		Log("NetworkedPropGrab[" + i + "] 1" + list.Count);
		while (list.Count < targetSlots)
			list.Add(null);
		Log("NetworkedPropGrab[" + i + "] 2" + list.Count);
	}
}
*/