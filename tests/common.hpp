#ifndef _VOXOMAP_TEST_COMMON_HPP_
#define _VOXOMAP_TEST_COMMON_HPP_

#include <set>
#include <vector>
#include <random>
#include "../utils/Vector3.hpp"

#ifndef _MSC_VER
#include <cxxabi.h>
#endif

#ifdef _WIN32
#include "windows.h"
#include "psapi.h"
#else
#include <unistd.h>
#include <sys/resource.h>
#endif

namespace voxomap
{
namespace test
{

size_t computeMemoryUsed()
{
#ifdef _WIN32
    PROCESS_MEMORY_COUNTERS_EX pmc;
    GetProcessMemoryInfo(GetCurrentProcess(), (PROCESS_MEMORY_COUNTERS*)&pmc, sizeof(pmc));
    return pmc.PrivateUsage;
#else
    struct rusage rusage;
    getrusage(RUSAGE_SELF, &rusage);
    #if defined(__APPLE__) && defined(__MACH__)
    return (size_t)rusage.ru_maxrss;
    #else
    return (size_t)(rusage.ru_maxrss * 1024L);
    #endif
#endif
}

/*! \struct voxel
    \test
    \brief Voxel structure use for benchmarks
*/
struct voxel
{
    voxel() = default;
    voxel(voxel const& other) = default;
    voxel(voxel&& other) = default;
    voxel(int64_t value) : value(value) {}
    voxel& operator=(voxel const& other) = default;
    voxel& operator=(voxel&& other) = default;
    bool mergeSide(voxel const& other) const { return true; }

    int64_t value = 0;
};

struct Data
{
    Data(int x, int y, int z, int64_t value)
        : x(x), y(y), z(z), value(value)
    {}
    Data(Data const& other) = default;

    int x;
    int y;
    int z;
    int64_t value;
};

std::vector<Data> gTestValues;

void initGlobalValues(int nbVoxel)
{
    if (!gTestValues.empty())
        return;

    std::mt19937 r(42);
    std::set<voxomap::Vector3I> setValues;

    for (size_t i = 0; i < nbVoxel; ++i)
    {
        int x = r() % 512 - 256;
        int y = r() % 512 - 256;
        int z = r() % 512 - 256;
        auto pair = setValues.emplace(x, y, z);
        if (pair.second == false)
        {
            --i;
            continue;
        }

        gTestValues.emplace_back(x, y, z, r());
    }
}

void eraseAllSubStr(std::string& mainStr, std::string const& toErase)
{
    for (size_t pos = mainStr.find(toErase); pos != std::string::npos; pos = mainStr.find(toErase))
    {
        mainStr.erase(pos, toErase.length());
    }
}

void cleanClassName(std::string& str)
{
    eraseAllSubStr(str, ",class std::vector");
    eraseAllSubStr(str, "voxomap::");
    eraseAllSubStr(str, "struct");
    eraseAllSubStr(str, "class");
    eraseAllSubStr(str, " ");
}

template <typename T>
std::string type_name()
{
#ifndef _MSC_VER
    typedef typename std::remove_reference<T>::type TR;
    char* str = abi::__cxa_demangle(typeid(TR).name(), nullptr, nullptr, nullptr);
    auto tmp = std::string(str);
    ::free(str);
    cleanClassName(tmp);
    return tmp;
#else
    std::string tmp(typeid(T).name());
    cleanClassName(tmp);
    return tmp;
#endif
}

} // namespace test
} // namespace voxomap

#endif // _VOXOMAP_TEST_COMMON_HPP_