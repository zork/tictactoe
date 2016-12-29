////
// random.cpp
////

#include "base/util/random.h"

#include "base/basic_types.h"
#include "base/logging.h"
#include "base/platform.h"
#include "base/util/singleton.h"

#if OS_WIN
#include <wincrypt.h>
#elif OS_CHROME
#include "ppapi/cpp/module.h"
#else
#include "base/file/file_posix.h"
#endif

#include <stdlib.h>
#include <memory>

namespace {
class PlatformRandom : public Singleton<PlatformRandom> {
 public:
  void RandBytes(void* output, int size) {
#if OS_WIN
    HCRYPTPROV prov;
    if (CryptAcquireContext(&prov, NULL, NULL, PROV_RSA_FULL,
                            CRYPT_VERIFYCONTEXT | CRYPT_SILENT)) {
      long int li = 0;
      if (!CryptGenRandom(prov, size, (BYTE*)output)) {
        NOTREACHED();
      }
      if (!CryptReleaseContext(prov, 0)) {
        NOTREACHED();
      }
    } else {
      NOTREACHED();
    }
#elif OS_CHROME
    DCHECK_EQ(sizeof(int), 4);
    for (int i = 0; i < size / 4; ++i) {
      ((int*)output)[i] = rand_r(&seed_);
    }
    for (int i = 0; i < size % 4; ++i) {
      ((uint8_t*)output)[i + (size - (size % 4))] = rand_r(&seed_);
    }
#else
    size_t read = random_file_.Read(output, size);
    DCHECK_EQ(read, size);
#endif
  }

 private:
  friend class Singleton<PlatformRandom>;

  PlatformRandom() {
#if OS_WIN
#elif OS_CHROME
    seed_ = pp::Module::Get()->core()->GetTimeTicks() * 1000;
#else
    bool success = random_file_.Open("/dev/urandom", FilePosix::kModeRead);
    DCHECK(success);
#endif
  }

#if OS_WIN
#elif OS_CHROME
  unsigned int seed_;
#else
  FilePosix random_file_;
#endif
};
}

// static
void Random::RandBytes(void* output, int size) {
  PlatformRandom::Get()->RandBytes(output, size);
}

// static
Random* Random::get() {
  static std::unique_ptr<Random> g_random;
  if (!g_random) {
    uint32_t seed;
    RandBytes(&seed, sizeof(seed));
    g_random.reset(new Random(seed));
  }
  return g_random.get();
}

Random::Random(uint32_t seed) : index_(0) {
  Initialize(seed);
}

Random::~Random() {}

uint32_t Random::Next() {
  // Extract a tempered pseudorandom number based on the index-th value,
  // calling GenerateNumbers() every 624 numbers
  if (index_ == 0) {
    GenerateNumbers();
  }

  uint64_t y = mt_[index_];
  y = y ^ (y >> 11);
  y = y ^ ((y << 7) & 0x9d2c5680);
  y = y ^ ((y << 15) & 0xefc60000);
  y = y ^ (y >> 18);

  index_ = (index_ + 1) % 624;
  return (uint32_t)y;
}

double Random::NextDouble() {
  double random_value = ((double)Next()) / 0x100000000U;
  DCHECK_GE(random_value, 0.0);
  DCHECK_LT(random_value, 1.0);
  return random_value;
}

// private:
void Random::Initialize(uint32_t seed) {
  // Initialize the generator from a seed
  index_ = 0;
  mt_[0] = seed;
  for (int i = 1; i < 624; ++i) {
    // loop over each other element
    mt_[i] = 0xffffffff & (0x6c078965 * (mt_[i - 1] ^ (mt_[i - 1] >> 30)) + i);
  }
}

void Random::GenerateNumbers() {
  // Generate an array of 624 untempered numbers
  for (int i = 0; i < 624; ++i) {
    uint64_t y = (mt_[i] & 0x80000000) +  // bit 31 (32nd bit) of MT[i]
                 (mt_[(i + 1) % 624] &
                  0x7fffffff);  // bits 0-30 (first 31 bits) of MT[...]
    mt_[i] = mt_[(i + 397) % 624] ^ (uint32_t)(y >> 1);
    if (y % 2) {
      // y is odd
      mt_[i] = mt_[i] ^ 0x9908b0df;
    }
  }
}
