#ifndef HASH_RING_WRAP_H
#define HASH_RING_WRAP_H

#include <napi.h>
#include "hash_ring.h"

class HashRingWrap : public Napi::ObjectWrap<HashRingWrap> {
 public:
  static Napi::Object Init(Napi::Env env, Napi::Object exports);
  HashRingWrap(const Napi::CallbackInfo& info);
  ~HashRingWrap();

 private:
  static Napi::FunctionReference constructor;

  Napi::Value GetNode(const Napi::CallbackInfo& info);
  Napi::Value GetBuckets(const Napi::CallbackInfo& info);

  HashRing *hashring = nullptr;

  static const uint32_t DEFAULT_PRECISION = 40;
};

#endif
