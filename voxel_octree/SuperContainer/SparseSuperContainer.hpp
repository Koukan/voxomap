#ifndef _VOXOMAP_SPARSESUPERCONTAINER_HPP_
#define _VOXOMAP_SPARSESUPERCONTAINER_HPP_

#include <cstdint>
#include <memory>
#include <string>
#include <vector>
#include <functional>
#include "../iterator.hpp"
#include "../SparseIDArray.hpp"

namespace voxomap
{

/*!
    \defgroup SuperContainer SuperContainer
    Super container used in leaves of the VoxelOctree to store others super or voxel containers.
    \ingroup VoxelOctree
*/

template <class Container> class VoxelNode;

/*! \class SparseSuperContainer
    \ingroup SuperContainer
    \brief Super container used in leaves of the VoxelOctree.
    Mix between a fixed size 3D array (like in ArraySuperContainer) and a dynamic array.
    Useful for little density of sub-containers.
    - Advantages:
        - Good access/update performance (~= ArraySuperContainer)
        - Small memory footprint
    - Disadvantages:
        - Less performant on add/remove than the ArraySuperContainer
        - Bigger memory footprint than the ArraySuperContainer when high density

    The container uses 3 arrays:
    - A dynamic vector that contains the super/voxel sub-containers
    - A fixed size array (like ArraySuperContainer) that contains the sub-container id inside the above vector, 0 if there is no sub-container
    - A dynamic vector that contains the unused sub-container inside the first vector. Due to performance issue, the removed sub-container pointer are not removed from the first vector.
*/
template <class T_Container, template <class...> class T_InternalContainer = std::vector>
struct SparseSuperContainer
{
    using Container = T_Container;
    using VoxelData = typename Container::VoxelData;
    using VoxelContainer = typename Container::VoxelContainer;
    using iterator = supercontainer_iterator<SparseSuperContainer<Container, T_InternalContainer>>;

    const static uint32_t NB_CONTAINERS = 8;
    const static uint32_t CONTAINER_MASK = NB_CONTAINERS - 1;
    const static uint32_t NB_VOXELS = NB_CONTAINERS * Container::NB_VOXELS;
    const static uint32_t COORD_MASK = ~(NB_VOXELS - 1);
    const static uint32_t VOXEL_MASK = Container::VOXEL_MASK;
    const static uint32_t NB_SUPERCONTAINER = 1 + Container::NB_SUPERCONTAINER;
    const static uint32_t SUPERCONTAINER_ID = NB_SUPERCONTAINER - 1;

    /*!
        \brief Default constructor
    */
    SparseSuperContainer() = default;
    /*!
        \brief Default Copy constructor
    */
    SparseSuperContainer(SparseSuperContainer const& other) = default;
    /*!
        \brief Default move constructor
    */
    SparseSuperContainer(SparseSuperContainer&& other) = default;
    /*!
        \brief Default destructor
    */
    ~SparseSuperContainer() = default;


    /*!
        \brief Initialization method, do nothing
    */
    void                init(VoxelNode<SparseSuperContainer<Container, T_InternalContainer>> const&) {}
    /*!
        \brief Returns number of voxels
    */
    uint16_t            getNbVoxel() const;
    /*!
        \brief Find voxel
        \param it Iterator containing voxel position information
        \return The voxel if exists, otherwise nullptr
    */
    template <typename Iterator>
    VoxelData*          findVoxel(Iterator& it);
    /*!
        \brief Find voxel
        \param it Iterator containing voxel position information
        \return The voxel if exists, otherwise nullptr
    */
    template <typename Iterator>
    VoxelData const*    findVoxel(Iterator& it) const;

    /*!
        \brief Check if there is sub-container
        \param x X index
        \return True if there is sub-container
    */
    bool                hasContainer(uint8_t x) const;
    /*!
        \brief Check if there is sub-container
        \param x X index
        \param y Y index
        \return True if there is sub-container
    */
    bool                hasContainer(uint8_t x, uint8_t y) const;
    /*!
        \brief Check if there is sub-container
        \param x X index
        \param y Y index
        \param z Z index
        \return True if there is sub-container
    */
    bool                hasContainer(uint8_t x, uint8_t y, uint8_t z) const;
    /*!
        \brief Find sub-container
        \param x X index
        \param y Y index
        \param z Z index
        \return Pointer on the sub-container if exists, otherwise nullptr
    */
    Container*          findContainer(uint8_t x, uint8_t y, uint8_t z);
    /*!
        \brief Find sub-container
        \param x X index
        \param y Y index
        \param z Z index
        \return Pointer on the sub-container if exists, otherwise nullptr
    */
    Container const*    findContainer(uint8_t x, uint8_t y, uint8_t z) const;

    /*!
        \brief Add a voxel, don't update an existing voxel
        \param it Iterator that contains the informations
        \param args Arguments to forward to voxel constructor
        \return True if success and update \a it
    */
    template <typename Iterator, typename... Args>
    bool                addVoxel(Iterator& it, Args&&... args);
    /*!
        \brief Update an existing voxel, don't create a new one
        \param it Iterator that contains the informations
        \param args Arguments to forward to voxel constructor
        \return True if success
    */
    template <typename Iterator, typename... Args>
    bool                updateVoxel(Iterator& it, Args&&... args);
    /*!
        \brief Add or update a voxel
        \param it Iterator that contains the informations
        \param args Arguments to forward to voxel constructor
    */
    template <typename Iterator, typename... Args>
    void                putVoxel(Iterator& it, Args&&... args);
    /*!
        \brief Remove an existing voxel
        \param it Iterator that contains the informations
        \param args Arguments to forward to removeVoxel method of the container
        \return True if success
    */
    template <typename Iterator, typename... Args>
    bool                removeVoxel(Iterator const& it, Args&&... args);

    /*!
        \brief Serialize the structure
        \param str String use for save the serialization
    */
    void                serialize(std::string& str) const;
    /*!
        \brief Unserialize \a str inside \a this
        \param str String that contains data
        \param size Size of the string
        \return Number of bytes read inside str
    */
    size_t              unserialize(char const* str, size_t size);

    /*!
        \brief Go through all voxels of the container and call the \a predicate for each
        \param it Begin iterator
        \param predicate Function called for each voxel found
    */
    template <typename Iterator>
    void                exploreVoxel(Iterator& it, std::function<void(Iterator const&)> const& predicate) const;

    void                exploreVoxelContainer(std::function<void(typename T_Container::VoxelContainer const&)> const& predicate) const;

private:
    SparseIDArray<std::unique_ptr<Container>, NB_CONTAINERS, T_InternalContainer> _sparseArray;
    uint32_t _nbVoxels = 0; //!< Number of voxels
};

}

#include "SparseSuperContainer.ipp"

#endif // _VOXOMAP_SPARSESUPERCONTAINER_HPP_
