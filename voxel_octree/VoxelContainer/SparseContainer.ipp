namespace voxomap
{

template <class T_Voxel, template<class...> class T_Container>
SparseContainer<T_Voxel, T_Container>::SparseContainer()
{
    _voxelId.reset(new uint8_t[NB_VOXELS * NB_VOXELS * NB_VOXELS]);
    std::memset(_voxelId.get(), 0, sizeof(uint8_t) * NB_VOXELS * NB_VOXELS * NB_VOXELS);
}

template <class T_Voxel, template<class...> class T_Container>
SparseContainer<T_Voxel, T_Container>::SparseContainer(SparseContainer const& other)
{
    _voxelData = other._voxelData;
    _idFreed = other._idFreed;
    if (_voxelData.size() <= std::numeric_limits<uint8_t>::max())
    {
        _voxelId.reset(new uint8_t[NB_VOXELS * NB_VOXELS * NB_VOXELS]);
        std::memcpy(_voxelId.get(), other._voxelId.get(), NB_VOXELS * NB_VOXELS * NB_VOXELS * sizeof(uint8_t));
    }
    else
    {
        _voxelId.reset(reinterpret_cast<uint8_t*>(new uint16_t[NB_VOXELS * NB_VOXELS * NB_VOXELS]));
        std::memcpy(_voxelId.get(), other._voxelId.get(), NB_VOXELS * NB_VOXELS * NB_VOXELS * sizeof(uint16_t));
    }
}

template <class T_Voxel, template<class...> class T_Container>
inline uint16_t SparseContainer<T_Voxel, T_Container>::getNbVoxel() const
{
    return static_cast<uint16_t>(_voxelData.size() - _idFreed.size());
}

template <class T_Voxel, template<class...> class T_Container>
inline bool SparseContainer<T_Voxel, T_Container>::hasVoxel(uint8_t x) const
{
    static uint16_t cmp_array[NB_VOXELS * NB_VOXELS] = { 0 };
    const uint16_t ID_SIZE = _voxelData.size() <= std::numeric_limits<uint8_t>::max() ? sizeof(uint8_t) : sizeof(uint16_t);
    const uint16_t XSIZE = NB_VOXELS * NB_VOXELS * ID_SIZE;

    return std::memcmp(_voxelId.get() + (XSIZE * uint16_t(x)), cmp_array, XSIZE) != 0;
}

template <class T_Voxel, template<class...> class T_Container>
inline bool SparseContainer<T_Voxel, T_Container>::hasVoxel(uint8_t x, uint8_t y) const
{
    static uint16_t cmp_array[NB_VOXELS] = { 0 };
    const uint16_t ID_SIZE = _voxelData.size() <= std::numeric_limits<uint8_t>::max() ? sizeof(uint8_t) : sizeof(uint16_t);
    const uint16_t XSIZE = NB_VOXELS * NB_VOXELS * ID_SIZE;
    const uint16_t YSIZE = NB_VOXELS * ID_SIZE;

    return std::memcmp(_voxelId.get() + (XSIZE * uint16_t(x)) + (YSIZE * uint16_t(y)), cmp_array, YSIZE) != 0;
}

template <class T_Voxel, template<class...> class T_Container>
inline bool SparseContainer<T_Voxel, T_Container>::hasVoxel(uint8_t x, uint8_t y, uint8_t z) const
{
    return this->getId(x, y, z) != 0;
}

template <class T_Voxel, template<class...> class T_Container>
T_Voxel* SparseContainer<T_Voxel, T_Container>::findVoxel(uint8_t x, uint8_t y, uint8_t z)
{
    auto id = this->getId(x, y, z);
    return id ? &_voxelData[id - 1] : nullptr;
}

template <class T_Voxel, template<class...> class T_Container>
T_Voxel const* SparseContainer<T_Voxel, T_Container>::findVoxel(uint8_t x, uint8_t y, uint8_t z) const
{
    auto id = this->getId(x, y, z);
    return id ? &_voxelData[id - 1] : nullptr;
}

template <class T_Voxel, template<class...> class T_Container>
template <typename Iterator>
T_Voxel* SparseContainer<T_Voxel, T_Container>::findVoxel(Iterator& it)
{
    it.voxel_container = static_cast<decltype(it.voxel_container)>(this);
    return this->findVoxel(it.x, it.y, it.z);
}

template <class T_Voxel, template<class...> class T_Container>
template <typename Iterator>
T_Voxel const* SparseContainer<T_Voxel, T_Container>::findVoxel(Iterator& it) const
{
    it.voxel_container = static_cast<decltype(it.voxel_container)>(const_cast<SparseContainer<T_Voxel, T_Container>*>(this));
    return this->findVoxel(it.x, it.y, it.z);
}

template <class T_Voxel, template<class...> class T_Container>
template <typename Iterator, typename... Args>
bool SparseContainer<T_Voxel, T_Container>::addVoxel(Iterator& it, Args&&... args)
{
    auto id = this->getId(it.x, it.y, it.z);

    if (id != 0)
    {
        it.voxel = &_voxelData[id - 1];
        return false;
    }
    id = getNewVoxelDataId();
    new (&_voxelData[id - 1]) T_Voxel(std::forward<Args>(args)...);
    this->setId(it.x, it.y, it.z, id);
    it.voxel = &_voxelData[id - 1];
    return true;
}

template <class T_Voxel, template<class...> class T_Container>
template <typename Iterator, typename... Args>
bool SparseContainer<T_Voxel, T_Container>::updateVoxel(Iterator& it, Args&&... args)
{
    auto id = this->getId(it.x, it.y, it.z);
    if (id == 0)
        return false;
    new (&_voxelData[id - 1]) T_Voxel(std::forward<Args>(args)...);
    it.voxel = &_voxelData[id - 1];
    return true;
}

template <class T_Voxel, template<class...> class T_Container>
template <typename Iterator, typename... Args>
void SparseContainer<T_Voxel, T_Container>::putVoxel(Iterator& it, Args&&... args)
{
    auto id = this->getId(it.x, it.y, it.z);
    if (id == 0)
        this->addVoxel(it, std::forward<Args>(args)...);
    else
        this->updateVoxel(it, std::forward<Args>(args)...);
}

template <class T_Voxel, template<class...> class T_Container>
template <typename Iterator>
bool SparseContainer<T_Voxel, T_Container>::removeVoxel(Iterator const& it, VoxelData* voxel)
{
    auto id = this->getId(it.x, it.y, it.z);
    if (id == 0)
        return false;
    _idFreed.emplace_back(id);
    this->setId(it.x, it.y, it.z, 0);
    if (voxel)
        *voxel = _voxelData[id - 1];
    return true;
}

template <class T_Voxel, template<class...> class T_Container>
template <typename Iterator>
void SparseContainer<T_Voxel, T_Container>::exploreVoxel(Iterator& it, std::function<void(Iterator const&)> const& predicate) const
{
    for (it.x = 0; it.x < NB_VOXELS; ++it.x)
    {
        if (!this->hasVoxel(it.x))
            continue;

        for (it.y = 0; it.y < NB_VOXELS; ++it.y)
        {
            if (!this->hasVoxel(it.x, it.y))
                continue;

            for (it.z = 0; it.z < NB_VOXELS; ++it.z)
            {
                it.voxel = const_cast<T_Voxel*>(this->findVoxel(it.x, it.y, it.z));
                if (it.voxel)
                    predicate(it);
            }
        }
    }
}

template <class T_Voxel, template<class...> class T_Container>
void SparseContainer<T_Voxel, T_Container>::serialize(std::string& str) const
{
    uint16_t nbVoxel = static_cast<uint16_t>(_voxelData.size());
    uint16_t nbVoxelFree = static_cast<uint16_t>(_idFreed.size());
    str.append(reinterpret_cast<char const*>(&nbVoxel), sizeof(nbVoxel));
    str.append(reinterpret_cast<char const*>(&nbVoxelFree), sizeof(nbVoxelFree));
    if (nbVoxel)
        str.append(reinterpret_cast<char const*>(_voxelData.data()), nbVoxel * sizeof(T_Voxel));
    if (nbVoxelFree)
        str.append(reinterpret_cast<char const*>(_idFreed.data()), nbVoxelFree * sizeof(uint16_t));
    if (_voxelData.size() <= 128 && NB_VOXELS <= 8) // 128 because it's maximum value storable with 7 bits
    {
        std::vector<SerializationData> data;
        data.reserve(_voxelData.size());
        for (uint16_t pos = 0; pos < (NB_VOXELS * NB_VOXELS * NB_VOXELS); ++pos)
        {
            if (_voxelId[pos] != 0)
                data.emplace_back(pos, _voxelId[pos] - 1);
        }
        str.append(reinterpret_cast<char const*>(data.data()), sizeof(SerializationData) * data.size());
    }
    else if (_voxelData.size() <= std::numeric_limits<uint8_t>::max())
        str.append(reinterpret_cast<char const*>(_voxelId.get()), NB_VOXELS * NB_VOXELS * NB_VOXELS * sizeof(uint8_t));
    else
        str.append(reinterpret_cast<char const*>(_voxelId.get()), NB_VOXELS * NB_VOXELS * NB_VOXELS * sizeof(uint16_t));
}

template <class T_Voxel, template<class...> class T_Container>
size_t SparseContainer<T_Voxel, T_Container>::unserialize(char const* str, size_t size)
{
    size_t minSize = 2 * sizeof(uint16_t);
    if (size < minSize)
        return 0;
    bool isUint8 = (_voxelData.size() <= std::numeric_limits<uint8_t>::max());
    uint16_t nbVoxel;
    uint16_t nbVoxelFree;
    char const* tmp = str;

    std::memcpy(&nbVoxel, tmp, sizeof(nbVoxel));
    tmp += sizeof(nbVoxel);
    std::memcpy(&nbVoxelFree, tmp, sizeof(nbVoxelFree));
    tmp += sizeof(nbVoxelFree);
    minSize += nbVoxel * sizeof(T_Voxel) + nbVoxelFree * sizeof(uint16_t);
    if (nbVoxel <= 128 && NB_VOXELS <= 8) // 128 because it's maximum value storable with 7 bits
        minSize += nbVoxel * sizeof(SerializationData);
    else if (_voxelData.size() <= std::numeric_limits<uint8_t>::max())
        minSize += NB_VOXELS * NB_VOXELS * NB_VOXELS * sizeof(uint8_t);
    else
        minSize += NB_VOXELS * NB_VOXELS * NB_VOXELS * sizeof(uint16_t);
    if (size < minSize)
        return 0;

    if (nbVoxel)
    {
        _voxelData.resize(nbVoxel);
        std::memcpy(const_cast<char*>(reinterpret_cast<char const*>(_voxelData.data())), tmp, nbVoxel * sizeof(T_Voxel));
        tmp += nbVoxel * sizeof(T_Voxel);
    }
    if (nbVoxelFree)
    {
        _idFreed.resize(nbVoxelFree);
        std::memcpy(const_cast<char*>(reinterpret_cast<char const*>(_idFreed.data())), tmp, nbVoxelFree * sizeof(uint16_t));
        tmp += nbVoxelFree * sizeof(uint16_t);
    }
    if (nbVoxel <= 128 && NB_VOXELS <= 8) // 128 because it's maximum value storable with 7 bits
    {
        if (!isUint8)
            _voxelId.reset(new uint8_t[NB_VOXELS * NB_VOXELS * NB_VOXELS]);
        std::memset(reinterpret_cast<char*>(_voxelId.get()), 0, NB_VOXELS * NB_VOXELS * NB_VOXELS * sizeof(uint8_t));
        auto* data = reinterpret_cast<SerializationData const*>(tmp);
        for (uint32_t i = 0; i < nbVoxel; ++i)
        {
            _voxelId[data[i].voxel_pos] = data[i].voxel_id + 1;
        }
    }
    else if (_voxelData.size() <= std::numeric_limits<uint8_t>::max())
    {
        if (!isUint8)
            _voxelId.reset(new uint8_t[NB_VOXELS * NB_VOXELS * NB_VOXELS]);
        std::memcpy(reinterpret_cast<char*>(_voxelId.get()), tmp, NB_VOXELS * NB_VOXELS * NB_VOXELS * sizeof(uint8_t));
    }
    else
    {
        if (isUint8)
            _voxelId.reset(reinterpret_cast<uint8_t*>(new uint16_t[NB_VOXELS * NB_VOXELS * NB_VOXELS]));
        std::memcpy(reinterpret_cast<char*>(_voxelId.get()), tmp, NB_VOXELS * NB_VOXELS * NB_VOXELS * sizeof(uint16_t));
    }
    return minSize;
}

template <class T_Voxel, template<class...> class T_Container>
inline uint16_t SparseContainer<T_Voxel, T_Container>::getId(uint8_t x, uint8_t y, uint8_t z) const
{
    if (_voxelData.size() <= std::numeric_limits<uint8_t>::max())
        return (*reinterpret_cast<uint8_t(*)[NB_VOXELS][NB_VOXELS][NB_VOXELS]>(_voxelId.get()))[x][y][z];
    else
        return (*reinterpret_cast<uint16_t(*)[NB_VOXELS][NB_VOXELS][NB_VOXELS]>(_voxelId.get()))[x][y][z];
}

template <class T_Voxel, template<class...> class T_Container>
inline void SparseContainer<T_Voxel, T_Container>::setId(uint8_t x, uint8_t y, uint8_t z, uint16_t id)
{
    if (_voxelData.size() <= std::numeric_limits<uint8_t>::max())
        (*reinterpret_cast<uint8_t(*)[NB_VOXELS][NB_VOXELS][NB_VOXELS]>(_voxelId.get()))[x][y][z] = static_cast<uint8_t>(id);
    else
        (*reinterpret_cast<uint16_t(*)[NB_VOXELS][NB_VOXELS][NB_VOXELS]>(_voxelId.get()))[x][y][z] = id;
}

template <class T_Voxel, template<class...> class T_Container>
uint16_t SparseContainer<T_Voxel, T_Container>::getNewVoxelDataId()
{
    if (!_idFreed.empty())
    {
        uint16_t id = _idFreed.back();
        _idFreed.pop_back();
        return id;
    }

    if (_voxelData.size() == std::numeric_limits<uint8_t>::max())
        this->reallocToUint16_t();
    _voxelData.emplace_back();
    return static_cast<uint16_t>(_voxelData.size());
}

template <class T_Voxel, template<class...> class T_Container>
void SparseContainer<T_Voxel, T_Container>::reallocToUint16_t()
{
    uint16_t* newArray = new uint16_t[NB_VOXELS * NB_VOXELS * NB_VOXELS];

    for (uint16_t i = 0; i < NB_VOXELS * NB_VOXELS * NB_VOXELS; ++i)
        newArray[i] = _voxelId[i];
    _voxelId.reset(reinterpret_cast<uint8_t*>(newArray));
}


template <class T_Voxel, template<class...> class T_Container>
SparseContainer<T_Voxel, T_Container>::SerializationData::SerializationData(uint16_t voxel_pos, uint16_t voxel_id)
    : voxel_pos(voxel_pos), voxel_id(voxel_id)
{
}

}