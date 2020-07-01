#ifndef _VOXOMAP_SPARSE3DARRAY_HPP_
#define _VOXOMAP_SPARSE3DARRAY_HPP_

#include <cstdint>
#include <memory>
#include <vector>
#include <string>

namespace voxomap
{

/*! \class Sparse3DArray
    \ingroup 
    \brief 
*/
template <typename T, uint8_t NB_VOXELS, template<class...> class T_Container = std::vector>
class Sparse3DArray
{
public:
    /*!
        \brief Default constructor
    */
    Sparse3DArray();
    /*!
        \brief Copy constructor
    */
    Sparse3DArray(Sparse3DArray const& other);

    inline uint16_t     getId(uint8_t x, uint8_t y, uint8_t z) const;
    inline void         setId(uint8_t x, uint8_t y, uint8_t z, uint16_t id);

    void                serialize(std::string& str) const;
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

    uint16_t            getNewId();
    void                reallocToUint16_t();
    template <typename Type>
    typename std::enable_if<std::is_trivially_copyable<Type>::value && !std::is_pointer<Type>::value>::type serializeData(std::string& str) const;
    template <typename Type>
    typename std::enable_if<std::is_pointer<Type>::value>::type serializeData(std::string& str) const;

    template <typename Type>
    typename std::enable_if<std::is_trivially_copyable<Type>::value && !std::is_pointer<Type>::value, size_t>::type unserializeData(char const* str, size_t nbData);
    template <typename Type>
    typename std::enable_if<std::is_pointer<Type>::value, size_t>::type unserializeData(char const* str, size_t nbData);

    T_Container<T>                  _datas;     //<! Contains the data
    T_Container<uint16_t>           _idFreed;   //<! List of unused data inside \a _datas
    std::unique_ptr<uint8_t[]>      _ids;       //<! 3D Array of id
};

}

#include "SparseIDArray.ipp"

#endif // _VOXOMAP_SPARSE3DARRAY_HPP_
