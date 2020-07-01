#ifndef _VOXOMAP_VOXELOCTREE_HPP_
#define _VOXOMAP_VOXELOCTREE_HPP_

#include <type_traits>
#include "../octree/Octree.hpp"
#include "VoxelArea.hpp"
#include "VoxelNode.hpp"
#include "../utils/BoundingBox.hpp"

namespace voxomap
{

/*!
    \defgroup VoxelOctree VoxelOctree
    Classes used to implement the voxels logic inside the Octree
*/

template <class T_Container> class VoxelNode;

/*! \class VoxelOctree
    \ingroup VoxelOctree
    \brief Octree optimized for voxel
*/
template <class T_Container>
class VoxelOctree : public Octree<VoxelNode<T_Container>>
{
public:
    using VoxelData = typename T_Container::VoxelData;
    using iterator = typename T_Container::iterator;

    /*!
        \brief Default constructor
    */
    VoxelOctree();
    /*!
        \brief Copy constructor
    */
    VoxelOctree(VoxelOctree<T_Container> const& other);
    /*!
        \brief Move constructor
    */
    VoxelOctree(VoxelOctree<T_Container>&& other);
    /*!
        \brief Default destructor
    */
    virtual ~VoxelOctree() = default;
    /*!
        \brief Assignement operator
    */
    VoxelOctree& operator=(VoxelOctree<T_Container> const& other);
    /*!
        \brief Move assignement operator
    */
    VoxelOctree& operator=(VoxelOctree<T_Container>&& other);

    /*!
        \brief Pushes \a node into the octree
        \param node Node to push
        \return Pointer to the added node, can be different of \a node if it is already present in the octree
    */
    VoxelNode<T_Container>*                  push(VoxelNode<T_Container>& node) override;
    /*!
        \brief Removes \a node from the octree
        \param node Node to remove
        \return Pointer to the removed node
    */
    std::unique_ptr<VoxelNode<T_Container>>  pop(VoxelNode<T_Container>& node) override;
    /*!
        \brief Clear the octree
        Removes all nodes and all elements.
    */
    void					clear() override;
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
    VoxelNode<T_Container>*	findVoxelNode(T x, T y, T z) const;
    
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
    bool                    removeVoxel(iterator it, Args&&... args);

    /*!
        \brief Returns the size of areas
    */
    unsigned int            getAreaSize() const;

    /*!
        \brief Calcul the bounding box
    */
    //void                    calculBoundingBox(Core::RectHitbox &hitbox) const;
    void                    removeOfCache(VoxelNode<T_Container> const& node);

    /*!
        \brief Browse all voxels
        \param predicate Function called for each voxel
    */
    void                    exploreVoxel(std::function<void(iterator const&)> const& predicate) const;
    /*!
        \brief Browse all voxel areas
        \param predicate Function called for each voxel container
    */
    void                    exploreVoxelContainer(std::function<void(VoxelNode<T_Container> const&)> const& predicate) const;
    /*!
        \brief Browse all voxel areas
        \param bounding_box The aligned axis bounding box
        \param in_predicate Function called for each voxel container inside the bounding box
        \param out_predicate Function called for each voxel container outside the bounding box
    */
    void                    exploreBoundingBox(BoundingBox<int> const& bounding_box,
                                               std::function<void(VoxelNode<T_Container>&)> const& in_predicate,
                                               std::function<void(VoxelNode<T_Container>&)> const& out_predicate);

    /*!
        \brief Get the number of voxels
    */
    unsigned int            getNbVoxels() const;
    /*!
        \brief Set the number of voxels
        \param nbVoxels Number of voxels
    */
    void                    setNbVoxels(unsigned int nbVoxels);

    /*!
     * \brief Returns an iterator to the first voxel of the octree
     */
    iterator                begin();
    /*!
     * \brief Returns an iterator to the element folowing the last voxel of the octree
     */
    iterator                end();

    /*!
        \brief Serialize the structure
        \param str String use for save the serialization
    */
    void                    serialize(std::string& str) const;
    /*!
        \brief Unserialize \a str
        \param str String that contains data
        \param strsize Size of the string
        \return Number of bytes read inside str
    */
    size_t                  unserialize(char const* str, size_t strsize);

private:
    /*!
        \brief Method to find voxel (for integer arguments)
        \param x X coordinate of the voxel
        \param y Y coordinate of the voxel
        \param z Z coordinate of the voxel
        \return iterator on the voxel
    */
    template <int NB_SUPERCONTAINER>
    typename std::enable_if<(NB_SUPERCONTAINER == 0), iterator>::type _findVoxel(int x, int y, int z);
    template <int NB_SUPERCONTAINER>
    typename std::enable_if<(NB_SUPERCONTAINER != 0), iterator>::type _findVoxel(int x, int y, int z);
    /*!
        \brief Method to find voxel (for floating point arguments)
        \param x X coordinate of the voxel
        \param y Y coordinate of the voxel
        \param z Z coordinate of the voxel
        \return iterator on the voxel
    */
    template <int NB_SUPERCONTAINER, typename T>
    typename std::enable_if<std::is_floating_point<T>::value, iterator>::type _findVoxel(T x, T y, T z);

    /*!
        \brief Method to find node that contain voxel (for integer arguments)
        \param x X coordinate of the voxel
        \param y Y coordinate of the voxel
        \param z Z coordinate of the voxel
        \return The found node
    */
    VoxelNode<T_Container>*      _findVoxelNode(int x, int y, int z) const;
    /*!
        \brief Method to find node that contain voxel (for floating point arguments)
        \param x X coordinate of the voxel
        \param y Y coordinate of the voxel
        \param z Z coordinate of the voxel
        \return The found node
    */
    template <typename T>
    typename std::enable_if<std::is_floating_point<T>::value, VoxelNode<T_Container>*>::type _findVoxelNode(T x, T y, T z) const;

protected:
    /*!
        \brief Adds the leaf node that contain the voxel container
        \param x X coordinate of the voxel
        \param y Y coordinate of the voxel
        \param z Z coordinate of the voxel
        \return The added node
    */
    VoxelNode<T_Container>*      pushContainerNode(int x, int y, int z);
    /*!
        \brief Adds the leaf node that contain the voxel container
        \param x X coordinate of the voxel
        \param y Y coordinate of the voxel
        \param z Z coordinate of the voxel
        \return The added node
    */
    template <typename T>
    typename std::enable_if<std::is_floating_point<T>::value, VoxelNode<T_Container>*>::type pushContainerNode(T x, T y, T z);

    mutable VoxelNode<T_Container>*	_nodeCache = nullptr;   //!< Cache for improve performance
    unsigned int					_nbVoxels = 0;          //!< Number of voxels
};

}

#include "VoxelOctree.ipp"

#endif // _VOXOMAP_VOXELOCTREE_HPP_
