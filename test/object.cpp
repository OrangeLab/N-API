#include <test.h>

EXTERN_C_START

static NAPIValue getThis(NAPIEnv env, NAPICallbackInfo callbackInfo)
{
    NAPIValue thisValue;
    assert(napi_get_cb_info(env, callbackInfo, nullptr, nullptr, &thisValue, nullptr) == NAPIOK);
    NAPIValue newTarget;
    assert(napi_get_new_target(env, callbackInfo, &newTarget) == NAPIOK);
    assert(!newTarget);

    return thisValue;
}

static NAPIValue newFunction(NAPIEnv env, NAPICallbackInfo callbackInfo)
{
    size_t argc = 3;
    NAPIValue argv[3];
    assert(napi_get_cb_info(env, callbackInfo, &argc, argv, nullptr, nullptr) == NAPIOK);
    assert(argc == 1 || argc == 3);
    if (argc == 1)
    {
        NAPIValueType valueType;
        assert(napi_typeof(env, argv[0], &valueType) == NAPIOK);
        assert(valueType == NAPIFunction);
        assert(napi_typeof(env, argv[1], &valueType) == NAPIOK);
        assert(valueType == NAPIUndefined);
        assert(napi_typeof(env, argv[2], &valueType) == NAPIOK);
        assert(valueType == NAPIUndefined);
        NAPIValue output;
        assert(napi_new_instance(env, argv[0], 0, nullptr, &output) == NAPIOK);

        return output;
    }
    else
    {
        NAPIValueType valueType;
        assert(napi_typeof(env, argv[0], &valueType) == NAPIOK);
        assert(valueType == NAPIFunction);
        assert(napi_typeof(env, argv[1], &valueType) == NAPIOK);
        assert(valueType == NAPINumber);
        assert(napi_typeof(env, argv[2], &valueType) == NAPIOK);
        assert(valueType == NAPIString);
        NAPIValue args[] = {argv[1], argv[2]};
        NAPIValue output;
        assert(napi_new_instance(env, argv[0], 2, args, &output) == NAPIOK);

        return output;
    }
}

static NAPIValue get(NAPIEnv env, NAPICallbackInfo info)
{
    size_t argc = 2;
    NAPIValue args[2];
    assert(napi_get_cb_info(env, info, &argc, args, nullptr, nullptr) == NAPIOK);

    assert(argc >= 2);

    NAPIValueType valueType;
    assert(napi_typeof(env, args[0], &valueType) == NAPIOK);

    assert(valueType == NAPIObject);

    assert(napi_typeof(env, args[1], &valueType) == NAPIOK);

    assert(valueType == NAPIString || valueType == NAPINumber);

    NAPIValue object = args[0];
    NAPIValue output;
    assert(napi_get_property(env, object, args[1], &output) == NAPIOK);

    return output;
}

static NAPIValue set(NAPIEnv env, NAPICallbackInfo info)
{
    size_t argc = 3;
    NAPIValue args[3];
    assert(napi_get_cb_info(env, info, &argc, args, nullptr, nullptr) == NAPIOK);

    assert(argc >= 3);

    NAPIValueType valueType;
    assert(napi_typeof(env, args[0], &valueType) == NAPIOK);

    assert(valueType == NAPIObject);

    assert(napi_typeof(env, args[1], &valueType) == NAPIOK);

    assert(valueType == NAPIString || valueType == NAPINumber);

    assert(napi_set_property(env, args[0], args[1], args[2]) == NAPIOK);

    return nullptr;
}

static NAPIValue has(NAPIEnv env, NAPICallbackInfo info)
{
    size_t argc = 2;
    NAPIValue args[2];
    assert(napi_get_cb_info(env, info, &argc, args, nullptr, nullptr) == NAPIOK);

    assert(argc >= 2);

    NAPIValueType valueType;
    assert(napi_typeof(env, args[0], &valueType) == NAPIOK);

    assert(valueType == NAPIObject);

    assert(napi_typeof(env, args[1], &valueType) == NAPIOK);

    assert(valueType == NAPIString || valueType == NAPINumber);

    bool hasProperty;
    assert(napi_has_property(env, args[0], args[1], &hasProperty) == NAPIOK);

    NAPIValue ret;
    assert(napi_get_boolean(env, hasProperty, &ret) == NAPIOK);

    return ret;
}

