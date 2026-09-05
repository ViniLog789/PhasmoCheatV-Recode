#include "teleport.h"

using namespace PhasmoCheatV::Features::Movement;

Teleport::Teleport() : FeatureCore(LANG("Teleport_Header"), TYPE_MOVEMENT)
{
    auto config = GetConfigManager();
    DECLARE_CONFIG(config, "SavedPointX", float, 0.0f);
    DECLARE_CONFIG(config, "SavedPointY", float, 0.0f);
    DECLARE_CONFIG(config, "SavedPointZ", float, 0.0f);
    DECLARE_CONFIG(config, "HasSavedPoint", bool, false);
}

void Teleport::OnMenuRender()
{
    if (ImGui::Button(LANG("Teleport_SavePos")))
        SaveCurrentPosition();

    ImGui::SameLine();

    if (ImGui::Button(LANG("Teleport_ToSaved")))
        TeleportToSavedPoint();

    static float customX = 0.f, customY = 0.f, customZ = 0.f;
    ImGui::InputFloat("X##teleport", &customX, 0.1f, 1.f, "%.2f");
    ImGui::InputFloat("Y##teleport", &customY, 0.1f, 1.f, "%.2f");
    ImGui::InputFloat("Z##teleport", &customZ, 0.1f, 1.f, "%.2f");

    if (ImGui::Button(LANG("Teleport_ToCoords")))
        TeleportToCoordinates(customX, customY, customZ);

    ImGui::Text(LANG("Teleport_Items"));

    if (SDK::MapController_sFields->instance)
        TeleportItems();

    if (ImGui::Button(LANG("Teleport_Bone")))
        TeleportBone();

    if (ImGui::Button(LANG("Teleport_Truck")))
        TeleportToTruck();

    ImGui::SameLine();

    if (ImGui::Button(LANG("Teleport_Ghost")))
        TeleportToGhost();

    ImGui::SameLine();

    const char* basementButtonText = LANG("Teleport_Basement");

    auto currentMap = Utils::GetMap();
    if (currentMap && currentMap->Fields.mapName)
    {
        std::string mapName = Utils::UnityStrToSysStr(*currentMap->Fields.mapName);

        if (mapName == "Prison") basementButtonText = LANG("Teleport_LobbyArea");
        else if (mapName == "Maple Lodge Campsite") basementButtonText = LANG("Teleport_CabinKitchen");
        else if (mapName == "Grafton Farmhouse") basementButtonText = LANG("Teleport_Storage");
        else if (mapName == "Brownstone High School") basementButtonText = LANG("Teleport_MainCorridor");
        else if (mapName == "Bleasdale Farmhouse") basementButtonText = LANG("Teleport_Attic");
        else if (mapName == "Point Hope") basementButtonText = LANG("Teleport_Bathroom");
        else if (mapName == "Camp Woodwind") basementButtonText = LANG("Teleport_FoodTent");
        else if (mapName == "Sunny Meadows" || mapName == "Sunny Meadows Restricted") basementButtonText = LANG("Teleport_Chapel");
    }

    if (ImGui::Button(basementButtonText))
        TeleportToBasement();

    ImGui::SameLine();

    if (ImGui::Button(LANG("Teleport_Entrance")))
        TeleportToEntrance();
}

