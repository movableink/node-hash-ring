#include <stdlib.h>
#include <stdio.h>
#include <math.h>   // For floorf
#include <string.h> // For strlen
#include "md5.h"
#include "hash_ring.h"

void HashRing::hash_digest(const char *in, unsigned char out[16]) {
  md5_state_t md5_state;
  md5_init(&md5_state);
  md5_append(&md5_state, (unsigned char*) in, strlen(in));
  md5_finish(&md5_state, out);
};

unsigned int HashRing::hash_val(const char *in) {
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

HashRing::HashRing(std::map<std::string, uint32_t> weights, uint32_t precision) {
  uint32_t weight_total = 0;
  unsigned int num_buckets = weights.size();

  NodeInfo *node_list = new NodeInfo[num_buckets];

  int i = 0;
  // Construct the bucket list based on the weight hash
  for (const auto& weight: weights) {
    NodeInfo *node = &(node_list[i]);
    node->id = weight.first;
    node->weight = weight.second;
    node_list[i] = *node;
    weight_total += node->weight;
    i++;
  }

  Vpoint *vpoint_list = new Vpoint[num_buckets * 4 * precision];
  unsigned int j, k;
  int vpoint_idx = 0;
  for (j = 0; j < num_buckets; j++) {
    float percent = (float) node_list[j].weight / (float) weight_total;
    unsigned int num_replicas = floorf(percent * precision * (float) num_buckets);
    for (k = 0; k < num_replicas; k++) {
      char ss[30];
      sprintf(ss, "%s-%d", node_list[j].id.c_str(), k);
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

  weightMap = weights;
}

HashRing::~HashRing() {
    delete [] ring.vpoints;
}

std::vector<std::string> HashRing::GetBuckets() {
  std::vector<std::string> keys;
  keys.reserve(weightMap.size());
  for(auto const& weight: weightMap) {
    keys.push_back(weight.first);
  }

  return keys;
}

std::string HashRing::GetNode(std::string str) {
  unsigned int h = hash_val(str.c_str());

  int high = ring.num_points;
  Vpoint *vpoint_arr = ring.vpoints;
  int low = 0, mid;
  unsigned int mid_val, mid_val_1;

  while (true) {
    mid = (int) ( (low + high) / 2);
    if (mid == ring.num_points) {
      // We're at the end. Go to 0
      return vpoint_arr[0].id;
    }

    mid_val = vpoint_arr[mid].point;
    mid_val_1 = mid == 0 ? 0 : vpoint_arr[mid-1].point;

    if (h <= mid_val && h > mid_val_1) {
      return vpoint_arr[mid].id;
    }

    if (mid_val < h) {
      low = mid + 1;
    } else {
      high = mid - 1;
    }

    if (low > high) {
      return vpoint_arr[0].id;
    }
  }
}
