#ifndef _VOXOMAP_ARRAYAREA_HPP_
#define _VOXOMAP_ARRAYAREA_HPP_

#include <cstdint>

namespace voxomap
{

template <class T_Area> class VoxelNode;

template <class T_Voxel>
struct ArrayArea
{
    using VoxelData = T_Voxel;
    struct iterator
    {
        VoxelNode<ArrayArea<VoxelData>>* node = nullptr;
        VoxelData* voxel = nullptr;
        uint8_t x = 0;
        uint8_t y = 0;
        uint8_t z = 0;

        operator bool() const { return this->voxel != nullptr; }
    };
    const static uint32_t NB_VOXELS = 8;

    ArrayArea();
    ArrayArea(ArrayArea const& other) = default;

    uint16_t            getNbVoxel() const;
    VoxelData*          getVoxel(uint8_t x, uint8_t y, uint8_t z);
    VoxelData const*    getVoxel(uint8_t x, uint8_t y, uint8_t z) const;

    template <typename Iterator, typename... Args>
    bool                addVoxel(Iterator& it, Args&&... args);
    template <typename Iterator, typename... Args>
    bool                updateVoxel(Iterator& it, Args&&... args);
    template <typename Iterator, typename... Args>
    bool                putVoxel(Iterator& it, Args&&... args);
    template <typename Iterator>
    bool                removeVoxel(Iterator& it, VoxelData* voxel = nullptr);

    void                serialize(std::string& str) const;
    size_t              unserialize(char const* str, size_t size);

public:
    uint16_t        nbVoxels = 0;
    union {
        VoxelData   area[NB_VOXELS][NB_VOXELS][NB_VOXELS];
        char _;    // Compiler hack to avoid the call of the default constructor of each VoxelData of area
    };

private:
    // used for initialize Area::area attribute without call constructor on each VoxelData of array
    static const VoxelData _emptyArea[NB_VOXELS][NB_VOXELS][NB_VOXELS];
};

}

#include "ArrayArea.ipp"

#endif // _VOXOMAP_ARRAYAREA_HPP_
