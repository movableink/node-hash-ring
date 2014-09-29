#include <node.h>
#include <nan.h>
#include "hash_ring.h"

extern "C"
void init (v8::Handle<v8::Object> target) {
    HashRing::Initialize(target);
}

NODE_MODULE(hash_ring, init);
