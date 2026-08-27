#pragma once
#include "sdk.h"

namespace SDK
{
    struct GhostModel;
    struct Player;
    struct GhostActivity;

    enum class GhostState : int32_t
    {
        Idle,
        Wander,
        Hunting,
        FavoriteRoom,
        Light,
        Door,
        Throwing,
        FuseBox,
        Appear,
        DoorKnock,
        WindowKnock,
        CarAlarm,
        Flicker,
        CCTV,
        RandomEvent,
        GhostAbility,
        Mannequin,
        TeleportObject,
        Interact,
        SummoningCircle,
        MusicBox,
        Dots,
        Salt
    };


    struct GhostAIFields
    {
        MonoBehaviourPunFields MonoBehaviourPunFields;
        void* stateMachine;
        GhostState currentState;
        char pad_000[0x4];
        GhostInfo* GhostInfo;
        NavMeshAgent* NavMeshAgent;
        void* GhostAudio;
        void* GhostInteraction;
        GhostActivity* GhostActivity;
        GhostModel* currentModel;
        void* eventGhostModels;
        void* ghostModelArray0;
        void* ghostModelArray1;
        int32_t currrentShadowCastingMode;
        char pad_94[0x4];
        void* vectorsList;
        float Field80;
        float Field84;
        void* hearing;
        void* sensorToolkit;
        bool Field98;
        char pad_001[0x7];
        Transform* raycastPoint;
        Transform* huntingRaycastPoint;
        Transform* feetRaycastPoint;
        float defaultSpeed;
        float saltSpeedMultiplier;
        float incenseSpeedMultiplier;
        float FieldC4;
        bool FieldC8;
        bool FieldC9;
        char pad_002[0x2];
        Vector3 FieldCC;
        GameObject* FieldD8;
        bool FieldE0;
        bool FieldE1;
        bool FieldE2;
        bool FieldE3;
        bool FieldE4;
        bool delayedBySmudgeStick;
        char pad_003[0x2];
        void* whiteSage;
        float activeIncenseDuration; // trash value lol
        float FieldF4;
        bool FieldF8;
        bool FieldF9;
        bool FieldFA;
        char pad_004[0x5];
        void* player100;
        Player* BansheeTarget;
        int32_t Field110;
        Vector3 Field114;
        bool Field120;
        char pad_005[0x7];
        void* playerLOSTargetList;
        void* SingleArray0;
        void* SignleArray1;
        void* SingleArray2;
        int32_t Field148;
        int32_t Field14C;
        int32_t Field150;
        int32_t Field154;
        int32_t Field158;
        char pad_006[0x4];
        void* SingleArray3;
        void* SingleArray4;
        void* SingleArray5;
        float Field178;
        char pad_007[0x4];
        void* Int32Array;
    };

    struct GhostAI
    {
        void* Clazz;
        void* Monitor;
        GhostAIFields Fields;
    };

    DEC_MET(GhostAI_Appear, void(*)(GhostAI* ghostAI, int effectIndex, MethodInfo* methodInfo), "Assembly-CSharp", "", "GhostAI", "Appear", 1);
    DEC_MET(GhostAI_Hunting, void(*)(GhostAI* ghostAI, bool isHunting, int obakeArrayID, void* PhotonMessageInfo, MethodInfo* methodInfo), "Assembly-CSharp", "", "GhostAI", "Hunting", 3);
    DEC_MET(GhostAI_Start, void(*)(GhostAI* ghostAI, MethodInfo* methodInfo), "Assembly-CSharp", "", "GhostAI", "Start", 0);
    DEC_MET(GhostAI_Update, void(*)(GhostAI* ghostAI, MethodInfo* methodInfo), "Assembly-CSharp", "", "GhostAI", "Update", 0);
    DEC_MET(GhostAI_ChangeState, void(*)(GhostAI* ghostAI, GhostState ghostState, void* photonObjectInteract, void* photonObjectInteractArray, MethodInfo* methodInfo), "Assembly-CSharp", "", "GhostAI", "ChangeState", 3);
    DEC_MET(GhostAI_SetGhostModel, void(*)(GhostAI* ghostAI, GhostModel* ghostModel, bool resetHuntSound, MethodInfo* methodInfo), "Assembly-CSharp", "", "GhostAI", "SetGhostModel", 2);
}
