#ifndef _VOXOMAP_VOXELNODE_HPP_
#define _VOXOMAP_VOXELNODE_HPP_

#include <cstdint>
#include <map>
#include <memory>
#include <type_traits>
#include <functional>
#include "../octree/Node.hpp"
#include "../utils/BoundingBox.hpp"

namespace voxomap
{

template <class T_Container> class VoxelOctree;

/*!
    \ingroup VoxelOctree
    \brief Trait controlling copy-on-write for a voxel container type.
    \details Default: disabled. The container is uniquely owned by its node
             (std::unique_ptr), so mutations pay no reference-count check. To opt
             into copy-on-write, prefer the CopyOnWrite adaptor below
             (VoxelOctree<CopyOnWrite<MyContainer>>); advanced users may instead
             specialize this trait directly.
*/
template <class T_Container>
struct voxel_use_cow : std::false_type {};

namespace detail
{
    //! Rebind an iterator template (container_iterator<X> / supercontainer_iterator<X>) onto a new container type.
    template <class It, class NewC> struct rebind_container_iterator;
    template <template <class> class Tmpl, class Old, class NewC>
    struct rebind_container_iterator<Tmpl<Old>, NewC> { using type = Tmpl<NewC>; };
}

/*!
    \ingroup VoxelOctree
    \brief Container adaptor that opts a VoxelOctree into copy-on-write.
    \details Choose the storage policy at the use site, per octree:
    \code
        VoxelOctree<MyContainer>              tree;      // default: unique ownership, no COW
        VoxelOctree<CopyOnWrite<MyContainer>> cowTree;   // shared_ptr storage + copy-on-write
    \endcode
             Both can coexist in the same program (distinct types). Copy-on-write
             clones a container before mutating it while it is shared (use_count > 1),
             which only happens through get/setSharedVoxelContainer.
    \note The wrapped container's init() hook is assumed to be a no-op, as it is
          for every built-in container.
*/
template <class C>
struct CopyOnWrite : C
{
    using C::C;
    using iterator = typename detail::rebind_container_iterator<typename C::iterator, CopyOnWrite<C>>::type;
    template <class N> void init(N const&) {}
};

template <class C>
struct voxel_use_cow<CopyOnWrite<C>> : std::true_type {};

/*! \class VoxelNode
    \ingroup VoxelOctree
    \brief Node optimized for voxel
*/
template <class T_Container>
class VoxelNode : public Node<VoxelNode<T_Container>>
{
public:
    using Container = T_Container;
    //! Container ownership: unique by default, shared when copy-on-write is opted in.
    using ContainerPtr = typename std::conditional<voxel_use_cow<T_Container>::value,
                                                    std::shared_ptr<T_Container>,
                                                    std::unique_ptr<T_Container>>::type;
    using VoxelData = typename T_Container::VoxelData;
    using iterator = typename T_Container::iterator;
    using P_Node = Node<VoxelNode<T_Container>>;

    /*! \struct NeighborAreaCache
        \brief Use by the method findRelativeVoxel to improve performance
    */
    struct NeighborAreaCache
    {
        /*!
            \brief Default constructor
        */
        NeighborAreaCache();

        std::map<Vector3I, VoxelNode<T_Container>*> nodes;   //!< Cache of nodes
        std::pair<VoxelNode<T_Container>*, bool> neighbor_nodes[3][3][3]; //!< Cache of neighbor nodes
    };

    /*!
        \brief Constructs VoxelNode with its properties
        \param x X coordinate
        \param y Y coordinate
        \param z Z coordinate
        \param size Size of the node
    */
    VoxelNode(int x, int y, int z, uint32_t size);
    /*!
        \brief Copy constructor
    */
    VoxelNode(VoxelNode const& other);
    /*!
        \brief Default destructor
    */
    virtual ~VoxelNode() = default;

    /*!
        \brief Returns an iterator to the first voxel of the node
    */
    iterator                begin();
    /*!
        \brief Returns an iterator to the voxel folowing the last voxel of the node
    */
    iterator                end();

