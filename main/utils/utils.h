#pragma once
#include "../Includes.h"
#include "../sdk/sdk.h"
#include "../../libs/il2cpp/il2cpp.h"

using namespace PhasmoCheatV;

namespace Utils {
	std::string GetPhasmoCheatVDirectory();
	void CreatePhasmoCheatVDirectory();
	bool NeedsMigration();
	bool MigrateDirectory();
	ImVec4 HSV2RGB(float h, float s, float v);
	SDK::NetworkPlayerSpot* GetNetworkPlayerSpot(const SDK::Player* player);
	SDK::Network* GetNetwork();
	SDK::Player* GetLocalPlayer();
	std::string getKeyName(int keyCode);
	std::string GhostEnumToStr(SDK::GhostState ghostState);
	std::string GhostEnumToStr(SDK::GhostType ghostType);
	std::string GhostEnumToStr(const SDK::GhostEvidence ghostEvidence);
	std::string GhostEnumToStrLocalized(SDK::GhostType type);
	std::string GhostEnumToStrLocalized(SDK::GhostEvidence GhostEved);
	std::string UnityStrToSysStr(const SDK::String& string);
	SDK::String* SysStrToUnityStr(const std::string& string);
	std::string GetPlayerName(const SDK::Player* player);
	SDK::ListPlayer* GetAllPlayers();
	int GetPlayerInx(const SDK::Player* player);
	bool WTS(const SDK::Vector3& worldPos, SDK::Vector3& displayPos);
	SDK::Vector3 GetPosVec3(const SDK::Player* player);
	SDK::Vector3 GetPosVec3(const SDK::GhostAI* ghostAI);
	SDK::Vector3 GetPosVec3(SDK::Component* component);
	SDK::Vector3 GetPosVec3(SDK::Transform* transform);
	std::string GhostEnumToStr(SDK::EvidenceType evidenceType);
	bool IsCursedItem(SDK::EvidenceType evidenceType);
	SDK::Transform* GetPlayerTransformCamera(const SDK::Player* player);
	SDK::LevelStats* GetLevelStats();
	double GetCheatUptimeSeconds();
	float toSanityOrInsanity(float insanity);
	float GetPlayerSanity(SDK::Player* player);
	bool IsLocalMasterClient();
	void TpPlayerToVec3(SDK::Player* player, const SDK::Vector3& position);
	void TpPlayerToPlayer(SDK::Player* player, const SDK::Player* twoplayer);
	SDK::LevelValues* GetLevelValues();
	SDK::Map* GetMap();
	std::string GetActiveSceneName();
	SDK::GameObject* FindObjectByName(std::string name);
	SDK::StoreItemInfo* GetStoreItemInfo();
	SDK::ObjectArray* FindObjectsOfType(std::string type);
	SDK::JournalController* GetMainMenuJournal();
	SDK::PhotonMessageInfo* CreatePhotonMessageInfo(SDK::PhotonView* photonView);
	SDK::ExitLevel* GetExitLevel();
	SDK::ServerManager* GetServerManager();
	void SetTagOnGObject(SDK::GameObject* gameObject, const std::string& tag);
	std::string GetTagOnGObject(SDK::GameObject* gameObject);
	bool CheckIsValidObjectByTag(SDK::GameObject* gameObject, const std::string& tag);
	void GetComponentsInChildren(SDK::GameObject* root, const char* componentName, std::vector<SDK::Component*>& outComponents, bool includeInactive);
	void DumpAllMethodsByClass(const char* className, const char* namespaze, const char* assemblyName);
	SDK::HandCamera* get_PlayerHandCamera(SDK::Player* player);
	bool IsGhostVisible(SDK::GhostAI* ghostAI);
	bool IsPlayerVisibleGhost(SDK::Camera* camera, SDK::Transform* transform);
	void DumpAllLayers();
	SDK::GameObject* GetPlayerCrosshairObj(SDK::Player* player);
	SDK::GhostAI* GetGhostAI();
	SDK::Transform* GetPotatoe();
	SDK::Type* GetType(std::string string);
	std::vector<std::tuple<std::string, SDK::GhostButton*, SDK::GameObject*>> get_AllGhostButtonsWithGO();
	std::string GetGameVersion();
	std::string GetUnityVersion();
	std::string GetPlayerName();
	bool Checks_IsRealSender(SDK::Player* pn_sender, SDK::PhotonView* view); // Method was lightweight
	float Distance(const SDK::Vector3& a, const SDK::Vector3& b);
	bool IsUniquePhoto(SDK::EvidenceType mediaType);
	bool TakePhoto(SDK::HandCamera* handCamera);
	SDK::Transform* FindChildByName(SDK::Transform* parent, const char* name);
	bool IsValidGO(SDK::GameObject* obj);
	SDK::GameObject* GameObject_Find(std::string name);
	bool IsInGame();
	SDK::Component* FindComponentByType(std::string type, bool retFirstComponent = false);
	bool IsObjectInFront(const SDK::Vector3& targetPos, const SDK::Vector3& referencePos, const SDK::Vector3& referenceForward);
	int32_t GetGhostTypeInt();

	// Helper functions
	template<typename T>
	void* VectorToIl2CppArray(const std::vector<T>& vec, const char* assembly, const char* namespaze, const char* clazzName)
	{
		auto klass = il2cpp_get_class(assembly, namespaze, clazzName);
		if (!klass)
			return nullptr;

		auto arr = il2cpp_array_new_wrap(klass, vec.size());
		if (!arr)
			return nullptr;

		auto arrT = (Il2CppArrayT<void*>*)arr;

		for (size_t i = 0; i < vec.size(); i++)
			il2cpp_array_set(arrT, i, (void*)vec[i]);

		return arr;
	}

	template<typename T>
	T* FindComponentRecursive(
		SDK::Transform* root,
		SDK::Type* type)
	{
		if (!root || !type)
			return nullptr;

		auto* go =
			SDK::Component_Get_GameObject(
				reinterpret_cast<SDK::Component*>(root),
				nullptr);

		if (go)
		{
			auto* comp =
				SDK::GameObject_GetComponent(
					go,
					type,
					nullptr);

			if (comp)
				return reinterpret_cast<T*>(comp);
		}

		int childCount =
			SDK::Transform_get_childCount(
				root,
				nullptr);

		for (int i = 0; i < childCount; i++)
		{
			auto* child =
				SDK::Transform_GetChild(
					root,
					i,
					nullptr);

			auto* result =
				FindComponentRecursive<T>(
					child,
					type);

			if (result)
				return result;
		}

		return nullptr;
	}
}