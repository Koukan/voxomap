#include <iostream>
#include <chrono>
#include <map>
#include <fstream>
#include "../voxel_octree/VoxelOctree.hpp"
#include "../voxel_octree/VoxelContainer/SparseContainer.hpp"
#include "../voxel_octree/VoxelContainer/ArrayContainer.hpp"
#include "../voxel_octree/VoxelContainer/SidedContainer.hpp"
#include "../voxel_octree/SuperContainer/ArraySuperContainer.hpp"
#include "../voxel_octree/SuperContainer/SparseSuperContainer.hpp"
#include "common.hpp"

static const size_t gNbVoxel = 500000;
static const size_t gContiguousArea = 256;
static size_t gNbError = 0;

class Report
{
public:
    static Report& get()
    {
        static Report instance;
        return instance;
    }

    template <typename ...Args>
    void addValues(std::string const& benchName, std::string const& className, Args&&... args)
    {
        _lines.emplace_back(benchName);
        _lines.back().append(";");
        _lines.back().append(className);
        this->_addValues(std::forward<Args>(args)...);
    }

    void writeCSV(std::string const& path)
    {
        std::fstream file;
        file.open(path.c_str(), std::ios_base::app | std::ios_base::out);
        if (file.bad())
            return;

        for (auto const& line : _lines)
        {
            file << line << '\n';
        }
    }

private:
    template <typename Value, typename... Args>
    void _addValues(char const* key, Value&& value, Args&&... args)
    {
        this->addValue(key, std::forward<Value>(value));
        this->_addValues(std::forward<Args>(args)...);
    }

    template <typename Value>
    void addValue(char const* key, Value value)
    {
        this->addValue(key, std::to_string(value));
    }

    void _addValues()
    {
    }

    std::vector<std::string> _lines;
};

template <>
void Report::addValue(char const* key, std::string value)
{
    _lines.back().append(";");
    _lines.back().append(key);
    _lines.back().append("=");
    _lines.back().append(value);
}

