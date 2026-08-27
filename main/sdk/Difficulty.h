#pragma once
#include "sdk.h"

namespace SDK
{

    enum class DifficultyValueType : int32_t
    {
        None,
        Low,
        Medium,
        High,
        VeryHigh
    };

    enum class DifficultyWeatherValueType : int32_t
    {
        Random,
        LightRain,
        HeavyRain,
        Snow,
        Wind,
        Clear,
        Fog,
        Sunrise,
        BloodMoon
    };

    enum class DifficultyFuseBoxValueType : int32_t
    {
        Broken,
        Off,
        On
    };

    enum class DiffType : int32_t
    {
        Amateur,
        Intermediate,
        Professional,
        Nightmare,
        Insanity,
        Challenge,
        Custom,
        Custom2,
        Custom3,
        ApocalypseBronze,
        ApocalypseSilver,
        ApocalypseGold
    };

    enum class DiffCursedItems : int32_t
    {
        none,
        tarotCards,
        ouijaBoard,
        mirror,
        musicBox,
        summoningCircle,
        voodooDoll,
        monkeyPaw
    };

    struct CursedItemsArray
    {
        void* Clazz;
        void* Monitor;
        void* Bounds;
        uint32_t MaxLength;
        DiffCursedItems Vector[1];
    };

    struct DifficultyFields
    {
        char pad_000[0x8]; // scriptable object
        DiffType difficulty;
        char pad_001[0x3];
        String* nameKey;
        String* descriptionKey;
        int32_t requiredLevel;
        int32_t sanityPillRestorationAmount;
        int32_t startingSanity;
        float sanityDrain;
        int32_t sprinting;
        bool flashlights;
        bool loseItemsOnDeath;
        char pad_002[2];
        float playerSpeed;
        int32_t evidenceGiven;
        DifficultyValueType changingFav;
        DifficultyValueType interaction;
        DifficultyValueType eventFreq;
        DifficultyValueType huntDuration;
        int32_t gracePeriod;
        float fingerprintChance;
        int32_t fingerprintDuration;
        bool friendlyGhost;
        char pad_003[3];
        DifficultyValueType killsExtendHuntDuration;
        DifficultyValueType roamingFrequency;
        float ghostSpeed;
        int32_t ghostTypeOverride;
        int32_t setupTime;
        DifficultyWeatherValueType selectedWeather;
        DifficultyValueType amountDoorsOpen;
        DifficultyValueType amountHidingPlaces;
        bool sanityMonitor;
        bool activityMonitor;
        bool fuseBoxVisible;
        char pad_004;
        DifficultyFuseBoxValueType fuseBoxStartPower;
        int32_t cursedPossesionsQuantity;
        char pad_005[0x4];
        void* chosenCursedItems;
        float overrideMultiplier;
        DifficultyWeatherValueType actualWeather;
        CursedItemsArray* actualCursedItems;
        void* itemTiers;
        const char* mapName;
        void* nameLocalisedOverrides;
        void* descriptionLocalisedOverrides;
    };

    struct Difficulty
    {
        void* Clazz;
        void* Monitor;
        DifficultyFields Fields;
    };

    struct DifficultySettings;

    DEC_MET(DifficultySettings_AddHuntDuration, void(*)(DifficultySettings* difficultySettings, float amount, MethodInfo* methodInfo), "Assembly-CSharp", "", "DifficultySettings", "AddHuntDuration", 1);
    DEC_MET(DifficultySettings_Start, void(*)(DifficultySettings* difficultySettings, MethodInfo* methodInfo), "Assembly-CSharp", "", "DifficultySettings", "Start", 0);
}