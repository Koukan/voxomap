#include <iostream>
#include <chrono>
#include "../voxel_octree/VoxelOctree.hpp"
#include "../voxel_octree/VoxelContainer/SparseContainer.hpp"
#include "../voxel_octree/VoxelContainer/ArrayContainer.hpp"
#include "../voxel_octree/VoxelContainer/SidedContainer.hpp"
#include "../voxel_octree/SuperContainer/ArraySuperContainer.hpp"
#include "../voxel_octree/SuperContainer/SparseSuperContainer.hpp"
#include "../utils/Raycast.hpp"

#include "common.hpp"

static const size_t gNbVoxel = 50000;

template <typename T_Container>
bool test_raycasting()
{
    voxomap::test::initGlobalValues(gNbVoxel);

    std::cout << "Launch test_raycasting (" << typeid(T_Container).name() << "):" << std::endl;
    voxomap::VoxelOctree<T_Container> octree;

    auto t1 = std::chrono::high_resolution_clock::now();
    for (size_t i = 0; i < voxomap::test::gTestValues.size(); ++i)
    {
        auto const& data = voxomap::test::gTestValues[i];
        octree.putVoxel(data.x, data.y, data.z, data.value);
    }
    auto t2 = std::chrono::high_resolution_clock::now();

    size_t nb_raycasting_error = 0;
    voxomap::Ray ray;
    ray.setOrigin({ 0.0, 0.0, 0.0 });
    for (size_t i = 0; i < voxomap::test::gTestValues.size(); ++i)
    {
        auto const& data = voxomap::test::gTestValues[i];

        ray.setDirection({ double(data.x) + 0.5, double(data.y) + 0.5, double(data.z) + 0.5 });
        auto predicate = [&data](typename T_Container::iterator const& it) -> bool {
            int vx, vy, vz;
            it.getVoxelPosition(vx, vy, vz);
            return vx == data.x && vy == data.y && vz == data.z;
        };
        auto result = voxomap::Raycast<T_Container>::get(ray, octree, predicate);

        if (result.distance != -1)
        {
            int vx, vy, vz;
            result.it.getVoxelPosition(vx, vy, vz);
            if (vx == data.x && vy == data.y && vz == data.z && result.it.voxel->value == data.value)
                continue;

            std::cout << "value: " << result.it.voxel->value << " = " << data.value << "\n";
        }
        else
            std::cout << "distance: " << result.distance << "\n";
        ++nb_raycasting_error;
    }
    auto t3 = std::chrono::high_resolution_clock::now();

    if (nb_raycasting_error == 0)
        std::cout << "No error detected" << std::endl;
    else
        std::cout << "Error: there is " << nb_raycasting_error << " raycasting errors." << std::endl;

    std::cout << "Total time: " << static_cast<int>(std::chrono::duration<double, std::milli>(t3 - t1).count()) << "ms." << std::endl;
    std::cout << std::endl;

    return nb_raycasting_error == 0;
}

template <typename T_Container>
bool benchmark_raycasting()
{
    voxomap::test::initGlobalValues(gNbVoxel);

    std::cout << "Launch benchmark_raycasting (" << typeid(T_Container).name() << "):" << std::endl;
    voxomap::VoxelOctree<T_Container> octree;

    auto t1 = std::chrono::high_resolution_clock::now();
    for (size_t i = 0; i < voxomap::test::gTestValues.size(); ++i)
    {
        auto const& data = voxomap::test::gTestValues[i];
        octree.putVoxel(data.x, data.y, data.z, data.value);
    }
    auto t2 = std::chrono::high_resolution_clock::now();

    size_t nb_raycasting_error = 0;
    voxomap::Ray ray;
    ray.setOrigin({ 0.0, 0.0, 0.0 });
    for (size_t i = 0; i < voxomap::test::gTestValues.size(); ++i)
    {
        auto const& data = voxomap::test::gTestValues[i];
        ray.setDirection({ double(data.x) + 0.5, double(data.y) + 0.5, double(data.z) + 0.5 });
        auto result = voxomap::Raycast<T_Container>::get(ray, octree);

        if (result.distance == -1)
            ++nb_raycasting_error;
    }
    auto t3 = std::chrono::high_resolution_clock::now();

    if (nb_raycasting_error == 0)
        std::cout << "No error detected" << std::endl;
    else
        std::cout << "Error: there is " << nb_raycasting_error << " raycasting errors." << std::endl;

    int raycast_time = static_cast<int>(std::chrono::duration<double, std::milli>(t3 - t2).count());
    std::cout << "Raycast time: " << raycast_time << "ms. , " << (voxomap::test::gTestValues.size() / (float(raycast_time) / 1000.f)) << " raycast/s" << std::endl;
    std::cout << "Total time: " << static_cast<int>(std::chrono::duration<double, std::milli>(t3 - t1).count()) << "ms." << std::endl;
    std::cout << std::endl;

    return nb_raycasting_error == 0;
}

