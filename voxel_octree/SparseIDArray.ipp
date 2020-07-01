namespace voxomap
{

template <typename T, uint8_t NB_VOXELS, template<class...> class T_Container>
Sparse3DArray<T, NB_VOXELS, T_Container>::Sparse3DArray()
{
    _ids.reset(new uint8_t[NB_VOXELS * NB_VOXELS * NB_VOXELS]);
    std::memset(_ids.get(), 0, sizeof(uint8_t) * NB_VOXELS * NB_VOXELS * NB_VOXELS);
}

template <typename T, uint8_t NB_VOXELS, template<class...> class T_Container>
Sparse3DArray<T, NB_VOXELS, T_Container>::Sparse3DArray(SparseIDArray const& other)
{
    if (_voxelData.size() <= std::numeric_limits<uint8_t>::max())
    {
        _ids.reset(new uint8_t[NB_VOXELS * NB_VOXELS * NB_VOXELS]);
        std::memcpy(_ids.get(), other._voxelId.get(), NB_VOXELS * NB_VOXELS * NB_VOXELS * sizeof(uint8_t));
    }
    else
    {
        _ids.reset(reinterpret_cast<uint8_t*>(new uint8_t[NB_VOXELS * NB_VOXELS * NB_VOXELS * 2]));
        std::memcpy(_ids.get(), other._voxelId.get(), NB_VOXELS * NB_VOXELS * NB_VOXELS * sizeof(uint16_t));
    }
}

template <typename T, uint8_t NB_VOXELS, template<class...> class T_Container>
inline uint16_t Sparse3DArray<T, NB_VOXELS, T_Container>::getId(uint8_t x, uint8_t y, uint8_t z) const
{
    if (_datas.size() <= std::numeric_limits<uint8_t>::max())
        return (*reinterpret_cast<uint8_t(*)[NB_VOXELS][NB_VOXELS][NB_VOXELS]>(_ids.get()))[x][y][z];
    else
        return (*reinterpret_cast<uint16_t(*)[NB_VOXELS][NB_VOXELS][NB_VOXELS]>(_ids.get()))[x][y][z];
}

template <typename T, uint8_t NB_VOXELS, template<class...> class T_Container>
inline void Sparse3DArray<T, NB_VOXELS, T_Container>::setId(uint8_t x, uint8_t y, uint8_t z, uint16_t id)
{
    if (_datas.size() <= std::numeric_limits<uint8_t>::max())
        (*reinterpret_cast<uint8_t(*)[NB_VOXELS][NB_VOXELS][NB_VOXELS]>(_ids.get()))[x][y][z] = static_cast<uint8_t>(id);
    else
        (*reinterpret_cast<uint16_t(*)[NB_VOXELS][NB_VOXELS][NB_VOXELS]>(_ids.get()))[x][y][z] = id;
}

template <typename T, uint8_t NB_VOXELS, template<class...> class T_Container>
uint16_t Sparse3DArray<T, NB_VOXELS, T_Container>::getNewId()
{
    if (!_idFreed.empty())
    {
        uint16_t id = _idFreed.back();
        _idFreed.pop_back();
        return id;
    }

    if (_datas.size() == std::numeric_limits<uint8_t>::max())
        this->reallocToUint16_t();
    _datas.emplace_back();
    return static_cast<uint16_t>(_datas.size());
}

template <typename T, uint8_t NB_VOXELS, template<class...> class T_Container>
void Sparse3DArray<T, NB_VOXELS, T_Container>::reallocToUint16_t()
{
    uint16_t* newArray = static_cast<uint16_t*>(new uint8_t[NB_VOXELS * NB_VOXELS * NB_VOXELS * 2]);

    for (uint16_t i = 0; i < NB_VOXELS * NB_VOXELS * NB_VOXELS; ++i)
        newArray[i] = _ids[i];
    _ids.reset(reinterpret_cast<uint8_t*>(newArray));
}

template <typename T, uint8_t NB_VOXELS, template<class...> class T_Container>
void Sparse3DArray<T, NB_VOXELS, T_Container>::serialize(std::string& str) const
{
    uint32_t totalSize = 0;
    uint32_t position = str.size();
    uint16_t nbData = static_cast<uint16_t>(_datas.size());
    uint16_t nbDataFree = static_cast<uint16_t>(_idFreed.size());

    str.append(reinterpret_cast<char const*>(&totalSize), sizeof(totalSize));
    str.append(reinterpret_cast<char const*>(&nbData), sizeof(nbData));
    str.append(reinterpret_cast<char const*>(&nbDataFree), sizeof(nbDataFree));
    if (_datas.size() <= 128 && NB_VOXELS <= 8) // 128 because it's maximum value storable with 7 bits
    {
        std::vector<SerializationData> data;
        data.reserve(_datas.size());
        for (uint16_t pos = 0; pos < (NB_VOXELS * NB_VOXELS * NB_VOXELS); ++pos)
        {
            if (_ids[pos] != 0)
                data.emplace_back(pos, _ids[pos] - 1);
        }
        str.append(reinterpret_cast<char const*>(data.data()), sizeof(SerializationData) * data.size());
    }
    else if (_datas.size() <= std::numeric_limits<uint8_t>::max())
        str.append(reinterpret_cast<char const*>(_ids.get()), NB_VOXELS * NB_VOXELS * NB_VOXELS * sizeof(uint8_t));
    else
        str.append(reinterpret_cast<char const*>(_ids.get()), NB_VOXELS * NB_VOXELS * NB_VOXELS * sizeof(uint16_t));

    if (nbData)
        this->serializeData<T>(str);
    if (nbDataFree)
        str.append(reinterpret_cast<char const*>(_idFreed.data()), nbDataFree * sizeof(uint16_t));

    totalSize = str.size() - position;
    std::memcpy(&str[position], &totalSize, sizeof(totalSize));
}

template <typename T, uint8_t NB_VOXELS, template<class...> class T_Container>
template <typename Type>
typename std::enable_if<std::is_trivially_copyable<Type>::value && !std::is_pointer<Type>::value>::type
Sparse3DArray<T, NB_VOXELS, T_Container>::serializeData(std::string& str) const
{
    str.append(reinterpret_cast<char const*>(_datas.data()), _datas.size() * sizeof(T_Voxel));
}

template <typename T, uint8_t NB_VOXELS, template<class...> class T_Container>
template <typename Type>
typename std::enable_if<std::is_pointer<Type>::value>::type
Sparse3DArray<T, NB_VOXELS, T_Container>::serializeData(std::string& str) const
{
    for (auto data : _datas)
    {
        data->serialize(str);
    }
}

template <typename T, uint8_t NB_VOXELS, template<class...> class T_Container>
size_t Sparse3DArray<T, NB_VOXELS, T_Container>::unserialize(char const* str, size_t size)
{
    uint32_t totalSize;
    if (size < sizeof(totalSize))
        return 0;
    std::memcpy(&totalSize, str, sizeof(totalSize));
    if (totalSize > size)
        return 0;

    bool isUint8 = (_datas.size() <= std::numeric_limits<uint8_t>::max());
    char const* tmp = str;
    tmp += sizeof(totalSize);

    uint16_t nbData;
    uint16_t nbDataFree;
    uint32_t size = NB_VOXELS * NB_VOXELS * NB_VOXELS;

    std::memcpy(&nbData, tmp, sizeof(nbData));
    tmp += sizeof(nbData);
    std::memcpy(&nbDataFree, tmp, sizeof(nbDataFree));
    tmp += sizeof(nbDataFree);

    if (nbData <= 128 && NB_VOXELS <= 8) // 128 because it's maximum value storable with 7 bits
    {
        if (!isUint8)
            _ids.reset(new uint8_t[size]);
        std::memset(reinterpret_cast<char*>(_ids.get()), 0, size);
        auto* data = reinterpret_cast<SerializationData const*>(tmp);
        for (size_t i = 0; i < nbData; ++i)
            _ids[data[i].voxel_pos] = data[i].voxel_id + 1;
    }
    else if (nbData <= std::numeric_limits<uint8_t>::max())
    {
        if (!isUint8)
            _ids.reset(new uint8_t[size]);
        std::memcpy(reinterpret_cast<char*>(_ids.get()), tmp, size);
    }
    else
    {
        size *= sizeof(uint16_t);
        if (isUint8)
            _ids.reset(new uint8_t[size]);
        std::memcpy(reinterpret_cast<char*>(_ids.get()), tmp, size);
    }
    tmp += size;

    if (nbData)
    {
        _datas.resize(nbData);
        std::memcpy(reinterpret_cast<char*>(&_datas[0]), tmp, nbData * sizeof(T));
        tmp += nbData * sizeof(T);
    }
    if (nbDataFree)
    {
        _idFreed.resize(nbDataFree);
        std::memcpy(reinterpret_cast<char*>(&_idFreed[0]), tmp, nbDataFree * sizeof(uint16_t));
        tmp += nbDataFree * sizeof(uint16_t);
    }

    return tmp - str;
}

template <typename T, uint8_t NB_VOXELS, template<class...> class T_Container>
template <typename Type>
typename std::enable_if<std::is_trivially_copyable<Type>::value && !std::is_pointer<Type>::value, size_t>::type
Sparse3DArray<T, NB_VOXELS, T_Container>::unserializeData(char const* str, size_t nbData)
{
    _datas.resize(nbData);
    std::memcpy(reinterpret_cast<char*>(&_datas[0]), str, nbData * sizeof(T));
    return nbData * sizeof(T);
}

template <typename T, uint8_t NB_VOXELS, template<class...> class T_Container>
template <typename Type>
typename std::enable_if<std::is_pointer<Type>::value, size_t>::type
Sparse3DArray<T, NB_VOXELS, T_Container>::unserializeData(char const* str, size_t nbData)
{
    _datas.resize(nbData);

}

}