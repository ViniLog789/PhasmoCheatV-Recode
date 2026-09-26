#pragma once
#include <cstdint>
#include <Windows.h>

#ifdef _WIN32
#include <Windows.h>
#endif

typedef void(*Il2CppMethodPointer)(); // All structs taken by for Il2Cpp Dumper

struct Il2CppType;
struct Il2CppObject;
struct MethodInfo_I;
struct FieldInfo;
struct Il2CppClass;
struct Il2CppMethodEntry;
struct VirtualInvokeData;
struct Il2CppClass_2;
struct Il2CppClass_1;
typedef uintptr_t il2cpp_array_size_t;

typedef void (*InvokerMethod)(Il2CppMethodPointer, const MethodInfo_I*, void*, void**, void*);

struct Il2CppType
{
    void* data;
    unsigned int bits;
};

struct Il2CppClass_1
{
    void* image;
    void* gc_desc;
    const char* name;
    const char* namespaze;
    Il2CppType byval_arg;
    Il2CppType this_arg;
    Il2CppClass* element_class;
    Il2CppClass* castClass;
    Il2CppClass* declaringType;
    Il2CppClass* parent;
    void* generic_class;
    void* typeMetadataHandle;
    void* interopData;
    Il2CppClass* klass;
    void* fields;
    void* events;
    void* properties;
    void* methods;
    Il2CppClass** nestedTypes;
    Il2CppClass** implementedInterfaces;
    void* interfaceOffsets; // Il2CppRuntimeInterfaceOffsetPair
};

struct Il2CppClass_2
{
    Il2CppClass** typeHierarchy;
    void* unity_user_data;
    uint32_t initializationExceptionGCHandle;
    uint32_t cctor_started;
    uint32_t cctor_finished;
    size_t cctor_thread;
    void* genericContainerHandle;
    uint32_t instance_size;
    uint32_t actualSize;
    uint32_t element_size;
    int32_t native_size;
    uint32_t static_fields_size;
    uint32_t thread_static_fields_size;
    int32_t thread_static_fields_offset;
    uint32_t flags;
    uint32_t token;
    uint16_t method_count;
    uint16_t property_count;
    uint16_t field_count;
    uint16_t event_count;
    uint16_t nested_type_count;
    uint16_t vtable_count;
    uint16_t interfaces_count;
    uint16_t interface_offsets_count;
    uint8_t typeHierarchyDepth;
    uint8_t genericRecursionDepth;
    uint8_t rank;
    uint8_t minimumAlignment;
    uint8_t naturalAligment;
    uint8_t packingSize;
    uint8_t bitflags1;
    uint8_t bitflags2;
};

struct VirtualInvokeData
{
    Il2CppMethodPointer methodPtr;
    const MethodInfo_I* method;
};

struct Il2CppClass
{
    Il2CppClass_1 _1;
    void* static_fields;
    void* rgctx_data; // Il2CppRGCTXData
    Il2CppClass_2 _2;
    VirtualInvokeData vtable[255];
};

struct Il2CppMethodEntry
{
    const char* name;
    MethodInfo_I* method;
    Il2CppMethodPointer methodPointer;
    uint8_t paramCount;
};

struct FieldInfo
{
    const char* name;
    const Il2CppType* type;
    Il2CppClass* parent;
    int32_t offset;
    uint32_t token;
    uint8_t attrs;
    uint8_t padding[3];
};

struct MethodInfo_I
{
    Il2CppMethodPointer methodPointer;
    Il2CppMethodPointer virtualMethodPointer;
    InvokerMethod invoker_method;
    const char* name;
    Il2CppClass* klass;
    const Il2CppType* return_type;
    const Il2CppType** parameters;
    uint32_t token;
    uint16_t flags;
    uint16_t iflags;
    uint16_t slot;
    uint8_t parameters_count;
    uint8_t bitflags;
};

template<typename T>
struct Il2CppArrayT
{
    void* klass;
    void* monitor;
    void* bounds;
    il2cpp_array_size_t max_length;
    T vector[0];
};

template<typename T>
inline void il2cpp_array_set(Il2CppArrayT<T>* arr, il2cpp_array_size_t index, T value)
{
    if (!arr || index >= arr->max_length)
        return;

    arr->vector[index] = value;
}

typedef struct Il2CppDomain Il2CppDomain;
typedef struct Il2CppAssembly Il2CppAssembly;
typedef struct Il2CppImage Il2CppImage;
typedef struct Il2CppString Il2CppString;