void Teleport::TeleportItems()
{
    const auto localPlayer = Utils::GetLocalPlayer();
    if (!localPlayer)
        return NOTIFY_ERROR_QUICK(LANG("TP_LocalPlayerMissing"));

    auto objects = SDK::GameObject_FindGameObjectsWithTag(
        Utils::SysStrToUnityStr("Item"), nullptr);
    if (!objects)
        return;

    uint32_t length = 0;
    memcpy(&length, reinterpret_cast<uint8_t*>(objects) + 0x18, sizeof(uint32_t));
    if (length == 0)
        return NOTIFY_WARNING_QUICK(LANG("TP_NoItems"));

    const auto localPosition = Utils::GetPosVec3(localPlayer);

    static int selectedItemIndex = 0;
    static std::vector<std::string> itemNames;
    static std::vector<SDK::GameObject*> itemObjects;
    static bool itemsCached = false;

    if (ImGui::Button(LANG("Teleport_RefreshItems")))
    {
        itemNames.clear();
        itemObjects.clear();

        for (uint32_t i = 0; i < length; ++i)
        {
            SDK::GameObject* object = nullptr;
            memcpy(&object,
                reinterpret_cast<uint8_t*>(objects) + 0x20 + i * sizeof(void*),
                sizeof(void*));
            if (!object)
                continue;

            auto namePtr = SDK::Object_Get_Name(
                reinterpret_cast<SDK::Object*>(object), nullptr);
            if (!namePtr)
                continue;

            std::string name = Utils::UnityStrToSysStr(*namePtr);
            itemNames.push_back(name + "##" + std::to_string(i));
            itemObjects.push_back(object);
        }

        if (!itemNames.empty())
        {
            selectedItemIndex = 0;
            itemsCached = true;
            NOTIFY_INFO_QUICK(LANG("TP_ItemsRefreshed"));
        }
        else
        {
            NOTIFY_WARNING_QUICK(LANG("TP_NoItems"));
        }
    }

    if (itemsCached && !itemNames.empty())
    {
        ImGui::Separator();
        ImGui::Text(LANG("Teleport_SelectItem"));

        static std::vector<const char*> itemNamesCStr;
        itemNamesCStr.clear();
        for (auto& s : itemNames)
            itemNamesCStr.push_back(s.c_str());

        ImGui::Combo("##ItemSelect", &selectedItemIndex,
            itemNamesCStr.data(),
            static_cast<int>(itemNamesCStr.size()));

        ImGui::SameLine();

        if (ImGui::Button(LANG("Teleport_Selected")))
        {
            if (selectedItemIndex >= 0 &&
                selectedItemIndex < static_cast<int>(itemObjects.size()))
            {
                auto object = itemObjects[selectedItemIndex];
                auto transform = SDK::GameObject_get_transform(object, nullptr);
                if (transform)
                {
                    auto pvType = SDK::System_Type_GetType(
                        Utils::SysStrToUnityStr("Photon.Pun.PhotonView"), nullptr);
                    auto photonView = reinterpret_cast<SDK::PhotonView*>(
                        SDK::GameObject_GetComponent(object, pvType, nullptr));
                    if (photonView)
                        SDK::PhotonView_RequestOwnership(photonView, nullptr);

                    SDK::Vector3 pos = localPosition;
                    pos.Y += 0.5f;
                    SDK::Transform_Set_Position(transform, pos, nullptr);
                    NOTIFY_INFO_QUICK(LANG("TP_ItemTeleported"));
                }
            }
        }
    }

    if (ImGui::Button(LANG("Teleport_AllItems")))
    {
        int count = 0;

        for (uint32_t i = 0; i < length; ++i)
        {
            SDK::GameObject* object = nullptr;
            memcpy(&object,
                reinterpret_cast<uint8_t*>(objects) + 0x20 + i * sizeof(void*),
                sizeof(void*));
            if (!object)
                continue;

            auto transform = SDK::GameObject_get_transform(object, nullptr);
            if (!transform)
                continue;

            auto pvType = SDK::System_Type_GetType(
                Utils::SysStrToUnityStr("Photon.Pun.PhotonView"), nullptr);
            auto photonView = reinterpret_cast<SDK::PhotonView*>(
                SDK::GameObject_GetComponent(object, pvType, nullptr));
            if (photonView)
                SDK::PhotonView_RequestOwnership(photonView, nullptr);

            bool isMine = SDK::PhotonView_get_IsMine(photonView, nullptr);
            if (!isMine)
                continue;

            SDK::Vector3 pos = localPosition;
            pos.Y += 0.5f;
            SDK::Transform_Set_Position(transform, pos, nullptr);
            count++;
        }

        char msg[128];
        snprintf(msg, sizeof(msg),
            "Teleported %d items to your position!", count);
        NOTIFY_INFO(msg, 3.0f);
    }
}

