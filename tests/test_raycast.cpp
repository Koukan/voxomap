#include <iostream>
#include <random>
#include <chrono>
#include <tuple>
#include "../voxel_octree/VoxelOctree.hpp"
#include "../voxel_octree/VoxelContainer/SparseContainer.hpp"
#include "../voxel_octree/VoxelContainer/ArrayContainer.hpp"
#include "../voxel_octree/VoxelContainer/SidedContainer.hpp"
#include "../voxel_octree/SuperContainer/ArraySuperContainer.hpp"
#include "../voxel_octree/SuperContainer/SparseSuperContainer.hpp"
#include "../utils/Raycast.hpp"

/*! \struct voxel
    \test
    \brief Voxel structure use for benchmarks
*/
struct voxel
{
    voxel() = default;
    voxel(voxel const& other) = default;
    voxel(voxel&& other) = default;
    voxel(int value) : value(value) {}
    voxel& operator=(voxel const& other) = default;
    voxel& operator=(voxel&& other) = default;
    bool mergeSide(voxel const& other) const { return true; }

    int64_t value = 0;
};

template <typename T_Container>
void test_raycasting()
{
    std::cout << "Launch test_raycasting:" << std::endl;
    const size_t nb_voxel = 50000;
    voxomap::VoxelOctree<T_Container> octree;
    std::mt19937 r(42);

    auto t1 = std::chrono::high_resolution_clock::now();
    for (size_t i = 0; i < nb_voxel; ++i)
    {
        int x = r() % 512 - 256;
        int y = r() % 512 - 256;
        int z = r() % 512 - 256;
        octree.putVoxel(x, y, z, r());
    }
    auto t2 = std::chrono::high_resolution_clock::now();

    r.seed(42);
    size_t nb_raycasting_error = 0;
    voxomap::Ray ray;
    ray.src.set(0.0, 0.0, 0.0);
    for (size_t i = 0; i < nb_voxel; ++i)
    {
        int x = r() % 512 - 256;
        int y = r() % 512 - 256;
        int z = r() % 512 - 256;
        int value = r();
        ray.dir.set(double(x) + 0.5, double(y) + 0.5, double(z) + 0.5);
        auto predicate = [x, y, z](typename T_Container::iterator const& it) -> bool { 
            int vx, vy, vz;
            it.getVoxelPosition(vx, vy, vz);
            return vx == x && vy == y && vz == z;
        };
        auto result = voxomap::Raycast<T_Container>::get(ray, octree, predicate);

        if (result.distance != -1)
        {
            int vx, vy, vz;
            result.it.getVoxelPosition(vx, vy, vz);
            if (vx == x && vy == y && vz == z && result.it.voxel->value == value)
                continue;
        }
        ++nb_raycasting_error;
    }
    auto t3 = std::chrono::high_resolution_clock::now();

    if (nb_raycasting_error == 0)
        std::cout << "No error detected" << std::endl;
    else
        std::cout << "Error: there is " << nb_raycasting_error << " raycasting errors." << std::endl;

    std::cout << "Total time: " << static_cast<int>(std::chrono::duration<double, std::milli>(t3 - t1).count()) << "ms." << std::endl;
    std::cout << std::endl;
}

