#include <iostream>
#include <random>
#include <chrono>
#include <tuple>
#include "../voxel_octree/VoxelOctree.hpp"
#include "../voxel_octree/SmartArea.hpp"

struct voxel
{
    voxel(int value = 0) : value(value) {}
    int value;
};

void bench_random()
{
    const size_t nb_voxel = 5'000'000;
    voxomap::VoxelOctree<voxomap::SmartArea<voxel>> octree;
    std::mt19937 r(42);

    auto t1 = std::chrono::high_resolution_clock::now();
    for (size_t i = 0; i < nb_voxel; ++i)
    {
        int x = r() % 5'000 - 2'500;
        int y = r() % 5'000 - 2'500;
        int z = r() % 5'000 - 2'500;
        octree.addVoxel(x, y, z, r());
    }
    auto t2 = std::chrono::high_resolution_clock::now();

    r.seed(42);
    size_t read_nb_error = 0;
    for (size_t i = 0; i < nb_voxel; ++i)
    {
        int x = r() % 5'000 - 2'500;
        int y = r() % 5'000 - 2'500;
        int z = r() % 5'000 - 2'500;
        int value = r();
        auto it = octree.findVoxel(x, y, z);
        if (!it || it.voxel->value != value)
            ++read_nb_error;
    }
    auto t3 = std::chrono::high_resolution_clock::now();

    r.seed(42);
    size_t rm_nb_error = 0;
    voxel rm_voxel;
    for (size_t i = 0; i < nb_voxel; ++i)
    {
        int x = r() % 5'000 - 2'500;
        int y = r() % 5'000 - 2'500;
        int z = r() % 5'000 - 2'500;
        int value = r();
        if (!octree.removeVoxel(x, y, z, &rm_voxel) || rm_voxel.value != value)
            ++rm_nb_error;
    }
    auto t4 = std::chrono::high_resolution_clock::now();

    if (read_nb_error == 0)
        std::cout << "No read error detected" << std::endl;
    else
        std::cout << read_nb_error << " read errors detected" << std::endl;

    if (rm_nb_error == 0)
        std::cout << "No remove error detected" << std::endl;
    else
        std::cout << rm_nb_error << " remove errors detected" << std::endl;

    int add_time = std::chrono::duration<double, std::milli>(t2 - t1).count();
    int read_time = std::chrono::duration<double, std::milli>(t3 - t2).count();
    int rm_time = std::chrono::duration<double, std::milli>(t4 - t3).count();
    std::cout << "Added " << nb_voxel << " points in " << add_time << "ms, " << int(nb_voxel / float(add_time / 1000.f)) << " point/s." << std::endl;
    std::cout << "Read " << nb_voxel << " points in " << read_time << "ms, " << int(nb_voxel / float(read_time / 1000.f)) << " point/s." << std::endl;
    std::cout << "Remove " << nb_voxel << " points in " << rm_time << "ms, " << int(nb_voxel / float(rm_time / 1000.f)) << " point/s." << std::endl;
    std::cout << "Total time: " << std::chrono::duration<double, std::milli>(t4 - t1).count() << "ms." << std::endl;
}

void bench_continuous()
{
    const size_t nb_voxel = 5'000'000;
    voxomap::VoxelOctree<voxomap::SmartArea<voxel>> octree;
    std::mt19937 r(42);

    std::map<std::tuple<int, int, int>, std::vector<std::tuple<int, int, int, int>>> voxels;

    for (size_t i = 0; i < nb_voxel; ++i)
    {
        int x = r() % 5'000 - 2'500;
        int y = r() % 5'000 - 2'500;
        int z = r() % 5'000 - 2'500;
        auto pos = std::make_tuple(x & ~7, y & ~7, z & ~7);
        voxels[pos].emplace_back(x, y, z, r());
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
    voxel rm_voxel;
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
        std::cout << read_nb_error << " read errors detected" << std::endl;

    if (rm_nb_error == 0)
        std::cout << "No remove error detected" << std::endl;
    else
        std::cout << rm_nb_error << " remove errors detected" << std::endl;

    int add_time = std::chrono::duration<double, std::milli>(t2 - t1).count();
    int read_time = std::chrono::duration<double, std::milli>(t3 - t2).count();
    int rm_time = std::chrono::duration<double, std::milli>(t4 - t3).count();
    std::cout << "Added " << nb_voxel << " points in " << add_time << "ms, " << int(nb_voxel / float(add_time / 1000.f)) << " point/s." << std::endl;
    std::cout << "Read " << nb_voxel << " points in " << read_time << "ms, " << int(nb_voxel / float(read_time / 1000.f)) << " point/s." << std::endl;
    std::cout << "Remove " << nb_voxel << " points in " << rm_time << "ms, " << int(nb_voxel / float(rm_time / 1000.f)) << " point/s." << std::endl;
    std::cout << "Total time: " << std::chrono::duration<double, std::milli>(t4 - t1).count() << "ms." << std::endl;
}

int main(int argc, char* argv[])
{
    bench_random();
    bench_continuous();
};