template <typename T_Container>
bool benchmark_raycasting_with_cache()
{
    voxomap::test::initGlobalValues(gNbVoxel);

    std::cout << "Launch benchmark_raycasting_with_cache (" << typeid(T_Container).name() << "):" << std::endl;
    voxomap::VoxelOctree<T_Container> octree;

    auto t1 = std::chrono::high_resolution_clock::now();
    for (size_t i = 0; i < voxomap::test::gTestValues.size(); ++i)
    {
        auto const& data = voxomap::test::gTestValues[i];
        octree.putVoxel(data.x, data.y, data.z, data.value);
    }
    auto t2 = std::chrono::high_resolution_clock::now();

    typename voxomap::Raycast<T_Container>::Cache cache;
    cache.fillCache(octree);

    auto t3 = std::chrono::high_resolution_clock::now();

    size_t nb_raycasting_error = 0;
    voxomap::Ray ray;
    ray.setOrigin({ 0.0, 0.0, 0.0 });
    for (size_t i = 0; i < voxomap::test::gTestValues.size(); ++i)
    {
        auto const& data = voxomap::test::gTestValues[i];
        ray.setDirection({ double(data.x) + 0.5, double(data.y) + 0.5, double(data.z) + 0.5 });
        auto result = voxomap::Raycast<T_Container>::get(ray, octree, cache);

        if (result.distance == -1)
            ++nb_raycasting_error;
    }
    auto t4 = std::chrono::high_resolution_clock::now();

    if (nb_raycasting_error == 0)
        std::cout << "No error detected" << std::endl;
    else
        std::cout << "Error: there is " << nb_raycasting_error << " raycasting errors." << std::endl;

    int cache_construction_time = static_cast<int>(std::chrono::duration<double, std::milli>(t3 - t2).count());
    int raycast_time = static_cast<int>(std::chrono::duration<double, std::milli>(t4 - t3).count());
    std::cout << "Cache construction time: " << cache_construction_time << "ms." << std::endl;
    std::cout << "Raycast time: " << raycast_time << "ms. , " << (voxomap::test::gTestValues.size() / (float(raycast_time) / 1000.f)) << " raycast/s" << std::endl;
    std::cout << "Total time: " << static_cast<int>(std::chrono::duration<double, std::milli>(t4 - t1).count()) << "ms." << std::endl;
    std::cout << std::endl;

    return nb_raycasting_error == 0;
}

static bool g_error = false;

template <typename T_Container>
void launchTest()
{
    std::cout << "------- TEST " << typeid(T_Container).name() << " -------\n\n";
    g_error |= !test_raycasting<T_Container>();
    g_error |= !benchmark_raycasting<T_Container>();
    g_error |= !benchmark_raycasting_with_cache<T_Container>();
    std::cout << "------- END -------\n\n\n";
}

template <typename T_Voxel>
using SparseContainer = voxomap::SparseContainer<T_Voxel>;
using voxel = voxomap::test::voxel;

int main(int argc, char* argv[])
{
    // No super container
    launchTest<voxomap::SparseContainer<voxel>>();
    launchTest<voxomap::ArrayContainer<voxel>>();
    launchTest<voxomap::SidedContainer<SparseContainer, voxel>>();
    launchTest<voxomap::SidedContainer<voxomap::ArrayContainer, voxel>>();

    // One super container
    launchTest<voxomap::SparseSuperContainer<voxomap::SparseContainer<voxel>>>();
    launchTest<voxomap::ArraySuperContainer<voxomap::ArrayContainer<voxel>>>();

    // Multiple super container
    launchTest<voxomap::SparseSuperContainer<voxomap::SparseSuperContainer<voxomap::SparseContainer<voxel>>>>();
    launchTest<voxomap::SparseSuperContainer<voxomap::SparseSuperContainer<voxomap::ArraySuperContainer<voxomap::SparseContainer<voxel>>>>>();
    launchTest<voxomap::ArraySuperContainer<voxomap::SparseSuperContainer<voxomap::ArraySuperContainer<voxomap::SparseContainer<voxel>>>>>();

    std::cout << "------ FINAL RESULT: ";
    if (g_error)
    {
        std::cout << "ERROR ERROR ERROR ERROR There is errors !!! ------" << std::endl;
        return EXIT_FAILURE;
    }
    
    std::cout << "GOOD. ------" << std::endl;
    return EXIT_SUCCESS;
};