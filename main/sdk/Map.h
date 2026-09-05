#pragma once
#include "sdk.h"

namespace SDK
{
    struct MapFields
    {
        char pad_000[8]; // ScriptableObject
        int32_t levelType;
        int32_t levelSize;
		bool isRestricted;
        char pad_001[0x7];
        String* mapName;
        String* descriptionLocaliseID;
        int32_t totalRooms;
        int32_t totalFloors;
        void* mapSelectSprite;
        void* loadingSpriteLarge;
        void* loadingSpriteSmall;
        String* sceneName;
        String* eventOverrides;
        int32_t uniqueMapID;
        int32_t activityID;
        int32_t unlockAtLevel;
        int32_t mapFlags;
    };

	struct Map
	{
		void* Clazz;
		void* Monitor;
		MapFields Fields;
	};
}