static NAPIValue deleteFunction(NAPIEnv env, NAPICallbackInfo info)
{
    size_t argc = 2;
    NAPIValue args[2];

    assert(napi_get_cb_info(env, info, &argc, args, nullptr, nullptr) == NAPIOK);
    assert(argc >= 2);

    NAPIValueType valueType;
    assert(napi_typeof(env, args[0], &valueType) == NAPIOK);

    assert(valueType == NAPIObject);

    assert(napi_typeof(env, args[1], &valueType) == NAPIOK);

    assert(valueType == NAPIString || valueType == NAPINumber);

    bool result;
    NAPIValue ret;
    assert(napi_delete_property(env, args[0], args[1], &result) == NAPIOK);
    assert(napi_get_boolean(env, result, &ret) == NAPIOK);

    return ret;
}

static NAPIValue isArray(NAPIEnv env, NAPICallbackInfo callbackInfo)
{
    size_t argc = 1;
    NAPIValue argv[1];
    assert(napi_get_cb_info(env, callbackInfo, &argc, argv, nullptr, nullptr) == NAPIOK);
    bool result;
    assert(napi_is_array(env, argv[0], &result) == NAPIOK);
    NAPIValue output;
    assert(napi_get_boolean(env, result, &output) == NAPIOK);

    return output;
}

static void externalFinalize(NAPIEnv env, void *finalizeData, void *finalizeHint)
{
    finalizeIsCalled = true;
    assert(env == finalizeData);
    assert(finalizeHint == (void *)1);
}

EXTERN_C_END

