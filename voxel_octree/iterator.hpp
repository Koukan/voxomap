#ifndef _VOXOMAP_ITERATOR_HPP_
#define _VOXOMAP_ITERATOR_HPP_

#include <cstdint>

namespace voxomap
{

template <typename T_Area> class VoxelNode;

template <typename T_Area>
struct iterator
{
    using VoxelData = typename T_Area::VoxelData;

    VoxelNode<T_Area>* node = nullptr;
    VoxelData* voxel = nullptr;
    uint8_t x = 0;
    uint8_t y = 0;
    uint8_t z = 0;

    iterator& operator++();
    iterator operator++(int);
    iterator* operator*();
    operator bool() const;
    T_Area& getArea() const;

protected:
    void begin(VoxelNode<T_Area>& node);
    void end(VoxelNode<T_Area>& node);
    friend VoxelNode<T_Area>;

    void findNextParentNode(VoxelNode<T_Area>& node);
    bool findNextChildNode(VoxelNode<T_Area>& node);
    bool findNextVoxel(VoxelNode<T_Area>& node);
};

}

#include "iterator.ipp"

#endif // _VOXOMAP_ITERATOR_HPP_
