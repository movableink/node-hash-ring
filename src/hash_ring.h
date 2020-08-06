#ifndef HASH_RING_H_
#define HASH_RING_H_

#include <string>
#include <map>
#include <vector>

typedef int (*compfn)(const void*, const void*);

typedef struct {
  unsigned int point;
  const char* id;
} Vpoint;

typedef struct {
  int num_points;
  Vpoint *vpoints;
  int precision;
} Ring;

typedef struct {
  const char* id;
  int weight;
} NodeInfo;

class HashRing {

public:
  explicit HashRing(std::map<std::string, uint32_t> weights, uint32_t precision);
  ~HashRing();

  std::string GetNode(std::string str);
  std::vector<std::string> GetBuckets();

private:
  static void hash_digest(const char *in, unsigned char out[16]);
  static unsigned int hash_val(const char *in);
  static int vpoint_compare(Vpoint *a, Vpoint *b);
  Ring ring;
  std::map<std::string, uint32_t> weightMap;
};

#endif // HASH_RING_H_
