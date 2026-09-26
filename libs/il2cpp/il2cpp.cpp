#include "il2cpp.h"
#include "../Includes.h"

static HMODULE g_gameAssembly = nullptr;

static t_il2cpp_domain_get p_il2cpp_domain_get = nullptr;
static t_il2cpp_thread_attach p_il2cpp_thread_attach = nullptr;
static t_il2cpp_domain_assembly_open p_il2cpp_domain_assembly_open = nullptr;
static t_il2cpp_assembly_get_image p_il2cpp_assembly_get_image = nullptr;
static t_il2cpp_class_from_name p_il2cpp_class_from_name = nullptr;
static t_il2cpp_object_new p_il2cpp_object_new = nullptr;
static t_il2cpp_class_get_method_from_name p_il2cpp_class_get_method_from_name = nullptr;
static t_il2cpp_runtime_invoke p_il2cpp_runtime_invoke = nullptr;
static t_il2cpp_string_new p_il2cpp_string_new = nullptr;
static t_il2cpp_string_new_utf16 p_il2cpp_string_new_utf16 = nullptr;
static t_il2cpp_class_get_type p_il2cpp_class_get_type = nullptr;
static t_il2cpp_type_get_object p_il2cpp_type_get_object = nullptr;
static t_il2cpp_object_get_class p_il2cpp_object_get_class = nullptr;
static t_il2cpp_class_get_field_from_name p_il2cpp_class_get_field_from_name = nullptr;
static t_il2cpp_field_set_value p_il2cpp_field_set_value = nullptr;
static t_il2cpp_class_get_methods p_il2cpp_class_get_methods = nullptr;
static t_il2cpp_method_get_name p_il2cpp_method_get_name = nullptr;
static t_il2cpp_class_get_name p_il2cpp_class_get_name = nullptr;
static t_il2cpp_class_get_namespace p_il2cpp_class_get_namespace = nullptr;
static t_il2cpp_array_new p_il2cpp_array_new = nullptr;
static t_il2cpp_value_box p_il2cpp_value_box = nullptr;
static t_il2cpp_object_unbox p_il2cpp_object_unbox = nullptr;

static bool ResolveSymbol(const char* name, FARPROC* out)
{
    if (!g_gameAssembly) return false;
    *out = GetProcAddress(g_gameAssembly, name);
    return *out != nullptr;
}

bool il2cpp_initialize()
{
    if (g_gameAssembly) return true;
    g_gameAssembly = GetModuleHandleA("GameAssembly.dll");
    if (!g_gameAssembly) return false;

    FARPROC proc = nullptr;
#define RESOLVE_SYMBOL(name, var) ResolveSymbol(name, &proc); var = (decltype(var))proc
    RESOLVE_SYMBOL("il2cpp_domain_get", p_il2cpp_domain_get);
    RESOLVE_SYMBOL("il2cpp_thread_attach", p_il2cpp_thread_attach);
    RESOLVE_SYMBOL("il2cpp_domain_assembly_open", p_il2cpp_domain_assembly_open);
    RESOLVE_SYMBOL("il2cpp_assembly_get_image", p_il2cpp_assembly_get_image);
    RESOLVE_SYMBOL("il2cpp_class_from_name", p_il2cpp_class_from_name);
    RESOLVE_SYMBOL("il2cpp_object_new", p_il2cpp_object_new);
    RESOLVE_SYMBOL("il2cpp_string_new", p_il2cpp_string_new);
    RESOLVE_SYMBOL("il2cpp_string_new_utf16", p_il2cpp_string_new_utf16);
    RESOLVE_SYMBOL("il2cpp_class_get_type", p_il2cpp_class_get_type);
    RESOLVE_SYMBOL("il2cpp_type_get_object", p_il2cpp_type_get_object);
    RESOLVE_SYMBOL("il2cpp_class_get_method_from_name", p_il2cpp_class_get_method_from_name);
    RESOLVE_SYMBOL("il2cpp_runtime_invoke", p_il2cpp_runtime_invoke);
    RESOLVE_SYMBOL("il2cpp_object_get_class", p_il2cpp_object_get_class);
    RESOLVE_SYMBOL("il2cpp_class_get_field_from_name", p_il2cpp_class_get_field_from_name);
    RESOLVE_SYMBOL("il2cpp_field_set_value", p_il2cpp_field_set_value);
    RESOLVE_SYMBOL("il2cpp_class_get_methods", p_il2cpp_class_get_methods);
    RESOLVE_SYMBOL("il2cpp_class_get_methods", p_il2cpp_class_get_methods);
    RESOLVE_SYMBOL("il2cpp_method_get_name", p_il2cpp_method_get_name);
    RESOLVE_SYMBOL("il2cpp_class_get_name", p_il2cpp_class_get_name);
    RESOLVE_SYMBOL("il2cpp_class_get_namespace", p_il2cpp_class_get_namespace);
    RESOLVE_SYMBOL("il2cpp_array_new", p_il2cpp_array_new);
    RESOLVE_SYMBOL("il2cpp_value_box", p_il2cpp_value_box);
    RESOLVE_SYMBOL("il2cpp_object_unbox", p_il2cpp_object_unbox);
#undef RESOLVE_SYMBOL

    return true;
}

