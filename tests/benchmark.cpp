#include <iostream>
#include <chrono>
#include "../voxel_octree/VoxelOctree.hpp"
#include "../voxel_octree/VoxelContainer/SparseContainer.hpp"
#include "../voxel_octree/VoxelContainer/ArrayContainer.hpp"
#include "../voxel_octree/VoxelContainer/SidedContainer.hpp"
#include "../voxel_octree/SuperContainer/ArraySuperContainer.hpp"
#include "../voxel_octree/SuperContainer/SparseSuperContainer.hpp"
#include "common.hpp"

static const size_t gNbVoxel = 500000;
static size_t gNbError = 0;
/*!
*/
template <typename T_Container>
bool bench_random()
{
    voxomap::test::initGlobalValues(gNbVoxel);

    std::cout << "Launch bench_random (" << voxomap::test::type_name<T_Container>() << "):" << std::endl;

    voxomap::VoxelOctree<T_Container> octree;

    auto t1 = std::chrono::high_resolution_clock::now();
    for (size_t i = 0; i < voxomap::test::gTestValues.size(); ++i)
    {
        auto const& data = voxomap::test::gTestValues[i];
        octree.putVoxel(data.x, data.y, data.z, data.value);
    }
    auto t2 = std::chrono::high_resolution_clock::now();

    size_t read_nb_error = 0;
    for (size_t i = 0; i < voxomap::test::gTestValues.size(); ++i)
    {
        auto const& data = voxomap::test::gTestValues[i];
        auto it = octree.findVoxel(data.x, data.y, data.z);
        if (!it || it.voxel->value != data.value)
            ++read_nb_error;
    }
    auto t3 = std::chrono::high_resolution_clock::now();

    size_t rm_nb_error = 0;
    typename T_Container::VoxelData rm_voxel;
    for (size_t i = 0; i < voxomap::test::gTestValues.size(); ++i)
    {
        auto const& data = voxomap::test::gTestValues[i];
        if (!octree.removeVoxel(data.x, data.y, data.z, &rm_voxel) || rm_voxel.value != data.value)
            ++rm_nb_error;
    }
    auto t4 = std::chrono::high_resolution_clock::now();

    if (read_nb_error == 0)
        std::cout << "No read error detected" << std::endl;
    else
        std::cout << "Error: " << read_nb_error << " read errors detected" << std::endl;

    if (rm_nb_error == 0)
        std::cout << "No remove error detected" << std::endl;
    else
        std::cout << "Error: " << rm_nb_error << " remove errors detected" << std::endl;

    int add_time = static_cast<int>(std::chrono::duration<double, std::milli>(t2 - t1).count());
    int read_time = static_cast<int>(std::chrono::duration<double, std::milli>(t3 - t2).count());
    int rm_time = static_cast<int>(std::chrono::duration<double, std::milli>(t4 - t3).count());
    std::cout << "Added " << gNbVoxel << " voxels in " << add_time << "ms, " << int(gNbVoxel / float(add_time / 1000.f)) << " voxels/s." << std::endl;
    std::cout << "Read " << gNbVoxel << " voxels in " << read_time << "ms, " << int(gNbVoxel / float(read_time / 1000.f)) << " voxels/s." << std::endl;
    std::cout << "Remove " << gNbVoxel << " voxels in " << rm_time << "ms, " << int(gNbVoxel / float(rm_time / 1000.f)) << " voxels/s." << std::endl;
    std::cout << "Total time: " << static_cast<int>(std::chrono::duration<double, std::milli>(t4 - t1).count()) << "ms." << std::endl;
    std::cout << std::endl;

    return read_nb_error == 0 && rm_nb_error == 0;
}

