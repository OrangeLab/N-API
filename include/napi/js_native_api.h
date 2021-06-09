#ifndef js_native_api_h
#define js_native_api_h

#include <napi/js_native_api_types.h>

EXTERN_C_START

#include <stdbool.h> // NOLINT(modernize-deprecated-headers)
#include <stddef.h>  // NOLINT(modernize-deprecated-headers)
#include <stdint.h>  // NOLINT(modernize-deprecated-headers)

#define NAPI_AUTO_LENGTH SIZE_MAX

NAPIStatus napi_get_last_error_info(NAPIEnv env, const NAPIExtendedErrorInfo **result);

NAPIStatus napi_get_undefined(NAPIEnv env, NAPIValue *result);

NAPIStatus napi_get_null(NAPIEnv env, NAPIValue *result);

NAPIStatus napi_get_global(NAPIEnv env, NAPIValue *result);

NAPIStatus napi_get_boolean(NAPIEnv env, bool value, NAPIValue *result);

NAPIStatus napi_create_double(NAPIEnv env, double value, NAPIValue *result);

NAPIStatus napi_create_int32(NAPIEnv env, int32_t value, NAPIValue *result);

NAPIStatus napi_create_uint32(NAPIEnv env, uint32_t value, NAPIValue *result);

NAPIStatus napi_create_int64(NAPIEnv env, int64_t value, NAPIValue *result);

// 推荐实现层针对 str 为空情况做处理，比如当做 ""
NAPIStatus napi_create_string_utf8(NAPIEnv env, const char *str, size_t length, NAPIValue *result);

// 推荐实现层针对 str 为空情况做处理，比如当做 ""
// data 可空
NAPIStatus napi_create_function(NAPIEnv env, const char *utf8name, size_t length, NAPICallback cb, void *data,
                                NAPIValue *result);

NAPIStatus napi_typeof(NAPIEnv env, NAPIValue value, NAPIValueType *result);

NAPIStatus napi_get_value_double(NAPIEnv env, NAPIValue value, double *result);

NAPIStatus napi_get_value_int32(NAPIEnv env, NAPIValue value, int32_t *result);

NAPIStatus napi_get_value_uint32(NAPIEnv env, NAPIValue value, uint32_t *result);

NAPIStatus napi_get_value_int64(NAPIEnv env, NAPIValue value, int64_t *result);

NAPIStatus napi_get_value_bool(NAPIEnv env, NAPIValue value, bool *result);

// buf 可空，表示计算长度（不包括 \0），当 buf 为空时候，result 不能为空
// 实际上 buf 不为空，bufSize == 0 也能用来表示计算长度，但是暂时返回 0，建议不要依赖这个行为
NAPIStatus napi_get_value_string_utf8(NAPIEnv env, NAPIValue value, char *buf, size_t bufSize, size_t *result);

NAPIStatus napi_coerce_to_bool(NAPIEnv env, NAPIValue value, NAPIValue *result);

NAPIStatus napi_coerce_to_number(NAPIEnv env, NAPIValue value, NAPIValue *result);

NAPIStatus napi_coerce_to_string(NAPIEnv env, NAPIValue value, NAPIValue *result);

NAPIStatus napi_set_property(NAPIEnv env, NAPIValue object, NAPIValue key, NAPIValue value);

NAPIStatus napi_has_property(NAPIEnv env, NAPIValue object, NAPIValue key, bool *result);

NAPIStatus napi_get_property(NAPIEnv env, NAPIValue object, NAPIValue key, NAPIValue *result);

// result 可空
NAPIStatus napi_delete_property(NAPIEnv env, NAPIValue object, NAPIValue key, bool *result);

NAPIStatus napi_strict_equals(NAPIEnv env, NAPIValue lhs, NAPIValue rhs, bool *result);

// result 可空
NAPIStatus napi_call_function(NAPIEnv env, NAPIValue thisValue, NAPIValue func, size_t argc, const NAPIValue *argv,
                              NAPIValue *result);

NAPIStatus napi_new_instance(NAPIEnv env, NAPIValue constructor, size_t argc, const NAPIValue *argv, NAPIValue *result);

NAPIStatus napi_instanceof(NAPIEnv env, NAPIValue object, NAPIValue constructor, bool *result);

// argv/thisArg/data 可空，当 argv 非空时，argc 也必须非空
// env callbackInfo 入参，argc 为 inout，其他出参
NAPIStatus napi_get_cb_info(NAPIEnv env, NAPICallbackInfo callbackInfo, size_t *argc, NAPIValue *argv,
                            NAPIValue *thisArg, void **data);

NAPIStatus napi_get_new_target(NAPIEnv env, NAPICallbackInfo callbackInfo, NAPIValue *result);

// finalizeCB/data/finalizeHint 可空
NAPIStatus napi_create_external(NAPIEnv env, void *data, NAPIFinalize finalizeCB, void *finalizeHint,
                                NAPIValue *result);

NAPIStatus napi_get_value_external(NAPIEnv env, NAPIValue value, void **result);

// Set initial_refcount to 0 for a weak reference, >0 for a strong reference.
NAPIStatus napi_create_reference(NAPIEnv env, NAPIValue value, uint32_t initialRefCount, NAPIRef *result);

NAPIStatus napi_delete_reference(NAPIEnv env, NAPIRef ref);

// Increments the reference count, optionally returning the resulting count.
// After this call the  reference will be a strong reference because its
// refcount is >0, and the referenced object is effectively "pinned".
// Calling this when the refcount is 0 and the object is unavailable
// results in an error.
NAPIStatus napi_reference_ref(NAPIEnv env, NAPIRef ref, uint32_t *result);

// Decrements the reference count, optionally returning the resulting count.
// If the result is 0 the reference is now weak and the object may be GC'd
// at any time if there are no other references. Calling this when the
// refcount is already 0 results in an error.
NAPIStatus napi_reference_unref(NAPIEnv env, NAPIRef ref, uint32_t *result);

NAPIStatus napi_get_reference_value(NAPIEnv env, NAPIRef ref, NAPIValue *result);

NAPIStatus napi_open_handle_scope(NAPIEnv env, NAPIHandleScope *result);

NAPIStatus napi_close_handle_scope(NAPIEnv env, NAPIHandleScope scope);

NAPIStatus napi_open_escapable_handle_scope(NAPIEnv env, NAPIEscapableHandleScope *result);

NAPIStatus napi_close_escapable_handle_scope(NAPIEnv env, NAPIEscapableHandleScope scope);

NAPIStatus napi_escape_handle(NAPIEnv env, NAPIEscapableHandleScope scope, NAPIValue escapee, NAPIValue *result);

NAPIStatus napi_throw(NAPIEnv env, NAPIValue error);

NAPIStatus napi_get_and_clear_last_exception(NAPIEnv env, NAPIValue *result);

// 自定义函数
// utf8Script/utf8SourceUrl/result 可空
NAPIStatus NAPIRunScript(NAPIEnv env, const char *script, const char *sourceUrl, NAPIValue *result);

// 推荐实现层针对 str 为空情况做处理，比如当做 ""
// constructor/data/properties 可空
NAPIStatus NAPIDefineClass(NAPIEnv env, const char *utf8name, size_t length, NAPICallback constructor, void *data,
                           NAPIValue *result);

NAPIStatus NAPICreateEnv(NAPIEnv *env);

NAPIStatus NAPIFreeEnv(NAPIEnv env);

EXTERN_C_END

#endif /* js_native_api_h */
