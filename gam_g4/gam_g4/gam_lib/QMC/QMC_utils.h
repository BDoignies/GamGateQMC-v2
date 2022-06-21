#pragma once

#include <limits>

using DimensionCount = std::size_t; //< For dimension
using PointCount = std::size_t;     //< For point ids
using SampleType = double;          //< For samples

// -10000: Dimension and point count may be added, even in case of 'UNKNOWN_DIMENSION'. To avoid every check
// a 10000 number range is allowed so that points can be added safely
static constexpr DimensionCount    UNKNOWN_DIMENSION = std::numeric_limits<DimensionCount>::max() - 10000;
static constexpr PointCount        UNKNOWN_POINTID   = std::numeric_limits<PointCount>::max() - 10000;

#include <source_location>
#include <string>

inline void ParseSourceLocation(const std::source_location& location, std::string& className, std::string& funcName)
{
    // Split function signature into class name and function name, ignoring templates names, parameters and return type

    bool isSpace = false;
    bool isScope = false;

    std::size_t beginClassName = 0;
    std::size_t endClassName = 0;
    std::size_t beginFuncName = 0;
    std::size_t endFuncName = 0;

    const std::string functionName = location.function_name();
    for (std::size_t i = 0; i < functionName.size(); i++)
    {
        if (functionName[i] == ' ')
        {
            isSpace = true;
            beginClassName = i + 1;
        }
        // Space separates return type from class name
        // note that if multiple are present in class name (namespaces)
        // name is properly set (including namespace)
        else if (functionName[i] == ':' && isSpace)
        {
            isScope = true;
            beginFuncName = i + 2;
            endClassName = i;
            i++;
        }
        // '(' is the begining of argument and the end of what we wanted
        // to parse
        else if (functionName[i] == '(')
        {
            endFuncName = i;
            break;
        }
    }

    if (!isScope)
    {
        beginFuncName = beginClassName;
        endClassName  = beginClassName;
    }

    className = std::string(functionName, beginClassName, endClassName - beginClassName);
    funcName  = std::string(functionName, beginFuncName , endFuncName - beginFuncName);
}