void Teleport::TeleportBone()
{
    if (!SDK::MapController_sFields->instance || !SDK::EvidenceController_sFields->instance)
        return NOTIFY_ERROR_QUICK(LANG("NeedToBeInGame"));

    const auto localPlayer = Utils::GetLocalPlayer();
    if (!localPlayer)
        return NOTIFY_ERROR_QUICK(LANG("TP_LocalPlayerMissing"));

    const auto localPos = Utils::GetPosVec3(localPlayer);
    const auto evidenceList = SDK::EvidenceController_sFields->instance->Fields.EvidenceList;

    if (!evidenceList || !evidenceList->Fields.Items || evidenceList->Fields.Size <= 0)
        return;

    const auto& items = evidenceList->Fields.Items->Vector;
    const uint32_t count = evidenceList->Fields.Size;

    for (uint32_t i = 0; i < count; i++)
    {
        const auto ev = items[i];
        if (!ev || !ev->Fields.MediaValues)
            continue;

        if (ev->Fields.MediaValues->Fields.EvidenceType != SDK::EvidenceType::DNA)
            continue;

        auto transform = SDK::Component_Get_Transform(
            reinterpret_cast<SDK::Component*>(ev), nullptr);
        if (!transform)
            continue;

        auto go = SDK::Component_Get_GameObject(
            reinterpret_cast<SDK::Component*>(ev), nullptr);
        if (go)
        {
            auto pvType = SDK::System_Type_GetType(
                Utils::SysStrToUnityStr("Photon.Pun.PhotonView"), nullptr);
            auto photonView = reinterpret_cast<SDK::PhotonView*>(
                SDK::GameObject_GetComponent(go, pvType, nullptr));
            if (photonView)
                SDK::PhotonView_RequestOwnership(photonView, nullptr);
            else
                LOG_ERROR("PhotonView not found!");
        }

        SDK::Vector3 pos = localPos;
        pos.Y += 0.5f;
        SDK::Transform_Set_Position(transform, pos, nullptr);
        NOTIFY_INFO_QUICK(LANG("TP_BoneTeleported"));
        return;
    }

    NOTIFY_WARNING_QUICK(LANG("BoneNotFound"));
}

void Teleport::SaveCurrentPosition()
{
    const auto localPlayer = Utils::GetLocalPlayer();
    if (!localPlayer)
        return NOTIFY_ERROR_QUICK(LANG("TP_LocalPlayerMissing"));

    SDK::Vector3 pos = Utils::GetPosVec3(localPlayer);
    auto cfg = GetConfigManager();

    SET_CONFIG_VALUE(cfg, "SavedPointX", float, pos.X);
    SET_CONFIG_VALUE(cfg, "SavedPointY", float, pos.Y);
    SET_CONFIG_VALUE(cfg, "SavedPointZ", float, pos.Z);
    SET_CONFIG_VALUE(cfg, "HasSavedPoint", bool, true);

    NOTIFY_INFO_QUICK(LANG("TP_PosSaved"));
}

void Teleport::TeleportToSavedPoint()
{
    auto cfg = GetConfigManager();
    if (!CONFIG_BOOL(cfg, "HasSavedPoint"))
        return NOTIFY_WARNING_QUICK(LANG("TP_NoSavedPoint"));

    const auto localPlayer = Utils::GetLocalPlayer();
    if (!localPlayer)
        return NOTIFY_ERROR_QUICK(LANG("TP_LocalPlayerMissing"));

    SDK::Vector3 pos{
        CONFIG_FLOAT(cfg, "SavedPointX"),
        CONFIG_FLOAT(cfg, "SavedPointY"),
        CONFIG_FLOAT(cfg, "SavedPointZ")
    };

    Utils::TpPlayerToVec3(localPlayer, pos);
    NOTIFY_INFO_QUICK(LANG("TP_ToSaved"));
}

void Teleport::TeleportToCoordinates(float x, float y, float z)
{
    const auto localPlayer = Utils::GetLocalPlayer();
    if (!localPlayer)
        return NOTIFY_ERROR_QUICK(LANG("TP_LocalPlayerMissing"));

    Utils::TpPlayerToVec3(localPlayer, SDK::Vector3{ x, y, z });

    char msg[128];
    snprintf(msg, sizeof(msg),
        "Teleported to: X=%.2f Y=%.2f Z=%.2f", x, y, z);
    NOTIFY_INFO(msg, 3.0f);
}

