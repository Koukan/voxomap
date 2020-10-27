#ifndef _VOXOMAP_ARRAYCONTAINER_HPP_
#define _VOXOMAP_ARRAYCONTAINER_HPP_

#include <cstdint>
#include "../iterator.hpp"

namespace voxomap
{

/*!
    \defgroup VoxelContainer VoxelContainer
    Voxel container used in leaves of the VoxelOctree
    \ingroup VoxelOctree
*/

template <class T_Container> class VoxelNode;

/*! \struct ArrayContainer
    \ingroup VoxelContainer
    \brief Voxel container used in leaves of the VoxelOctree.
    Based on a fixed size 3D array, useful for high density of voxels.
    - Advantage:
        - Good global performances
    - Disadvantage:
        - Big memory footprint (RAM and serialized), same footprint with 1 or 512 voxels.
*/
template <class T_Voxel>
struct ArrayContainer
{
    static_assert(std::is_trivially_copyable<T_Voxel>::value, "ArrayContainer only accept trivially copyable object");

    using VoxelData = T_Voxel;
	using VoxelContainer = ArrayContainer<T_Voxel>;
    using iterator = container_iterator<ArrayContainer<T_Voxel>>;

    const static uint32_t NB_VOXELS = 8;
	const static uint32_t COORD_MASK = ~(NB_VOXELS - 1);
	const static uint32_t VOXEL_MASK = NB_VOXELS - 1;
	const static uint32_t NB_SUPERCONTAINER = 0;

    /*!
        \brief Default constructor
    */
    ArrayContainer();
    /*!
        \brief Copy constructor
    */
    ArrayContainer(ArrayContainer const& other);
    /*!
        \brief Default move constructor
    */
    ArrayContainer(ArrayContainer&& other) = default;

    /*!
        \brief Initialization method, do nothing
    */
    void                init(VoxelNode<ArrayContainer<VoxelData>> const&) {}
    /*!
        \brief Returns number of voxels
    */
    uint16_t            getNbVoxel() const;
    /*!
        \brief Check if there is voxel inside
        \param x X index
        \return True if there is a voxel
    */
    bool                hasVoxel(uint8_t x) const;
    /*!
        \brief Check if there is voxel inside
        \param x X index
        \param y Y index
        \return True if there is a voxel
    */
    bool                hasVoxel(uint8_t x, uint8_t y) const;
    /*!
        \brief Check if voxel exist
        \param x X index
        \param y Y index
        \param z Z index
        \return True if voxel exist
    */
    bool                hasVoxel(uint8_t x, uint8_t y, uint8_t z) const;
    /*!
        \brief Find voxel
        \param x X index
        \param y Y index
        \param z Z index
        \return The voxel if exists, otherwise nullptr
    */
    VoxelData*			findVoxel(uint8_t x, uint8_t y, uint8_t z);
    /*!
        \brief Find voxel
        \param x X index
        \param y Y index
        \param z Z index
        \return The voxel if exists, otherwise nullptr
    */
    VoxelData const*	findVoxel(uint8_t x, uint8_t y, uint8_t z) const;
    /*!
        \brief Find voxel
        \param it The iterator
        \return The voxel if exists, otherwise nullptr
    */
	template <typename Iterator>
    VoxelData*          findVoxel(Iterator& it);
    /*!
        \brief Find voxel
        \param it The iterator
        \return The voxel if exists, otherwise nullptr
    */
	template <typename Iterator>
    VoxelData const*    findVoxel(Iterator& it) const;

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
        \param voxel Pointer on a voxel structure, filled with the data of the removed voxel
        \return True if success
    */
    template <typename Iterator>
    bool				removeVoxel(Iterator const& it, VoxelData* voxel = nullptr);
    
    /*!
        \brief Go through all voxels of the container and call the \a predicate for each
        \param it Begin iterator
        \param predicate Function called for each voxel found
    */
    template <typename Iterator>
    void				exploreVoxel(Iterator& it, std::function<void(Iterator const&)> const& predicate) const;

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


public:
    uint16_t        nbVoxels = 0;   //!< Number of voxels
    union {
        VoxelData   area[NB_VOXELS][NB_VOXELS][NB_VOXELS];  //!< Array of voxels
        char _;    //!< Compiler hack to avoid the call of the default constructor of each VoxelData of area
    };

private:
    /*!
        \brief Method use to copy data when voxel is trivially constructible
    */
    template <typename T>
    typename std::enable_if<std::is_trivially_constructible<T>::value>::type copy(T const& other);
    /*!
        \brief Method use to copy data when voxel is not trivially constructible
    */
    template <typename T>
    typename std::enable_if<!std::is_trivially_constructible<T>::value>::type copy(T const& other);

    /*!
        Used for initialize ArrayContainer::area attribute without call constructor on each VoxelData of array
    */
    static const VoxelData _emptyArea[NB_VOXELS][NB_VOXELS][NB_VOXELS];
};

}

#include "ArrayContainer.ipp"

#endif // _VOXOMAP_ARRAYCONTAINER_HPP_
