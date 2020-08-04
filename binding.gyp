{
  "targets": [
    {
      "target_name": "hash_ring",
      "cflags!": [ "--std=c++11 -fno-strict-aliasing" ],
      "cflags_cc!": [ "--std=c++11 -fno-strict-aliasing" ],
      "sources": [
        "./HashRingWrap.cc",
        "src/md5.cc",
        "src/hash_ring.cc"
      ],
      "defines": [ "NAPI_DISABLE_CPP_EXCEPTIONS" ],
      "dependencies": ["<!(node -p \"require('node-addon-api').gyp\")"],
      "include_dirs": [
        "<!@(node -p \"require('node-addon-api').include\")",
        "src/"
      ]
    }
  ]
}
