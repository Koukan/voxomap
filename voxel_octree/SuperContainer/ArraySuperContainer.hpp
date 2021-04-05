#ifndef _VOXOMAP_ARRAYSUPERCONTAINER_HPP_
#define _VOXOMAP_ARRAYSUPERCONTAINER_HPP_

#include <cstdint>
#include <memory>
#include <string>
#include "../iterator.hpp"

namespace voxomap
{

/*!
    \defgroup SuperContainer SuperContainer
    Super container used in leaves of the VoxelOctree to store others super or voxel containers.
    \ingroup VoxelOctree
*/

template <class Container> class VoxelNode;

/*! \struct ArraySuperContainer
    \ingroup SuperContainer
    \brief Super container used in leaves of the VoxelOctree.
    Based on a fixed size 3D array, useful for high density of voxels.
    - Advantage:
        - Good global performances
    - Disadvantage:
        - Big memory footprint (RAM and serialized), same footprint with 1 or 512 voxel/super sub-container.
*/
template <class T_Container>
struct ArraySuperContainer
{
    using Container = T_Container;
    using VoxelData = typename Container::VoxelData;
    using VoxelContainer = typename Container::VoxelContainer;
    using iterator = supercontainer_iterator<ArraySuperContainer<Container>>;

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
    ArraySuperContainer() = default;
    /*!
        \brief Copy constructor
    */
    ArraySuperContainer(ArraySuperContainer const& other);
    /*!
        \brief Default move constructor
    */
    ArraySuperContainer(ArraySuperContainer&& other) = default;
    /*!
        \brief Default destructor
    */
    ~ArraySuperContainer() = default;


    /*!
        \brief Initialization method, do nothing
    */
    void                init(VoxelNode<ArraySuperContainer<Container>> const&) {}
    /*!
        \brief Returns number of voxels
    */
    uint32_t            getNbVoxel() const;
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

    void                exploreVoxelContainer(std::function<void(typename Container::VoxelContainer const&)> const& predicate) const;

private:
    std::unique_ptr<Container> _containerArray[NB_CONTAINERS][NB_CONTAINERS][NB_CONTAINERS] = { 0 };  //!< Array of voxel containers
    uint32_t _nbVoxels = 0; //!< Number of voxels
};

}

#include "ArraySuperContainer.ipp"

#endif // _VOXOMAP_ARRAYSUPERCONTAINER_HPP_
