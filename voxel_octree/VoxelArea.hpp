#ifndef _VOXOMAP_VOXELAREA_HPP_
#define _VOXOMAP_VOXELAREA_HPP_

#include <map>
#include <cstdint>

namespace voxomap
{

template <class T_Area> class VoxelOctree;
template <class T_Area> class VoxelNode;

template <class T_Area>
class VoxelArea
{
public:
    using VoxelData = typename T_Area::VoxelData;
    using iterator = typename T_Area::iterator;

    VoxelArea(int x = 0, int y = 0, int z = 0);
    VoxelArea(VoxelNode<T_Area> const& node, int x = 0, int y = 0, int z = 0);
    VoxelArea(VoxelOctree<T_Area> const& octree, int x = 0, int y = 0, int z = 0);
    VoxelArea(iterator const& it);
    
    iterator                findVoxel(int x, int y, int z);
    VoxelNode<T_Area>*      findVoxelNode(int x, int y, int z);
    std::vector<iterator>   findNeighbors(float radius);

    void                    changeNode(VoxelNode<T_Area> const* node);
    void                    changeOctree(VoxelOctree<T_Area> const* octree);
    void                    changePosition(int x, int y, int z);

private:
    int                         _x = 0;
    int                         _y = 0;
    int                         _z = 0;
    VoxelOctree<T_Area> const*  _octree = nullptr;
    VoxelNode<T_Area> const*    _node = nullptr;
};

}

#include "VoxelArea.ipp"

#endif // _VOXOMAP_VOXELAREA_HPP_
