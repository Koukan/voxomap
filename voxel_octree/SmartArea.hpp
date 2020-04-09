#ifndef _VOXOMAP_SMARTAREA_HPP_
#define _VOXOMAP_SMARTAREA_HPP_

#include <cstdint>
#include <vector>
#include "iterator.hpp"

namespace voxomap
{

template <typename T_Area> class VoxelNode;

/*! \class SmartArea
    \ingroup VoxelContainer
    \brief Voxel container used in leaves of the VoxelOctree.
    Mix between a fixed size 3D array (like in ArrayArea) and a dynamic array.
    Useful for little density of voxels.
    - Advantages:
        - Good access/update performance (~= ArrayArea)
        - Small memory footprint
    - Disadvantages:
        - Less performant on add/remove than the ArrayArea
        - Bigger memory footprint than the ArrayArea when high density

    The container uses 3 arrays:
    - A dynamic vector that contains the voxels
    - A fixed size array (like ArrayArea) that contains the voxel id inside the above vector, 0 if there is no voxel
    - A dynamic vector that contains the unused voxel inside the first vector. Due to performance issue, the removed voxel are not removed from the first vector.
*/
template <class T_Voxel, template<class...> class T_Container = std::vector>
class SmartArea
{
public:
    using VoxelData = T_Voxel;
    using iterator = voxomap::iterator<SmartArea<T_Voxel, T_Container>>;

public:
    const static uint32_t NB_VOXELS = 8;
    const static uint32_t AREA_MASK = ~(NB_VOXELS - 1);

    /*!
        \brief Default constructor
    */
    SmartArea();
    /*!
        \brief Copy constructor
    */
    SmartArea(SmartArea const& other);

    /*!
        \brief Initialization method, do nothing
    */
    void                init(VoxelNode<SmartArea<VoxelData>> const&) {}
    /*!
        \brief Returns number of voxels
    */
    uint16_t            getNbVoxel() const;
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
    VoxelData*          findVoxel(uint8_t x, uint8_t y, uint8_t z);
    /*!
       \brief Find voxel
       \param x X index
       \param y Y index
       \param z Z index
       \return The voxel if exists, otherwise nullptr
   */
    VoxelData const*    findVoxel(uint8_t x, uint8_t y, uint8_t z) const;

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
        \return True if success and update \a it
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
    Iterator            removeVoxel(Iterator it, VoxelData* voxel = nullptr);

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

private:
    // Serialization structure, use when there is less than 128 voxels inside area
    struct SerializationData
    {
        SerializationData(uint16_t voxel_pos, uint16_t voxel_id);
        SerializationData(SerializationData const& other) = default;
        SerializationData(SerializationData&& other) = default;

#if (defined(__BYTE_ORDER__) && __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__) || defined(__BIG_ENDIAN__)
        uint16_t voxel_id : 7;
        uint16_t voxel_pos : 9;
#else
        uint16_t voxel_pos : 9;
        uint16_t voxel_id : 7;
#endif
    };

    inline uint16_t     getId(uint8_t x, uint8_t y, uint8_t z) const;
    inline void         setId(uint8_t x, uint8_t y, uint8_t z, uint16_t id);
    uint16_t            getNewVoxelDataId();
    void                reallocToUint16_t();

    T_Container<T_Voxel>            _voxelData; //<! Contains the voxel data
    T_Container<uint16_t>           _idFreed;   //<! List of unused voxel inside \a _voxelData
    std::unique_ptr<uint8_t[]>      _voxelId;   //<! 3D Array of voxel id inside \a _voxelData
};

}

#include "SmartArea.ipp"

#endif // _VOXOMAP_SMARTAREA_HPP_
