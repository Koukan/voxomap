#include <algorithm>

namespace voxomap
{

template <typename T, uint8_t T_Size, template<class...> class T_Container>
AbstractSparseIDArray<T, T_Size, T_Container>::AbstractSparseIDArray()
{
    _ids.reset(new uint8_t[T_Size * T_Size * T_Size]);
    std::memset(_ids.get(), 0, sizeof(uint8_t) * T_Size * T_Size * T_Size);
}

template <typename T, uint8_t T_Size, template<class...> class T_Container>
AbstractSparseIDArray<T, T_Size, T_Container>::AbstractSparseIDArray(AbstractSparseIDArray const& other)
{
    this->copy(other._data);
    _idFreed = other._idFreed;
    if (_data.size() <= std::numeric_limits<uint8_t>::max())
    {
        _ids.reset(new uint8_t[T_Size * T_Size * T_Size]);
        std::memcpy(_ids.get(), other._ids.get(), T_Size * T_Size * T_Size * sizeof(uint8_t));
    }
    else
    {
        _ids.reset(reinterpret_cast<uint8_t*>(new uint8_t[T_Size * T_Size * T_Size * 2]));
        std::memcpy(_ids.get(), other._ids.get(), T_Size * T_Size * T_Size * sizeof(uint16_t));
    }
}

template <typename T, uint8_t T_Size, template<class...> class T_Container>
inline uint16_t AbstractSparseIDArray<T, T_Size, T_Container>::getNbData() const
{
    return static_cast<uint16_t>(_data.size() - _idFreed.size());
}

template <typename T, uint8_t T_Size, template<class...> class T_Container>
inline bool AbstractSparseIDArray<T, T_Size, T_Container>::hasData(uint8_t x) const
{
    static uint16_t cmp_array[T_Size * T_Size] = { 0 };
    const uint16_t ID_T_Size = _data.size() <= std::numeric_limits<uint8_t>::max() ? sizeof(uint8_t) : sizeof(uint16_t);
    const uint16_t XT_Size = T_Size * T_Size * ID_T_Size;

    return std::memcmp(_ids.get() + (XT_Size * uint16_t(x)), cmp_array, XT_Size) != 0;
}

template <typename T, uint8_t T_Size, template<class...> class T_Container>
inline bool AbstractSparseIDArray<T, T_Size, T_Container>::hasData(uint8_t x, uint8_t y) const
{
    static uint16_t cmp_array[T_Size] = { 0 };
    const uint16_t ID_T_Size = _data.size() <= std::numeric_limits<uint8_t>::max() ? sizeof(uint8_t) : sizeof(uint16_t);
    const uint16_t XT_Size = T_Size * T_Size * ID_T_Size;
    const uint16_t YT_Size = T_Size * ID_T_Size;

    return std::memcmp(_ids.get() + (XT_Size * uint16_t(x)) + (YT_Size * uint16_t(y)), cmp_array, YT_Size) != 0;
}

template <typename T, uint8_t T_Size, template<class...> class T_Container>
inline bool AbstractSparseIDArray<T, T_Size, T_Container>::hasData(uint8_t x, uint8_t y, uint8_t z) const
{
    return this->getId(x, y, z) != 0;
}

template <typename T, uint8_t T_Size, template<class...> class T_Container>
inline T* AbstractSparseIDArray<T, T_Size, T_Container>::findData(uint8_t x, uint8_t y, uint8_t z)
{
    auto id = this->getId(x, y, z);
    return id ? &_data[id - 1] : nullptr;
}

template <typename T, uint8_t T_Size, template<class...> class T_Container>
inline T const* AbstractSparseIDArray<T, T_Size, T_Container>::findData(uint8_t x, uint8_t y, uint8_t z) const
{
    auto id = this->getId(x, y, z);
    return id ? &_data[id - 1] : nullptr;
}

template <typename T, uint8_t T_Size, template<class...> class T_Container>
template <typename... Args>
bool AbstractSparseIDArray<T, T_Size, T_Container>::addData(uint8_t x, uint8_t y, uint8_t z, T*& data, Args&&... args)
{
    auto id = this->getId(x, y, z);

    if (id != 0)
    {
        data = &_data[id - 1];
        return false;
    }
    id = getNewId();
    new (&_data[id - 1]) T(std::forward<Args>(args)...);
    this->setId(x, y, z, id);
    data = &_data[id - 1];
    return true;
}

template <typename T, uint8_t T_Size, template<class...> class T_Container>
template <typename... Args>
bool AbstractSparseIDArray<T, T_Size, T_Container>::updateData(uint8_t x, uint8_t y, uint8_t z, T*& data, Args&&... args)
{
    auto id = this->getId(x, y, z);
    if (id == 0)
        return false;
    new (&_data[id - 1]) T(std::forward<Args>(args)...);
    data = &_data[id - 1];
    return true;
}

template <typename T, uint8_t T_Size, template<class...> class T_Container>
template <typename... Args>
void AbstractSparseIDArray<T, T_Size, T_Container>::putData(uint8_t x, uint8_t y, uint8_t z, T*& data, Args&&... args)
{
    auto id = this->getId(x, y, z);
    if (id == 0)
        this->addData(x, y, z, data, std::forward<Args>(args)...);
    else
        this->updateData(x, y, z, data, std::forward<Args>(args)...);
}

template <typename T, uint8_t T_Size, template<class...> class T_Container>
bool AbstractSparseIDArray<T, T_Size, T_Container>::removeData(uint8_t x, uint8_t y, uint8_t z)
{
    auto id = this->getId(x, y, z);
    if (id == 0)
        return false;
    _idFreed.emplace_back(id);
    this->setId(x, y, z, 0);
    this->reset(_data[id - 1]);
    return true;
}

template <typename T, uint8_t T_Size, template<class...> class T_Container>
bool AbstractSparseIDArray<T, T_Size, T_Container>::removeData(uint8_t x, uint8_t y, uint8_t z, T* voxel)
{
    auto id = this->getId(x, y, z);
    if (id == 0)
        return false;
    _idFreed.emplace_back(id);
    this->setId(x, y, z, 0);
    if (voxel)
        *voxel = _data[id - 1];
    this->reset(_data[id - 1]);
    return true;
}

template <typename T, uint8_t T_Size, template<class...> class T_Container>
inline uint16_t AbstractSparseIDArray<T, T_Size, T_Container>::getId(uint8_t x, uint8_t y, uint8_t z) const
{
    if (_data.size() <= std::numeric_limits<uint8_t>::max())
        return (*reinterpret_cast<uint8_t(*)[T_Size][T_Size][T_Size]>(_ids.get()))[x][y][z];
    else
        return (*reinterpret_cast<uint16_t(*)[T_Size][T_Size][T_Size]>(_ids.get()))[x][y][z];
}

template <typename T, uint8_t T_Size, template<class...> class T_Container>
inline void AbstractSparseIDArray<T, T_Size, T_Container>::setId(uint8_t x, uint8_t y, uint8_t z, uint16_t id)
{
    if (_data.size() <= std::numeric_limits<uint8_t>::max())
        (*reinterpret_cast<uint8_t(*)[T_Size][T_Size][T_Size]>(_ids.get()))[x][y][z] = static_cast<uint8_t>(id);
    else
        (*reinterpret_cast<uint16_t(*)[T_Size][T_Size][T_Size]>(_ids.get()))[x][y][z] = id;
}

template <typename T, uint8_t T_Size, template<class...> class T_Container>
uint16_t AbstractSparseIDArray<T, T_Size, T_Container>::getNewId()
{
    if (!_idFreed.empty())
    {
        uint16_t id = _idFreed.back();
        _idFreed.pop_back();
        return id;
    }

    if (_data.size() == std::numeric_limits<uint8_t>::max())
        this->reallocIds<uint8_t, uint16_t>();
    _data.emplace_back();
    return static_cast<uint16_t>(_data.size());
}

template <typename T, uint8_t T_Size, template<class...> class T_Container>
template <typename T_Old, typename T_New>
void AbstractSparseIDArray<T, T_Size, T_Container>::reallocIds()
{
    T_Old* oldArray = reinterpret_cast<T_Old*>(_ids.get());
    T_New* newArray = reinterpret_cast<T_New*>(new uint8_t[T_Size * T_Size * T_Size * sizeof(T_New)]);

    for (size_t i = 0; i < T_Size * T_Size * T_Size; ++i)
        newArray[i] = static_cast<T_New>(oldArray[i]);
    _ids.reset(reinterpret_cast<uint8_t*>(newArray));
}

template <typename T, uint8_t T_Size, template<class...> class T_Container>
template <typename Type>
void AbstractSparseIDArray<T, T_Size, T_Container>::changeId(uint16_t oldId, uint16_t newId)
{
    Type* ids = reinterpret_cast<Type*>(_ids.get());

    for (size_t i = 0; i < T_Size * T_Size * T_Size; ++i)
    {
        if (ids[i] == oldId)
        {
            ids[i] = static_cast<Type>(newId);
            return;
        }
    }
}

template <typename T, uint8_t T_Size, template<class...> class T_Container>
template <typename Type>
inline void AbstractSparseIDArray<T, T_Size, T_Container>::reset(Type& data)
{
    data.~Type();
}

template <typename T, uint8_t T_Size, template<class...> class T_Container>
template <typename Type>
inline void AbstractSparseIDArray<T, T_Size, T_Container>::reset(std::unique_ptr<Type>& data)
{
    data.reset();
}

template <typename T, uint8_t T_Size, template<class...> class T_Container>
template <typename Type>
inline void AbstractSparseIDArray<T, T_Size, T_Container>::reset(std::shared_ptr<Type>& data)
{
    data.reset();
}

template <typename T, uint8_t T_Size, template<class...> class T_Container>
template <typename T_Data>
inline void AbstractSparseIDArray<T, T_Size, T_Container>::copy(T_Container<T_Data> const& other)
{
    _data = other;
}

template <typename T, uint8_t T_Size, template<class...> class T_Container>
template <typename T_Data>
void AbstractSparseIDArray<T, T_Size, T_Container>::copy(T_Container<std::unique_ptr<T_Data>> const& other)
{
    _data.resize(other.size());
    for (size_t i = 0; i < other.size(); ++i)
    {
        _data[i].reset(new T_Data(*other[i]));
    }
}

template <typename T, uint8_t T_Size, template<class...> class T_Container>
void AbstractSparseIDArray<T, T_Size, T_Container>::serialize(std::string& str) const
{
    uint32_t totalSize = 0;
    uint32_t position = static_cast<uint32_t>(str.size());
    str.append(reinterpret_cast<char const*>(&totalSize), sizeof(totalSize));
    const_cast<AbstractSparseIDArray*>(this)->shrinkToFit();

    if (_data.empty())
        return;
    uint16_t nbData = static_cast<uint16_t>(_data.size());
    str.append(reinterpret_cast<char const*>(&nbData), sizeof(nbData));
    if (_data.size() <= 128 && T_Size <= 8) // 128 because it's maximum value storable with 7 bits
    {
        std::vector<SerializationData> data;
        data.reserve(_data.size());
        for (uint16_t pos = 0; pos < (T_Size * T_Size * T_Size) && data.size() < _data.size(); ++pos)
        {
            if (_ids[pos] != 0)
                data.emplace_back(pos, _ids[pos] - 1);
        }
        str.append(reinterpret_cast<char const*>(data.data()), sizeof(SerializationData) * data.size());
    }
    else if (_data.size() <= std::numeric_limits<uint8_t>::max())
        str.append(reinterpret_cast<char const*>(_ids.get()), T_Size * T_Size * T_Size * sizeof(uint8_t));
    else
        str.append(reinterpret_cast<char const*>(_ids.get()), T_Size * T_Size * T_Size * sizeof(uint16_t));
    
    this->serializeContainer(str, _data);
    totalSize = static_cast<uint32_t>(str.size()) - position;
    std::memcpy(&str[position], &totalSize, sizeof(totalSize));
}

template <typename T, uint8_t T_Size, template<class...> class T_Container>
template <typename T_Data>
inline typename std::enable_if<std::is_trivially_copyable<T_Data>::value>::type
AbstractSparseIDArray<T, T_Size, T_Container>::serializeContainer(std::string& str, T_Container<T_Data> const& datas) const
{
    str.append(reinterpret_cast<char const*>(datas.data()), datas.size() * sizeof(T_Data));
}

template <typename T, uint8_t T_Size, template<class...> class T_Container>
template <typename T_Data>
inline typename std::enable_if<!std::is_trivially_copyable<T_Data>::value>::type
AbstractSparseIDArray<T, T_Size, T_Container>::serializeContainer(std::string& str, T_Container<T_Data> const& datas) const
{
    for (auto const& data : datas)
    {
        this->serializeData(str, data);
    }
}

template <typename T, uint8_t T_Size, template<class...> class T_Container>
template <typename T_Data>
inline void AbstractSparseIDArray<T, T_Size, T_Container>::serializeContainer(std::string& str, T_Container<T_Data*> const& datas) const
{
    for (auto data : datas)
    {
        this->serializeData(str, *data);
    }
}

template <typename T, uint8_t T_Size, template<class...> class T_Container>
template <typename T_Data>
inline void AbstractSparseIDArray<T, T_Size, T_Container>::serializeContainer(std::string& str, T_Container<std::unique_ptr<T_Data>> const& datas) const
{
    for (auto const& data : datas)
    {
        this->serializeData(str, *data);
    }
}

template <typename T, uint8_t T_Size, template<class...> class T_Container>
template <typename T_Data>
inline void AbstractSparseIDArray<T, T_Size, T_Container>::serializeContainer(std::string& str, T_Container<std::shared_ptr<T_Data>> const& datas) const
{
    for (auto const& data : datas)
    {
        this->serializeData(str, *data);
    }
}

template <typename T, uint8_t T_Size, template<class...> class T_Container>
template <typename T_Data>
inline typename std::enable_if<std::is_trivially_copyable<T_Data>::value>::type
AbstractSparseIDArray<T, T_Size, T_Container>::serializeData(std::string& str, T_Data const& data) const
{
    str.append(reinterpret_cast<char const*>(&data), sizeof(data));
}

template <typename T, uint8_t T_Size, template<class...> class T_Container>
template <typename T_Data>
inline typename std::enable_if<!std::is_trivially_copyable<T_Data>::value>::type
AbstractSparseIDArray<T, T_Size, T_Container>::serializeData(std::string& str, T_Data const& data) const
{
    data.serialize(str);
}


template <typename T, uint8_t T_Size, template<class...> class T_Container>
size_t AbstractSparseIDArray<T, T_Size, T_Container>::unserialize(char const* str, size_t size)
{
    uint32_t totalSize;
    if (size < sizeof(totalSize))
        return 0;
    std::memcpy(&totalSize, str, sizeof(totalSize));
    if (totalSize > size)
        return 0;
    if (totalSize == 0)
        return sizeof(totalSize);
    
    bool isUint8 = (_data.size() <= std::numeric_limits<uint8_t>::max());
    char const* tmp = str;
    tmp += sizeof(totalSize);
    
    uint16_t nbData;
    uint32_t size3d = T_Size * T_Size * T_Size;
    
    std::memcpy(&nbData, tmp, sizeof(nbData));
    tmp += sizeof(nbData);

    if (nbData <= 128 && T_Size <= 8) // 128 because it's maximum value storable with 7 bits
    {
        if (!isUint8)
            _ids.reset(new uint8_t[size3d]);
        std::memset(reinterpret_cast<char*>(_ids.get()), 0, size3d);
        auto* data = reinterpret_cast<SerializationData const*>(tmp);
        for (size_t i = 0; i < nbData; ++i)
            _ids[data[i].position] = data[i].id + 1;
        tmp += nbData * sizeof(SerializationData);
    }
    else if (nbData <= std::numeric_limits<uint8_t>::max())
    {
        if (!isUint8)
            _ids.reset(new uint8_t[size3d]);
        std::memcpy(reinterpret_cast<char*>(_ids.get()), tmp, size3d);
        tmp += size3d;
    }
    else
    {
        size3d *= sizeof(uint16_t);
        if (isUint8)
            _ids.reset(new uint8_t[size3d]);
        std::memcpy(reinterpret_cast<char*>(_ids.get()), tmp, size3d);
        tmp += size3d;
    }

    _data.resize(nbData);
    tmp += this->unserializeContainer(tmp, size - (tmp - str), _data);
    return tmp - str;
}

template <typename T, uint8_t T_Size, template<class...> class T_Container>
template <typename T_Data>
inline typename std::enable_if<std::is_trivially_copyable<T_Data>::value, size_t>::type
AbstractSparseIDArray<T, T_Size, T_Container>::unserializeContainer(char const* str, size_t, T_Container<T_Data>& datas)
{
    std::memcpy(reinterpret_cast<char*>(&datas[0]), str, datas.size() * sizeof(T_Data));
    return datas.size() * sizeof(T_Data);
}

template <typename T, uint8_t T_Size, template<class...> class T_Container>
template <typename T_Data>
inline typename std::enable_if<!std::is_trivially_copyable<T_Data>::value, size_t>::type
AbstractSparseIDArray<T, T_Size, T_Container>::unserializeContainer(char const* str, size_t size, T_Container<T_Data>& datas)
{
    size_t uSize = 0;
    for (auto& data : datas)
    {
        uSize += this->unserializeData(&str[uSize], size - uSize, data);
    }
    return uSize;
}

template <typename T, uint8_t T_Size, template<class...> class T_Container>
template <typename T_Data>
inline size_t AbstractSparseIDArray<T, T_Size, T_Container>::unserializeContainer(char const* str, size_t size, T_Container<T_Data*>& datas)
{
    size_t uSize = 0;
    for (auto& data : datas)
    {
        data = new T_Data;
        uSize += this->unserializeData(&str[uSize], size - uSize, *data);
    }
    return uSize;
}

template <typename T, uint8_t T_Size, template<class...> class T_Container>
template <typename T_Data>
inline size_t AbstractSparseIDArray<T, T_Size, T_Container>::unserializeContainer(char const* str, size_t size, T_Container<std::unique_ptr<T_Data>>& datas)
{
    size_t uSize = 0;
    for (auto& data : datas)
    {
        data.reset(new T_Data);
        uSize += this->unserializeData(&str[uSize], size - uSize, *data);
    }
    return uSize;
}

template <typename T, uint8_t T_Size, template<class...> class T_Container>
template <typename T_Data>
inline size_t AbstractSparseIDArray<T, T_Size, T_Container>::unserializeContainer(char const* str, size_t size, T_Container<std::shared_ptr<T_Data>>& datas)
{
    size_t uSize = 0;
    for (auto& data : datas)
    {
        data = std::make_shared<T_Data>();
        uSize += this->unserializeData(&str[uSize], size - uSize, *data);
    }
    return uSize;
}

template <typename T, uint8_t T_Size, template<class...> class T_Container>
template <typename T_Data>
inline typename std::enable_if<std::is_trivially_copyable<T_Data>::value, size_t>::type
AbstractSparseIDArray<T, T_Size, T_Container>::unserializeData(char const* str, size_t, T_Data& data)
{
    std::memcpy(reinterpret_cast<char*>(&data), str, sizeof(data));
    return sizeof(data);
}

template <typename T, uint8_t T_Size, template<class...> class T_Container>
template <typename T_Data>
inline typename std::enable_if<!std::is_trivially_copyable<T_Data>::value, size_t>::type
AbstractSparseIDArray<T, T_Size, T_Container>::unserializeData(char const* str, size_t size, T_Data& data)
{
    return data.unserialize(str, size);
}

template <typename T, uint8_t T_Size, template<class...> class T_Container>
void AbstractSparseIDArray<T, T_Size, T_Container>::shrinkToFit()
{
    if (_idFreed.empty())
        return;

    if (_data.size() != _idFreed.size())
    {
        bool isUint8_t = _data.size() <= std::numeric_limits<uint8_t>::max();
        std::sort(_idFreed.begin(), _idFreed.end());
        int last = static_cast<int>(_data.size());
        int lastFreeId = static_cast<int>(_idFreed.size() - 1);
        for (; lastFreeId >= 0 && _idFreed[lastFreeId] == last; --lastFreeId, --last);
        for (size_t i = 0; i < _idFreed.size() && _idFreed[i] < last; ++i)
        {
            uint16_t id = _idFreed[i];
            _data[id - 1] = std::move(_data[last - 1]);

            if (isUint8_t)
                this->changeId<uint8_t>(last, id);
            else
                this->changeId<uint16_t>(last, id);

            --last;
            for (; lastFreeId >= 0 && i < lastFreeId && _idFreed[lastFreeId] == last; --lastFreeId, --last);
        }
        _data.resize(_data.size() - _idFreed.size());

        if (!isUint8_t && _data.size() <= std::numeric_limits<uint8_t>::max())
            this->reallocIds<uint16_t, uint8_t>();
    }
    else
        _data.clear();
    _idFreed.clear();
}


template <typename T, uint8_t T_Size, template<class...> class T_Container>
AbstractSparseIDArray<T, T_Size, T_Container>::SerializationData::SerializationData(uint16_t position, uint16_t id)
    : position(position), id(id)
{
}



template <typename T, uint8_t T_Size, template<class...> class T_Container>
inline T* SparseIDArray<std::unique_ptr<T>, T_Size, T_Container>::findData(uint8_t x, uint8_t y, uint8_t z)
{
    auto id = this->getId(x, y, z);
    return id ? this->_data[id - 1].get() : nullptr;
}

template <typename T, uint8_t T_Size, template<class...> class T_Container>
inline T const* SparseIDArray<std::unique_ptr<T>, T_Size, T_Container>::findData(uint8_t x, uint8_t y, uint8_t z) const
{
    auto id = this->getId(x, y, z);
    return id ? this->_data[id - 1].get() : nullptr;
}

}