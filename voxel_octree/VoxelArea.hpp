#ifndef _VOXOMAP_VOXELAREA_HPP_
#define _VOXOMAP_VOXELAREA_HPP_

#include <map>
#include <cstdint>

namespace voxomap
{

template <class T_Container> class VoxelOctree;
template <class T_Container> class VoxelNode;

template <class T_Container>
class VoxelArea
{
public:
    using VoxelData = typename T_Container::VoxelData;
    using iterator = typename T_Container::iterator;

    VoxelArea(int x = 0, int y = 0, int z = 0);
    VoxelArea(VoxelNode<T_Container> const& node, int x = 0, int y = 0, int z = 0);
    VoxelArea(VoxelOctree<T_Container> const& octree, int x = 0, int y = 0, int z = 0);
    VoxelArea(iterator const& it);
    
    iterator                findVoxel(int x, int y, int z);
    VoxelNode<T_Container>* findVoxelNode(int x, int y, int z);
    std::vector<iterator>   findNeighbors(float radius);

    void                    changeNode(VoxelNode<T_Container> const* node);
    void                    changeOctree(VoxelOctree<T_Container> const* octree);
    void                    changePosition(int x, int y, int z);

private:
    VoxelNode<T_Container>* _findVoxelNode(int x, int y, int z);

    int                             _x = 0;
    int                             _y = 0;
    int                             _z = 0;
    VoxelOctree<T_Container> const* _octree = nullptr;
    VoxelNode<T_Container> const*   _node = nullptr;
};

}

#include "VoxelArea.ipp"

#endif // _VOXOMAP_VOXELAREA_HPP_
