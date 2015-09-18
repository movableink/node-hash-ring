#include <stdlib.h>
#include <stdio.h>
#include <math.h>   // For floorf
#include <string.h> // For strlen
#include "md5.h"
#include <nan.h>
#include "hash_ring.h"
#include <iostream>
using namespace std;
using namespace v8;
using namespace node;
using namespace Nan;

void HashRing::hash_digest(char *in, unsigned char out[16]) {
    md5_state_t md5_state;
    md5_init(&md5_state);
    md5_append(&md5_state, (unsigned char*) in, strlen(in));
    md5_finish(&md5_state, out);
};

unsigned int HashRing::hash_val(char *in) {
    unsigned char digest[16];
    hash_digest(in, digest);
    return (unsigned int) (
                           (digest[3] << 24) |
                           (digest[2] << 16) |
                           (digest[1] << 8) |
                           digest[0]
                           );
}

int HashRing::vpoint_compare(Vpoint *a, Vpoint *b) {
    return (a->point < b->point) ? -1 : ((a->point > b->point) ? 1 : 0);
}

NAN_MODULE_INIT(HashRing::Initialize) {
    Local<FunctionTemplate> tpl = Nan::New<FunctionTemplate>(New);
    tpl->SetClassName(Nan::New<v8::String>("HashRing").ToLocalChecked());
    tpl->InstanceTemplate()->SetInternalFieldCount(1);

    SetPrototypeMethod(tpl, "getNode", GetNode);

    Set(target, Nan::New<v8::String>("HashRing").ToLocalChecked(), tpl->GetFunction());
}

HashRing::HashRing(Local<Object> weight_hash) : Nan::ObjectWrap() {
    Local<v8::Array> node_names = Nan::GetPropertyNames(weight_hash).ToLocalChecked();
    Local<String> node_name;
    uint32_t weight_total = 0;
    unsigned int num_servers = node_names->Length();

    NodeInfo *node_list = new NodeInfo[num_servers];
    // Construct the server list based on the weight hash
    for (unsigned int i = 0; i < num_servers; i++) {
        NodeInfo *node = &(node_list[i]);
        node_name = Nan::Get(node_names, i).ToLocalChecked()->ToString();
        String::Utf8Value utfVal(node_name);
        node->id = new char[utfVal.length()];
        strcpy(node->id, *utfVal);
        node->weight = Nan::Get(weight_hash, node_name).ToLocalChecked()->Uint32Value();
        node_list[i] = *node;
        weight_total += node->weight;
    }

    Vpoint *vpoint_list = new Vpoint[num_servers * 160];
    unsigned int j, k;
    int vpoint_idx = 0;
    for (j = 0; j < num_servers; j++) {
        float percent = (float) node_list[j].weight / (float) weight_total;
        unsigned int num_replicas = floorf(percent * 40.0 * (float) num_servers);
        for (k = 0; k < num_replicas; k++) {
            char ss[30];
            sprintf(ss, "%s-%d", node_list[j].id, k);
            unsigned char digest[16];
            hash_digest(ss, digest);
            int m;
            for (m = 0; m < 4; m++) {
                vpoint_list[vpoint_idx].point = (digest[3 + m*4] << 24) |
                    (digest[2 + m*4] << 16) |
                    (digest[1 + m*4] << 8) |
                    digest[m*4];
                vpoint_list[vpoint_idx].id = node_list[j].id;
                vpoint_idx++;
            }
        }
    }
    delete [] node_list;
    qsort((void*) vpoint_list, vpoint_idx, sizeof(Vpoint), (compfn) vpoint_compare);

    ring.vpoints = vpoint_list;
    ring.num_points = vpoint_idx;
}

HashRing::~HashRing(){
    delete [] ring.vpoints;
}

NAN_METHOD(HashRing::New) {
    if (info.IsConstructCall() && info.Length() >= 1 && info[0]->IsObject()) {
        Local<Object> weight_hash = info[0]->ToObject();
        HashRing* hash_ring = new HashRing(weight_hash);

        hash_ring->Wrap(info.This());
        info.GetReturnValue().Set(info.This());
    } else {
        Nan::ThrowTypeError("Bad argument");
    }
}

NAN_METHOD(HashRing::GetNode) {
    HashRing *hash_ring = Nan::ObjectWrap::Unwrap<HashRing>(info.This());
    Ring* ring = &(hash_ring->ring);

    Local<String> str = info[0]->ToString();
    String::Utf8Value utfVal(str);
    char* key = *utfVal;
    unsigned int h = hash_val(key);

    int high = ring->num_points;
    Vpoint *vpoint_arr = ring->vpoints;
    int low = 0, mid;
    unsigned int mid_val, mid_val_1;
    while (true)
        {
            mid = (int) ( (low + high) / 2);
            if (mid == ring->num_points) {
                // We're at the end. Go to 0
                info.GetReturnValue().Set(Nan::New<v8::String>(vpoint_arr[0].id).ToLocalChecked());
                break;
            }

            mid_val = vpoint_arr[mid].point;
            mid_val_1 = mid == 0 ? 0 : vpoint_arr[mid-1].point;
            if (h <= mid_val && h > mid_val_1) {
                info.GetReturnValue().Set(Nan::New<v8::String>(vpoint_arr[mid].id).ToLocalChecked());
                break;
            }
            if (mid_val < h)
                low = mid + 1;
            else
                high = mid - 1;
            if (low > high) {
                info.GetReturnValue().Set(Nan::New<v8::String>(vpoint_arr[0].id).ToLocalChecked());
                break;
            }
        }
}
