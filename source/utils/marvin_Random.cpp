#include "marvin_Random.h"
#include <enable_warnings.h>
namespace Audio {
    Random::Random(std::random_device& rd) : m_rng(rd()) {
    }
} // namespace Audio
#include <disable_warnings.h>