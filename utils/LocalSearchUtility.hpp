#ifndef _VOXOMAP_LOCALSEARCHUTILITY_HPP_
#define _VOXOMAP_LOCALSEARCHUTILITY_HPP_

#include <vector>
#include <cstdint>

namespace voxomap
{

template <class T_Container> class VoxelOctree;
template <class T_Container> class VoxelNode;

/*! \class LocalSearchUtility
    \ingroup Utility
    \brief Tools used to search voxels around a reference position
*/
template <class T_Container>
class LocalSearchUtility
{
public:
    using VoxelData = typename T_Container::VoxelData;
    using iterator = typename T_Container::iterator;

    /*!
        \brief Constructor
        \param it Iterator 
    */
    LocalSearchUtility(iterator const& it);
    /*!
        \brief Constructor
        \param node The reference node
        \param x X position inside \a node
        \param y Y position inside \a node
        \param z Z position inside \a node
    */
    LocalSearchUtility(VoxelNode<T_Container> const& node, uint8_t x, uint8_t y, uint8_t z);
    /*!
        \brief Constructor
        \param octree The reference octree
        \param x X position
        \param y Y position
        \param z Z position
    */
    LocalSearchUtility(VoxelOctree<T_Container> const& octree, int x, int y, int z);
    
    /*!
        \brief Find voxel
        \param x X position of the voxel
        \param y Y position of the voxel
        \param z Z position of the voxel
        \return Iterator on the found voxel, invalid iterator otherwise
    */
    iterator                findVoxel(int x, int y, int z);
    /*!
        \brief Find the node that can contains the voxel
        \param x X position of the voxel
        \param y Y position of the voxel
        \param z Z position of the voxel
        \return The pointer on the found node, nullptr otherwise
    */
    VoxelNode<T_Container>* findVoxelNode(int x, int y, int z);
    /*!
        \brief !! NOT IMPLEMENTED YET !! Find voxels inside the radius
        \return Vector of iterator that point on found voxels
    */
    std::vector<iterator>   findNeighbors(float radius);

private:
    VoxelNode<T_Container>* _findVoxelNode(int x, int y, int z);

    int                             _x = 0;
    int                             _y = 0;
    int                             _z = 0;
    VoxelOctree<T_Container> const* _octree = nullptr;
    VoxelNode<T_Container> const*   _node = nullptr;
};

}

#include "LocalSearchUtility.ipp"

#endif // _VOXOMAP_LOCALSEARCHUTILITY_HPP_
