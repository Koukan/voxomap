namespace voxomap
{

template <class T_Voxel, template<class...> class T_Container>
SmartArea<T_Voxel, T_Container>::SmartArea()
{
    _voxelId.reset(new uint8_t[NB_VOXELS * NB_VOXELS * NB_VOXELS]);
    ::memset(_voxelId.get(), 0, sizeof(uint8_t) * NB_VOXELS * NB_VOXELS * NB_VOXELS);
}

template <class T_Voxel, template<class...> class T_Container>
SmartArea<T_Voxel, T_Container>::SmartArea(SmartArea const& other)
{
    _voxelData = other._voxelData;
    _idFreed = other._idFreed;
    if (_voxelData.size() <= std::numeric_limits<uint8_t>::max())
    {
        _voxelId.reset(new uint8_t[NB_VOXELS * NB_VOXELS * NB_VOXELS]);
        ::memcpy(_voxelId.get(), other._voxelId.get(), NB_VOXELS * NB_VOXELS * NB_VOXELS * sizeof(uint8_t));
    }
    else
    {
        _voxelId.reset(reinterpret_cast<uint8_t*>(new uint16_t[NB_VOXELS * NB_VOXELS * NB_VOXELS]));
        ::memcpy(_voxelId.get(), other._voxelId.get(), NB_VOXELS * NB_VOXELS * NB_VOXELS * sizeof(uint16_t));
    }
}

template <class T_Voxel, template<class...> class T_Container>
inline uint16_t SmartArea<T_Voxel, T_Container>::getNbVoxel() const
{
    return static_cast<uint16_t>(_voxelData.size() - _idFreed.size());
}

template <class T_Voxel, template<class...> class T_Container>
inline bool SmartArea<T_Voxel, T_Container>::hasVoxel(uint8_t x, uint8_t y, uint8_t z) const
{
    return this->getId(x, y, z) != 0;
}

template <class T_Voxel, template<class...> class T_Container>
inline T_Voxel* SmartArea<T_Voxel, T_Container>::getVoxel(uint8_t x, uint8_t y, uint8_t z)
{
    auto id = this->getId(x, y, z);
    return id ? &_voxelData[id - 1] : nullptr;
}

template <class T_Voxel, template<class...> class T_Container>
template <typename T_Area, typename... Args>
T_Voxel* SmartArea<T_Voxel, T_Container>::addVoxel(VoxelNode<T_Area>& node, uint8_t x, uint8_t y, uint8_t z, Args&&... args)
{
    auto id = this->getId(x, y, z);
    if (id != 0)
        return nullptr;
    id = getNewVoxelDataId();
    new (&_voxelData[id - 1]) T_Voxel(std::forward<Args>(args)...);
    this->setId(x, y, z, id);
    return &_voxelData[id - 1];
}

template <class T_Voxel, template<class...> class T_Container>
template <typename T_Area, typename... Args>
T_Voxel* SmartArea<T_Voxel, T_Container>::updateVoxel(VoxelNode<T_Area>& node, uint8_t x, uint8_t y, uint8_t z, Args&&... args)
{
    auto id = this->getId(x, y, z);
    if (id == 0)
        return nullptr;
    new (&_voxelData[id - 1]) T_Voxel(std::forward<Args>(args)...);
    return &_voxelData[id - 1];
}

template <class T_Voxel, template<class...> class T_Container>
template <typename T_Area, typename... Args>
T_Voxel* SmartArea<T_Voxel, T_Container>::putVoxel(VoxelNode<T_Area>& node, uint8_t x, uint8_t y, uint8_t z, Args&&... args)
{
    auto id = this->getId(x, y, z);
    if (id == 0)
        return this->addVoxel(node, x, y, z, std::forward<Args>(args)...);
    else
        return this->updateVoxel(node, x, y, z, std::forward<Args>(args)...);
}

template <class T_Voxel, template<class...> class T_Container>
template <typename T_Area>
bool SmartArea<T_Voxel, T_Container>::removeVoxel(VoxelNode<T_Area>& node, uint8_t x, uint8_t y, uint8_t z)
{
    auto id = this->getId(x, y, z);
    if (id == 0)
        return false;
    _idFreed.emplace_back(id);
    this->setId(x, y, z, 0);
    return true;
}

template <class T_Voxel, template<class...> class T_Container>
template <typename T_Area>
bool SmartArea<T_Voxel, T_Container>::removeVoxel(VoxelNode<T_Area>& node, uint8_t x, uint8_t y, uint8_t z, typename SmartArea<T_Voxel, T_Container>::VoxelData& data)
{
    auto id = this->getId(x, y, z);
    if (id == 0)
        return false;
    _idFreed.emplace_back(id);
    this->setId(x, y, z, 0);
    data = _voxelData[id];
    return true;
}

template <class T_Voxel, template<class...> class T_Container>
void SmartArea<T_Voxel, T_Container>::serialize(std::string& str) const
{
    uint16_t nbVoxel = _voxelData.size();
    uint16_t nbVoxelFree = _idFreed.size();
    str.append(reinterpret_cast<char const*>(&nbVoxel), sizeof(nbVoxel));
    str.append(reinterpret_cast<char const*>(&nbVoxelFree), sizeof(nbVoxelFree));
    if (nbVoxel)
        str.append(reinterpret_cast<char const*>(_voxelData.data()), nbVoxel * sizeof(T_Voxel));
    if (nbVoxelFree)
        str.append(reinterpret_cast<char const*>(_idFreed.data()), nbVoxelFree * sizeof(uint16_t));
    if (_voxelData.size() <= std::numeric_limits<uint8_t>::max())
        str.append(reinterpret_cast<char const*>(_voxelId.get()), NB_VOXELS * NB_VOXELS * NB_VOXELS * sizeof(uint8_t));
    else
        str.append(reinterpret_cast<char const*>(_voxelId.get()), NB_VOXELS * NB_VOXELS * NB_VOXELS * sizeof(uint16_t));
}

template <class T_Voxel, template<class...> class T_Container>
size_t SmartArea<T_Voxel, T_Container>::unserialize(char const* str, size_t size)
{
    size_t minSize = 2 * sizeof(uint16_t);
    if (size < minSize)
        return 0;
    bool isUint8 = (_voxelData.size() <= std::numeric_limits<uint8_t>::max());
    uint16_t nbVoxel;
    uint16_t nbVoxelFree;
    char const* tmp = str;

    ::memcpy(&nbVoxel, tmp, sizeof(nbVoxel));
    tmp += sizeof(nbVoxel);
    ::memcpy(&nbVoxelFree, tmp, sizeof(nbVoxelFree));
    tmp += sizeof(nbVoxelFree);
    minSize += nbVoxel * sizeof(T_Voxel) + nbVoxelFree * sizeof(uint16_t);
    if (_voxelData.size() <= std::numeric_limits<uint8_t>::max())
        minSize += NB_VOXELS * NB_VOXELS * NB_VOXELS * sizeof(uint8_t);
    else
        minSize += NB_VOXELS * NB_VOXELS * NB_VOXELS * sizeof(uint16_t);
    if (size < minSize)
        return 0;

    if (nbVoxel)
    {
        _voxelData.resize(nbVoxel);
        ::memcpy(const_cast<char*>(reinterpret_cast<char const*>(_voxelData.data())), tmp, nbVoxel * sizeof(T_Voxel));
        tmp += nbVoxel * sizeof(T_Voxel);
    }
    if (nbVoxelFree)
    {
        _idFreed.resize(nbVoxelFree);
        ::memcpy(const_cast<char*>(reinterpret_cast<char const*>(_idFreed.data())), tmp, nbVoxelFree * sizeof(uint16_t));
        tmp += nbVoxelFree * sizeof(uint16_t);
    }
    if (_voxelData.size() <= std::numeric_limits<uint8_t>::max())
    {
        if (!isUint8)
            _voxelId.reset(new uint8_t[NB_VOXELS * NB_VOXELS * NB_VOXELS]);
        ::memcpy(reinterpret_cast<char*>(_voxelId.get()), tmp, NB_VOXELS * NB_VOXELS * NB_VOXELS * sizeof(uint8_t));
    }
    else
    {
        if (isUint8)
            _voxelId.reset(reinterpret_cast<uint8_t*>(new uint16_t[NB_VOXELS * NB_VOXELS * NB_VOXELS]));
        ::memcpy(reinterpret_cast<char*>(_voxelId.get()), tmp, NB_VOXELS * NB_VOXELS * NB_VOXELS * sizeof(uint16_t));
    }
    return minSize;
}


template <class T_Voxel, template<class...> class T_Container>
inline uint16_t SmartArea<T_Voxel, T_Container>::getId(uint8_t x, uint8_t y, uint8_t z) const
{
    if (_voxelData.size() <= std::numeric_limits<uint8_t>::max())
        return (*reinterpret_cast<uint8_t(*)[NB_VOXELS][NB_VOXELS][NB_VOXELS]>(_voxelId.get()))[x][y][z];
    else
        return (*reinterpret_cast<uint16_t(*)[NB_VOXELS][NB_VOXELS][NB_VOXELS]>(_voxelId.get()))[x][y][z];
}

template <class T_Voxel, template<class...> class T_Container>
inline void SmartArea<T_Voxel, T_Container>::setId(uint8_t x, uint8_t y, uint8_t z, uint16_t id)
{
    if (_voxelData.size() <= std::numeric_limits<uint8_t>::max())
        (*reinterpret_cast<uint8_t(*)[NB_VOXELS][NB_VOXELS][NB_VOXELS]>(_voxelId.get()))[x][y][z] = static_cast<uint8_t>(id);
    else
        (*reinterpret_cast<uint16_t(*)[NB_VOXELS][NB_VOXELS][NB_VOXELS]>(_voxelId.get()))[x][y][z] = id;
}

template <class T_Voxel, template<class...> class T_Container>
uint16_t SmartArea<T_Voxel, T_Container>::getNewVoxelDataId()
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
void SmartArea<T_Voxel, T_Container>::reallocToUint16_t()
{
    uint16_t* newArray = new uint16_t[NB_VOXELS * NB_VOXELS * NB_VOXELS];

    for (uint16_t i = 0; i < NB_VOXELS * NB_VOXELS * NB_VOXELS; ++i)
        newArray[i] = _voxelId[i];
    _voxelId.reset(reinterpret_cast<uint8_t*>(newArray));
}

}