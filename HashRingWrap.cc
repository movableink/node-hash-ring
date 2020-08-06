#include "./HashRingWrap.h"
#include <map>

Napi::FunctionReference HashRingWrap::constructor;

Napi::Object HashRingWrap::Init(Napi::Env env, Napi::Object exports) {
  Napi::HandleScope scope(env);

  Napi::Function func = DefineClass(env, "HashRing", {
      InstanceMethod("getNode", &HashRingWrap::GetNode),
      InstanceMethod("getBuckets", &HashRingWrap::GetBuckets)
    });

  constructor = Napi::Persistent(func);
  constructor.SuppressDestruct();

  exports.Set("HashRing", func);
  return exports;
}

HashRingWrap::HashRingWrap(const Napi::CallbackInfo& info) : Napi::ObjectWrap<HashRingWrap>(info) {
  const Napi::Env env = info.Env();

  if (info.Length() < 1) {
    Napi::TypeError::New(env, "Wrong number of arguments").ThrowAsJavaScriptException();
    return;
  }

  if (!info[0].IsObject()) {
    Napi::TypeError::New(env, "You must pass bucket weights as an object (property name is bucket name, property value is weight)").ThrowAsJavaScriptException();
    return;
  }

  Napi::Object weights = info[0].As<Napi::Object>();

  Napi::Array nodeNames = weights.GetPropertyNames();
  unsigned int numBuckets = nodeNames.Length();

  std::map<std::string, uint32_t> weightMap;

  // Ensure that weight keys are strings and weight values are numbers
  for (unsigned int i = 0; i < numBuckets; i++) {
    Napi::Value nodeName = nodeNames.Get(i);
    Napi::Value nodeValue = weights.Get(nodeName);

    if (!nodeName.IsString() || !nodeValue.IsNumber()) {
      Napi::TypeError::New(env, "You must pass bucket weights as an object (property name is bucket name, property value is weight)").ThrowAsJavaScriptException();
      return;
    }

    weightMap.insert({ nodeName.ToString().Utf8Value(), nodeValue.ToNumber().Uint32Value() });
  }

  uint32_t precision = HashRingWrap::DEFAULT_PRECISION;

  if (info.Length() > 1) {
    if (info[1].IsNumber()) {
      precision = info[1].As<Napi::Number>().Uint32Value();
    }

    if (precision <= 0) {
      Napi::TypeError::New(env, "Precision must be greater than 0").ThrowAsJavaScriptException();
      return;
    }
  }

  this->hashring = new HashRing(weightMap, precision);
}

Napi::Value HashRingWrap::GetNode(const Napi::CallbackInfo& info) {
  const Napi::Env env = info.Env();

  if (info.Length() != 1) {
    Napi::TypeError::New(env, "Wrong number of arguments").ThrowAsJavaScriptException();
    return env.Null();
  }

  if (!info[0].IsString()) {
    Napi::TypeError::New(env, "Only strings are supported").ThrowAsJavaScriptException();
    return env.Null();
  }

  std::string str = info[0].As<Napi::String>().Utf8Value();

  Napi::String result = Napi::String::New(env, this->hashring->GetNode(str));
  return result;
}

Napi::Value HashRingWrap::GetBuckets(const Napi::CallbackInfo& info) {
  const Napi::Env env = info.Env();

  auto weightKeys = this->hashring->GetBuckets();
  Napi::Array keys = Napi::Array::New(env, weightKeys.size());

  int i = 0;
  for (std::string key : weightKeys) {
    keys.Set(i++, Napi::String::New(env, key));
  }

  return keys;
}

Napi::Object Init(Napi::Env env, Napi::Object exports) {
  HashRingWrap::Init(env, exports);
  return exports;
}

HashRingWrap::~HashRingWrap() {
  if (this->hashring != nullptr) {
    delete this->hashring;
  }
}

NODE_API_MODULE(NODE_GYP_MODULE_NAME, Init);
