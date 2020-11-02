#include <iostream>
#include <chrono>
#include "../voxel_octree/VoxelOctree.hpp"
#include "../voxel_octree/VoxelContainer/SparseContainer.hpp"
#include "../voxel_octree/VoxelContainer/ArrayContainer.hpp"
#include "../voxel_octree/VoxelContainer/SidedContainer.hpp"
#include "../voxel_octree/SuperContainer/ArraySuperContainer.hpp"
#include "../voxel_octree/SuperContainer/SparseSuperContainer.hpp"
#include "../utils/LocalSearchUtility.hpp"
#include "common.hpp"

static const size_t gNbVoxel = 500000;
static size_t gNbError = 0;

template <typename T_Container>
bool test_find_relative_voxel()
{
    voxomap::test::initGlobalValues(gNbVoxel);

    std::cout << "Launch test_find_relative_voxel (" << voxomap::test::type_name<T_Container>() << "):" << std::endl;

    voxomap::VoxelOctree<T_Container> octree;

    auto t1 = std::chrono::high_resolution_clock::now();
    for (size_t i = 0; i < voxomap::test::gTestValues.size(); ++i)
    {
        auto const& data = voxomap::test::gTestValues[i];
        octree.addVoxel(data.x, data.y, data.z, data.value);
    }
    auto t2 = std::chrono::high_resolution_clock::now();

    size_t read_nb_error = 0;
    size_t nb_error = 0;
    size_t nb_found_voxel = 0;
    for (size_t i = 0; i < voxomap::test::gTestValues.size() / 4; ++i)
    {
        auto const& data = voxomap::test::gTestValues[i];
        auto it = octree.findVoxel(data.x, data.y, data.z);

        if (!it || it.voxel->value != data.value)
            ++read_nb_error;
        else
        {
            int ox = data.x - it.node->getX();
            int oy = data.y - it.node->getY();
            int oz = data.z - it.node->getZ();

            for (int ix = -4; ix < 4; ++ix)
            {
                for (int iy = -4; iy < 4; ++iy)
                {
                    for (int iz = -4; iz < 4; ++iz)
                    {
                        auto nit = octree.findVoxel(data.x + ix, data.y + iy, data.z + iz);
                        if (nit)
                            ++nb_found_voxel;
                        auto rit = it.node->findRelativeVoxel(ox + ix, oy + iy, oz + iz);
                        if (nit != rit)
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
        std::cout << "No container error detected" << std::endl;
    else
        std::cout << "Error: " << nb_error << " container errors detected" << std::endl;

    int add_time = static_cast<int>(std::chrono::duration<double, std::milli>(t2 - t1).count());
    int read_time = static_cast<int>(std::chrono::duration<double, std::milli>(t3 - t2).count());
    std::cout << "Total time: " << static_cast<int>(std::chrono::duration<double, std::milli>(t3 - t1).count()) << "ms." << std::endl;
    std::cout << std::endl;

    return read_nb_error == 0 && nb_found_voxel != 0 && nb_error == 0;
}

template <typename T_Container>
bool test_find_relative_voxel_with_cache()
{
    voxomap::test::initGlobalValues(gNbVoxel);

    std::cout << "Launch test_find_relative_voxel_with_cache (" << voxomap::test::type_name<T_Container>() << "):" << std::endl;

    voxomap::VoxelOctree<T_Container> octree;

    auto t1 = std::chrono::high_resolution_clock::now();
    for (size_t i = 0; i < voxomap::test::gTestValues.size(); ++i)
    {
        auto const& data = voxomap::test::gTestValues[i];
        octree.addVoxel(data.x, data.y, data.z, data.value);
    }
    auto t2 = std::chrono::high_resolution_clock::now();

    size_t read_nb_error = 0;
    size_t nb_error = 0;
    size_t nb_found_voxel = 0;
    for (size_t i = 0; i < voxomap::test::gTestValues.size() / 4; ++i)
    {
        auto const& data = voxomap::test::gTestValues[i];
        auto it = octree.findVoxel(data.x, data.y, data.z);

        if (!it || it.voxel->value != data.value)
            ++read_nb_error;
        else
        {
            int ox = data.x - it.node->getX();
            int oy = data.y - it.node->getY();
            int oz = data.z - it.node->getZ();
            typename voxomap::VoxelNode<T_Container>::NeighborAreaCache neighbor_cache;

            for (int ix = -4; ix < 4; ++ix)
            {
                for (int iy = -4; iy < 4; ++iy)
                {
                    for (int iz = -4; iz < 4; ++iz)
                    {
                        auto nit = octree.findVoxel(data.x + ix, data.y + iy, data.z + iz);
                        if (nit)
                            ++nb_found_voxel;

                        auto rit = it.node->findRelativeVoxel(neighbor_cache, ox + ix, oy + iy, oz + iz);
                        if (nit != rit)
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
        std::cout << "No container error detected" << std::endl;
    else
        std::cout << "Error: " << nb_error << " container errors detected" << std::endl;

    int add_time = static_cast<int>(std::chrono::duration<double, std::milli>(t2 - t1).count());
    int read_time = static_cast<int>(std::chrono::duration<double, std::milli>(t3 - t2).count());
    std::cout << "Total time: " << static_cast<int>(std::chrono::duration<double, std::milli>(t3 - t1).count()) << "ms." << std::endl;
    std::cout << std::endl;

    return read_nb_error == 0 && nb_found_voxel != 0 && nb_error == 0;
}

template <typename T_Container>
bool test_local_search_utility()
{
    voxomap::test::initGlobalValues(gNbVoxel);

    std::cout << "Launch test_local_search_utility (" << voxomap::test::type_name<T_Container>() << "):" << std::endl;

    voxomap::VoxelOctree<T_Container> octree;

    auto t1 = std::chrono::high_resolution_clock::now();
    for (size_t i = 0; i < voxomap::test::gTestValues.size(); ++i)
    {
        auto const& data = voxomap::test::gTestValues[i];
        octree.addVoxel(data.x, data.y, data.z, data.value);
    }
    auto t2 = std::chrono::high_resolution_clock::now();

    size_t read_nb_error = 0;
    size_t nb_error = 0;
    size_t nb_found_voxel = 0;
    for (size_t i = 0; i < voxomap::test::gTestValues.size() / 4; ++i)
    {
        auto const& data = voxomap::test::gTestValues[i];
        auto it = octree.findVoxel(data.x, data.y, data.z);

        if (!it || it.voxel->value != data.value)
            ++read_nb_error;
        else
        {
            voxomap::LocalSearchUtility<T_Container> search_area(it);

            for (int ix = -4; ix < 4; ++ix)
            {
                for (int iy = -4; iy < 4; ++iy)
                {
                    for (int iz = -4; iz < 4; ++iz)
                    {
                        auto rit = octree.findVoxel(data.x + ix, data.y + iy, data.z + iz);
                        if (rit)
                            ++nb_found_voxel;

                        auto pit = search_area.findVoxel(ix, iy, iz);
                        if (rit != pit)
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
        std::cout << "No container error detected" << std::endl;
    else
        std::cout << "Error: " << nb_error << " container errors detected" << std::endl;

    int add_time = static_cast<int>(std::chrono::duration<double, std::milli>(t2 - t1).count());
    int read_time = static_cast<int>(std::chrono::duration<double, std::milli>(t3 - t2).count());
    std::cout << "Total time: " << static_cast<int>(std::chrono::duration<double, std::milli>(t3 - t1).count()) << "ms." << std::endl;
    std::cout << std::endl;

    return read_nb_error == 0 && nb_found_voxel != 0 && nb_error == 0;
}

template <typename T_Container>
bool test_iterator()
{
    voxomap::test::initGlobalValues(gNbVoxel);

    std::cout << "Launch test_iterator (" << voxomap::test::type_name<T_Container>() << "):" << std::endl;

    voxomap::VoxelOctree<T_Container> octree;
    size_t nb_added_voxel = 0;
    size_t nb_count_voxel_position = 0;

    auto t1 = std::chrono::high_resolution_clock::now();
    for (size_t i = 0; i < voxomap::test::gTestValues.size(); ++i)
    {
        auto const& data = voxomap::test::gTestValues[i];
        auto pair = octree.addVoxel(data.x, data.y, data.z, data.value);
        if (pair.second)
        {
            ++nb_added_voxel;
            int cx, cy, cz;
            pair.first.getVoxelPosition(cx, cy, cz);
            if (data.x == cx && data.y == cy && data.z == cz)
                ++nb_count_voxel_position;
        }
    }
    auto t2 = std::chrono::high_resolution_clock::now();

    size_t nb_count_voxel = 0;
    for (auto it : octree)
    {
        ++nb_count_voxel;
    }
    auto t3 = std::chrono::high_resolution_clock::now();

    size_t nb_count_voxel_explored = 0;
    octree.exploreVoxel([&nb_count_voxel_explored](typename T_Container::iterator const&) {
        ++nb_count_voxel_explored;
    });
    auto t4 = std::chrono::high_resolution_clock::now();

    auto it = octree.begin();
    while (it != octree.end())
    {
        octree.removeVoxel(it++);
    }
    auto t5 = std::chrono::high_resolution_clock::now();

    size_t nb_remaining_voxel = 0;
    octree.exploreVoxel([&nb_remaining_voxel](typename T_Container::iterator const&) {
        ++nb_remaining_voxel;
    });

    if (nb_added_voxel == nb_count_voxel)
        std::cout << "No error detected, go through all voxels." << std::endl;
    else
        std::cout << "Error: forget to go through " << (nb_added_voxel - nb_count_voxel) << " voxels." << std::endl;

    if (nb_count_voxel_position == nb_count_voxel)
        std::cout << "No error detected, check all voxel positions." << std::endl;
    else
        std::cout << "Error: bad position on " << (nb_count_voxel_position - nb_count_voxel) << " voxels." << std::endl;

    if (nb_remaining_voxel == 0)
        std::cout << "No error detected" << std::endl;
    else
        std::cout << "Error: there still are " << nb_remaining_voxel << " voxels inside the octree." << std::endl;

    int add_time = static_cast<int>(std::chrono::duration<double, std::milli>(t2 - t1).count());
    int read_time = static_cast<int>(std::chrono::duration<double, std::milli>(t3 - t2).count());
    int explore_time = static_cast<int>(std::chrono::duration<double, std::milli>(t4 - t3).count());
    int rm_time = static_cast<int>(std::chrono::duration<double, std::milli>(t5 - t4).count());
    std::cout << "Added " << nb_added_voxel << " voxels in " << add_time << "ms, " << int(nb_added_voxel / float(add_time / 1000.f)) << " voxels/s." << std::endl;
    std::cout << "Read " << nb_added_voxel << " voxels in " << read_time << "ms, " << int(nb_added_voxel / float(read_time / 1000.f)) << " voxels/s." << std::endl;
    std::cout << "Explore " << nb_count_voxel_explored << " voxels in " << explore_time << "ms, " << int(nb_count_voxel_explored / float(explore_time / 1000.f)) << " voxels/s." << std::endl;
    std::cout << "Remove " << nb_added_voxel << " voxels in " << rm_time << "ms, " << int(nb_added_voxel / float(rm_time / 1000.f)) << " voxels/s." << std::endl;
    std::cout << "Total time: " << static_cast<int>(std::chrono::duration<double, std::milli>(t4 - t1).count()) << "ms." << std::endl;
    std::cout << std::endl;

    return nb_added_voxel == nb_count_voxel && nb_count_voxel_position == nb_count_voxel && nb_remaining_voxel == 0;
}

template <typename T_Container>
static void checkError(typename T_Container::VoxelData const& voxel, typename T_Container::iterator const& it, voxomap::SideEnum side, size_t& nb_error)
{
    if (voxel & side)
    {
        if (!it)
            ++nb_error;
    }
    else if (it)
        ++nb_error;
}

template <typename T_Container>
bool test_sided_container()
{
    voxomap::test::initGlobalValues(gNbVoxel);

    std::cout << "Launch test_sided_container (" << voxomap::test::type_name<T_Container>() << "):" << std::endl;

    voxomap::VoxelOctree<T_Container> octree;
    size_t nb_added_voxel = 0;

    auto t1 = std::chrono::high_resolution_clock::now();
    for (size_t i = 0; i < voxomap::test::gTestValues.size(); ++i)
    {
        auto const& data = voxomap::test::gTestValues[i];
        if (octree.addVoxel(data.x, data.y, data.z, data.value).second)
            ++nb_added_voxel;
    }
    auto t2 = std::chrono::high_resolution_clock::now();

    size_t nb_error = 0;
    for (auto it : octree)
    {
        voxomap::LocalSearchUtility<T_Container> search_area(*it);

        if ((it->node->getX() == 120 || it->node->getX() == 128) && it->node->getY() == 104 && it->node->getZ() == 312 &&
            (it->x == 0 || it->x == 7) && it->y == 7 && it->z == 7)
        {
            checkError<T_Container>(*it->voxel, search_area.findVoxel(1, 0, 0), voxomap::XPOS, nb_error);
            checkError<T_Container>(*it->voxel, search_area.findVoxel(-1, 0, 0), voxomap::XNEG, nb_error);
            checkError<T_Container>(*it->voxel, search_area.findVoxel(0, 1, 0), voxomap::YPOS, nb_error);
            checkError<T_Container>(*it->voxel, search_area.findVoxel(0, -1, 0), voxomap::YNEG, nb_error);
            checkError<T_Container>(*it->voxel, search_area.findVoxel(0, 0, 1), voxomap::ZPOS, nb_error);
            checkError<T_Container>(*it->voxel, search_area.findVoxel(0, 0, -1), voxomap::ZNEG, nb_error);
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
    std::cout << "Check Side in " << check_time << "ms." << std::endl;
    std::cout << "Total time: " << static_cast<int>(std::chrono::duration<double, std::milli>(t3 - t1).count()) << "ms." << std::endl;
    std::cout << std::endl;

    return nb_error == 0;
}

template <typename T_Container>
bool test_remove_voxel()
{
    voxomap::test::initGlobalValues(gNbVoxel);

    std::cout << "Launch test_remove_voxel (" << voxomap::test::type_name<T_Container>() << "):" << std::endl;
    voxomap::VoxelOctree<T_Container> octree;

    std::vector<voxomap::test::Data> testValues;
    std::vector<voxomap::test::Data> removeTestValues;
    std::mt19937 r(42);
    for (auto const& data : voxomap::test::gTestValues)
    {
        if (r() % 3)
            removeTestValues.emplace_back(data);
        else
            testValues.emplace_back(data);
    }

    auto t1 = std::chrono::high_resolution_clock::now();
    for (auto const& data : voxomap::test::gTestValues)
        octree.putVoxel(data.x, data.y, data.z, data.value);
    for (auto const& data : removeTestValues)
        octree.removeVoxel(data.x, data.y, data.z);
    auto t2 = std::chrono::high_resolution_clock::now();

    size_t read_nb_error = 0;
    for (auto const& data : testValues)
    {
        auto it = octree.findVoxel(data.x, data.y, data.z);
        if (!it || it.voxel->value != data.value)
            ++read_nb_error;
    }
    for (auto const& data : removeTestValues)
    {
        auto it = octree.findVoxel(data.x, data.y, data.z);
        if (it)
            ++read_nb_error;
    }
    auto t3 = std::chrono::high_resolution_clock::now();

    if (read_nb_error == 0)
        std::cout << "No error detected" << std::endl;
    else
        std::cout << "Error: there is " << read_nb_error << " errors." << std::endl;

    std::cout << "Total time: " << static_cast<int>(std::chrono::duration<double, std::milli>(t3 - t1).count()) << "ms." << std::endl;
    std::cout << std::endl;

    return read_nb_error == 0;
}

template <typename T_Container>
bool test_serialization()
{
    voxomap::test::initGlobalValues(gNbVoxel);

    std::cout << "Launch test_serialization (" << voxomap::test::type_name<T_Container>() << "):" << std::endl;
    voxomap::VoxelOctree<T_Container> octree;

    std::vector<voxomap::test::Data> testValues;
    std::vector<voxomap::test::Data> removeTestValues;
    std::mt19937 r(42);
    for (auto const& data : voxomap::test::gTestValues)
    {
        if (r() % 3)
            removeTestValues.emplace_back(data);
        else
            testValues.emplace_back(data);
    }

    auto t1 = std::chrono::high_resolution_clock::now();
    for (auto const& data : voxomap::test::gTestValues)
        octree.putVoxel(data.x, data.y, data.z, data.value);
    for (auto const& data : removeTestValues)
        octree.removeVoxel(data.x, data.y, data.z);
    auto t2 = std::chrono::high_resolution_clock::now();

    size_t size = 0;
    voxomap::VoxelOctree<T_Container> octree_2;
    {
        std::string serialized_octree;
        octree.serialize(serialized_octree);
        size = serialized_octree.size();
        octree.clear();
        octree_2.unserialize(serialized_octree.data(), serialized_octree.size());
    }
    auto t3 = std::chrono::high_resolution_clock::now();

    size_t read_nb_error = 0;
    for (auto const& data : testValues)
    {
        auto it = octree_2.findVoxel(data.x, data.y, data.z);
        if (!it || it.voxel->value != data.value)
            ++read_nb_error;
    }
    for (auto const& data : removeTestValues)
    {
        auto it = octree_2.findVoxel(data.x, data.y, data.z);
        if (it)
            ++read_nb_error;
    }
    auto t4 = std::chrono::high_resolution_clock::now();

    if (read_nb_error == 0)
        std::cout << "No error detected" << std::endl;
    else
        std::cout << "Error: there is " << read_nb_error << " errors." << std::endl;

    std::cout << "Serialization size: ";
    if (size < 1024)
        std::cout << size << "B.";
    else if (size < (1024 * 1024))
        std::cout << (size / 1024) << "KB.";
    else
        std::cout << (size / (1024 * 1024)) << "MB.";
    std::cout << std::endl;

    std::cout << "Serialization time: " << static_cast<int>(std::chrono::duration<double, std::milli>(t3 - t2).count()) << "ms." << std::endl;
    std::cout << "Total time: " << static_cast<int>(std::chrono::duration<double, std::milli>(t4 - t1).count()) << "ms." << std::endl;
    std::cout << std::endl;

    return read_nb_error == 0;
}

static bool g_error = false;

template <typename T_Container>
void launchTest()
{
    std::cout << "------- TEST " << voxomap::test::type_name<T_Container>() << " -------\n\n";
    g_error |= !test_iterator<T_Container>();
    g_error |= !test_remove_voxel<T_Container>();
    g_error |= !test_serialization<T_Container>();
    g_error |= !test_find_relative_voxel<T_Container>();
    g_error |= !test_find_relative_voxel_with_cache<T_Container>();
    g_error |= !test_local_search_utility<T_Container>();
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
    test_sided_container<voxomap::SidedContainer<SparseContainer, voxel>>();
    launchTest<voxomap::SidedContainer<SparseContainer, voxel>>();
    test_sided_container<voxomap::SidedContainer<voxomap::ArrayContainer, voxel>>();
    launchTest<voxomap::SidedContainer<voxomap::ArrayContainer, voxel>>();

    // One super container
    launchTest<voxomap::SparseSuperContainer<voxomap::SparseContainer<voxel>>>();
    launchTest<voxomap::ArraySuperContainer<voxomap::ArrayContainer<voxel>>>();
    test_sided_container<voxomap::SparseSuperContainer<voxomap::SidedContainer<SparseContainer, voxel>>>();
    launchTest<voxomap::SparseSuperContainer<voxomap::SidedContainer<SparseContainer, voxel>>>();
    test_sided_container<voxomap::ArraySuperContainer<voxomap::SidedContainer<voxomap::ArrayContainer, voxel>>>();
    launchTest<voxomap::ArraySuperContainer<voxomap::SidedContainer<voxomap::ArrayContainer, voxel>>>();

    // Multiple super container
    launchTest<voxomap::SparseSuperContainer<voxomap::SparseSuperContainer<voxomap::SparseContainer<voxel>>>>();
    launchTest<voxomap::SparseSuperContainer<voxomap::SparseSuperContainer<voxomap::ArraySuperContainer<voxomap::SparseContainer<voxel>>>>>();
    launchTest<voxomap::ArraySuperContainer<voxomap::SparseSuperContainer<voxomap::ArraySuperContainer<voxomap::SparseContainer<voxel>>>>>();
    test_sided_container<voxomap::SparseSuperContainer<voxomap::SparseSuperContainer<voxomap::SidedContainer<SparseContainer, voxel>>>>();
    launchTest<voxomap::SparseSuperContainer<voxomap::SparseSuperContainer<voxomap::SidedContainer<SparseContainer, voxel>>>>();
    test_sided_container<voxomap::ArraySuperContainer<voxomap::SparseSuperContainer<voxomap::ArraySuperContainer<voxomap::SidedContainer<voxomap::ArrayContainer, voxel>>>>>();
    launchTest<voxomap::ArraySuperContainer<voxomap::SparseSuperContainer<voxomap::ArraySuperContainer<voxomap::SidedContainer<voxomap::ArrayContainer, voxel>>>>>();

    std::cout << "------ FINAL RESULT: ";
    if (g_error)
    {
        std::cout << "ERROR ERROR ERROR ERROR There is errors !!! ------" << std::endl;
        return EXIT_FAILURE;
    }

    std::cout << "GOOD. ------" << std::endl;
    return EXIT_SUCCESS;
};