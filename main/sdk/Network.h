#pragma once
#include "sdk.h"

namespace SDK
{
	struct Network;
	struct LocalPCPlayer;

	struct __declspec(align(8)) NetworkPlayerSpotFields
	{
		bool PlayerReady;
		char pad_11[0x7];
		void* PhotonPlayer;
		String* UnityPlayerID;
		int Experience;
		int Level;
		int Prestige;
		char pad_34[0x4];
		Player* Player;
		float PlayerVolume;
		char pad_44[0x4];
		String* AccountName;
		bool IsKicked;
		bool IsHacker;
		bool IsBlocked;
		char pad_53[0x5];
		void* RoleBadges;
		void* Role;
		int PrestigeIndex;
		int ThemeIndex;
		char pad_6C[0x4];
		void* VotedContract;
		int32_t PlatformType;
		bool HasReceivedPlayerInformation;
		bool PlayerIsBlocked;
		char pad_7E[0x2];
		int32_t LegacyLevel;
		int32_t LegacyAccent;
		int32_t LegacyBackground;
		int32_t LegacyColor;
		void* PlayerCosmetics;
		void* PlayerEquipment;
		bool hasBroughtItems;
		char pad_A1[0x3];
		int32_t totalEquipmentCost;
		void* OnBlockMuteStateChanged;
	};

	struct NetworkPlayerSpot
	{
		void* Clazz;
		void* Monitor;
		NetworkPlayerSpotFields Fields;
	};

	struct NetworkPlayerSpotArray
	{
		void* Clazz;
		void* Monitor;
		void* Bounds;
		void* MaxLength;
		NetworkPlayerSpot* Vector[32];
	};

	struct __declspec(align(8)) ListNetworkPlayerSpotFields
	{
		NetworkPlayerSpotArray* Items;
		int32_t Size;
		int32_t Version;
		void* SyncRoot;
	};

	struct ListNetworkPlayerSpot
	{
		void* Clazz;
		void* Monitor;
		ListNetworkPlayerSpotFields Fields;
	};

	struct NetworkFields
	{
		MonoBehaviourPunCallbacksFields MonoBehaviourPunCallbacksFields;
		Player* localPlayer;
		LocalPlayer* localPlayer_;
		ListNetworkPlayerSpot* NetworkPlayerSpots;
	};

	struct Network
	{
		void* Clazz;
		void* Monitor;
		NetworkFields Fields;
	};

	DEC_MET(Network_Get_Network, Network* (*)(MethodInfo* methodInfo), "Assembly-CSharp", "", "Network", "get_Instance", 0);
	DEC_MET(Network_get_nonNetworkedPCPlayer, LocalPCPlayer* (*)(Network* network, MethodInfo* methodInfo), "Assembly-CSharp", "", "Network", "get_nonNetworkedPCPlayer", 0);
}
