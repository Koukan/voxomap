#ifndef _VOXOMAP_SPARSEIDARRAY_HPP_
#define _VOXOMAP_SPARSEIDARRAY_HPP_

#include <cstdint>
#include <memory>
#include <vector>
#include <string>

namespace voxomap
{

/*! \class SparseIDArray
    \ingroup Utility
    \brief 
*/
template <typename T, uint8_t T_Size, template<class...> class T_Container = std::vector>
class AbstractSparseIDArray
{
    static_assert(T_Size <= 32, "SparseIDArray can't have size > 32");  // because ids are store on a uint16_t

protected:
    /*!
        \brief Default constructor
    */
    AbstractSparseIDArray();
    /*!
        \brief Copy constructor
    */
    AbstractSparseIDArray(AbstractSparseIDArray const& other);

public:
    /*!
        \brief Returns number of datas
    */
    uint16_t            getNbData() const;

    /*!
        \brief Check if there is data inside
        \param x X index
        \return True if there is a data
    */
    bool                hasData(uint8_t x) const;
    /*!
        \brief Check if there is data inside
        \param x X index
        \param y Y index
        \return True if there is a data
    */
    bool                hasData(uint8_t x, uint8_t y) const;
    /*!
        \brief Check if data exist
        \param x X index
        \param y Y index
        \param z Z index
        \return True if data exist
    */
    bool                hasData(uint8_t x, uint8_t y, uint8_t z) const;

    /*!
        \brief Find data
        \param x X index
        \param y Y index
        \param z Z index
        \return The data if exists, otherwise nullptr
    */
    T*                  findData(uint8_t x, uint8_t y, uint8_t z);
    /*!
        \brief Find data
        \param x X index
        \param y Y index
        \param z Z index
        \return The data if exists, otherwise nullptr
    */
    T const*            findData(uint8_t x, uint8_t y, uint8_t z) const;

    /*!
        \brief Add a data, don't update an existing data
        \param x X position of the data
        \param y Y position of the data
        \param z Z position of the data
        \param data Return the pointer on the data created
        \param args Arguments to forward to data constructor
        \return True if success
    */
    template <typename... Args>
    int                 addData(uint8_t x, uint8_t y, uint8_t z, T*& data, Args&&... args);
    /*!
        \brief Update an existing data, don't create a new one
        \param x X position of the data
        \param y Y position of the data
        \param z Z position of the data
        \param data Return the pointer on the data updated
        \param args Arguments to forward to data constructor
        \return True if success
    */
    template <typename... Args>
    int                 updateData(uint8_t x, uint8_t y, uint8_t z, T*& data, Args&&... args);
    /*!
        \brief Add or update a data
        \param x X position of the data
        \param y Y position of the data
        \param z Z position of the data
        \param data Return the pointer on the data created or updated
        \param args Arguments to forward to data constructor
    */
    template <typename... Args>
    int                 putData(uint8_t x, uint8_t y, uint8_t z, T*& data, Args&&... args);
    /*!
        \brief Remove an existing data
        \param x X position of the data
        \param y Y position of the data
        \param z Z position of the data
        \param data Pointer on a data structure, filled with the data of the removed data
        \return True if success
    */
    int                 removeData(uint8_t x, uint8_t y, uint8_t z);
    /*!
        \brief Remove an existing data
        \param x X position of the data
        \param y Y position of the data
        \param z Z position of the data
        \param data Pointer on a data structure, filled with the data of the removed data
        \return True if success
    */
    int                 removeData(uint8_t x, uint8_t y, uint8_t z, T* data);

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
        \brief Requests the sparse array to reduce its capacity to fit its size.
    */
    void                shrinkToFit();

protected:
    // Serialization structure, use when there is less than 128 voxels inside area
    struct SerializationData
    {
        SerializationData(uint16_t position, uint16_t id);
        SerializationData(SerializationData const& other) = default;
        SerializationData(SerializationData&& other) = default;

#if (defined(__BYTE_ORDER__) && __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__) || defined(__BIG_ENDIAN__)
        uint16_t id : 7;
        uint16_t position : 9;
#else
        uint16_t position : 9;
        uint16_t id : 7;
#endif
    };

    uint16_t            getId(uint8_t x, uint8_t y, uint8_t z) const;
    void                setId(uint8_t x, uint8_t y, uint8_t z, uint16_t id);
    uint16_t            getNewId();
    template <typename T_Old, typename T_New>
    void                reallocIds();
    template <typename Type>
    void                changeId(uint16_t oldId, uint16_t newId);
    template <typename Type>
    void reset(Type& data);
    template <typename Type>
    void reset(std::unique_ptr<Type>& data);
    template <typename Type>
    void reset(std::shared_ptr<Type>& data);

