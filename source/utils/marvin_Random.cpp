#include "marvin/utils/marvin_Random.h"
#include "marvin/library/enable_warnings.h"
namespace Audio {
    Random::Random(std::random_device& rd) : m_rng(rd()) {
    }
} // namespace Audio
#include "marvin/library/disable_warnings.h"
