#include <node.h>
#include <v8.h>
#include <assert.h>
#include <string.h>
#include <cstdlib>
#include <cstddef>

extern "C" {
    #include "ncx.h"
    #include "ncxmod.h"
    #include "ncxtypes.h"
    #include "status.h"
    #include "procdefs.h"
    #include "agt.h"
    #include "agt_cb.h"
    #include "agt_timer.h"
    #include "agt_util.h"
    #include "agt_rpc.h"
    #include "dlq.h"
    #include "rpc.h"
}

namespace yuma {
    using v8::Context;
    using v8::Function;
    using v8::FunctionCallbackInfo;
    using v8::Isolate;
    using v8::Local;
    using v8::MaybeLocal;
    using v8::Object;
    using v8::String;
    using v8::Value;
    using v8::Exception;
    using v8::Null;
    using v8::External;
    using v8::Number;
    using v8::Array;
    using v8::Integer;
    using v8::Boolean;

    char* ToCString(Isolate* isolate, Local<Value> value) {
        if (value->IsNullOrUndefined()) {
            return nullptr;
        }

        String::Utf8Value utf8Value(isolate, value);
        if (*utf8Value == nullptr) {
            return nullptr;
        }
          
        return strdup(*utf8Value);
    }

    void ThrowError(Isolate* isolate, const char* message) {
        isolate->ThrowException(Exception::Error(
            String::NewFromUtf8(isolate, message).ToLocalChecked()
        ));
    }

    void Init(const FunctionCallbackInfo<Value>& args) {
        Isolate* isolate = args.GetIsolate();

        status_t res;
        int argc = 1;
        char* argv[] = {"hello"};
        char buff[] = "yuma for node";
        res = ncx_init(FALSE, LOG_DEBUG_INFO, TRUE, buff, argc, argv);

        res = ncxmod_load_module(NCXMOD_YUMA_NETCONF, NULL, NULL, NULL);
        if (res != 0) {
            ThrowError(isolate, "Failed to initialize yuma: NCXMOD_YUMA_NETCONF");
        }

        res = ncxmod_load_module(NCXMOD_NETCONFD, NULL, NULL, NULL);

        if (res != 0) {
            ThrowError(isolate, "Failed to initialize yuma: NCXMOD_NETCONFD");
        }

        args.GetReturnValue().Set(res);
    }

    void SchemaModuleLoad(const FunctionCallbackInfo<Value>& args) {
        Isolate* isolate = args.GetIsolate();

        status_t res;
        ncx_module_t* module;
        char* module_name = NULL;
        if (args.Length() != 0) {
            module_name = ToCString(isolate, args[0]);
        }

        res = ncxmod_load_module((const xmlChar*)module_name, NULL, NULL, &module);

        if(res != 0) {
            char *error;
            if(module_name) {
                sprintf(error, "Failed to load module: %s", module_name);
            } else {
                sprintf(error, "Failed to load module");
            }
            ThrowError(isolate, error);
        }

        Local<Array> result = Array::New(isolate, 2);
        result->Set(isolate->GetCurrentContext(), 0, Number::New(isolate, res));

        if(module) {
            result->Set(isolate->GetCurrentContext(), 1, External::New(isolate, module));
        } else {
            result->Set(isolate->GetCurrentContext(), 1, Null(isolate));
        }

        args.GetReturnValue().Set(result);

        free(module_name);
    }

    void CFGload(const FunctionCallbackInfo<Value>& args) {
        Isolate* Isolate = args.GetIsolate();

        status_t res;
        
    }

    void InitYuma(Local<Object> exports) {
        NODE_SET_METHOD(exports, "init", Init);
        NODE_SET_METHOD(exports, "schema_module_load", SchemaModuleLoad);
    }
 
}