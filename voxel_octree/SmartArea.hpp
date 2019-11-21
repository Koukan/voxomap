#ifndef _VOXOMAP_SMARTAREA_HPP_
#define _VOXOMAP_SMARTAREA_HPP_

#include <cstdint>
#include <vector>

namespace voxomap
{

template <typename T_Area> class VoxelNode;

template <class T_Voxel, template<class...> class T_Container = std::vector>
class SmartArea
{
public:
    using VoxelData = T_Voxel;
    struct iterator
    {
        VoxelNode<SmartArea<VoxelData>>* node = nullptr;
        VoxelData* voxel = nullptr;
        uint8_t x = 0;
        uint8_t y = 0;
        uint8_t z = 0;

        operator bool() const { return this->voxel != nullptr; }
    };

public:
    const static uint32_t NB_VOXELS = 8;
    const static uint32_t AREA_MASK = ~(NB_VOXELS - 1);

    SmartArea();
    SmartArea(SmartArea const& other);

    uint16_t            getNbVoxel() const;
    bool                hasVoxel(uint8_t x, uint8_t y, uint8_t z) const;
    VoxelData*          findVoxel(uint8_t x, uint8_t y, uint8_t z);
    VoxelData const*    findVoxel(uint8_t x, uint8_t y, uint8_t z) const;

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

private:
    inline uint16_t     getId(uint8_t x, uint8_t y, uint8_t z) const;
    inline void         setId(uint8_t x, uint8_t y, uint8_t z, uint16_t id);
    uint16_t            getNewVoxelDataId();
    void                reallocToUint16_t();

    T_Container<T_Voxel>            _voxelData;
    T_Container<uint16_t>           _idFreed;
    std::unique_ptr<uint8_t[]>      _voxelId;
};

}

#include "SmartArea.ipp"

#endif // _VOXOMAP_SMARTAREA_HPP_
