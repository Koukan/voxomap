#ifndef _VOXOMAP_ARRAYAREA_HPP_
#define _VOXOMAP_ARRAYAREA_HPP_

#include <cstdint>
#include "iterator.hpp"

namespace voxomap
{

template <class T_Area> class VoxelNode;

template <class T_Voxel>
struct ArrayArea
{
    using VoxelData = T_Voxel;
    using iterator = voxomap::iterator<ArrayArea<T_Voxel>>;

    const static uint32_t NB_VOXELS = 8;
    const static uint32_t AREA_MASK = ~(NB_VOXELS - 1);

    ArrayArea();
    ArrayArea(ArrayArea const& other);
    ArrayArea(ArrayArea&& other) = default;

    void                init(VoxelNode<ArrayArea<VoxelData>> const&) {}
    uint16_t            getNbVoxel() const;
    VoxelData*          findVoxel(uint8_t x, uint8_t y, uint8_t z);
    VoxelData const*    findVoxel(uint8_t x, uint8_t y, uint8_t z) const;

    template <typename Iterator, typename... Args>
    bool                addVoxel(Iterator& it, Args&&... args);
    template <typename Iterator, typename... Args>
    bool                updateVoxel(Iterator& it, Args&&... args);
    template <typename Iterator, typename... Args>
    void                putVoxel(Iterator& it, Args&&... args);
    template <typename Iterator>
    Iterator            removeVoxel(Iterator it, VoxelData* voxel = nullptr);

    void                serialize(std::string& str) const;
    size_t              unserialize(char const* str, size_t size);

public:
    uint16_t        nbVoxels = 0;
    union {
        VoxelData   area[NB_VOXELS][NB_VOXELS][NB_VOXELS];
        char _;    // Compiler hack to avoid the call of the default constructor of each VoxelData of area
    };

private:
    template <typename T>
    typename std::enable_if<std::is_trivially_constructible<T>::value>::type copy(T const& other);
    template <typename T>
    typename std::enable_if<!std::is_trivially_constructible<T>::value>::type copy(T const& other);

    // used for initialize Area::area attribute without call constructor on each VoxelData of array
    static const VoxelData _emptyArea[NB_VOXELS][NB_VOXELS][NB_VOXELS];
};

}

#include "ArrayArea.ipp"

#endif // _VOXOMAP_ARRAYAREA_HPP_
