#pragma once

#include <limits>

using DimensionCount = std::size_t; //< For dimension
using PointCount = std::size_t;     //< For point ids
using SampleType = double;          //< For samples

static constexpr DimensionCount    UNKNOWN_DIMENSION = std::numeric_limits<DimensionCount>::max() - 1;
static constexpr PointCount        UNKNOWN_POINTID   = std::numeric_limits<PointCount>::max() - 1;