Il2CppMethodPointer il2cpp_get_method_pointer(const char* assemblyName, const char* namespaze, const char* className, const char* methodName, int argsCount)
{
    if (!il2cpp_initialize()) return nullptr;

    Il2CppDomain* domain = il2cpp_domain_get();
    if (!domain) return nullptr;

    Il2CppAssembly* assembly = p_il2cpp_domain_assembly_open(domain, assemblyName);
    if (!assembly) return nullptr;

    Il2CppImage* image = p_il2cpp_assembly_get_image(assembly);
    if (!image) return nullptr;

    Il2CppClass* klass = p_il2cpp_class_from_name(image, namespaze, className);
    if (!klass) return nullptr;

    const MethodInfo_I* method = p_il2cpp_class_get_method_from_name(klass, methodName, argsCount);
    if (!method) return nullptr;

    return method->methodPointer;
}

Il2CppObject* il2cpp_value_box(Il2CppClass* klass, void* data)
{
    return p_il2cpp_value_box ? p_il2cpp_value_box(klass, data) : nullptr;
}

Il2CppObject* il2cpp_array_new_wrap(Il2CppClass* klass, il2cpp_array_size_t length)
{
    return p_il2cpp_array_new ? p_il2cpp_array_new(klass, length) : nullptr;
}

Il2CppString* il2cpp_string_new_utf16_wrap(const wchar_t* str, int len)
{
    return p_il2cpp_string_new_utf16 ? p_il2cpp_string_new_utf16(str, len) : nullptr;
}

const Il2CppType* il2cpp_class_get_type(Il2CppClass* klass) { return p_il2cpp_class_get_type ? p_il2cpp_class_get_type(klass) : nullptr; }
Il2CppObject* il2cpp_type_get_object(const Il2CppType* type) { return p_il2cpp_type_get_object ? p_il2cpp_type_get_object(type) : nullptr; }
const MethodInfo_I* il2cpp_class_get_method_from_name_wrap(Il2CppClass* klass, const char* name, int args) { return p_il2cpp_class_get_method_from_name ? p_il2cpp_class_get_method_from_name(klass, name, args) : nullptr; }
void il2cpp_runtime_invoke_wrap(const MethodInfo_I* method, void* obj, void** params, void* exc) { if (p_il2cpp_runtime_invoke) p_il2cpp_runtime_invoke(method, obj, params, exc); }
Il2CppClass* il2cpp_object_get_class_wrap(Il2CppObject* obj) { return p_il2cpp_object_get_class ? p_il2cpp_object_get_class(obj) : nullptr; }
FieldInfo* il2cpp_class_get_field_from_name_wrap(Il2CppClass* klass, const char* name) { return p_il2cpp_class_get_field_from_name ? p_il2cpp_class_get_field_from_name(klass, name) : nullptr; }
void il2cpp_field_set_value_wrap(Il2CppObject* obj, FieldInfo* field, void* value) { if (p_il2cpp_field_set_value) p_il2cpp_field_set_value(obj, field, value); }
Il2CppDomain* il2cpp_domain_get() { return p_il2cpp_domain_get ? p_il2cpp_domain_get() : nullptr; }
void il2cpp_thread_attach(Il2CppDomain* domain) { if (p_il2cpp_thread_attach) p_il2cpp_thread_attach(domain); }
Il2CppObject* il2cpp_object_new_from_class(Il2CppClass* klass) { return p_il2cpp_object_new ? p_il2cpp_object_new(klass) : nullptr; }
Il2CppString* il2cpp_string_new_wrap(const char* str) { return p_il2cpp_string_new ? p_il2cpp_string_new(str) : nullptr; }

