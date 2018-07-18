#include <nan.h>
#include "hash_ring.h"

using namespace v8;

void initAll (Handle<Object> exports) {
    HashRing::Initialize(exports);
}

NODE_MODULE(hash_ring, initAll);
