#include <node.h>
#include <v8.h>
#include <cstdlib>
#include <cstddef>

namespace yangrpc {
  void InitYangRpc(v8::Local<v8::Object> exports);
}

namespace yuma {
  void InitYuma(v8::Local<v8::Object> exports);
}

void InitAll(v8::Local<v8::Object> exports) {
  v8::Isolate* isolate = v8::Isolate::GetCurrent();
  v8::Local<v8::Context> context = isolate->GetCurrentContext();
  
  v8::Local<v8::Object> yangrpc_obj = v8::Object::New(isolate);
  v8::Local<v8::Object> yuma_obj = v8::Object::New(isolate);
  
  yangrpc::InitYangRpc(yangrpc_obj);
  yuma::InitYuma(yuma_obj);
  
  exports->Set(context,
    v8::String::NewFromUtf8(isolate, "yangrpc").ToLocalChecked(),
    yangrpc_obj
  );

  exports->Set(context,
    v8::String::NewFromUtf8(isolate, "yuma").ToLocalChecked(),
    yuma_obj
  );
  
}

NODE_MODULE(NODE_GYP_MODULE_NAME, InitAll)