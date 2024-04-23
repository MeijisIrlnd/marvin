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
#include "marvin/library/marvin_EnableWarnings.h"
namespace marvin::utils {
    Random::Random(std::random_device& rd) : m_rng(rd()) {
    }
} // namespace marvin::utils
#include "marvin/library/marvin_DisableWarnings.h"
