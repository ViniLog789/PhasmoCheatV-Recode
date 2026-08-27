#pragma once
#include "sdk.h"

namespace SDK
{
    struct LevelStatsFields
    {
        int32_t diedAmount;
        int32_t revivedAmount;
        float distanceWalked;
        float distanceSprinted;
        float sanityGained;
        float sanityLost;
        float timeSpentInDark;
        float timeSpentInLight;
        float timeSpentInGhostsRoom;
        float timeSpentInvestigating;
        float timeSpentInTruck;
        float timeSpentBeingChased;
        int32_t photosTaken;
        int32_t videosTaken;
        int32_t soundsTaken;
        int32_t ghostsRepelled;
        int32_t phrasesRecognized;
        bool ghostSuccessfullyGuessed;
        char pad_000[0x3];
        int32_t objectivesCompleted;
        int32_t objectivesFailed;
        int32_t moneyEarned;
        int32_t moneySpent;
        int32_t itemsBought;
        int32_t itemsLost;
        int32_t amountOfCursedPossessionsUsed;
        int32_t amountOfCursedHuntsTriggered;
        bool collectedBone;
        char pad_001[0x3];
        float averageFPS;
        float ghostDistanceTravelled;
        int32_t itemsThrown;
        int32_t objectsUsed;
        int32_t doorsMoved;
        int32_t lightsSwitched;
        int32_t fuseboxToggles;
        int32_t roomsChanged;
        int32_t abilitiesUsed;
        float totalHuntTime;
        float timeInFavouriteRoom;
        float timeOutFavouriteRoom;
        int32_t amountOfGhostEvents;
        int32_t amountOfGhostHunts;
        int32_t amountOfGhostInteractions;
        String* boneRoom;
        int32_t shortRoams;
        int32_t longRoams;
        float timeSpentChasingPlayers;
		int32_t ghost; // idk what this is, but it's an int32_t
        void* Authentication;
    };

    struct LevelStats
    {
        void* Clazz;
        void* monitor;
        LevelStatsFields Fields;
    };


    DEC_MET(LevelStats_get_Instance, LevelStats* (*)(MethodInfo* methodInfo), "Assembly-CSharp", "", "LevelStats", "get_Instance", 0);
}