MethodInfo_I* il2cpp_class_get_methods_wrap(Il2CppClass* klass, void** iter)
{
    return p_il2cpp_class_get_methods
        ? p_il2cpp_class_get_methods(klass, iter)
        : nullptr;
}

const char* il2cpp_method_get_name_wrap2(const MethodInfo_I* method)
{
    return p_il2cpp_method_get_name
        ? p_il2cpp_method_get_name(method)
        : "<unknown>";
}

Il2CppAssembly* il2cpp_domain_assembly_open_wrap(Il2CppDomain* domain, const char* name)
{
    return p_il2cpp_domain_assembly_open
        ? p_il2cpp_domain_assembly_open(domain, name)
        : nullptr;
}

Il2CppImage* il2cpp_assembly_get_image_wrap(Il2CppAssembly* assembly)
{
    return p_il2cpp_assembly_get_image
        ? p_il2cpp_assembly_get_image(assembly)
        : nullptr;
}

Il2CppClass* il2cpp_class_from_name_wrap(Il2CppImage* image, const char* namespaze, const char* name)
{
    return p_il2cpp_class_from_name
        ? p_il2cpp_class_from_name(image, namespaze, name)
        : nullptr;
}

void* il2cpp_object_unbox(Il2CppObject* obj)
{
    return p_il2cpp_object_unbox ? p_il2cpp_object_unbox(obj) : nullptr;
}

Il2CppClass* il2cpp_get_class(const char* assemblyName, const char* namespaze, const char* name)
{
    if (!il2cpp_initialize()) return nullptr;
    Il2CppDomain* domain = il2cpp_domain_get();
    if (!domain) return nullptr;

    Il2CppAssembly* asmHandle = p_il2cpp_domain_assembly_open(domain, assemblyName);
    if (!asmHandle) return nullptr;

    Il2CppImage* image = p_il2cpp_assembly_get_image(asmHandle);
    if (!image) return nullptr;

    return p_il2cpp_class_from_name ? p_il2cpp_class_from_name(image, namespaze, name) : nullptr;
}

Il2CppObject* CreateIl2CppObject(const char* assemblyName, const char* namespaze, const char* name)
{
    if (!il2cpp_initialize()) return nullptr;
    Il2CppDomain* domain = il2cpp_domain_get();
    if (!domain) return nullptr;

    il2cpp_thread_attach(domain);

    Il2CppClass* klass = il2cpp_get_class(assemblyName, namespaze, name);
    if (!klass) return nullptr;

    Il2CppObject* obj = il2cpp_object_new_from_class(klass);
    if (!obj) return nullptr;

    if (p_il2cpp_class_get_method_from_name && p_il2cpp_runtime_invoke)
    {
        const MethodInfo_I* ctor = p_il2cpp_class_get_method_from_name(klass, ".ctor", 0);
        if (ctor) p_il2cpp_runtime_invoke(ctor, obj, nullptr, nullptr);
    }

    return obj;
}