#pragma once
#include "sdk.h"

namespace SDK
{
	struct Door;

    struct PhotonObjectInteractFields
    {
        MonoBehaviourFields MonoBehaviourFields;
        void* photonView;
        void* photonTransformView;
        void* vrPropGrab;
        Rigidbody* rigidbody;
        bool Field30;
        bool isProp;
        bool isItem;
        bool isGrabbable;
        bool isDroppable;
        bool isUsable;
        bool field9;
        bool field10;
        bool field11;
        bool Field39;
        bool Field3A;
        bool Field3B;
        uint8_t pad_000[7];
        void* drawer;
        Door* door;
        bool field14;
        void* colliderArray1;
        void* colliderArray2;
        void* colliderArray3;
        float throwMultiplier;
        Transform* transform;
        void* unityEvent1;
        void* unityEvent2;
        void* unityEvent3;
        void* unityEvent4;
        void* unityEvent5;
        void* unityEvent6;
        void* unityEvent7;
        void* unityEvent8;
        void* unityEvent9;
        void* unityEvent10;
        void* unityEvent11;
        bool field28;
        bool field29;
        bool field30;
        GameObject* gameObject1;
        void* myLeftHandModels;
        GameObject* gameObject2;
        void* myRightHandModels;
        Vector3 field35;
        Quaternion field36;
        Vector3 field37;
        Vector3 field38;
        GameObject* gameObject3;
        GameObject* gameObject4;
        void* beltGrabbable;
        Transform* transform2;
    };

	struct PhotonObjectInteract
	{
		void* Clazz;
		void* Monitor;
		PhotonObjectInteractFields Fields;
	};

	DEC_MET(PhotonObjectInteract_Start, void(*)(PhotonObjectInteract* photonObjectInteract, MethodInfo* methodInfo), "Assembly-CSharp", "", "PhotonObjectInteract", "Start", 0);
    DEC_MET(PhotonObjectInteract_ForceDrop, void(*)(PhotonObjectInteract* photonObjectInteract, bool isMineCheck, bool diedOrDisconnected, MethodInfo* methodInfo), "Assembly-CSharp", "", "PhotonObjectInteract", "ForceDrop", 2);

    // DEC_ADDR(PCPropGrab_GrabDistance, float, "GameAssembly.dll", 0x0000000); // old, not used // PCPropGrab$$OnPickup
    DEC_MET_INSTR(PickupDistance, "Assembly-CSharp", "", "PCPropGrab", "OnPickup", 1, "\xF3\x0F\x10\x15", "xxx?", ResolveRip);
    DEC_PATTERN_INSTR(Call_ForceDrop, L"GameAssembly.dll", "il2cpp", ""); // removed in 0.17.1.5 // PCPropGrab__SwitchHand

    inline float PickupDistance_rd() { return Memory::Read<float>((float*)PickupDistance_addr); }
    inline void PickupDistance_wr(float value) { Memory::Write<float>((float*)PickupDistance_addr, value); }
}
