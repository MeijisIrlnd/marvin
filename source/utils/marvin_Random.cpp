#include "marvin_Random.h"
#include "library/enable_warnings.h"
namespace Audio {
    Random::Random(std::random_device& rd) : m_rng(rd()) {
    }
} // namespace Audio
#include "library/disable_warnings.h"
