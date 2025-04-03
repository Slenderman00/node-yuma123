{
  "targets": [
    {
      "target_name": "yuma123",
      "sources": [
        "src/main.cc",
        "src/yangrpc.cc",
        "src/yuma.cc"
      ],
      "include_dirs": [
        "<!@(node -p \"require('node-addon-api').include\")",
        "/usr/include/yuma/ncx",
        "/usr/include/yuma/platform",
        "/usr/include/yuma/yangrpc",
        "/usr/include/yuma/agt"
      ],
      "libraries": [
        "-lyangrpc",
        "-lyumancx"
      ],
      "cflags!": [ "-fno-exceptions" ],
      "cflags_cc!": [ "-fno-exceptions" ],
      "defines": [ "NAPI_DISABLE_CPP_EXCEPTIONS" ]
    }
  ]
}