#ifndef _VOXOMAP_VOXELNODE_HPP_
#define _VOXOMAP_VOXELNODE_HPP_

#include <cstdint>
#include <map>
#include <memory>
#include <functional>
#include "../octree/Node.hpp"
#include "../utils/BoundingBox.hpp"

namespace voxomap
{

template <class T_Container> class VoxelOctree;

/*! \class VoxelNode
    \ingroup VoxelOctree
    \brief Node optimized for voxel
*/
template <class T_Container>
class VoxelNode : public Node<VoxelNode<T_Container>>
{
public:
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
    VoxelNode(int x, int y, int z, int size);
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
    void                    exploreVoxelContainer(std::function<void(VoxelNode const&)> const& predicate) const;
    /*!
        \brief Browse all voxel containers and call \a predicate on each
        \param predicate Function called for each voxel area
    */
    void                    exploreVoxelContainer(std::function<void(VoxelNode&)> const& predicate);
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
        \param str String that contains data
        \param strsize Size of the string
        \return Number of bytes read inside str
    */
    size_t                  unserialize(char const* str, size_t strsize);

private:
    /*!
        \brief Serialize \a node in \a str
    */
    uint32_t                serializeNode(std::string& str) const;
    
    std::shared_ptr<T_Container> _container;  //!< Voxel container
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