void Teleport::TeleportToTruck()
{
    const auto localPlayer = Utils::GetLocalPlayer();
    if (!localPlayer)
        return NOTIFY_ERROR_QUICK(LANG("TP_LocalPlayerMissing"));

    if (!SDK::MapController_sFields->instance)
        return NOTIFY_ERROR_QUICK(LANG("NeedToBeInGame"));

    auto obj = Utils::FindObjectByName("TruckFloorCollider");

    if (!obj)
        return NOTIFY_ERROR_QUICK(LANG("TP_TruckNotFound"));

    auto transform = SDK::GameObject_get_transform(obj, nullptr);
    if (!transform)
        return NOTIFY_ERROR_QUICK(LANG("TP_TruckNotFound"));

    auto pos = SDK::Transform_Get_Position(transform, nullptr);
    pos.Y += 2.f;

    Utils::TpPlayerToVec3(localPlayer, pos);

    NOTIFY_INFO_QUICK(LANG("TP_ToTruck"));
}

void Teleport::TeleportToGhost()
{
    const auto localPlayer = Utils::GetLocalPlayer();
    if (!localPlayer)
        return NOTIFY_ERROR_QUICK(LANG("TP_LocalPlayerMissing"));

    if (!Utils::GetGhostAI())
        return NOTIFY_ERROR_QUICK(LANG("NeedToBeInGame"));

    Utils::TpPlayerToVec3(localPlayer,
        Utils::GetPosVec3(Utils::GetGhostAI()));
    NOTIFY_INFO_QUICK(LANG("TP_ToGhost"));
}

void Teleport::TeleportToBasement()
{
    const auto localPlayer = Utils::GetLocalPlayer();
    if (!localPlayer)
        return NOTIFY_ERROR_QUICK(LANG("TP_LocalPlayerMissing"));

    if (!SDK::MapController_sFields->instance)
        return NOTIFY_ERROR_QUICK(LANG("NeedToBeInGame"));

    SDK::GameObject* circleObj = Utils::FindObjectByName("Summoning Circle Spawn");

    if (!circleObj)
        return NOTIFY_ERROR_QUICK("Summoning Circle Spawn not found!");

    auto transform = SDK::GameObject_get_transform(circleObj, nullptr);
    if (!transform)
        return NOTIFY_ERROR_QUICK("Transform not found!");

    SDK::Vector3 pos = SDK::Transform_Get_Position(transform, nullptr);
    pos.Y += 1.0f;

    Utils::TpPlayerToVec3(localPlayer, pos);
    NOTIFY_INFO_QUICK("Teleported to Basement!");
}

void Teleport::TeleportToEntrance()
{
    const auto localPlayer = Utils::GetLocalPlayer();
    if (!localPlayer)
        return NOTIFY_ERROR_QUICK(LANG("TP_LocalPlayerMissing"));

    if (!SDK::LevelController_sFields->instance)
        return NOTIFY_ERROR_QUICK(LANG("NeedToBeInGame"));

    auto exitDoorArray = SDK::LevelController_sFields->instance->Fields.exitDoors;
    if (!exitDoorArray || !exitDoorArray->Vector)
        return NOTIFY_ERROR_QUICK("Exit door not found!");

    SDK::Door* exitDoor = exitDoorArray->Vector[0];
    if (!exitDoor)
        return NOTIFY_ERROR_QUICK("Exit door is null!");

    auto transform = SDK::Component_Get_Transform(reinterpret_cast<SDK::Component*>(exitDoor), nullptr);
    if (!transform)
        return NOTIFY_ERROR_QUICK("Exit door transform not found!");

    SDK::Vector3 doorPos = SDK::Transform_Get_Position(transform, nullptr);
    float checkRadius = 0.1f;
    SDK::Vector3 safePos = doorPos;

    auto IsBlocked = [&](SDK::Vector3 pos) -> bool
        {
            SDK::ColliderArray* colliders = SDK::Physics_OverlapSphere(pos, checkRadius, nullptr);
            bool blocked = colliders && colliders->MaxLength > 0;
            return blocked;
        };

    int attempts = 10;

    while (IsBlocked(safePos) && attempts > 0)
    {
        safePos.X += ((rand() % 3) - 1) * 0.3f;
        safePos.Z += ((rand() % 3) - 1) * 0.3f;
        attempts--;
    }

    Utils::TpPlayerToVec3(localPlayer, safePos);
    NOTIFY_INFO_QUICK(LANG("TP_ToEntrance"));
}