    /*!
        \brief Returns the number of voxels
    */
    unsigned int            getNbVoxel() const;
    /*!
        \brief Returns true if node has voxel
    */
    bool                    hasVoxel() const;
    /*!
        \brief Returns the voxel container
    */
	T_Container*            getVoxelContainer();
    /*!
        \brief Returns the voxel container
    */
	T_Container const*      getVoxelContainer() const;
    /*!
        \brief Returns a shared pointer to the voxel container
    */
    std::shared_ptr<T_Container>	getSharedVoxelContainer();
    /*!
        \brief Sets the voxel container
    */
    void                    setVoxelContainer(std::shared_ptr<T_Container> container);
    /*!
        \brief Search voxel
        \param x X coordinate
        \param y Y coordinate
        \param z Z coordinate
        \return iterator on the found voxel
    */
    iterator                findVoxel(int x, int y, int z);
    /*!
        \brief Search voxel
        \param it Iterator with the voxel position 
        \return True if a voxel was found, \a it was updated
    */
    bool                    findVoxel(iterator& it);
    /*!
        \brief Search voxel with position relative to the node position
        \param x X coordinate
        \param y Y coordinate
        \param z Z coordinate
        \return iterator on the found voxel
    */
    iterator                findRelativeVoxel(int x, int y, int z) const;
    /*!
        \brief Search voxel with position relative to the node position.
        Use cache structure for improve performance in case of lot of calls.
        \param neighbor_cache Cache structure
        \param x X coordinate
        \param y Y coordinate
        \param z Z coordinate
        \return iterator on the found voxel
    */
    iterator                findRelativeVoxel(NeighborAreaCache& neighbor_cache, int x, int y, int z) const;
    /*!
        \brief Add the voxel if not exist
        \param it Iterator that contain coordinates
        \param args Arguments forward to VoxelData constructor
        \return True if a voxel was added
    */
    template <typename... Args>
    bool                    addVoxel(iterator& it, Args&&... args);
    /*!
        \brief Update the voxel if already exist
        \param it Iterator that contain coordinates
        \param args Arguments forward to VoxelData constructor
        \return True if a voxel was updated
    */
    template <typename... Args>
    bool                    updateVoxel(iterator& it, Args&&... args);
    /*!
        \brief Add or update the voxel
        \param it Iterator that contain coordinates
        \param args Arguments forward to VoxelData constructor
    */
    template <typename... Args>
    void                    putVoxel(iterator& it, Args&&... args);
    /*!
        \brief Remove an existing voxel
        \param it Iterator that contain coordinates
        \param args Arguments forward to VoxelData constructor
        \return True if success
    */
    template <typename... Args>
    bool                    removeVoxel(iterator const& it, Args&&... args);

    /*!
        \brief Browse all voxels and call \a predicate on each
        \param predicate Function called for each voxel
    */
    void                    exploreVoxel(std::function<void(iterator const&)> const& predicate) const;

    /*!
        \brief Browse all voxel containers and call \a predicate on each
        \param predicate Function called for each voxel container
    */
    void                    exploreVoxelContainer(std::function<void(typename T_Container::VoxelContainer const&)> const& predicate) const;

    /*!
        \brief Browse all voxel nodes and call \a predicate on each
        \param predicate Function called for each voxel node
    */
    void                    exploreVoxelNode(std::function<void(VoxelNode const&)> const& predicate) const;
    /*!
        \brief Browse all voxel containers and call \a predicate on each
        \param predicate Function called for each voxel area
    */
    void                    exploreVoxelNode(std::function<void(VoxelNode&)> const& predicate);
    /*!
        \brief Browse all voxel containers
        \param bounding_box The aligned axis bounding box
        \param in_predicate Function called for each voxel container inside the bounding box
        \param out_predicate Function called for each voxel container outside the bounding box
    */
    void                    exploreBoundingBox(BoundingBox<int> const& bounding_box,
                                               std::function<void(VoxelNode&)> const& in_predicate,
                                               std::function<void(VoxelNode&)> const& out_predicate);

    /*!
        \brief Copy the voxel container if a modification occured (add/remove/update voxel)
        and the container is shared with another octree
    */
    void                    copyOnWrite();
    /*!
        \brief Merge \a this with \a node
        Add existing voxel in \a node inside \a this but don't update voxel that already exists in \a this
    */
    void                    merge(VoxelNode& node);
    /*!
        \brief Returns true if there is no voxel
    */
    bool                    empty() const;
    /*!
        \brief Serialize the structure
        \param str String use for save the serialization
    */
    void                    serialize(std::string& str) const;
    /*!
        \brief Unserialize \a str
        \param octree Octree where to push unserialized node
        \param str String that contains data
        \param strsize Size of the string
        \return Number of bytes read inside str
    */
    static size_t           unserialize(VoxelOctree<T_Container>& octree, char const* str, size_t strsize);

private:
    /*!
        \brief Serialize \a node in \a str
    */
    uint32_t                serializeNode(std::string& str) const;
    
    ContainerPtr            _container;  //!< Voxel container (see voxel_use_cow)
    friend T_Container;

public:
	/*!
		\brief Method to find voxel container index inside its container
		\param src Coordinate (x, y or z)
		\param container_id Id of the container
		\return The index
	*/
	inline static int       findContainerPosition(int src, int container_id);
	/*!
        \brief Method to find voxel index inside its container
        \param src Coordinate (x, y or z)
        \return The index
    */
	inline static int       findVoxelPosition(int src);
};

}

#include "VoxelNode.ipp"

#endif // _VOXOMAP_VOXELNODE_HPP_
