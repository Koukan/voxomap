namespace voxomap
{

template <class Container, class T_PoolAllocator>
ArraySuperContainer<Container, T_PoolAllocator>::ArraySuperContainer()
{
    ::memset(_containerArray, 0, sizeof(_containerArray));
}

template <class Container, class T_PoolAllocator>
ArraySuperContainer<Container, T_PoolAllocator>::ArraySuperContainer(ArraySuperContainer const& other)
{
	for (int x = 0; x < NB_CONTAINERS; ++x)
	{
		for (int y = 0; y < NB_CONTAINERS; ++y)
		{
			for (int z = 0; z < NB_CONTAINERS; ++z)
			{
                if (other._containerArray[x][y][z])
                    _containerArray[x][y][z] = T_PoolAllocator::get(*other._containerArray[x][y][z]);
                else
                    _containerArray[x][y][z] = nullptr;
			}
		}
	}
}

template <class Container, class T_PoolAllocator>
ArraySuperContainer<Container, T_PoolAllocator>::~ArraySuperContainer()
{
    for (int x = 0; x < NB_CONTAINERS; ++x)
    {
        for (int y = 0; y < NB_CONTAINERS; ++y)
        {
            for (int z = 0; z < NB_CONTAINERS; ++z)
            {
                if (_containerArray[x][y][z])
                    T_PoolAllocator::release(_containerArray[x][y][z]);
            }
        }
    }
}

template <class Container, class T_PoolAllocator>
inline uint32_t ArraySuperContainer<Container, T_PoolAllocator>::getNbVoxel() const
{
	return _nbVoxels;
}

template <class Container, class T_PoolAllocator>
bool ArraySuperContainer<Container, T_PoolAllocator>::hasContainer(uint8_t x) const
{
    static const Container* _cmp_array[NB_CONTAINERS * NB_CONTAINERS];
    return std::memcmp(&_containerArray[x], _cmp_array, NB_CONTAINERS * NB_CONTAINERS * sizeof(Container*));
}

template <class Container, class T_PoolAllocator>
bool ArraySuperContainer<Container, T_PoolAllocator>::hasContainer(uint8_t x, uint8_t y) const
{
    static const Container* _cmp_array[NB_CONTAINERS];
    return std::memcmp(&_containerArray[x][y], _cmp_array, NB_CONTAINERS * sizeof(Container*));
}

template <class Container, class T_PoolAllocator>
bool ArraySuperContainer<Container, T_PoolAllocator>::hasContainer(uint8_t x, uint8_t y, uint8_t z) const
{
    return _containerArray[x][y][z] != nullptr;
}

template <class Container, class T_PoolAllocator>
template <typename Iterator>
inline typename ArraySuperContainer<Container, T_PoolAllocator>::VoxelData* ArraySuperContainer<Container, T_PoolAllocator>::findVoxel(Iterator& it)
{
    uint8_t sx = it.containerPosition[SUPERCONTAINER_ID].x;
    uint8_t sy = it.containerPosition[SUPERCONTAINER_ID].y;
    uint8_t sz = it.containerPosition[SUPERCONTAINER_ID].z;
	auto& container = _containerArray[sx][sy][sz];
	if (!container)
		return nullptr;

	return container->findVoxel(it);
}

template <class Container, class T_PoolAllocator>
template <typename Iterator>
inline typename ArraySuperContainer<Container, T_PoolAllocator>::VoxelData const* ArraySuperContainer<Container, T_PoolAllocator>::findVoxel(Iterator& it) const
{
    uint8_t sx = it.containerPosition[SUPERCONTAINER_ID].x;
    uint8_t sy = it.containerPosition[SUPERCONTAINER_ID].y;
    uint8_t sz = it.containerPosition[SUPERCONTAINER_ID].z;
	auto container = _containerArray[sx][sy][sz];
	if (!container)
		return nullptr;

	return container->findVoxel(it);
}

template <class Container, class T_PoolAllocator>
Container* ArraySuperContainer<Container, T_PoolAllocator>::findContainer(uint8_t x, uint8_t y, uint8_t z)
{
    return _containerArray[x][y][z];
}

template <class Container, class T_PoolAllocator>
Container const* ArraySuperContainer<Container, T_PoolAllocator>::findContainer(uint8_t x, uint8_t y, uint8_t z) const
{
    return _containerArray[x][y][z];
}

template <class Container, class T_PoolAllocator>
template <typename Iterator, typename... Args>
int ArraySuperContainer<Container, T_PoolAllocator>::addVoxel(Iterator& it, Args&&... args)
{
    uint8_t sx = it.containerPosition[SUPERCONTAINER_ID].x;
    uint8_t sy = it.containerPosition[SUPERCONTAINER_ID].y;
    uint8_t sz = it.containerPosition[SUPERCONTAINER_ID].z;
	auto& container = _containerArray[sx][sy][sz];
	 
    if (!container)
        container = T_PoolAllocator::get();

    int ret = container->addVoxel(it, std::forward<Args>(args)...);
    _nbVoxels += ret;
    return ret;
}

template <class Container, class T_PoolAllocator>
template <typename Iterator, typename... Args>
int ArraySuperContainer<Container, T_PoolAllocator>::updateVoxel(Iterator& it, Args&&... args)
{
    uint8_t sx = it.containerPosition[SUPERCONTAINER_ID].x;
    uint8_t sy = it.containerPosition[SUPERCONTAINER_ID].y;
    uint8_t sz = it.containerPosition[SUPERCONTAINER_ID].z;
	auto& container = _containerArray[sx][sy][sz];

	if (!container)
		return 0;

	return container->updateVoxel(it, std::forward<Args>(args)...);
}

template <class Container, class T_PoolAllocator>
template <typename Iterator, typename... Args>
int ArraySuperContainer<Container, T_PoolAllocator>::putVoxel(Iterator& it, Args&&... args)
{
    uint8_t sx = it.containerPosition[SUPERCONTAINER_ID].x;
    uint8_t sy = it.containerPosition[SUPERCONTAINER_ID].y;
    uint8_t sz = it.containerPosition[SUPERCONTAINER_ID].z;
	auto& container = _containerArray[sx][sy][sz];

	if (container)
	{
        int ret = container->putVoxel(it, std::forward<Args>(args)...);
		_nbVoxels += ret;
        return ret;
	}
	else
	{
        container = T_PoolAllocator::get();
		container->addVoxel(it, std::forward<Args>(args)...);
		++_nbVoxels;
        return 1;
	}
}

template <class Container, class T_PoolAllocator>
template <typename Iterator, typename... Args>
int ArraySuperContainer<Container, T_PoolAllocator>::removeVoxel(Iterator const& it, Args&&... args)
{
    uint8_t sx = it.containerPosition[SUPERCONTAINER_ID].x;
    uint8_t sy = it.containerPosition[SUPERCONTAINER_ID].y;
    uint8_t sz = it.containerPosition[SUPERCONTAINER_ID].z;
	auto& container = _containerArray[sx][sy][sz];

    if (!container)
        return 0;

    int ret = container->removeVoxel(it, std::forward<Args>(args)...);
    _nbVoxels -= ret;
    if (container->getNbVoxel() == 0)
    {
        T_PoolAllocator::release(container);
        container = nullptr;
    }
    return ret;
}

template <class Container, class T_PoolAllocator>
void ArraySuperContainer<Container, T_PoolAllocator>::serialize(std::string& str) const
{
    uint8_t xyz[3];
    uint32_t total_size = 0;
    size_t position = str.size();
    uint16_t nb_container = 0;

    str.append(reinterpret_cast<char*>(&total_size), sizeof(total_size));
    str.append(reinterpret_cast<char*>(&nb_container), sizeof(nb_container));
    str.append(reinterpret_cast<char const*>(&_nbVoxels), sizeof(_nbVoxels));
    for (xyz[0] = 0; xyz[0] < NB_CONTAINERS; ++xyz[0])
    {
        if (!this->hasContainer(xyz[0]))
            continue;

        for (xyz[1] = 0; xyz[1] < NB_CONTAINERS; ++xyz[1])
        {
            if (!this->hasContainer(xyz[0], xyz[1]))
                continue;

            for (xyz[2] = 0; xyz[2] < NB_CONTAINERS; ++xyz[2])
            {
                auto container = this->findContainer(xyz[0], xyz[1], xyz[2]);
                if (container)
                {
                    ++nb_container;
                    str.append(reinterpret_cast<char*>(xyz), sizeof(xyz));
                    container->serialize(str);
                }
            }
        }
    }

    total_size = static_cast<uint32_t>(str.size() - position);
    std::memcpy(&str[position], &total_size, sizeof(total_size));
    position += sizeof(total_size);
    std::memcpy(&str[position], &nb_container, sizeof(nb_container));
}

template <class Container, class T_PoolAllocator>
size_t ArraySuperContainer<Container, T_PoolAllocator>::unserialize(char const* str, size_t size)
{
    uint32_t total_size;
    size_t pos = 0;

    if (size < sizeof(total_size))
        return 0;
    std::memcpy(&total_size, str, sizeof(total_size));
    if (size < total_size)
        return 0;
    uint16_t nb_container;
    uint8_t xyz[3];

    pos += sizeof(total_size);
    std::memcpy(&nb_container, &str[pos], sizeof(nb_container));
    pos += sizeof(nb_container);
    std::memcpy(&_nbVoxels, &str[pos], sizeof(_nbVoxels));
    pos += sizeof(_nbVoxels);
    for (uint16_t i = 0; i < nb_container; ++i)
    {
        std::memcpy(xyz, &str[pos], sizeof(xyz));
        pos += sizeof(xyz);
        auto* container = T_PoolAllocator::get();
        _containerArray[xyz[0]][xyz[1]][xyz[2]] = container;
        size_t size = container->unserialize(&str[pos], total_size - pos);
        pos += size;
    }

    return total_size;
}

template <class Container, class T_PoolAllocator>
template <typename Iterator>
void ArraySuperContainer<Container, T_PoolAllocator>::exploreVoxel(Iterator& it, std::function<void(Iterator const&)> const& predicate) const
{
    uint8_t& sx = it.containerPosition[SUPERCONTAINER_ID].x;
    uint8_t& sy = it.containerPosition[SUPERCONTAINER_ID].y;
    uint8_t& sz = it.containerPosition[SUPERCONTAINER_ID].z;

    for (sx = 0; sx < NB_CONTAINERS; ++sx)
    {
        if (!this->hasContainer(sx))
            continue;

        for (sy = 0; sy < NB_CONTAINERS; ++sy)
        {
            if (!this->hasContainer(sx, sy))
                continue;

            for (sz = 0; sz < NB_CONTAINERS; ++sz)
            {
                if (this->hasContainer(sx, sy, sz))
                    _containerArray[sx][sy][sz]->exploreVoxel(it, predicate);
            }
        }
    }
}

template <class Container, class T_PoolAllocator>
void ArraySuperContainer<Container, T_PoolAllocator>::exploreVoxelContainer(std::function<void(typename Container::VoxelContainer const&)> const& predicate) const
{
    for (uint8_t sx = 0; sx < NB_CONTAINERS; ++sx)
    {
        if (!this->hasContainer(sx))
            continue;

        for (uint8_t sy = 0; sy < NB_CONTAINERS; ++sy)
        {
            if (!this->hasContainer(sx, sy))
                continue;

            for (uint8_t sz = 0; sz < NB_CONTAINERS; ++sz)
            {
                if (this->hasContainer(sx, sy, sz))
                    _containerArray[sx][sy][sz]->exploreVoxelContainer(predicate);
            }
        }
    }
}

}