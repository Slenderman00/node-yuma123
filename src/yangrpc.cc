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
    #include "dlq.h"
    #include "rpc.h"
    #include "yangrpc.h"
}

namespace yangrpc {
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
    

    static int yangrpc_init_done = 0;

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

    void Connect(const FunctionCallbackInfo<Value>& args) {
        Isolate* isolate = args.GetIsolate();
        Local<Context> context = isolate->GetCurrentContext();

        if (args.Length() < 6) {
            ThrowError(isolate, "Wrong number of arguments");
            return;
        }

        char* server = ToCString(isolate, args[0]);
        int port = args[1]->Int32Value(context).FromJust();
        char* user = ToCString(isolate, args[2]);
        char* password = ToCString(isolate, args[3]);
        char* public_key = ToCString(isolate, args[4]);
        char* private_key = ToCString(isolate, args[5]);

        char* other_args = nullptr;
        if (args.Length() > 6 && !args[6]->IsNullOrUndefined()) {
            other_args = ToCString(isolate, args[6]);
        }

        if (yangrpc_init_done == 0) {
            status_t res = yangrpc_init(NULL);
            assert(res == NO_ERR);
            yangrpc_init_done = 1;
        }

        yangrpc_cb_ptr_t yangrpc_cb_ptr;
        status_t res = yangrpc_connect(server, port, user, password, public_key, private_key, other_args, &yangrpc_cb_ptr);

        free(server);
        free(user);
        free(password);
        free(public_key);
        free(private_key);
        if (other_args) free(other_args);
        
        if (res != NO_ERR) {
            args.GetReturnValue().SetNull();
            return;
        }

        args.GetReturnValue().Set(External::New(isolate, yangrpc_cb_ptr));
    }

    void Exec(const FunctionCallbackInfo<Value>& args) {
        Isolate* isolate = args.GetIsolate();

        if (args.Length() < 2) {
            ThrowError(isolate, "Wrong number of arguments");
            return;
        }

        void* yangrpc_cb_ptr = External::Cast(*args[0])->Value();
        void* rpc_val = External::Cast(*args[1])->Value();

        val_value_t* reply_val;
        status_t res = yangrpc_exec(
            (yangrpc_cb_ptr_t)yangrpc_cb_ptr, 
            (val_value_t*)rpc_val, 
            &reply_val
        );

        Local<Array> result = Array::New(isolate, 2);
        result->Set(isolate->GetCurrentContext(), 0, Number::New(isolate, res));
        
        if (reply_val) {
            result->Set(isolate->GetCurrentContext(), 1, External::New(isolate, reply_val));
        } else {
            result->Set(isolate->GetCurrentContext(), 1, Null(isolate));
        }
        
        args.GetReturnValue().Set(result);
    }

    void ParseCli(const FunctionCallbackInfo<Value>& args) {
        Isolate* isolate = args.GetIsolate();

        if (args.Length() < 2) {
            ThrowError(isolate, "Wrong number of arguments");
            return;
        }

        void* yangrpc_cb_ptr = External::Cast(*args[0])->Value();
        char* cmd = ToCString(isolate, args[1]);

        val_value_t* rpc_val;
        status_t res = yangrpc_parse_cli(
            (yangrpc_cb_ptr_t)yangrpc_cb_ptr, 
            cmd, 
            &rpc_val
        );

        free(cmd);

        Local<Array> result = Array::New(isolate, 2);
        result->Set(isolate->GetCurrentContext(), 0, Number::New(isolate, res));
        
        if (rpc_val) {
            result->Set(isolate->GetCurrentContext(), 1, External::New(isolate, rpc_val));
        } else {
            result->Set(isolate->GetCurrentContext(), 1, Null(isolate));
        }
        
        args.GetReturnValue().Set(result);
    }


    void Close(const FunctionCallbackInfo<Value>& args) {
        Isolate* isolate = args.GetIsolate();
        
        if (args.Length() < 1) {
            ThrowError(isolate, "Wrong number of arguments");
            return;
        }
        
        void* yangrpc_cb_ptr = External::Cast(*args[0])->Value();
        
        yangrpc_close((yangrpc_cb_ptr_t)yangrpc_cb_ptr);
        
        args.GetReturnValue().SetUndefined();
    }

    void InitYangRpc(Local<Object> exports) {
        NODE_SET_METHOD(exports, "connect", Connect);
        NODE_SET_METHOD(exports, "rpc", Exec);
        NODE_SET_METHOD(exports, "parse_cli", ParseCli);
        NODE_SET_METHOD(exports, "close", Close);
    }
}