template <typename T_Container>
bool bench_continuous()
{
    voxomap::test::initGlobalValues(gNbVoxel);

    std::cout << "Launch bench_continuous (" << voxomap::test::type_name<T_Container>() << "):" << std::endl;

    voxomap::VoxelOctree<T_Container> octree;
    std::map<std::tuple<int, int, int>, std::vector<std::tuple<int, int, int, int64_t>>> voxels;

    for (size_t i = 0; i < voxomap::test::gTestValues.size(); ++i)
    {
        auto const& data = voxomap::test::gTestValues[i];
        auto pos = std::make_tuple(data.x & ~T_Container::COORD_MASK, data.y & ~T_Container::COORD_MASK, data.z & ~T_Container::COORD_MASK);
        voxels[pos].emplace_back(data.x, data.y, data.z, data.value);
    }

    auto t1 = std::chrono::high_resolution_clock::now();
    for (auto const& pair : voxels)
    {
        for (auto const& t : pair.second)
        {
            octree.addVoxel(std::get<0>(t), std::get<1>(t), std::get<2>(t), std::get<3>(t));
        }
    }
    auto t2 = std::chrono::high_resolution_clock::now();

    size_t read_nb_error = 0;
    for (auto const& pair : voxels)
    {
        for (auto const& t : pair.second)
        {
            auto it = octree.findVoxel(std::get<0>(t), std::get<1>(t), std::get<2>(t));
            if (!it || it.voxel->value != std::get<3>(t))
                ++read_nb_error;
        }
    }
    auto t3 = std::chrono::high_resolution_clock::now();

    size_t rm_nb_error = 0;
    typename T_Container::VoxelData rm_voxel;
    for (auto const& pair : voxels)
    {
        for (auto const& t : pair.second)
        {
            if (!octree.removeVoxel(std::get<0>(t), std::get<1>(t), std::get<2>(t), &rm_voxel) || rm_voxel.value != std::get<3>(t))
                ++rm_nb_error;
        }
    }
    auto t4 = std::chrono::high_resolution_clock::now();

    if (read_nb_error == 0)
        std::cout << "No read error detected" << std::endl;
    else
        std::cout << "Error: " << read_nb_error << " read errors detected" << std::endl;

    if (rm_nb_error == 0)
        std::cout << "No remove error detected" << std::endl;
    else
        std::cout << "Error: " << rm_nb_error << " remove errors detected" << std::endl;

    int add_time = static_cast<int>(std::chrono::duration<double, std::milli>(t2 - t1).count());
    int read_time = static_cast<int>(std::chrono::duration<double, std::milli>(t3 - t2).count());
    int rm_time = static_cast<int>(std::chrono::duration<double, std::milli>(t4 - t3).count());
    std::cout << "Added " << gNbVoxel << " voxels in " << add_time << "ms, " << int(gNbVoxel / float(add_time / 1000.f)) << " voxels/s." << std::endl;
    std::cout << "Read " << gNbVoxel << " voxels in " << read_time << "ms, " << int(gNbVoxel / float(read_time / 1000.f)) << " voxels/s." << std::endl;
    std::cout << "Remove " << gNbVoxel << " voxels in " << rm_time << "ms, " << int(gNbVoxel / float(rm_time / 1000.f)) << " voxels/s." << std::endl;
    std::cout << "Total time: " << static_cast<int>(std::chrono::duration<double, std::milli>(t4 - t1).count()) << "ms." << std::endl;
    std::cout << std::endl;

    return read_nb_error == 0 && rm_nb_error == 0;
}

