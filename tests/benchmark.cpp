#include <iostream>
#include <random>
#include <chrono>
#include <tuple>
#include "../voxel_octree/VoxelOctree.hpp"
#include "../voxel_octree/SmartArea.hpp"
#include "../voxel_octree/ArrayArea.hpp"
#include "../voxel_octree/FaceArea.hpp"
#include "../voxel_octree/VoxelArea.hpp"

struct voxel
{
    voxel() = default;
    voxel(voxel const& other) = default;
    voxel(voxel&& other) = default;
    voxel(int value) : value(value) {}
    operator bool() const { return value != 0; }
    voxel& operator=(voxel const& other) = default;
    voxel& operator=(voxel&& other) = default;
    bool mergeFace(voxel const& other) const { return true; }

    int64_t value = 0;
};

template <typename T_Area>
void bench_random()
{
    std::cout << "Launch bench_random:" << std::endl;

    const size_t nb_voxel = 500000;
    voxomap::VoxelOctree<T_Area> octree;
    std::mt19937 r(42);

    auto t1 = std::chrono::high_resolution_clock::now();
    for (size_t i = 0; i < nb_voxel; ++i)
    {
        int x = r() % 5000 - 2500;
        int y = r() % 5000 - 2500;
        int z = r() % 5000 - 2500;
        octree.putVoxel(x, y, z, r());
    }
    auto t2 = std::chrono::high_resolution_clock::now();

    r.seed(42);
    size_t read_nb_error = 0;
    for (size_t i = 0; i < nb_voxel; ++i)
    {
        int x = r() % 5000 - 2500;
        int y = r() % 5000 - 2500;
        int z = r() % 5000 - 2500;
        int value = r();
        auto it = octree.findVoxel(x, y, z);
        if (!it || it.voxel->value != value)
            ++read_nb_error;
    }
    auto t3 = std::chrono::high_resolution_clock::now();

    r.seed(42);
    size_t rm_nb_error = 0;
    typename T_Area::VoxelData rm_voxel;
    for (size_t i = 0; i < nb_voxel; ++i)
    {
        int x = r() % 5000 - 2500;
        int y = r() % 5000 - 2500;
        int z = r() % 5000 - 2500;
        int value = r();
        if (!octree.removeVoxel(x, y, z, &rm_voxel) || rm_voxel.value != value)
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
    std::cout << "Added " << nb_voxel << " voxels in " << add_time << "ms, " << int(nb_voxel / float(add_time / 1000.f)) << " voxels/s." << std::endl;
    std::cout << "Read " << nb_voxel << " voxels in " << read_time << "ms, " << int(nb_voxel / float(read_time / 1000.f)) << " voxels/s." << std::endl;
    std::cout << "Remove " << nb_voxel << " voxels in " << rm_time << "ms, " << int(nb_voxel / float(rm_time / 1000.f)) << " voxels/s." << std::endl;
    std::cout << "Total time: " << static_cast<int>(std::chrono::duration<double, std::milli>(t4 - t1).count()) << "ms." << std::endl;
    std::cout << std::endl;
}

template <typename T_Area>
void bench_continuous()
{
    std::cout << "Launch bench_continuous:" << std::endl;

    const size_t nb_voxel = 500000;
    voxomap::VoxelOctree<T_Area> octree;
    std::mt19937 r(42);

    std::map<std::tuple<int, int, int>, std::vector<std::tuple<int, int, int, int>>> voxels;

    for (size_t i = 0; i < nb_voxel; ++i)
    {
        int x = r() % 5000 - 2500;
        int y = r() % 5000 - 2500;
        int z = r() % 5000 - 2500;
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
    typename T_Area::VoxelData rm_voxel;
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
    std::cout << "Added " << nb_voxel << " voxels in " << add_time << "ms, " << int(nb_voxel / float(add_time / 1000.f)) << " voxels/s." << std::endl;
    std::cout << "Read " << nb_voxel << " voxels in " << read_time << "ms, " << int(nb_voxel / float(read_time / 1000.f)) << " voxels/s." << std::endl;
    std::cout << "Remove " << nb_voxel << " voxels in " << rm_time << "ms, " << int(nb_voxel / float(rm_time / 1000.f)) << " voxels/s." << std::endl;
    std::cout << "Total time: " << static_cast<int>(std::chrono::duration<double, std::milli>(t4 - t1).count()) << "ms." << std::endl;
    std::cout << std::endl;
}

template <typename T_Area>
void bench_update_voxel()
{
    std::cout << "Launch bench_update_voxel:" << std::endl;

    const size_t nb_voxel = 500000;
    voxomap::VoxelOctree<T_Area> octree;
    std::mt19937 r(42);
    std::vector<bool> added;
    added.reserve(nb_voxel);

    auto t1 = std::chrono::high_resolution_clock::now();
    for (size_t i = 0; i < nb_voxel; ++i)
    {
        int x = r() % 1000 - 500;
        int y = r() % 1000 - 500;
        int z = r() % 1000 - 500;
        added.emplace_back(octree.addVoxel(x, y, z, r()).second);
    }
    auto t2 = std::chrono::high_resolution_clock::now();

    r.seed(54);
    for (auto it : octree)
    {
        octree.updateVoxel(*it, r());
    }
    r.seed(54);
    size_t nb_error = 0;
    for (auto it : octree)
    {
        int value = r();
        if (it->voxel->value != value)
        {
            ++nb_error;
        }
    }
    auto t3 = std::chrono::high_resolution_clock::now();

    if (nb_error == 0)
        std::cout << "No error detected" << std::endl;
    else
        std::cout << "Error: " << nb_error << " errors detected" << std::endl;

    int update_time = static_cast<int>(std::chrono::duration<double, std::milli>(t3 - t2).count());
    std::cout << "Update " << nb_voxel << " voxels in " << update_time << "ms, " << int(nb_voxel / float(update_time / 1000.f)) << " voxels/s." << std::endl;
    std::cout << "Total time: " << static_cast<int>(std::chrono::duration<double, std::milli>(t3 - t1).count()) << "ms." << std::endl;
    std::cout << std::endl;
}

template <typename T_Area>
void bench_voxel_area()
{
    std::cout << "Launch bench_voxel_area:" << std::endl;

    const size_t nb_voxel = 250000;
    voxomap::VoxelOctree<T_Area> octree;
    std::mt19937 r(42);
    std::vector<bool> added;
    added.reserve(nb_voxel);

    auto t1 = std::chrono::high_resolution_clock::now();
    for (size_t i = 0; i < nb_voxel; ++i)
    {
        int x = r() % 1000 - 500;
        int y = r() % 1000 - 500;
        int z = r() % 1000 - 500;
        added.emplace_back(octree.addVoxel(x, y, z, r()).second);
    }
    auto t2 = std::chrono::high_resolution_clock::now();

    r.seed(42);
    size_t read_nb_error = 0;
    size_t nb_error = 0;
    size_t nb_found_voxel = 0;
    for (size_t i = 0; i < nb_voxel; ++i)
    {
        int x = r() % 1000 - 500;
        int y = r() % 1000 - 500;
        int z = r() % 1000 - 500;
        int value = r();

        auto it = octree.findVoxel(x, y, z);
        voxomap::VoxelArea<T_Area> search_area(it);

        if (!added[i])
            continue;
        if (!it || it.voxel->value != value)
            ++read_nb_error;
        else
        {
            for (int ix = 0; ix < 8; ++ix)
            {
                for (int iy = 0; iy < 8; ++iy)
                {
                    for (int iz = 0; iz < 8; ++iz)
                    {
                        it = octree.findVoxel(x + ix, y + iy, z + iz);
                        if (it)
                            ++nb_found_voxel;
                        if (it != search_area.findVoxel(ix, iy, iz))
                            ++nb_error;
                    }
                }
            }
        }
    }
    auto t3 = std::chrono::high_resolution_clock::now();

    if (read_nb_error == 0)
        std::cout << "No read error detected" << std::endl;
    else
        std::cout << "Error: " << read_nb_error << " read errors detected" << std::endl;

    if (nb_found_voxel == 0)
        std::cout << "Error: " << "No voxel found" << std::endl;

    if (nb_error == 0)
        std::cout << "No area error detected" << std::endl;
    else
        std::cout << "Error: " << nb_error << " area errors detected" << std::endl;

    int add_time = static_cast<int>(std::chrono::duration<double, std::milli>(t2 - t1).count());
    int read_time = static_cast<int>(std::chrono::duration<double, std::milli>(t3 - t2).count());
    std::cout << "Total time: " << static_cast<int>(std::chrono::duration<double, std::milli>(t3 - t1).count()) << "ms." << std::endl;
    std::cout << std::endl;
}

template <typename T_Area>
void test_iterator()
{
    std::cout << "Launch test_iterator:" << std::endl;

    const size_t nb_voxel = 500000;
    voxomap::VoxelOctree<T_Area> octree;
    std::mt19937 r(84);
    size_t nb_added_voxel = 0;

    auto t1 = std::chrono::high_resolution_clock::now();
    for (size_t i = 0; i < nb_voxel; ++i)
    {
        int x = r() % 1000 - 500;
        int y = r() % 1000 - 500;
        int z = r() % 1000 - 500;
        if (octree.addVoxel(x, y, z, r()).second)
            ++nb_added_voxel;
    }
    auto t2 = std::chrono::high_resolution_clock::now();

    size_t nb_count_voxel = 0;
    for (auto voxel : octree)
    {
        ++nb_count_voxel;
    }
    auto t3 = std::chrono::high_resolution_clock::now();

    auto it = octree.begin();
    while (it != octree.end())
    {
        it = octree.removeVoxel(it);
    }
    auto t4 = std::chrono::high_resolution_clock::now();

    size_t nb_remaining_voxel = 0;
    octree.exploreVoxel([&nb_remaining_voxel](voxomap::VoxelNode<T_Area> const&, voxel const&, uint8_t, uint8_t, uint8_t) {
        ++nb_remaining_voxel;
    });

    if (nb_added_voxel == nb_count_voxel)
        std::cout << "No error detected, go through all voxels." << std::endl;
    else
        std::cout << "Error: forget to go through " << (nb_added_voxel - nb_count_voxel) << " voxels." << std::endl;

    if (nb_remaining_voxel == 0)
        std::cout << "No error detected" << std::endl;
    else
        std::cout << "Error: there still are " << nb_remaining_voxel << " voxels inside the octree." << std::endl;

    int add_time = static_cast<int>(std::chrono::duration<double, std::milli>(t2 - t1).count());
    int read_time = static_cast<int>(std::chrono::duration<double, std::milli>(t3 - t2).count());
    int rm_time = static_cast<int>(std::chrono::duration<double, std::milli>(t4 - t3).count());
    std::cout << "Added " << nb_added_voxel << " voxels in " << add_time << "ms, " << int(nb_added_voxel / float(add_time / 1000.f)) << " voxels/s." << std::endl;
    std::cout << "Read " << nb_added_voxel << " voxels in " << read_time << "ms, " << int(nb_added_voxel / float(read_time / 1000.f)) << " voxels/s." << std::endl;
    std::cout << "Remove " << nb_added_voxel << " voxels in " << rm_time << "ms, " << int(nb_added_voxel / float(rm_time / 1000.f)) << " voxels/s." << std::endl;
    std::cout << "Total time: " << static_cast<int>(std::chrono::duration<double, std::milli>(t4 - t1).count()) << "ms." << std::endl;
    std::cout << std::endl;
}

template <typename T_Area>
static void checkError(typename T_Area::VoxelData const& voxel, typename T_Area::iterator const& it, voxomap::FaceEnum face, size_t& nb_error)
{
    if (voxel & face)
    {
        if (!it)
            ++nb_error;
    }
    else if (it)
        ++nb_error;
}

template <typename T_Area>
void test_face_area()
{
    std::cout << "Launch test_face_area:" << std::endl;

    const size_t nb_voxel = 500000;
    voxomap::VoxelOctree<T_Area> octree;
    std::mt19937 r(84);
    size_t nb_added_voxel = 0;

    auto t1 = std::chrono::high_resolution_clock::now();
    for (size_t i = 0; i < nb_voxel; ++i)
    {
        int x = r() % 1000 - 500;
        int y = r() % 1000 - 500;
        int z = r() % 1000 - 500;

        if (octree.addVoxel(x, y, z, r()).second)
            ++nb_added_voxel;
    }
    auto t2 = std::chrono::high_resolution_clock::now();

    size_t nb_error = 0;
    for (auto it : octree)
    {
        voxomap::VoxelArea<T_Area> search_area(*it);

        if ((it->node->getX() == 120 || it->node->getX() == 128) && it->node->getY() == 104 && it->node->getZ() == 312 &&
            (it->x == 0 || it->x == 7) && it->y == 7 && it->z == 7)
        {
            checkError<T_Area>(*it->voxel, search_area.findVoxel(1, 0, 0), voxomap::RIGHT, nb_error);
            checkError<T_Area>(*it->voxel, search_area.findVoxel(-1, 0, 0), voxomap::LEFT, nb_error);
            checkError<T_Area>(*it->voxel, search_area.findVoxel(0, 1, 0), voxomap::TOP, nb_error);
            checkError<T_Area>(*it->voxel, search_area.findVoxel(0, -1, 0), voxomap::BOTTOM, nb_error);
            checkError<T_Area>(*it->voxel, search_area.findVoxel(0, 0, 1), voxomap::BACK, nb_error);
            checkError<T_Area>(*it->voxel, search_area.findVoxel(0, 0, -1), voxomap::FRONT, nb_error);
        }
    }
    auto t3 = std::chrono::high_resolution_clock::now();

    if (nb_error == 0)
        std::cout << "No error detected" << std::endl;
    else
        std::cout << "Error: there is " << nb_error << " errors." << std::endl;

    int add_time = static_cast<int>(std::chrono::duration<double, std::milli>(t2 - t1).count());
    int check_time = static_cast<int>(std::chrono::duration<double, std::milli>(t3 - t2).count());
    std::cout << "Added " << nb_added_voxel << " voxels in " << add_time << "ms, " << int(nb_added_voxel / float(add_time / 1000.f)) << " voxels/s." << std::endl;
    std::cout << "Check Face in " << check_time << "ms." << std::endl;
    std::cout << "Total time: " << static_cast<int>(std::chrono::duration<double, std::milli>(t3 - t1).count()) << "ms." << std::endl;
    std::cout << std::endl;
}

template <typename T_Voxel>
using SmartArea = voxomap::SmartArea<T_Voxel>;

int main(int argc, char* argv[])
{
    // Test with SmartArea
    bench_random<voxomap::SmartArea<voxel>>();
    bench_continuous<voxomap::SmartArea<voxel>>();
    bench_update_voxel<voxomap::SmartArea<voxel>>();
    bench_voxel_area<voxomap::SmartArea<voxel>>();
    test_iterator<voxomap::SmartArea<voxel>>();

    // Test with ArrayArea
    bench_random<voxomap::ArrayArea<voxel>>();
    bench_continuous<voxomap::ArrayArea<voxel>>();
    bench_update_voxel<voxomap::ArrayArea<voxel>>();
    bench_voxel_area<voxomap::ArrayArea<voxel>>();
    test_iterator<voxomap::ArrayArea<voxel>>();

    // Test with FaceArea<SmartArea>
    bench_random<voxomap::FaceArea<SmartArea, voxel>>();
    bench_continuous<voxomap::FaceArea<SmartArea, voxel>>();
    bench_update_voxel<voxomap::FaceArea<SmartArea, voxel>>();
    bench_voxel_area<voxomap::FaceArea<SmartArea, voxel>>();
    test_iterator<voxomap::FaceArea<SmartArea, voxel>>();
    test_face_area<voxomap::FaceArea<SmartArea, voxel>>();

    // Test with FaceArea<ArrayArea>
    bench_random<voxomap::FaceArea<voxomap::ArrayArea, voxel>>();
    bench_continuous<voxomap::FaceArea<voxomap::ArrayArea, voxel>>();
    bench_update_voxel<voxomap::FaceArea<voxomap::ArrayArea, voxel>>();
    bench_voxel_area<voxomap::FaceArea<voxomap::ArrayArea, voxel>>();
    test_iterator<voxomap::FaceArea<voxomap::ArrayArea, voxel>>();
    test_face_area<voxomap::FaceArea<voxomap::ArrayArea, voxel>>();
};
