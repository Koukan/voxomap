namespace voxomap
{

template <class Container>
ArraySuperContainer<Container>::ArraySuperContainer(ArraySuperContainer const& other)
{
	for (int x = 0; x < NB_CONTAINERS; ++x)
	{
		for (int y = 0; y < NB_CONTAINERS; ++y)
		{
			for (int z = 0; z < NB_CONTAINERS; ++z)
			{
				if (other._containerArray[x][y][z])
                    _containerArray[x][y][z].reset(new Container(*other._containerArray[x][y][z]));
			}
		}
	}
}

template <class Container>
inline uint16_t ArraySuperContainer<Container>::getNbVoxel() const
{
	return _nbVoxels;
}

template <class Container>
bool ArraySuperContainer<Container>::hasContainer(uint8_t x) const
{
    static const Container* _cmp_array[NB_CONTAINERS * NB_CONTAINERS];
    return std::memcmp(&_containerArray[x], _cmp_array, NB_CONTAINERS * NB_CONTAINERS * sizeof(Container*));
}

template <class Container>
bool ArraySuperContainer<Container>::hasContainer(uint8_t x, uint8_t y) const
{
    static const Container* _cmp_array[NB_CONTAINERS];
    return std::memcmp(&_containerArray[x][y], _cmp_array, NB_CONTAINERS * sizeof(Container*));
}

template <class Container>
bool ArraySuperContainer<Container>::hasContainer(uint8_t x, uint8_t y, uint8_t z) const
{
    return _containerArray[x][y][z] != nullptr;
}

template <class Container>
template <typename Iterator>
inline typename ArraySuperContainer<Container>::VoxelData* ArraySuperContainer<Container>::findVoxel(Iterator& it)
{
    uint8_t sx = it.containerPosition[SUPERCONTAINER_ID].x;
    uint8_t sy = it.containerPosition[SUPERCONTAINER_ID].y;
    uint8_t sz = it.containerPosition[SUPERCONTAINER_ID].z;
	auto& container = _containerArray[sx][sy][sz];
	if (!container)
		return nullptr;

	return container->findVoxel(it);
}

template <class Container>
template <typename Iterator>
inline typename ArraySuperContainer<Container>::VoxelData const* ArraySuperContainer<Container>::findVoxel(Iterator& it) const
{
    uint8_t sx = it.containerPosition[SUPERCONTAINER_ID].x;
    uint8_t sy = it.containerPosition[SUPERCONTAINER_ID].y;
    uint8_t sz = it.containerPosition[SUPERCONTAINER_ID].z;
	auto container = _containerArray[sx][sy][sz];
	if (!container)
		return nullptr;

	return container->findVoxel(it);
}

template <class Container>
Container* ArraySuperContainer<Container>::findContainer(uint8_t x, uint8_t y, uint8_t z)
{
	return _containerArray[x][y][z].get();
}

template <class Container>
Container const* ArraySuperContainer<Container>::findContainer(uint8_t x, uint8_t y, uint8_t z) const
{
	return _containerArray[x][y][z].get();
}

template <class Container>
template <typename Iterator, typename... Args>
bool ArraySuperContainer<Container>::addVoxel(Iterator& it, Args&&... args)
{
    uint8_t sx = it.containerPosition[SUPERCONTAINER_ID].x;
    uint8_t sy = it.containerPosition[SUPERCONTAINER_ID].y;
    uint8_t sz = it.containerPosition[SUPERCONTAINER_ID].z;
	auto& container = _containerArray[sx][sy][sz];
	 
	if (!container)
		container.reset(new Container());

	_nbVoxels -= container->getNbVoxel();
	auto ret = container->addVoxel(it, std::forward<Args>(args)...);
	_nbVoxels += container->getNbVoxel();
	return ret;
}

template <class Container>
template <typename Iterator, typename... Args>
bool ArraySuperContainer<Container>::updateVoxel(Iterator& it, Args&&... args)
{
    uint8_t sx = it.containerPosition[SUPERCONTAINER_ID].x;
    uint8_t sy = it.containerPosition[SUPERCONTAINER_ID].y;
    uint8_t sz = it.containerPosition[SUPERCONTAINER_ID].z;
	auto& container = _containerArray[sx][sy][sz];

	if (!container)
		return false;

	return container->updateVoxel(it, std::forward<Args>(args)...);
}

template <class Container>
template <typename Iterator, typename... Args>
void ArraySuperContainer<Container>::putVoxel(Iterator& it, Args&&... args)
{
    uint8_t sx = it.containerPosition[SUPERCONTAINER_ID].x;
    uint8_t sy = it.containerPosition[SUPERCONTAINER_ID].y;
    uint8_t sz = it.containerPosition[SUPERCONTAINER_ID].z;
	auto& container = _containerArray[sx][sy][sz];


	if (container)
	{
		_nbVoxels -= container->getNbVoxel();
		container->putVoxel(it, std::forward<Args>(args)...);
		_nbVoxels += container->getNbVoxel();
	}
	else
	{
		container.reset(new Container());
		container->addVoxel(it, std::forward<Args>(args)...);
		_nbVoxels += container->getNbVoxel();
	}
}

template <class Container>
template <typename Iterator, typename... Args>
bool ArraySuperContainer<Container>::removeVoxel(Iterator const& it, Args&&... args)
{
    uint8_t sx = it.containerPosition[SUPERCONTAINER_ID].x;
    uint8_t sy = it.containerPosition[SUPERCONTAINER_ID].y;
    uint8_t sz = it.containerPosition[SUPERCONTAINER_ID].z;
	auto& container = _containerArray[sx][sy][sz];

	if (!container)
		return it;

	_nbVoxels -= container->getNbVoxel();
	auto ret = container->removeVoxel(it, std::forward<Args>(args)...);
	_nbVoxels += container->getNbVoxel();
	if (container->getNbVoxel() == 0)
		container.reset(nullptr);
	return ret;
}

template <class Container>
void ArraySuperContainer<Container>::serialize(std::string& str) const
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

template <class Container>
size_t ArraySuperContainer<Container>::unserialize(char const* str, size_t size)
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
        auto container = new Container();
        _containerArray[xyz[0]][xyz[1]][xyz[2]].reset(container);
        size_t size = container->unserialize(&str[pos], total_size - pos);
        pos += size;
    }

    return total_size;
}

template <class Container>
template <typename Iterator>
void ArraySuperContainer<Container>::exploreVoxel(Iterator& it, std::function<void(Iterator const&)> const& predicate) const
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

template <class Container>
void ArraySuperContainer<Container>::exploreVoxelContainer(std::function<void(typename Container::VoxelContainer const&)> const& predicate) const
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