template <typename T_Container>
bool bench_update_voxel()
{
    voxomap::test::initGlobalValues(gNbVoxel);

    std::cout << "Launch bench_update_voxel (" << voxomap::test::type_name<T_Container>() << "):" << std::endl;

    voxomap::VoxelOctree<T_Container> octree;

    auto t1 = std::chrono::high_resolution_clock::now();
    for (size_t i = 0; i < voxomap::test::gTestValues.size(); ++i)
    {
        auto const& data = voxomap::test::gTestValues[i];
        octree.addVoxel(data.x, data.y, data.z, data.value);
    }
    auto t2 = std::chrono::high_resolution_clock::now();

    std::mt19937 r(84);
    for (auto it : octree)
    {
        octree.updateVoxel(*it, r());
    }
    auto t3 = std::chrono::high_resolution_clock::now();
    
    r.seed(84);
    size_t nb_error = 0;
    for (auto it : octree)
    {
        int64_t value = r();
        if (it->voxel->value != value)
        {
            ++nb_error;
        }
    }
    auto t4 = std::chrono::high_resolution_clock::now();
    
    if (nb_error == 0)
        std::cout << "No error detected" << std::endl;
    else
        std::cout << "Error: " << nb_error << " errors detected" << std::endl;
    
    int update_time = static_cast<int>(std::chrono::duration<double, std::milli>(t3 - t2).count());
    std::cout << "Update " << gNbVoxel << " voxels in " << update_time << "ms, " << int(gNbVoxel / float(update_time / 1000.f)) << " voxels/s." << std::endl;
    std::cout << "Total time: " << static_cast<int>(std::chrono::duration<double, std::milli>(t4 - t1).count()) << "ms." << std::endl;
    std::cout << std::endl;

    return nb_error == 0;
}

static bool g_error = false;

template <typename T_Container>
void launchBenchmark()
{
    std::cout << "------- BENCHMARK " << voxomap::test::type_name<T_Container>() << " -------\n\n";
    g_error |= !bench_random<T_Container>();
    //g_error |= !bench_continuous<T_Container>();
    //g_error |= !bench_update_voxel<T_Container>();
    std::cout << "------- END -------\n\n\n";
}

template <typename T_Voxel>
using SparseContainer = voxomap::SparseContainer<T_Voxel>;
using voxel = voxomap::test::voxel;

int main(int argc, char* argv[])
{
    // No super container
    //launchBenchmark<voxomap::SparseContainer<voxel>>();
    //launchBenchmark<voxomap::ArrayContainer<voxel>>();
    launchBenchmark<voxomap::SidedContainer<SparseContainer, voxel>>();
    launchBenchmark<voxomap::SidedContainer<voxomap::ArrayContainer, voxel>>();

    // One super container
    //launchBenchmark<voxomap::SparseSuperContainer<voxomap::SparseContainer<voxel>>>();
    //launchBenchmark<voxomap::ArraySuperContainer<voxomap::ArrayContainer<voxel>>>();
    launchBenchmark<voxomap::SparseSuperContainer<voxomap::SidedContainer<SparseContainer, voxel>>>();
    launchBenchmark<voxomap::ArraySuperContainer<voxomap::SidedContainer<voxomap::ArrayContainer, voxel>>>();
    //
    //// Multiple super container
    //launchBenchmark<voxomap::SparseSuperContainer<voxomap::SparseSuperContainer<voxomap::SparseContainer<voxel>>>>();
    //launchBenchmark<voxomap::SparseSuperContainer<voxomap::SparseSuperContainer<voxomap::ArraySuperContainer<voxomap::SparseContainer<voxel>>>>>();
    //launchBenchmark<voxomap::ArraySuperContainer<voxomap::SparseSuperContainer<voxomap::ArraySuperContainer<voxomap::SparseContainer<voxel>>>>>();
    launchBenchmark<voxomap::SparseSuperContainer<voxomap::SparseSuperContainer<voxomap::SidedContainer<SparseContainer, voxel>>>>();
    launchBenchmark<voxomap::SparseSuperContainer<voxomap::SparseSuperContainer<voxomap::ArraySuperContainer<voxomap::SidedContainer<SparseContainer, voxel>>>>>();

    std::cout << "------ FINAL RESULT: ";
    if (g_error)
    {
        std::cout << "ERROR ERROR ERROR ERROR There is errors !!! ------" << std::endl;
        return EXIT_FAILURE;
    }

    std::cout << "GOOD. ------" << std::endl;
    return EXIT_SUCCESS;
};