/*!
*/
template <typename T_Container>
bool bench_random()
{
    voxomap::test::initGlobalValues(gNbVoxel);
    auto memoryBeforeUsed = voxomap::test::computeMemoryUsed();

    std::string className = voxomap::test::type_name<T_Container>();
    std::cout << "Launch bench_random (" << className << "):" << std::endl;

    auto t1 = std::chrono::high_resolution_clock::now();
    voxomap::VoxelOctree<T_Container> octree;

    for (size_t i = 0; i < voxomap::test::gTestValues.size(); ++i)
    {
        auto const& data = voxomap::test::gTestValues[i];
        octree.putVoxel(data.x, data.y, data.z, data.value);
    }
    auto t2 = std::chrono::high_resolution_clock::now();
    auto memoryUsed = voxomap::test::computeMemoryUsed();

    size_t read_nb_error = 0;
    for (size_t i = 0; i < voxomap::test::gTestValues.size(); ++i)
    {
        auto const& data = voxomap::test::gTestValues[i];
        auto it = octree.findVoxel(data.x, data.y, data.z);
        if (!it || it.voxel->value != data.value)
            ++read_nb_error;
    }
    auto t3 = std::chrono::high_resolution_clock::now();

    size_t it_count = 0;
    for (auto it : octree)
    {
        ++it_count;
    }
    auto t4 = std::chrono::high_resolution_clock::now();

    size_t update_nb_error = 0;
    for (size_t i = 0; i < voxomap::test::gTestValues.size(); ++i)
    {
        auto const& data = voxomap::test::gTestValues[i];
        auto it = octree.updateVoxel(data.x, data.y, data.z, data.value + 1);
        if (!it || it.voxel->value != data.value + 1)
            ++update_nb_error;
    }
    auto t5 = std::chrono::high_resolution_clock::now();

    size_t rm_nb_error = 0;
    for (size_t i = 0; i < voxomap::test::gTestValues.size(); ++i)
    {
        auto const& data = voxomap::test::gTestValues[i];
        if (!octree.removeVoxel(data.x, data.y, data.z))
            ++rm_nb_error;
    }
    auto t6 = std::chrono::high_resolution_clock::now();

    if (read_nb_error == 0)
        std::cout << "No read error detected" << std::endl;
    else
        std::cout << "Error: " << read_nb_error << " read errors detected" << std::endl;

    if (rm_nb_error == 0)
        std::cout << "No remove error detected" << std::endl;
    else
        std::cout << "Error: " << rm_nb_error << " remove errors detected" << std::endl;

    if (update_nb_error == 0)
        std::cout << "No update error detected" << std::endl;
    else
        std::cout << "Error: " << update_nb_error << " update errors detected" << std::endl;

    if (it_count == gNbVoxel)
        std::cout << "No iterator error detected" << std::endl;
    else
        std::cout << "Error: " << (gNbVoxel - it_count) << " iterator errors detected" << std::endl;

    int add_time = static_cast<int>(std::chrono::duration<double, std::milli>(t2 - t1).count());
    int read_time = static_cast<int>(std::chrono::duration<double, std::milli>(t3 - t2).count());
    int it_time = static_cast<int>(std::chrono::duration<double, std::milli>(t4 - t3).count());
    int update_time = static_cast<int>(std::chrono::duration<double, std::milli>(t5 - t4).count());
    int rm_time = static_cast<int>(std::chrono::duration<double, std::milli>(t6 - t5).count());
    size_t memory = ((memoryUsed - memoryBeforeUsed) / 1024 / 1024);
    std::cout << "Added " << gNbVoxel << " voxels in " << add_time << "ms, " << int(gNbVoxel / float(add_time / 1000.f)) << " voxels/s." << std::endl;
    std::cout << "Read " << gNbVoxel << " voxels in " << read_time << "ms, " << int(gNbVoxel / float(read_time / 1000.f)) << " voxels/s." << std::endl;
    std::cout << "Iterator " << gNbVoxel << " voxels in " << it_time << "ms, " << int(gNbVoxel / float(it_time / 1000.f)) << " voxels/s." << std::endl;
    std::cout << "Update " << gNbVoxel << " voxels in " << update_time << "ms, " << int(gNbVoxel / float(update_time / 1000.f)) << " voxels/s." << std::endl;
    std::cout << "Remove " << gNbVoxel << " voxels in " << rm_time << "ms, " << int(gNbVoxel / float(rm_time / 1000.f)) << " voxels/s." << std::endl;
    std::cout << "Total time: " << static_cast<int>(std::chrono::duration<double, std::milli>(t5 - t1).count()) << "ms." << std::endl;
    std::cout << "Total memory used " << memory << "MB" << std::endl;
    std::cout << std::endl;

    Report::get().addValues("random", className,
        "add", int(gNbVoxel / float(add_time / 1000.f)),
        "remove", int(gNbVoxel / float(rm_time / 1000.f)),
        "read", int(gNbVoxel / float(read_time / 1000.f)),
        "iterator", int(gNbVoxel / float(it_time / 1000.f)),
        "update", int(gNbVoxel / float(update_time / 1000.f)),
        "memory", memory,
        "nb_voxel", std::to_string(gNbVoxel)
    );

    return read_nb_error == 0 && rm_nb_error == 0 && update_nb_error == 0 && it_count == gNbVoxel;
}

