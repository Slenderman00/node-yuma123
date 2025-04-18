#include <node.h>
#include <v8.h>
#include <assert.h>
#include <string.h>
#include <cstdlib>
#include <cstddef>
#include <uv.h>

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
    using v8::Promise;
    using v8::Persistent;
    using v8::HandleScope;
    
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

        Local<Array> result = Array::New(isolate, 2);
        result->Set(isolate->GetCurrentContext(), 0, Number::New(isolate, res));
        
        if (yangrpc_cb_ptr) {
            result->Set(isolate->GetCurrentContext(), 1, External::New(isolate, yangrpc_cb_ptr));
        } else {
            result->Set(isolate->GetCurrentContext(), 1, Null(isolate));
        }
        
        args.GetReturnValue().Set(result);
    }

    struct ConnectBaton {
        char* server;
        int port;
        char* user;
        char* password;
        char* public_key;
        char* private_key;
        char* other_args;
        yangrpc_cb_ptr_t yangrpc_cb_ptr;
        status_t res;
        Persistent<Promise::Resolver> resolver;
        Isolate* isolate;
    };

    static void ConnectWork(uv_work_t* req) {
        ConnectBaton* baton = static_cast<ConnectBaton*>(req->data);
        
        baton->res = yangrpc_connect(
            baton->server, 
            baton->port, 
            baton->user, 
            baton->password, 
            baton->public_key, 
            baton->private_key, 
            baton->other_args, 
            &baton->yangrpc_cb_ptr
        );
    }

    static void ConnectAfter(uv_work_t* req, int status) {
        Isolate* isolate = Isolate::GetCurrent();
        HandleScope scope(isolate);
        
        ConnectBaton* baton = static_cast<ConnectBaton*>(req->data);
        
        Local<Promise::Resolver> resolver = Local<Promise::Resolver>::New(
            isolate, baton->resolver);
        Local<Context> context = isolate->GetCurrentContext();
        
        if (baton->res == NO_ERR && baton->yangrpc_cb_ptr) {
            resolver->Resolve(context, External::New(isolate, baton->yangrpc_cb_ptr));
        } else {
            Local<Object> errorObj = Object::New(isolate);
            errorObj->Set(
                context, 
                String::NewFromUtf8(isolate, "code").ToLocalChecked(),
                Number::New(isolate, baton->res)
            );
            resolver->Reject(context, errorObj);
        }
        
        baton->resolver.Reset();
        free(baton->server);
        free(baton->user);
        free(baton->password);
        if (baton->public_key) free(baton->public_key);
        if (baton->private_key) free(baton->private_key);
        if (baton->other_args) free(baton->other_args);
        delete baton;
        delete req;
    }

    void AsyncConnect(const FunctionCallbackInfo<Value>& args) {
        Isolate* isolate = args.GetIsolate();
        Local<Context> context = isolate->GetCurrentContext();

        if (args.Length() < 6) {
            ThrowError(isolate, "Wrong number of arguments");
            return;
        }

        Local<Promise::Resolver> resolver = Promise::Resolver::New(context).ToLocalChecked();
        Local<Promise> promise = resolver->GetPromise();
        args.GetReturnValue().Set(promise);
        
        ConnectBaton* baton = new ConnectBaton();
        baton->server = ToCString(isolate, args[0]);
        baton->port = args[1]->Int32Value(context).FromJust();
        baton->user = ToCString(isolate, args[2]);
        baton->password = ToCString(isolate, args[3]);
        baton->public_key = ToCString(isolate, args[4]);
        baton->private_key = ToCString(isolate, args[5]);
        
        baton->other_args = nullptr;
        if (args.Length() > 6 && !args[6]->IsNullOrUndefined()) {
            baton->other_args = ToCString(isolate, args[6]);
        }
        
        baton->yangrpc_cb_ptr = nullptr;
        baton->isolate = isolate;
        baton->resolver.Reset(isolate, resolver);
        
        if (yangrpc_init_done == 0) {
            status_t res = yangrpc_init(NULL);
            assert(res == NO_ERR);
            yangrpc_init_done = 1;
        }
        
        uv_work_t* req = new uv_work_t();
        req->data = baton;
        
        uv_queue_work(
            uv_default_loop(),
            req,
            ConnectWork,
            ConnectAfter
        );
    }

    void Rpc(const FunctionCallbackInfo<Value>& args) {
        Isolate* isolate = args.GetIsolate();

        if (args.Length() < 2) {
            ThrowError(isolate, "Wrong number of arguments");
            return;
        }

        void* yangrpc_cb_ptr = External::Cast(*args[0])->Value();
        void* rpc_val = External::Cast(*args[1])->Value();

        val_value_t* reply_val;
        status_t res = yangrpc_exec((yangrpc_cb_ptr_t)yangrpc_cb_ptr, (val_value_t*)rpc_val, &reply_val);

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

        if(!yangrpc_init_done) {
            ThrowError(isolate, "Connection already closed");
            return;
        }
        
        if (args.Length() < 1) {
            ThrowError(isolate, "Wrong number of arguments");
            return;
        }
        
        void* yangrpc_cb_ptr = External::Cast(*args[0])->Value();
        
        yangrpc_close((yangrpc_cb_ptr_t)yangrpc_cb_ptr);

        yangrpc_init_done = 0;
        
        args.GetReturnValue().SetUndefined();
    }

    void InitYangRpc(Local<Object> exports) {
        NODE_SET_METHOD(exports, "connect", Connect);
        NODE_SET_METHOD(exports, "async_connect", AsyncConnect);
        NODE_SET_METHOD(exports, "rpc", Rpc);
        NODE_SET_METHOD(exports, "parse_cli", ParseCli);
        NODE_SET_METHOD(exports, "close", Close);
    }
}