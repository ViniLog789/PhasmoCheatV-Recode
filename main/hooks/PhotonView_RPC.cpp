#include "../Includes.h"
#include "../features/features_includes.h"

using namespace PhasmoCheatV;

void Hooks::hkPhotonView_RPC(SDK::PhotonView* photonView, SDK::String* methodName, SDK::RpcTarget target, void* parameters, SDK::MethodInfo* methodInfo)
{
    LOG_RPC(
        "PhotonView address: 0x%p\n"
        "MethodName: %s\n"
        "Target: %d\n",
        photonView,
        methodName ? Utils::UnityStrToSysStr(*methodName).c_str() : "null",
        target
    );

	CALL_METHOD_IF_ACTIVE_ARGS(Misc, PhotoModifier, PhotoModifierX5Photo, photonView, methodName, target, parameters, methodInfo);

    if (auto* feature = (::PhasmoCheatV::MainFeatureHandler)
        ->GetFeature<PhasmoCheatV::Features::Cursed::CursedItemsControll>("CursedItemsControll");
        Utils::UnityStrToSysStr(*methodName) == "ToEveryone_SyncUseAnimationFunctionalityPressed_Rpc" &&
        feature && feature->IsActive())
    {
        if (feature->TarotCardInfCards(photonView))
            return;
    }

    if (Utils::UnityStrToSysStr(*methodName) == "EnableOrDisableObjectRPC")
    {
		auto params = reinterpret_cast<SDK::System_Object_array*>(parameters);

        // bad idea..
    }

    SDK::PhotonView_RPC(photonView, methodName, target, parameters, methodInfo);
}