TEST_F(Test, Object)
{
    NAPIValue externalValue;
    ASSERT_EQ(napi_create_external(globalEnv, globalEnv, externalFinalize, (void *)1, &externalValue), NAPIOK);
    void *data;
    ASSERT_EQ(napi_get_value_external(globalEnv, externalValue, &data), NAPIOK);
    ASSERT_EQ(data, globalEnv);
    NAPIValueType valueType;
    ASSERT_EQ(napi_typeof(globalEnv, externalValue, &valueType), NAPIOK);
    ASSERT_EQ(valueType, NAPIExternal);

    NAPIValue getValue, setValue, hasValue, deleteValue, isArrayValue, newValue, getThisValue;
    ASSERT_EQ(napi_create_function(globalEnv, nullptr, -1, get, nullptr, &getValue), NAPIOK);
    ASSERT_EQ(napi_create_function(globalEnv, nullptr, -1, set, nullptr, &setValue), NAPIOK);
    ASSERT_EQ(napi_create_function(globalEnv, nullptr, -1, has, nullptr, &hasValue), NAPIOK);
    ASSERT_EQ(napi_create_function(globalEnv, nullptr, -1, deleteFunction, nullptr, &deleteValue), NAPIOK);
    ASSERT_EQ(napi_create_function(globalEnv, nullptr, -1, isArray, nullptr, &isArrayValue), NAPIOK);
    ASSERT_EQ(napi_create_function(globalEnv, nullptr, -1, newFunction, nullptr, &newValue), NAPIOK);
    ASSERT_EQ(napi_create_function(globalEnv, nullptr, -1, getThis, nullptr, &getThisValue), NAPIOK);
    NAPIValue stringValue;
    ASSERT_EQ(napi_create_string_utf8(globalEnv, "get", -1, &stringValue), NAPIOK);
    ASSERT_EQ(napi_set_property(globalEnv, addonValue, stringValue, getValue), NAPIOK);
    ASSERT_EQ(napi_create_string_utf8(globalEnv, "set", -1, &stringValue), NAPIOK);
    ASSERT_EQ(napi_set_property(globalEnv, addonValue, stringValue, setValue), NAPIOK);
    ASSERT_EQ(napi_create_string_utf8(globalEnv, "has", -1, &stringValue), NAPIOK);
    ASSERT_EQ(napi_set_property(globalEnv, addonValue, stringValue, hasValue), NAPIOK);
    ASSERT_EQ(napi_create_string_utf8(globalEnv, "delete", -1, &stringValue), NAPIOK);
    ASSERT_EQ(napi_set_property(globalEnv, addonValue, stringValue, deleteValue), NAPIOK);
    ASSERT_EQ(napi_create_string_utf8(globalEnv, "isArray", -1, &stringValue), NAPIOK);
    ASSERT_EQ(napi_set_property(globalEnv, addonValue, stringValue, isArrayValue), NAPIOK);
    ASSERT_EQ(napi_create_string_utf8(globalEnv, "new", -1, &stringValue), NAPIOK);
    ASSERT_EQ(napi_set_property(globalEnv, addonValue, stringValue, newValue), NAPIOK);
    ASSERT_EQ(napi_create_string_utf8(globalEnv, "getThis", -1, &stringValue), NAPIOK);
    ASSERT_EQ(napi_set_property(globalEnv, addonValue, stringValue, getThisValue), NAPIOK);
    ASSERT_EQ(
        NAPIRunScript(
            globalEnv,
            "(()=>{\"use strict\";function l(l,a){if(!(l instanceof a))throw new TypeError(\"Cannot call a class as a "
            "function\")}function a(l,a,s){return a in "
            "l?Object.defineProperty(l,a,{value:s,enumerable:!0,configurable:!0,writable:!0}):l[a]=s,l}globalThis."
            "assert(globalThis.addon.isArray([])),globalThis.assert(globalThis.addon.getThis()===globalThis.addon);var "
            "s=function s(o,t){l(this,s),a(this,\"arg1\",void 0),a(this,\"arg2\",void "
            "0),this.arg1=o,this.arg2=t},o=function "
            "a(){l(this,a)},t=globalThis.addon.new(o);globalThis.assert(Object.getPrototypeOf(t)===o.prototype);var "
            "e=globalThis.addon.new(s,1,\"hello\");globalThis.assert(Object.getPrototypeOf(e)===s.prototype),"
            "globalThis.assert(1===e.arg1),globalThis.assert(\"hello\"===e.arg2);var "
            "i={hello:\"world\",0:\"hello\"};globalThis.assert(\"world\"===globalThis.addon.get(i,\"hello\")),"
            "globalThis.assert(\"hello\"===globalThis.addon.get(i,0)),globalThis.assert(globalThis.addon.has(i,"
            "\"hello\")),globalThis.assert(globalThis.addon.has(i,0)),globalThis.assert(globalThis.addon.delete(i,"
            "\"hello\")),globalThis.assert(globalThis.addon.delete(i,0)),globalThis.assert(!globalThis.addon.has(i,"
            "\"hello\")),globalThis.assert(!globalThis.addon.has(i,0));var "
            "r=function(){this.foo=42,this.bar=43,this[0]=1,this[1]=2};r.prototype.bar=44,r.prototype[1]=3,r.prototype."
            "baz=45,r.prototype[2]=4;var h=new "
            "r;globalThis.assert(42===globalThis.addon.get(h,\"foo\")),globalThis.assert(43===globalThis.addon.get(h,"
            "\"bar\")),globalThis.assert(45===globalThis.addon.get(h,\"baz\")),globalThis.assert(globalThis.addon.get("
            "h,\"toString\")===Object.prototype.toString),globalThis.assert(globalThis.addon.has(h,\"toString\")),"
            "globalThis.assert(1===globalThis.addon.get(h,0)),globalThis.assert(2===globalThis.addon.get(h,1)),"
            "globalThis.assert(4===globalThis.addon.get(h,2)),globalThis.assert(globalThis.addon.delete(h,2)),"
            "globalThis.assert(globalThis.addon.delete(h,\"baz\")),globalThis.assert(globalThis.addon.has(h,2)),"
            "globalThis.assert(globalThis.addon.has(h,\"baz\"));var "
            "g=function(l){globalThis.assert(!!l),globalThis.assert(null==l?void "
            "0:l.configurable),globalThis.assert(null==l?void 0:l.enumerable),globalThis.assert(null==l?void "
            "0:l.writable),globalThis.assert(null==l?void "
            "0:l.value),globalThis.assert(!(null!=l&&l.get)),globalThis.assert(!(null!=l&&l.set))},b={};globalThis."
            "addon.set(b,\"world\",!0),globalThis.addon.set(b,0,!0),g(Object.getOwnPropertyDescriptor(b,\"world\")),g("
            "Object.getOwnPropertyDescriptor(b,0))})();",
            "https://www.napi.com/object.js", nullptr),
        NAPIOK);
}