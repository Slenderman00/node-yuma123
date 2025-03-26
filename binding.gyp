{
  "targets": [
    {
      "target_name": "yang",
      "sources": [
        "src/main.cc",
        "src/yangrpc.cc"
      ],
      "include_dirs": [
        "<!@(node -p \"require('node-addon-api').include\")",
        "/usr/include/yuma/ncx",
        "/usr/include/yuma/platform",
        "/usr/include/yuma/yangrpc"
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