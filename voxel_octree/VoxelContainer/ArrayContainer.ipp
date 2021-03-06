namespace voxomap
{

template <class T_Voxel>
const typename ArrayContainer<T_Voxel>::VoxelData ArrayContainer<T_Voxel>::_emptyArea[NB_VOXELS][NB_VOXELS][NB_VOXELS];

template <class T_Voxel>
inline ArrayContainer<T_Voxel>::ArrayContainer()
{
    std::memcpy(this->area, _emptyArea, sizeof(_emptyArea));
}

template <class T_Voxel>
inline ArrayContainer<T_Voxel>::ArrayContainer(ArrayContainer const& other)
{
    this->copy(other);
}

template <class T_Voxel>
inline uint16_t ArrayContainer<T_Voxel>::getNbVoxel() const
{
    return nbVoxels;
}

template <class T_Voxel>
bool ArrayContainer<T_Voxel>::hasVoxel(uint8_t x) const
{
    return std::memcmp(&this->area[x], _emptyArea, NB_VOXELS * NB_VOXELS * sizeof(T_Voxel)) != 0;
}

template <class T_Voxel>
bool ArrayContainer<T_Voxel>::hasVoxel(uint8_t x, uint8_t y) const
{
    return std::memcmp(&this->area[x][y], _emptyArea, NB_VOXELS * sizeof(T_Voxel)) != 0;
}

template <class T_Voxel>
bool ArrayContainer<T_Voxel>::hasVoxel(uint8_t x, uint8_t y, uint8_t z) const
{
    return std::memcmp(&this->area[x][y][z], _emptyArea, sizeof(T_Voxel)) != 0;
}

template <class T_Voxel>
inline typename ArrayContainer<T_Voxel>::VoxelData* ArrayContainer<T_Voxel>::findVoxel(uint8_t x, uint8_t y, uint8_t z)
{
    return this->hasVoxel(x, y, z) ? &this->area[x][y][z] : nullptr;
}

template <class T_Voxel>
inline typename ArrayContainer<T_Voxel>::VoxelData const* ArrayContainer<T_Voxel>::findVoxel(uint8_t x, uint8_t y, uint8_t z) const
{
    return this->hasVoxel(x, y, z) ? &this->area[x][y][z] : nullptr;
}

template <class T_Voxel>
template <typename Iterator>
T_Voxel* ArrayContainer<T_Voxel>::findVoxel(Iterator& it)
{
    it.voxelContainer = static_cast<decltype(it.voxelContainer)>(this);
    return this->findVoxel(it.x, it.y, it.z);
}

template <class T_Voxel>
template <typename Iterator>
T_Voxel const* ArrayContainer<T_Voxel>::findVoxel(Iterator& it) const
{
    it.voxelContainer = static_cast<decltype(it.voxelContainer)>(const_cast<ArrayContainer<T_Voxel>*>(this));
    return this->findVoxel(it.x, it.y, it.z);
}

template <class T_Voxel>
template <typename Iterator, typename... Args>
bool ArrayContainer<T_Voxel>::addVoxel(Iterator& it, Args&&... args)
{
    it.voxelContainer = static_cast<decltype(it.voxelContainer)>(const_cast<ArrayContainer<T_Voxel>*>(this));
    if (this->hasVoxel(it.x, it.y, it.z))
        return false;

    it.voxel = &this->area[it.x][it.y][it.z];
    new (it.voxel) VoxelData(std::forward<Args>(args)...);
    ++nbVoxels;
    return true;
}

template <class T_Voxel>
template <typename Iterator, typename... Args>
bool ArrayContainer<T_Voxel>::updateVoxel(Iterator& it, Args&&... args)
{
    it.voxel = this->findVoxel(it.x, it.y, it.z);
    if (!it.voxel)
        return false;
    new (it.voxel) VoxelData(std::forward<Args>(args)...);
    return true;
}

template <class T_Voxel>
template <typename Iterator, typename... Args>
void ArrayContainer<T_Voxel>::putVoxel(Iterator& it, Args&&... args)
{
    if (this->hasVoxel(it.x, it.y, it.z))
        this->updateVoxel(it, std::forward<Args>(args)...);
    else
        this->addVoxel(it, std::forward<Args>(args)...);
}

template <class T_Voxel>
template <typename Iterator>
bool ArrayContainer<T_Voxel>::removeVoxel(Iterator const& it, VoxelData* return_voxel)
{
    VoxelData* voxel = this->findVoxel(it.x, it.y, it.z);

    if (!voxel)
        return false;
    if (return_voxel)
        *return_voxel = *voxel;
    --nbVoxels;
    new (voxel) VoxelData();
    return true;
}

template <class T_Voxel>
template <typename Iterator>
void ArrayContainer<T_Voxel>::exploreVoxel(Iterator& it, std::function<void(Iterator const&)> const& predicate) const
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

template <class T_Voxel>
void ArrayContainer<T_Voxel>::exploreVoxelContainer(std::function<void(ArrayContainer const&)> const& predicate) const
{
    predicate(*this);
}


template <class T_Voxel>
void ArrayContainer<T_Voxel>::serialize(std::string& str) const
{
    str.append(reinterpret_cast<char const*>(&nbVoxels), sizeof(nbVoxels));

    if (nbVoxels * (sizeof(T_Voxel) + sizeof(uint16_t)) < sizeof(this->area))
    {
        T_Voxel const* voxels = reinterpret_cast<T_Voxel const*>(this->area);
        str.reserve(str.size() + nbVoxels * (sizeof(T_Voxel) + sizeof(uint16_t)));
        for (uint16_t id = 0; id < NB_VOXELS * NB_VOXELS * NB_VOXELS; ++id)
        {
            if (std::memcmp(&voxels[id], _emptyArea, sizeof(T_Voxel)) == 0)
                continue;

            str.append(reinterpret_cast<char const*>(&id), sizeof(id));
            str.append(reinterpret_cast<char const*>(&voxels[id]), sizeof(T_Voxel));
        }
    }
    else
        str.append(reinterpret_cast<char const*>(this->area), sizeof(this->area));
}

template <class T_Voxel>
size_t ArrayContainer<T_Voxel>::unserialize(char const* str, size_t size)
{
    if (size < sizeof(nbVoxels))
        return 0;
    size_t uSize = 0;
    std::memcpy(&nbVoxels, str, sizeof(nbVoxels));
    uSize += sizeof(nbVoxels);

    if (nbVoxels * (sizeof(T_Voxel) + sizeof(uint16_t)) < sizeof(this->area))
    {
        uint16_t id;
        T_Voxel* voxels = reinterpret_cast<T_Voxel*>(this->area);
        for (uint16_t i = 0; i < nbVoxels; ++i)
        {
            std::memcpy(&id, &str[uSize], sizeof(id));
            uSize += sizeof(id);
            std::memcpy(&voxels[id], &str[uSize], sizeof(T_Voxel));
            uSize += sizeof(T_Voxel);
        }
    }
    else
    {
        std::memcpy(this->area, &str[uSize], sizeof(this->area));
        uSize += sizeof(this->area);
    }
    return uSize;
}

template <class T_Voxel>
template <typename T>
typename std::enable_if<std::is_trivially_constructible<T>::value>::type ArrayContainer<T_Voxel>::copy(T const& other)
{
    std::memcpy(area, other.area, sizeof(area));
}

template <class T_Voxel>
template <typename T>
typename std::enable_if<!std::is_trivially_constructible<T>::value>::type ArrayContainer<T_Voxel>::copy(T const& other)
{
    for (uint8_t x = 0; x < NB_VOXELS; ++x)
    {
        for (uint8_t y = 0; y < NB_VOXELS; ++y)
        {
            for (uint8_t z = 0; z < NB_VOXELS; ++z)
            {
                new (&this->area[x][y][z]) T_Voxel(other.area[x][y][z]);
            }
        }
    }
}

}