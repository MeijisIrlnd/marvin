// ========================================================================================================
//  _______ _______ ______ ___ ___ _______ _______
// |   |   |   _   |   __ \   |   |_     _|    |  |
// |       |       |      <   |   |_|   |_|       |
// |__|_|__|___|___|___|__|\_____/|_______|__|____|
//
// This file is part of the Marvin open source library and is licensed under the terms of the MIT License.
//
// ========================================================================================================

#include "marvin/utils/marvin_Random.h"
#include "marvin/library/enable_warnings.h"
namespace Audio {
    Random::Random(std::random_device& rd) : m_rng(rd()) {
    }
} // namespace Audio
#include "marvin/library/disable_warnings.h"
