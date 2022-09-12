#pragma once

#include "Randomize.hh"

#include <algorithm>
#include <vector>
#include <limits>
#include <functional>

/**
 * @brief Enum type for disabled interactions 
 */
enum class DisabledBehavior
{
    AT_REST,    //< Particle will not move
    INFINITE,   //< 'Infinite' step length (projection to world border)
};

// Types of interaction
constexpr static inline unsigned char AT_REST = 0;
constexpr static inline unsigned char ALONG = 1;
constexpr static inline unsigned char POST = 2;

using DisabledStepFunction = std::function<double(unsigned char)>;

/**
 * @brief Maps DisabledBehavior enum to function (avoid having python lambdas)
 * 
 * @param behavior The selected behavior
 * 
 * @return A function implementing the selected behavior
 */
inline DisabledStepFunction GetFunctionFromEnum(DisabledBehavior behavior)
{
    switch(behavior)
    {
    case DisabledBehavior::INFINITE:
        return [](unsigned char) -> double { return std::numeric_limits<double>::max(); };
    case DisabledBehavior::AT_REST: // At rest is also default
    default:
        return [](unsigned char) -> double { return 0.0; };
    }
}

/**
 * @brief Wrapper class to inform about process limitation
 */
struct ProcessLimits
{
    inline ProcessLimits(std::size_t maxI, DisabledBehavior dB) : 
        maxInteractions(maxI), behaviorOnLimit(GetFunctionFromEnum(dB))
    { }

    inline ProcessLimits(std::size_t maxI, const DisabledStepFunction& func) : 
        maxInteractions(maxI), behaviorOnLimit(func)
    { }

    std::size_t maxInteractions;            //< Max number of process interaction
    DisabledStepFunction behaviorOnLimit;   //< What to when max number have been reached
};
