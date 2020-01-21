#ifndef _VOXOMAP_VOXELOCTREE_HPP_
#define _VOXOMAP_VOXELOCTREE_HPP_

#include <type_traits>
#include "../octree/Octree.hpp"
#include "VoxelArea.hpp"
#include "VoxelNode.hpp"
#include "../maths/BoundingBox.hpp"

namespace voxomap
{

template <class T_Area> class VoxelNode;

/*!
    \brief Octree optimized for voxel
*/
template <class T_Area>
class VoxelOctree : public Octree<VoxelNode<T_Area>>
{
public:
    using VoxelData = typename T_Area::VoxelData;
    using iterator = typename T_Area::iterator;

    /*!
        \brief Constructs VoxelOctree
        \param voxelSize Size of voxels
    */
    VoxelOctree();
    VoxelOctree(VoxelOctree<T_Area> const& other);
    VoxelOctree(VoxelOctree<T_Area>&& other);
    virtual ~VoxelOctree() = default;
    VoxelOctree& operator=(VoxelOctree<T_Area> const& other);
    VoxelOctree& operator=(VoxelOctree<T_Area>&& other);

    VoxelNode<T_Area>*                  push(VoxelNode<T_Area>& node) override;
    std::unique_ptr<VoxelNode<T_Area>>  pop(VoxelNode<T_Area>& node) override;
    void                                clear() override;
    /*!
        \brief Returns a voxel iterator
        \param x X coordinate
        \param y Y coordinate
        \param z Z coordinate
        \return iterator
    */
    template <typename T>
    iterator                findVoxel(T x, T y, T z);
    /*!
        \brief Returns a node, can be NULL
        \param x X coordinate
        \param y Y coordinate
        \param z Z coordinate
        \return The node which contain the voxel, can be NULL
    */
    template <typename T>
    VoxelNode<T_Area>*      findVoxelNode(T x, T y, T z) const;
    
    /*!
        \brief Add the voxel if not exist
        \param x X coordinate
        \param y Y coordinate
        \param z Z coordinate
        \param args Arguments forward to VoxelData constructor
        \return Returns a pair consisting of an iterator to the inserted element, or the already-existing element if no insertion happened, and a bool denoting whether the insertion took place (true if insertion happened, false if it did not).
    */
    template <typename T, typename... Args>
    std::pair<iterator, bool> addVoxel(T x, T y, T z, Args&&... args);
    /*!
        \brief Update the voxel if already exist
        \param x X coordinate
        \param y Y coordinate
        \param z Z coordinate
        \param args Arguments forward to VoxelData constructor
        \return iterator
    */
    template <typename T, typename... Args>
    iterator                updateVoxel(T x, T y, T z, Args&&... args);
    /*!
        \brief Update the voxel if already exist
        \param it Iterator of the voxel to remove
        \param args Arguments forward to VoxelData constructor
        \return iterator
    */
    template <typename... Args>
    iterator                updateVoxel(iterator it, Args&&... args);
    /*!
        \brief Add or update the voxel
        \param x X coordinate
        \param y Y coordinate
        \param z Z coordinate
        \param args Arguments forward to VoxelData constructor
        \return iterator
    */
    template <typename T, typename... Args>
    iterator                putVoxel(T x, T y, T z, Args&&... args);

    /*!
        \brief Removes a voxel
        \param x X coordinate
        \param y Y coordinate
        \param z Z coordinate
        \param args Arguments forward to removeVoxel area method
        \return True if success
    */
    template <typename T, typename... Args>
    bool                    removeVoxel(T x, T y, T z, Args&&... args);
    /*!
        \brief Removes a voxel
        \param it Iterator of the voxel to remove
        \param args Arguments forward to removeVoxel area method
        \return True if success
    */
    template <typename... Args>
    iterator                removeVoxel(iterator it, Args&&... args);

    /*!
        \brief Returns the size of areas
    */
    unsigned int            getAreaSize() const;

    /*!
        \brief Calcul the bounding box
    */
    //void                    calculBoundingBox(Core::RectHitbox &hitbox) const;
    void                    removeOfCache(VoxelNode<T_Area> const& node);

    void                    exploreVoxel(std::function<void(VoxelNode<T_Area> const&, VoxelData const&, uint8_t, uint8_t, uint8_t)> const& predicate) const;
    void                    exploreVoxelArea(std::function<void(VoxelNode<T_Area> const&)> const& predicate) const;
    void                    exploreBoundingBox(BoundingBox<int> const& bounding_box,
                                               std::function<void(VoxelNode<T_Area>&)> const& in_predicate,
                                               std::function<void(VoxelNode<T_Area>&)> const& out_predicate);

    /*!
        \brief getter and setter for the number of voxels
    */
    unsigned int            getNbVoxels() const;
    void                    setNbVoxels(unsigned int nbVoxels);

    /*!
     * \brief Return begin iterator of the octree
     */
    iterator                begin();
    /*!
     * \brief Return end iterator of the octree
     */
    iterator                end();

private:
    iterator                _findVoxel(int x, int y, int z);
    template <typename T>
    typename std::enable_if<std::is_floating_point<T>::value, iterator>::type _findVoxel(T x, T y, T z);

    VoxelNode<T_Area>*      _findVoxelNode(int x, int y, int z) const;
    template <typename T>
    typename std::enable_if<std::is_floating_point<T>::value, VoxelNode<T_Area>*>::type _findVoxelNode(T x, T y, T z) const;

protected:
    /*!
        \brief Adds the area nodes, an area represent a chunck of voxels
    */
    VoxelNode<T_Area>*      pushAreaNode(int x, int y, int z);
    template <typename T>
    typename std::enable_if<std::is_floating_point<T>::value, VoxelNode<T_Area>*>::type pushAreaNode(T x, T y, T z);

    mutable VoxelNode<T_Area>*  _nodeCache = nullptr;   //!< Cache for improve performance
    unsigned int                _nbVoxels = 0;
};

}

#include "VoxelOctree.ipp"

#endif // _VOXOMAP_VOXELOCTREE_HPP_
