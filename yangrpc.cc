#include <node.h>
#include <v8.h>
#include "ncx.h"
#include "ncxmod.h"
#include "ncxtypes.h"
#include "status.h"
#include "procdefs.h"
#include "dlq.h"
#include "rpc.h"
#include "yangrpc.h"

namespace yangrpc {
    using v8::FunctionCallbackInfo;
    using v8::Isolate;
    using v8::Local;
    using v8::Object;
    using v8::String;
    using v8::Value;
    using v8::Exception;

    static int yangrpc_init_done = 0;

    void Connect(const FunctionCallbackInfo<Value>& args) {
        Isolate* isolate = args.GetIsolate();

        if (args.Length() < 6) {
            isolate->ThrowException(Exception::TypeError(String::NewFromUtf8(isolate, "Wrong number of arguments").ToLocalChecked()));
            return;
        }

        //Extracting arguments
        v8::String::Utf8Value server(isolate, args[0]);
        int port = args[1]->Int32Value(isolate->GetCurrentContext()).FromJust();
        v8::String::Utf8Value user(isolate, args[2]);
        v8::String::Utf8Value password(isolate, args[3]);
        v8::String::Utf8Value private_key(isolate, args[4]);
        v8::String::Utf8Value public_key(isolate, args[5]);
        v8::String::Utf8Value other_args(isolate, args[6]);
    }
}