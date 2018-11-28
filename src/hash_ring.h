#ifndef HASH_RING_H_
#define HASH_RING_H_

#include <nan.h>

typedef int (*compfn)(const void*, const void*);

typedef struct {
    unsigned int point;
    char* id;
} Vpoint;

typedef struct {
    int num_points;
    Vpoint *vpoints;
    int precision;
} Ring;

typedef struct {
    char* id;
    int weight;
} NodeInfo;

class HashRing : public Nan::ObjectWrap {

    Ring ring;

  public:
    explicit HashRing(v8::Local<v8::Object> weight_hash, uint32_t precision);
    ~HashRing();

    static NAN_MODULE_INIT(Initialize);

    static NAN_METHOD(New);
    static NAN_METHOD(GetNode);

  private:
    static void hash_digest(char *in, unsigned char out[16]);
    static unsigned int hash_val(char *in);
    static int vpoint_compare(Vpoint *a, Vpoint *b);
};

#endif // HASH_RING_H_