template <typename T_Container>
void benchmark_raycasting()
{
    std::cout << "Launch benchmark_raycasting:" << std::endl;
    const size_t nb_voxel = 50000;
    voxomap::VoxelOctree<T_Container> octree;
    std::mt19937 r(42);

    auto t1 = std::chrono::high_resolution_clock::now();
    for (size_t i = 0; i < nb_voxel; ++i)
    {
        int x = r() % 512 - 256;
        int y = r() % 512 - 256;
        int z = r() % 512 - 256;
        octree.putVoxel(x, y, z, r());
    }
    auto t2 = std::chrono::high_resolution_clock::now();

    r.seed(42);
    size_t nb_raycasting_error = 0;
    voxomap::Ray ray;
    ray.src.set(0.0, 0.0, 0.0);
    for (size_t i = 0; i < nb_voxel; ++i)
    {
        int x = r() % 512 - 256;
        int y = r() % 512 - 256;
        int z = r() % 512 - 256;
        int value = r();
        ray.dir.set(double(x) + 0.5, double(y) + 0.5, double(z) + 0.5);
        ray.dir.normalize();
        auto result = voxomap::Raycast<T_Container>::get(ray, octree);

        if (result.distance != -1)
            continue;
        ++nb_raycasting_error;
    }
    auto t3 = std::chrono::high_resolution_clock::now();

    if (nb_raycasting_error == 0)
        std::cout << "No error detected" << std::endl;
    else
        std::cout << "Error: there is " << nb_raycasting_error << " raycasting errors." << std::endl;

    int raycast_time = static_cast<int>(std::chrono::duration<double, std::milli>(t3 - t2).count());
    std::cout << "Raycast time: " << raycast_time << "ms. , " << (nb_voxel / (float(raycast_time) / 1000.f)) << " raycast/s" << std::endl;
    std::cout << "Total time: " << static_cast<int>(std::chrono::duration<double, std::milli>(t3 - t1).count()) << "ms." << std::endl;
    std::cout << std::endl;
}


template <typename T_Voxel>
using SparseContainer = voxomap::SparseContainer<T_Voxel>;

int main(int argc, char* argv[])
{
    // Test with SparseContainer
    test_raycasting<voxomap::SparseContainer<voxel>>();
    benchmark_raycasting<voxomap::SparseContainer<voxel>>();

    // Test with ArrayContainer
    test_raycasting<voxomap::ArrayContainer<voxel>>();
    benchmark_raycasting<voxomap::ArrayContainer<voxel>>();

    // Test with SparseContainer inside SparseSuperContainer
    test_raycasting<voxomap::SparseSuperContainer<voxomap::SparseContainer<voxel>>>();
    benchmark_raycasting<voxomap::SparseSuperContainer<voxomap::SparseContainer<voxel>>>();

    // Test with ArrayContainer inside ArrayeSuperContainer
    test_raycasting<voxomap::ArraySuperContainer<voxomap::ArrayContainer<voxel>>>();
    benchmark_raycasting<voxomap::ArraySuperContainer<voxomap::ArrayContainer<voxel>>>();

    //// Test with SideArea<SmartArea>
    test_raycasting<voxomap::SidedContainer<SparseContainer, voxel>>();
    benchmark_raycasting<voxomap::SidedContainer<SparseContainer, voxel>>();

    //// Test with SideArea<ArrayArea>
    test_raycasting<voxomap::SidedContainer<voxomap::ArrayContainer, voxel>>();
    benchmark_raycasting<voxomap::SidedContainer<voxomap::ArrayContainer, voxel>>();

    // Test multiple super container
    test_raycasting<voxomap::SparseSuperContainer<voxomap::SparseSuperContainer<voxomap::SparseContainer<voxel>>>>();
    test_raycasting<voxomap::SparseSuperContainer<voxomap::SparseSuperContainer<voxomap::ArraySuperContainer<voxomap::SparseContainer<voxel>>>>>();
    test_raycasting<voxomap::ArraySuperContainer<voxomap::SparseSuperContainer<voxomap::ArraySuperContainer<voxomap::SparseContainer<voxel>>>>>();

    benchmark_raycasting<voxomap::SparseSuperContainer<voxomap::SparseSuperContainer<voxomap::SparseContainer<voxel>>>>();
    benchmark_raycasting<voxomap::SparseSuperContainer<voxomap::SparseSuperContainer<voxomap::ArraySuperContainer<voxomap::SparseContainer<voxel>>>>>();
    benchmark_raycasting<voxomap::ArraySuperContainer<voxomap::SparseSuperContainer<voxomap::ArraySuperContainer<voxomap::SparseContainer<voxel>>>>>();
};