extern "C" {
    typedef Il2CppDomain* (*t_il2cpp_domain_get)(); 
    typedef void (*t_il2cpp_thread_attach)(Il2CppDomain*);
    typedef Il2CppAssembly* (*t_il2cpp_domain_assembly_open)(Il2CppDomain*, const char*);
    typedef Il2CppImage* (*t_il2cpp_assembly_get_image)(Il2CppAssembly*);
    typedef Il2CppClass* (*t_il2cpp_class_from_name)(Il2CppImage*, const char* namespaze, const char* name);
    typedef Il2CppObject* (*t_il2cpp_object_new)(Il2CppClass*);
    typedef Il2CppString* (*t_il2cpp_string_new)(const char*);
    typedef Il2CppString* (*t_il2cpp_string_new_utf16)(const wchar_t* str, int len);
    typedef const MethodInfo_I* (*t_il2cpp_class_get_method_from_name)(Il2CppClass*, const char* name, int argsCount);
    typedef void (*t_il2cpp_runtime_invoke)(const MethodInfo_I*, void*, void**, void*);
    typedef const char* (*t_il2cpp_type_get_name)(const Il2CppType*);
    typedef const Il2CppType* (*t_il2cpp_class_get_type)(Il2CppClass* klass);
    typedef Il2CppObject* (*t_il2cpp_type_get_object)(const Il2CppType* type);
    typedef Il2CppClass* (*t_il2cpp_object_get_class)(Il2CppObject* obj);
    typedef FieldInfo* (*t_il2cpp_class_get_field_from_name)(Il2CppClass* klass, const char* name);
    typedef void (*t_il2cpp_field_set_value)(Il2CppObject* obj, FieldInfo* field, void* value);
    typedef MethodInfo_I* (*t_il2cpp_class_get_methods)(Il2CppClass*, void**);
    typedef const char* (*t_il2cpp_method_get_name)(const MethodInfo_I*);
    typedef const char* (*t_il2cpp_class_get_name)(Il2CppClass*);
    typedef const char* (*t_il2cpp_class_get_namespace)(Il2CppClass*);
    typedef Il2CppObject* (*t_il2cpp_array_new)(Il2CppClass* elementTypeInfo, il2cpp_array_size_t length);
    typedef Il2CppObject* (*t_il2cpp_value_box)(Il2CppClass* klass, void* data);
    typedef void* (*t_il2cpp_object_unbox)(Il2CppObject* obj);

    bool il2cpp_initialize();
    Il2CppDomain* il2cpp_domain_get();
    void il2cpp_thread_attach(Il2CppDomain* domain);
    const Il2CppType* il2cpp_class_get_type(Il2CppClass* klass);
    Il2CppObject* il2cpp_type_get_object(const Il2CppType* type);
    const MethodInfo_I* il2cpp_class_get_method_from_name_wrap(Il2CppClass* klass, const char* name, int args);
    void il2cpp_runtime_invoke_wrap(const MethodInfo_I* method, void* obj, void** params, void* exc);
    Il2CppClass* il2cpp_get_class(const char* assemblyName, const char* namespaze, const char* name);
    Il2CppObject* il2cpp_object_new_from_class(Il2CppClass* klass);
    Il2CppObject* CreateIl2CppObject(const char* assemblyName, const char* namespaze, const char* name);
    Il2CppString* il2cpp_string_new_wrap(const char* str);
    Il2CppString* il2cpp_string_new_utf16_wrap(const wchar_t* str, int len);
    Il2CppClass* il2cpp_object_get_class_wrap(Il2CppObject* obj);
    FieldInfo* il2cpp_class_get_field_from_name_wrap(Il2CppClass* klass, const char* name);
    void il2cpp_field_set_value_wrap(Il2CppObject* obj, FieldInfo* field, void* value);
    MethodInfo_I* il2cpp_class_get_methods_wrap(Il2CppClass* klass, void** iter);
    const char* il2cpp_method_get_name_wrap2(const MethodInfo_I* method);
    Il2CppAssembly* il2cpp_domain_assembly_open_wrap(Il2CppDomain* domain, const char* name);
    Il2CppImage* il2cpp_assembly_get_image_wrap(Il2CppAssembly* assembly);
    Il2CppClass* il2cpp_class_from_name_wrap(Il2CppImage* image, const char* namespaze, const char* name);
    Il2CppObject* il2cpp_array_new_wrap(Il2CppClass* klass, il2cpp_array_size_t length);
    Il2CppObject* il2cpp_value_box(Il2CppClass* klass, void* data);
    void* il2cpp_object_unbox(Il2CppObject* obj);

    Il2CppMethodPointer il2cpp_get_method_pointer(const char* assemblyName, const char* namespaze, const char* className, const char* methodName, int argsCount);
}