#pragma once
#include "sdk.h"

namespace SDK
{
    struct Object
    {
        void* Clazz;
        void* Monitor;
        void* Fields;
    };

    struct ObjectArray
    {
        void* Clazz;
        void* Monitor;
        void* Bounds;
        uint32_t MaxLength;
        Object* Vector[1];
    };

    struct __declspec(align(8)) ListObjectFields
    {
        ObjectArray* Items;
        int32_t Size;
        int32_t Version;
        void* SyncRoot;
    };

    struct ListObject
    {
        void* Clazz;
        void* Monitor;
        ListObjectFields Fields;
    };

    DEC_MET(Object_Get_Name, SDK::String* (*)(Object* object, MethodInfo* methodInfo), "UnityEngine.CoreModule", "UnityEngine", "Object", "get_name", 0);
    DEC_MET(Object_Destroy, void(*)(SDK::Object* object, SDK::MethodInfo* methodInfo), "UnityEngine.CoreModule", "UnityEngine", "Object", "Destroy", 1);
    DEC_MET(Object_FindObjectsOfType, ObjectArray* (*)(Type* type, MethodInfo* methodInfo), "UnityEngine.CoreModule", "UnityEngine", "Object", "FindObjectsOfType", 1);
	DEC_MET(Object_set_name, void(*)(Object* object, String* name, MethodInfo* methodInfo), "UnityEngine.CoreModule", "UnityEngine", "Object", "set_name", 1);
    DEC_MET(Object_FindObjectFromInstanceID, Object* (*)(int32_t instanceId, MethodInfo* methodInfo), "UnityEngine.CoreModule", "UnityEngine", "Object", "FindObjectFromInstanceID", 1);
    DEC_MET(Object_IsNativeAlive, bool(*)(Object* object, MethodInfo* methodInfo), "UnityEngine.CoreModule", "UnityEngine", "Object", "IsNativeObjectAlive", 1);
    DEC_MET(Object_DontDestroyOnLoad, void(*)(Object* object, MethodInfo* methodInfo), "UnityEngine.CoreModule", "UnityEngine", "Object", "DontDestroyOnLoad", 1);
}