    template <typename... Args>
    void                _addData(uint8_t x, uint8_t y, uint8_t z, T*& data, Args&&... args);
    template <typename... Args>
    void                _updateData(uint16_t id, uint8_t x, uint8_t y, uint8_t z, T*& data, Args&&... args);

    template <typename T_Data>
    void copy(T_Container<T_Data> const& other);
    template <typename T_Data>
    void copy(T_Container<std::unique_ptr<T_Data>> const& other);

    template <typename T_Data>
    typename std::enable_if<std::is_trivially_copyable<T_Data>::value>::type serializeContainer(std::string& str, T_Container<T_Data> const& datas) const;
    template <typename T_Data>
    typename std::enable_if<!std::is_trivially_copyable<T_Data>::value>::type serializeContainer(std::string& str, T_Container<T_Data> const& datas) const;
    template <typename T_Data>
    void serializeContainer(std::string& str, T_Container<T_Data*> const& datas) const;
    template <typename T_Data>
    void serializeContainer(std::string& str, T_Container<std::unique_ptr<T_Data>> const& datas) const;
    template <typename T_Data>
    void serializeContainer(std::string& str, T_Container<std::shared_ptr<T_Data>> const& datas) const;
    template <typename T_Data>
    typename std::enable_if<std::is_trivially_copyable<T_Data>::value>::type serializeData(std::string& str, T_Data const& data) const;
    template <typename T_Data>
    typename std::enable_if<!std::is_trivially_copyable<T_Data>::value>::type serializeData(std::string& str, T_Data const& data) const;

    template <typename T_Data>
    typename std::enable_if<std::is_trivially_copyable<T_Data>::value, size_t>::type unserializeContainer(char const* str, size_t size, T_Container<T_Data>& datas);
    template <typename T_Data>
    typename std::enable_if<!std::is_trivially_copyable<T_Data>::value, size_t>::type unserializeContainer(char const* str, size_t size, T_Container<T_Data>& datas);
    template <typename T_Data>
    size_t unserializeContainer(char const* str, size_t size, T_Container<T_Data*>& datas);
    template <typename T_Data>
    size_t unserializeContainer(char const* str, size_t size, T_Container<std::unique_ptr<T_Data>>& datas);
    template <typename T_Data>
    size_t unserializeContainer(char const* str, size_t size, T_Container<std::shared_ptr<T_Data>>& datas);
    template <typename T_Data>
    typename std::enable_if<std::is_trivially_copyable<T_Data>::value, size_t>::type unserializeData(char const* str, size_t size, T_Data& data);
    template <typename T_Data>
    typename std::enable_if<!std::is_trivially_copyable<T_Data>::value, size_t>::type unserializeData(char const* str, size_t size, T_Data& data);

    T_Container<T>                  _data;     //<! Contains the data
    T_Container<uint16_t>           _idFreed;   //<! List of unused data inside \a _datas
    std::unique_ptr<uint8_t[]>      _ids;       //<! 3D Array of id
};


template <typename T, uint8_t T_Size, template<class...> class T_Container = std::vector>
class SparseIDArray : public AbstractSparseIDArray<T, T_Size, T_Container>
{
    using AbstractSparseIDArray<T, T_Size, T_Container>::AbstractSparseIDArray;
};


/*!
    \brief Specialize SparseIDArray class for unique_ptr
*/
template <typename T, uint8_t T_Size, template<class...> class T_Container>
class SparseIDArray<std::unique_ptr<T>, T_Size, T_Container> : public AbstractSparseIDArray<std::unique_ptr<T>, T_Size, T_Container>
{
public:
    SparseIDArray() = default;
    SparseIDArray(SparseIDArray const& other) = default;
    SparseIDArray(SparseIDArray&& other) = default;

    /*!
        \brief Find data
        \param x X index
        \param y Y index
        \param z Z index
        \return The data if exists, otherwise nullptr
    */
    T* findData(uint8_t x, uint8_t y, uint8_t z);
    /*!
        \brief Find data
        \param x X index
        \param y Y index
        \param z Z index
        \return The data if exists, otherwise nullptr
    */
    T const* findData(uint8_t x, uint8_t y, uint8_t z) const;
};

}

#include "SparseIDArray.ipp"

#endif // _VOXOMAP_SPARSEIDARRAY_HPP_