template <typename T_Container>
bool bench_contiguous()
{
    std::string className = voxomap::test::type_name<T_Container>();
    std::cout << "Launch bench_continuous (" << className << "):" << std::endl;

    std::mt19937 r(42);
    std::vector<voxomap::test::Data> testValues;

    for (int x = 0; x < gContiguousArea; ++x)
    {
        for (int y = 0; y < gContiguousArea; ++y)
        {
            for (int z = 0; z < gContiguousArea; ++z)
            {
                if (r() % 6 < 5)
                    testValues.emplace_back(x, y, z, r());
            }
        }
    }
    auto memoryBeforeUsed = voxomap::test::computeMemoryUsed();

    auto t1 = std::chrono::high_resolution_clock::now();
    voxomap::VoxelOctree<T_Container> octree;

    for (auto const& data : testValues)
    {
        octree.putVoxel(data.x, data.y, data.z, data.value);
    }
    auto t2 = std::chrono::high_resolution_clock::now();
    auto memoryUsed = voxomap::test::computeMemoryUsed();

    size_t read_nb_error = 0;
    for (auto const& data : testValues)
    {
        auto it = octree.findVoxel(data.x, data.y, data.z);
        if (!it || it.voxel->value != data.value)
            ++read_nb_error;
    }
    auto t3 = std::chrono::high_resolution_clock::now();

    size_t it_count = 0;
    for (auto it : octree)
    {
        ++it_count;
    }
    auto t4 = std::chrono::high_resolution_clock::now();

    size_t update_nb_error = 0;
    for (auto const& data : testValues)
    {
        auto it = octree.updateVoxel(data.x, data.y, data.z, data.value + 1);
        if (!it || it.voxel->value != data.value + 1)
            ++update_nb_error;
    }
    auto t5 = std::chrono::high_resolution_clock::now();

    size_t rm_nb_error = 0;
    for (auto const& data : testValues)
    {
        if (!octree.removeVoxel(data.x, data.y, data.z))
            ++rm_nb_error;
    }
    auto t6 = std::chrono::high_resolution_clock::now();

    if (read_nb_error == 0)
        std::cout << "No read error detected" << std::endl;
    else
        std::cout << "Error: " << read_nb_error << " read errors detected" << std::endl;

    if (rm_nb_error == 0)
        std::cout << "No remove error detected" << std::endl;
    else
        std::cout << "Error: " << rm_nb_error << " remove errors detected" << std::endl;

    if (update_nb_error == 0)
        std::cout << "No update error detected" << std::endl;
    else
        std::cout << "Error: " << update_nb_error << " update errors detected" << std::endl;

    if (it_count == testValues.size())
        std::cout << "No iterator error detected" << std::endl;
    else
        std::cout << "Error: " << (testValues.size() - it_count) << " iterator errors detected" << std::endl;

    int add_time = static_cast<int>(std::chrono::duration<double, std::milli>(t2 - t1).count());
    int read_time = static_cast<int>(std::chrono::duration<double, std::milli>(t3 - t2).count());
    int it_time = static_cast<int>(std::chrono::duration<double, std::milli>(t4 - t3).count());
    int update_time = static_cast<int>(std::chrono::duration<double, std::milli>(t5 - t4).count());
    int rm_time = static_cast<int>(std::chrono::duration<double, std::milli>(t6 - t5).count());
    size_t memory = ((memoryUsed - memoryBeforeUsed) / 1024 / 1024);
    std::cout << "Added " << testValues.size() << " voxels in " << add_time << "ms, " << int(testValues.size() / float(add_time / 1000.f)) << " voxels/s." << std::endl;
    std::cout << "Read " << testValues.size() << " voxels in " << read_time << "ms, " << int(testValues.size() / float(read_time / 1000.f)) << " voxels/s." << std::endl;
    std::cout << "Iterator " << testValues.size() << " voxels in " << it_time << "ms, " << int(testValues.size() / float(it_time / 1000.f)) << " voxels/s." << std::endl;
    std::cout << "Update " << testValues.size() << " voxels in " << update_time << "ms, " << int(testValues.size() / float(update_time / 1000.f)) << " voxels/s." << std::endl;
    std::cout << "Remove " << testValues.size() << " voxels in " << rm_time << "ms, " << int(testValues.size() / float(rm_time / 1000.f)) << " voxels/s." << std::endl;
    std::cout << "Total time: " << static_cast<int>(std::chrono::duration<double, std::milli>(t5 - t1).count()) << "ms." << std::endl;
    std::cout << "Total memory used " << memory << "MB" << std::endl;
    std::cout << std::endl;

    Report::get().addValues("contiguous", className,
        "add", int(testValues.size() / float(add_time / 1000.f)),
        "remove", int(testValues.size() / float(rm_time / 1000.f)),
        "read", int(testValues.size() / float(read_time / 1000.f)),
        "iterator", int(testValues.size() / float(it_time / 1000.f)),
        "update", int(testValues.size() / float(update_time / 1000.f)),
        "memory", memory,
        "nb_voxel", testValues.size()
    );

    return read_nb_error == 0 && rm_nb_error == 0 && update_nb_error == 0 && it_count == testValues.size();
}

static bool g_error = false;

template <typename T_Container>
void launchBenchmark()
{
    std::cout << "------- BENCHMARK " << voxomap::test::type_name<T_Container>() << " -------\n\n";
    g_error |= !bench_random<T_Container>();
    g_error |= !bench_contiguous<T_Container>();
    std::cout << "------- END -------\n\n\n";
}

template <typename T_Voxel>
using SparseContainer = voxomap::SparseContainer<T_Voxel>;
using voxel = voxomap::test::voxel;

struct ATest
{
    virtual void call() const = 0;
    virtual std::string getTestName() const = 0;
};

template <typename T>
struct Test : public ATest
{
    void call() const override { launchBenchmark<T>(); }
    std::string getTestName() const override { return voxomap::test::type_name<T>(); }
};

void usage(char const* name, std::vector<std::unique_ptr<ATest>> const& tests)
{
    std::cout << "Usage: " << name << " [TEST_ID]" << std::endl;
    std::cout << "Voxomap benchmark utility." << std::endl;
    std::cout << std::endl;
    std::cout << "List of tests:" << std::endl;
    int i = 0;
    for (int i = 0; i < tests.size(); ++i)
    {
        std::cout << "  - [" << i << "] " << tests[i]->getTestName() << std::endl;
    }
}

