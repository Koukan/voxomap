namespace voxomap
{

template <class Container>
inline SparseSuperContainer<Container>::SparseSuperContainer()
{
	std::memset(_container_array, 0, sizeof(_container_array));
}

template <class Container>
inline SparseSuperContainer<Container>::SparseSuperContainer(SparseSuperContainer const& other)
	: _nbVoxels(other._nbVoxels)
{
	std::memcpy(_container_array, other._container_array, sizeof(_container_array));
	_containers.resize(other._containers.size());
	for (size_t i = 0; i < other._containers.size(); ++i)
	{
		if (other._containers[i])
		{
			_containers[i].reset(new Container(*other._containers[i]));
		}
	}
}

template <class Container>
inline uint16_t SparseSuperContainer<Container>::getNbVoxel() const
{
	return _nbVoxels;
}
 
template <class Container>
template <typename Iterator>
inline typename SparseSuperContainer<Container>::VoxelData* SparseSuperContainer<Container>::findVoxel(Iterator& it)
{
	uint8_t sx = std::get<0>(it.container_position[SUPERCONTAINER_ID]);
	uint8_t sy = std::get<1>(it.container_position[SUPERCONTAINER_ID]);
	uint8_t sz = std::get<2>(it.container_position[SUPERCONTAINER_ID]);
	auto container_id = _container_array[sx][sy][sz];
	if (!container_id)
		return nullptr;

	return _containers[container_id - 1]->findVoxel(it);
}

template <class Container>
template <typename Iterator>
inline typename SparseSuperContainer<Container>::VoxelData const* SparseSuperContainer<Container>::findVoxel(Iterator& it) const
{
	uint8_t sx = std::get<0>(it.container_position[SUPERCONTAINER_ID]);
	uint8_t sy = std::get<1>(it.container_position[SUPERCONTAINER_ID]);
	uint8_t sz = std::get<2>(it.container_position[SUPERCONTAINER_ID]);
	auto container_id = _container_array[sx][sy][sz];
	if (!container_id)
		return nullptr;

	return _containers[container_id - 1]->findVoxel(it);
}

template <class Container>
bool SparseSuperContainer<Container>::hasContainer(uint8_t x) const
{
    static const uint16_t cmp_array[NB_CONTAINERS * NB_CONTAINERS] = { 0 };
    return std::memcmp(&_container_array[x], cmp_array, sizeof(cmp_array)) != 0;
}

template <class Container>
bool SparseSuperContainer<Container>::hasContainer(uint8_t x, uint8_t y) const
{
    static const uint16_t cmp_array[NB_CONTAINERS] = { 0 };
    return std::memcmp(&_container_array[x][y], cmp_array, sizeof(cmp_array)) != 0;
}

template <class Container>
bool SparseSuperContainer<Container>::hasContainer(uint8_t x, uint8_t y, uint8_t z) const
{
    return _container_array[x][y][z] != 0;
}

template <class Container>
Container* SparseSuperContainer<Container>::findContainer(uint8_t x, uint8_t y, uint8_t z)
{
	auto container_id = _container_array[x][y][z];
	if (!container_id)
		return nullptr;

	return _containers[container_id - 1].get();
}

template <class Container>
Container const* SparseSuperContainer<Container>::findContainer(uint8_t x, uint8_t y, uint8_t z) const
{
	auto container_id = _container_array[x][y][z];
	if (!container_id)
		return nullptr;
	
	return _containers[container_id - 1].get();
}

template <class Container>
template <typename Iterator, typename... Args>
bool SparseSuperContainer<Container>::addVoxel(Iterator& it, Args&&... args)
{
	uint8_t sx = std::get<0>(it.container_position[SUPERCONTAINER_ID]);
	uint8_t sy = std::get<1>(it.container_position[SUPERCONTAINER_ID]);
	uint8_t sz = std::get<2>(it.container_position[SUPERCONTAINER_ID]);
	auto container_id = _container_array[sx][sy][sz];

	if (!container_id)
	{
		_containers.emplace_back(new Container());
		container_id = static_cast<uint16_t>(_containers.size());
		_container_array[sx][sy][sz] = container_id;
	}
	auto container = _containers[container_id - 1].get();

	_nbVoxels -= container->getNbVoxel();
	auto ret = container->addVoxel(it, std::forward<Args>(args)...);
	_nbVoxels += container->getNbVoxel();
	return ret;
}

template <class Container>
template <typename Iterator, typename... Args>
bool SparseSuperContainer<Container>::updateVoxel(Iterator& it, Args&&... args)
{
	uint8_t sx = std::get<0>(it.container_position[SUPERCONTAINER_ID]);
	uint8_t sy = std::get<1>(it.container_position[SUPERCONTAINER_ID]);
	uint8_t sz = std::get<2>(it.container_position[SUPERCONTAINER_ID]);
	auto container_id = _container_array[sx][sy][sz];

	if (!container_id)
		return false;

	return _containers[container_id - 1]->updateVoxel(it, std::forward<Args>(args)...);
}

template <class Container>
template <typename Iterator, typename... Args>
void SparseSuperContainer<Container>::putVoxel(Iterator& it, Args&&... args)
{
	uint8_t sx = std::get<0>(it.container_position[SUPERCONTAINER_ID]);
	uint8_t sy = std::get<1>(it.container_position[SUPERCONTAINER_ID]);
	uint8_t sz = std::get<2>(it.container_position[SUPERCONTAINER_ID]);
	auto& container_id = _container_array[sx][sy][sz];

	if (container_id)
	{
		auto container = _containers[container_id - 1].get();
		_nbVoxels -= container->getNbVoxel();
		container->putVoxel(it, std::forward<Args>(args)...);
		_nbVoxels += container->getNbVoxel();
	}
	else
	{
		auto container = new Container();
		_containers.emplace_back(container);
		container_id = static_cast<uint16_t>(_containers.size());
		container->addVoxel(it, std::forward<Args>(args)...);
		_nbVoxels += container->getNbVoxel();
	}
}

template <class Container>
template <typename Iterator, typename... Args>
bool SparseSuperContainer<Container>::removeVoxel(Iterator const& it, Args&&... args)
{
	uint8_t sx = std::get<0>(it.container_position[SUPERCONTAINER_ID]);
	uint8_t sy = std::get<1>(it.container_position[SUPERCONTAINER_ID]);
	uint8_t sz = std::get<2>(it.container_position[SUPERCONTAINER_ID]);
	auto& container_id = _container_array[sx][sy][sz];
	if (!container_id)
		return it;

	auto& container = _containers[container_id - 1];
	_nbVoxels -= container->getNbVoxel();
	auto ret = container->removeVoxel(it, std::forward<Args>(args)...);
	_nbVoxels += container->getNbVoxel();
	if (container->getNbVoxel() == 0)
	{
		container.reset();
		container_id = 0;
	}
	return ret;
}

template <class Container>
void SparseSuperContainer<Container>::serialize(std::string& str) const
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
size_t SparseSuperContainer<Container>::unserialize(char const* str, size_t size)
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

        _containers.emplace_back(new Container());
        _container_array[xyz[0]][xyz[1]][xyz[2]] = static_cast<uint16_t>(_containers.size());
        size_t size = _containers.back()->unserialize(&str[pos], total_size - pos);
        pos += size;
    }

    return total_size;
}

template <class Container>
template <typename Iterator>
void SparseSuperContainer<Container>::exploreVoxel(Iterator& it, std::function<void(Iterator const&)> const& predicate) const
{
    uint8_t& sx = std::get<0>(it.container_position[SUPERCONTAINER_ID]);
    uint8_t& sy = std::get<1>(it.container_position[SUPERCONTAINER_ID]);
    uint8_t& sz = std::get<2>(it.container_position[SUPERCONTAINER_ID]);
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
				auto container_id = _container_array[sx][sy][sz];
				if (container_id)
				{
                    _containers[container_id - 1]->exploreVoxel(it, predicate);
				}
			}
		}
	}
}

}