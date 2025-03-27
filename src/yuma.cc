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
        if (args.Length() < 0) {
            module_name = ToCString(isolate, args[0]);
        }

        res = ncxmod_load_module((const xmlChar*)module_name, NULL, NULL, &module);

        if(res != 0) {
            char *error = (char *) malloc(strlen("Failed to load module: ") + strlen(module_name) + 1);
            if(module_name) {
                sprintf(error, "Failed to load module: %s", module_name);
            } else {
                sprintf(error, "Failed to load module");
            }
            ThrowError(isolate, error);
            free(error);
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

    void CfgLoad(const FunctionCallbackInfo<Value>& args) {
        Isolate* isolate = args.GetIsolate();

        status_t res;
        int argc = 1;
        char* argv[] = {"hello"};
        char* cfg_filename;
        char* startup_arg;
        boolean showver;
        help_mode_t showhelpmode;
        agt_profile_t *profile;
        cfg_template_t *runningcfg;

        if(args.Length() < 1) {
            cfg_filename = ToCString(isolate, args[0]);
        } else {
            ThrowError(isolate, "Wrong number of arguments");
        }

        startup_arg = (char *) malloc(strlen(cfg_filename) + 1);
        sprintf(startup_arg, "%s", cfg_filename);

        res = agt_init1(argc, argv, &showver, &showhelpmode);
        
        if(res != 0) {
            ThrowError(isolate, "Agt intialization failed");
        }

        if (showver || showhelpmode != HELP_MODE_NONE) {
            printf("ver 1.0\n");
        }

        profile = agt_get_profile();
        res = ncxmod_load_module(NCXMOD_WITH_DEFAULTS, NULL, &profile->agt_savedevQ, NULL);

        if(res != 0) {
            ThrowError(isolate, "Failed to load module: NCXMOD_WITH_DEFAULTS");
        }

        profile->agt_has_startup = TRUE;
        profile->agt_startup = (xmlChar *) startup_arg;

        res = agt_init2();

        if(res != 0) {
            ThrowError(isolate, "Agt2 intialization failed");
        }

        runningcfg = cfg_get_config(NCX_CFG_RUNNING);

        if(runningcfg == NULL) {
            ThrowError(isolate, "Running cfg is NULL");
        }

        Local<Array> result = Array::New(isolate, 2);
        result->Set(isolate->GetCurrentContext(), 0, Number::New(isolate, res));
        result->Set(isolate->GetCurrentContext(), 1, External::New(isolate, runningcfg->root));

        args.GetReturnValue().Set(result);

        free(startup_arg);
    }

    void ValFindChild(const FunctionCallbackInfo<Value>& args) {
        Isolate* isolate = args.GetIsolate();

        int res;
        val_value_t* child_val;

        if(args.Length() < 3) {
            ThrowError(isolate, "Wrong number of arguments");
        }

        val_value_t* parent_val = (val_value_t *) External::Cast(*args[0])->Value();
        char* _namespace = ToCString(isolate, args[1]);
        char* name = ToCString(isolate, args[2]); 

        child_val = val_find_child(parent_val, (xmlChar *) _namespace, (xmlChar *) name);

        if(child_val==NULL) {
            res=-1;
        } else {
            res=0;
        }

        Local<Array> result = Array::New(isolate, 2);
        result->Set(isolate->GetCurrentContext(), 0, Number::New(isolate, res));
        result->Set(isolate->GetCurrentContext(), 1, External::New(isolate, child_val));

        free(_namespace);
        free(name);

        args.GetReturnValue().Set(result);
    }

    void ValString(const FunctionCallbackInfo<Value>& args) {
        Isolate* isolate = args.GetIsolate();

        if(args.Length() < 1) {
            ThrowError(isolate, "Wrong number of arguments");
        }

        val_value_t* val = (val_value_t *) External::Cast(*args[0])->Value();
    
        args.GetReturnValue().Set(External::New(isolate, VAL_STRING(val)));
    }

    void ValDumpValue(const FunctionCallbackInfo<Value>& args) {
        Isolate* isolate = args.GetIsolate();

        int res;

        if(args.Length() < 2) {
            ThrowError(isolate, "Wrong number of arguments");
        }

        val_value_t* val = (val_value_t *) External::Cast(*args[0])->Value();
        int flag = args[1]->Int32Value(isolate->GetCurrentContext()).FromJust();
        val_dump_value(val, flag);

        args.GetReturnValue().SetNull();
    }

    void ValMakeSerializedString(const FunctionCallbackInfo<Value>& args) {
        Isolate* isolate = args.GetIsolate();

        status_t res;
        char* str;

        if(args.Length() < 2) {
            ThrowError(isolate, "Wrong number of argumens");
        }

        val_value_t* val = (val_value_t *) External::Cast(*args[0])->Value();
        ncx_display_mode_t* mode_ptr = (ncx_display_mode_t *) External::Cast(*args[1])->Value();

        res = val_make_serialized_string(val, *mode_ptr, (xmlChar**) &str);

        Local<Array> result = Array::New(isolate, 2);
        result->Set(isolate->GetCurrentContext(), 0, Number::New(isolate, res));
        if (str) {
            result->Set(isolate->GetCurrentContext(), 1, String::NewFromUtf8(isolate, str).ToLocalChecked());
            free(str);
        } else {
            result->Set(isolate->GetCurrentContext(), 1, Null(isolate));
        }

        args.GetReturnValue().Set(result);
    }
    
    void ValFreeValue(const FunctionCallbackInfo<Value>& args) {
        Isolate* isolate = args.GetIsolate();

        if(args.Length() < 1) {
            ThrowError(isolate, "Wrong number of argumens");
        }

        val_value_t* val = (val_value_t *) External::Cast(*args[0])->Value();
        val_free_value(val);

        args.GetReturnValue().SetNull();
    }

    void InitYuma(Local<Object> exports) {
        NODE_SET_METHOD(exports, "init", Init);
        NODE_SET_METHOD(exports, "schema_module_load", SchemaModuleLoad);
        NODE_SET_METHOD(exports, "cfg_load", CfgLoad);
        NODE_SET_METHOD(exports, "val_find_child", ValFindChild);
        NODE_SET_METHOD(exports, "val_string", ValString);
        NODE_SET_METHOD(exports, "val_dump_value", ValDumpValue);
        NODE_SET_METHOD(exports, "val_make_serialized_string", ValMakeSerializedString);
        NODE_SET_METHOD(exports, "val_free_value", ValFreeValue);
    }
 
}