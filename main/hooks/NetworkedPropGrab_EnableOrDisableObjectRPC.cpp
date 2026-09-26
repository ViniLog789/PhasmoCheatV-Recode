#include "../Includes.h"
#include "../features/features_includes.h"

void Hooks::hkNetworkedPropGrab_EnableOrDisableObjectRPC(SDK::NetworkedPropGrab* prop, int32_t photonId, bool enabled, SDK::PhotonMessageInfo* photonInfo, SDK::MethodInfo* methodInfo)
{
	LOG_CALL("Called NetworkedPopGrab_EnableOrDisableObjectRPC");

    // u can check enabled == true or false, but it useless (maybe)

    if (photonId > 0 && GET_FEATURE_CONFIG_VALUE(Players, Pickup, "PocketEverything", bool))
    {
        bool senderIsLocal = static_cast<void*>(photonInfo->Fields.Sender) == static_cast<void*>(SDK::PhotonNetwork_Get_LocalPlayer(nullptr)); // im lazy for fix Player > PRPlayer lol

        if (auto photonView = SDK::PhotonView_Find(photonId, nullptr))
        {
            if (auto g_obj = SDK::Component_Get_GameObject(reinterpret_cast<SDK::Component*>(photonView), nullptr))
            {
                auto obj = reinterpret_cast<SDK::Object*>(g_obj);
                    
                bool isCursedItem = SDK::GameObject_GetComponentByName(g_obj, Utils::SysStrToUnityStr("CursedItem"), nullptr);
                bool isTripod = SDK::GameObject_GetComponentByName(g_obj, Utils::SysStrToUnityStr("Tripod"), nullptr);

                if (isTripod || isCursedItem)
                    return;
            }
        }
    }

	SDK::NetworkedPropGrab_EnableOrDisableObjectRPC(prop, photonId, enabled, photonInfo, methodInfo);
}