int main(int argc, char* argv[])
{
    std::vector<std::unique_ptr<ATest>> tests;

    // No super container
    tests.emplace_back(new Test<voxomap::ArrayContainer<voxel>>());
    tests.emplace_back(new Test<voxomap::SparseContainer<voxel>>());
    // Sided
    tests.emplace_back(new Test<voxomap::SidedContainer<voxomap::ArrayContainer, voxel>>());
    tests.emplace_back(new Test<voxomap::SidedContainer<SparseContainer, voxel>>());

    // One super container
    tests.emplace_back(new Test<voxomap::ArraySuperContainer<voxomap::ArrayContainer<voxel>>>());
    tests.emplace_back(new Test<voxomap::ArraySuperContainer<voxomap::SparseContainer<voxel>>>());
    tests.emplace_back(new Test<voxomap::SparseSuperContainer<voxomap::ArrayContainer<voxel>>>());
    tests.emplace_back(new Test<voxomap::SparseSuperContainer<voxomap::SparseContainer<voxel>>>());
    // Sided
    tests.emplace_back(new Test<voxomap::ArraySuperContainer<voxomap::SidedContainer<voxomap::ArrayContainer, voxel>>>());
    tests.emplace_back(new Test<voxomap::ArraySuperContainer<voxomap::SidedContainer<SparseContainer, voxel>>>());
    tests.emplace_back(new Test<voxomap::SparseSuperContainer<voxomap::SidedContainer<voxomap::ArrayContainer, voxel>>>());
    tests.emplace_back(new Test<voxomap::SparseSuperContainer<voxomap::SidedContainer<SparseContainer, voxel>>>());

    // Multiple super container
    tests.emplace_back(new Test<voxomap::ArraySuperContainer<voxomap::ArraySuperContainer<voxomap::ArrayContainer<voxel>>>>());
    tests.emplace_back(new Test<voxomap::ArraySuperContainer<voxomap::ArraySuperContainer<voxomap::SparseContainer<voxel>>>>());
    tests.emplace_back(new Test<voxomap::SparseSuperContainer<voxomap::ArraySuperContainer<voxomap::ArrayContainer<voxel>>>>());
    tests.emplace_back(new Test<voxomap::SparseSuperContainer<voxomap::ArraySuperContainer<voxomap::SparseContainer<voxel>>>>());
    tests.emplace_back(new Test<voxomap::SparseSuperContainer<voxomap::SparseSuperContainer<voxomap::SparseContainer<voxel>>>>());
    // Sided
    tests.emplace_back(new Test<voxomap::ArraySuperContainer<voxomap::ArraySuperContainer<voxomap::SidedContainer<voxomap::ArrayContainer, voxel>>>>());
    tests.emplace_back(new Test<voxomap::ArraySuperContainer<voxomap::ArraySuperContainer<voxomap::SidedContainer<SparseContainer, voxel>>>>());
    tests.emplace_back(new Test<voxomap::SparseSuperContainer<voxomap::ArraySuperContainer<voxomap::SidedContainer<voxomap::ArrayContainer, voxel>>>>());
    tests.emplace_back(new Test<voxomap::SparseSuperContainer<voxomap::ArraySuperContainer<voxomap::SidedContainer<SparseContainer, voxel>>>>());
    tests.emplace_back(new Test<voxomap::SparseSuperContainer<voxomap::SparseSuperContainer<voxomap::SidedContainer<SparseContainer, voxel>>>>());

    if (argc == 1)
    {
        for (auto const& test : tests)
        {
            test->call();
        }
    }
    else if (argc > 1)
    {
        try
        {
            int id = std::stoi(argv[1]);
            if (id >= tests.size() || id < 0)
            {
                usage(argv[0], tests);
                exit(-1);
            }
            tests[id]->call();
        }
        catch (std::invalid_argument const&)
        {
            usage(argv[0], tests);
            exit(-1);
        }
    }

    Report::get().writeCSV("benchmark_report.csv");

    std::cout << "------ FINAL RESULT: ";
    if (g_error)
    {
        std::cout << "ERROR ERROR ERROR ERROR There is errors !!! ------" << std::endl;
        return EXIT_FAILURE;
    }

    std::cout << "GOOD. ------" << std::endl;
    return EXIT_SUCCESS;
};
