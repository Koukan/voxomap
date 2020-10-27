namespace voxomap
{

template <class Container, template <class...> class InternalContainer>
inline uint16_t SparseSuperContainer<Container, InternalContainer>::getNbVoxel() const
{
	return _nbVoxels;
}
 
template <class Container, template <class...> class InternalContainer>
template <typename Iterator>
inline typename SparseSuperContainer<Container, InternalContainer>::VoxelData*
SparseSuperContainer<Container, InternalContainer>::findVoxel(Iterator& it)
{
	uint8_t sx = std::get<0>(it.containerPosition[SUPERCONTAINER_ID]);
	uint8_t sy = std::get<1>(it.containerPosition[SUPERCONTAINER_ID]);
	uint8_t sz = std::get<2>(it.containerPosition[SUPERCONTAINER_ID]);
	auto container = _sparseArray.findData(sx, sy, sz);

	if (!container)
		return nullptr;
	return container->findVoxel(it);
}

template <class Container, template <class...> class InternalContainer>
template <typename Iterator>
inline typename SparseSuperContainer<Container, InternalContainer>::VoxelData const*
SparseSuperContainer<Container, InternalContainer>::findVoxel(Iterator& it) const
{
	uint8_t sx = std::get<0>(it.containerPosition[SUPERCONTAINER_ID]);
	uint8_t sy = std::get<1>(it.containerPosition[SUPERCONTAINER_ID]);
	uint8_t sz = std::get<2>(it.containerPosition[SUPERCONTAINER_ID]);
	auto container = _sparseArray.findData(sx, sy, sz);

	return container ? container->findVoxel(it) : nullptr;
}

template <class Container, template <class...> class InternalContainer>
bool SparseSuperContainer<Container, InternalContainer>::hasContainer(uint8_t x) const
{
	return _sparseArray.hasData(x);
}

template <class Container, template <class...> class InternalContainer>
bool SparseSuperContainer<Container, InternalContainer>::hasContainer(uint8_t x, uint8_t y) const
{
	return _sparseArray.hasData(x, y);
}

template <class Container, template <class...> class InternalContainer>
bool SparseSuperContainer<Container, InternalContainer>::hasContainer(uint8_t x, uint8_t y, uint8_t z) const
{
	return _sparseArray.hasData(x, y, z);
}

template <class Container, template <class...> class InternalContainer>
Container* SparseSuperContainer<Container, InternalContainer>::findContainer(uint8_t x, uint8_t y, uint8_t z)
{
	return _sparseArray.findData(x, y, z);
}

template <class Container, template <class...> class InternalContainer>
Container const* SparseSuperContainer<Container, InternalContainer>::findContainer(uint8_t x, uint8_t y, uint8_t z) const
{
	return _sparseArray.findData(x, y, z);
}

template <class Container, template <class...> class InternalContainer>
template <typename Iterator, typename... Args>
bool SparseSuperContainer<Container, InternalContainer>::addVoxel(Iterator& it, Args&&... args)
{
	uint8_t sx = std::get<0>(it.containerPosition[SUPERCONTAINER_ID]);
	uint8_t sy = std::get<1>(it.containerPosition[SUPERCONTAINER_ID]);
	uint8_t sz = std::get<2>(it.containerPosition[SUPERCONTAINER_ID]);
	auto container = _sparseArray.findData(sx, sy, sz);

	if (container == nullptr)
	{
		std::unique_ptr<Container>* container_ptr;

		container = new Container();
		_sparseArray.addData(sx, sy, sz, container_ptr);
		container_ptr->reset(container);
	}
	_nbVoxels -= container->getNbVoxel();
	auto ret = container->addVoxel(it, std::forward<Args>(args)...);
	_nbVoxels += container->getNbVoxel();

	return ret;
}

template <class Container, template <class...> class InternalContainer>
template <typename Iterator, typename... Args>
bool SparseSuperContainer<Container, InternalContainer>::updateVoxel(Iterator& it, Args&&... args)
{
	uint8_t sx = std::get<0>(it.containerPosition[SUPERCONTAINER_ID]);
	uint8_t sy = std::get<1>(it.containerPosition[SUPERCONTAINER_ID]);
	uint8_t sz = std::get<2>(it.containerPosition[SUPERCONTAINER_ID]);
	auto container = _sparseArray.findData(sx, sy, sz);

	if (container == nullptr)
		return false;
	return container->updateVoxel(it, std::forward<Args>(args)...);
}

template <class Container, template <class...> class InternalContainer>
template <typename Iterator, typename... Args>
void SparseSuperContainer<Container, InternalContainer>::putVoxel(Iterator& it, Args&&... args)
{
	uint8_t sx = std::get<0>(it.containerPosition[SUPERCONTAINER_ID]);
	uint8_t sy = std::get<1>(it.containerPosition[SUPERCONTAINER_ID]);
	uint8_t sz = std::get<2>(it.containerPosition[SUPERCONTAINER_ID]);
	auto container = _sparseArray.findData(sx, sy, sz);

	if (container == nullptr)
	{
		std::unique_ptr<Container>* container_ptr;

		container = new Container();
		_sparseArray.addData(sx, sy, sz, container_ptr);
		container_ptr->reset(container);
	}
	_nbVoxels -= container->getNbVoxel();
	container->putVoxel(it, std::forward<Args>(args)...);
	_nbVoxels += container->getNbVoxel();
}

template <class Container, template <class...> class InternalContainer>
template <typename Iterator, typename... Args>
bool SparseSuperContainer<Container, InternalContainer>::removeVoxel(Iterator const& it, Args&&... args)
{
	uint8_t sx = std::get<0>(it.containerPosition[SUPERCONTAINER_ID]);
	uint8_t sy = std::get<1>(it.containerPosition[SUPERCONTAINER_ID]);
	uint8_t sz = std::get<2>(it.containerPosition[SUPERCONTAINER_ID]);
	auto container = _sparseArray.findData(sx, sy, sz);

	if (container == nullptr)
		return false;
	_nbVoxels -= container->getNbVoxel();
	auto ret = container->removeVoxel(it, std::forward<Args>(args)...);
	_nbVoxels += container->getNbVoxel();
	if (container->getNbVoxel() == 0)
	{
		_sparseArray.removeData(sx, sy, sz);
	}
	return ret;
}

template <class Container, template <class...> class InternalContainer>
void SparseSuperContainer<Container, InternalContainer>::serialize(std::string& str) const
{
	_sparseArray.serialize(str);
	str.append(reinterpret_cast<char const*>(&_nbVoxels), sizeof(_nbVoxels));
}

template <class Container, template <class...> class InternalContainer>
size_t SparseSuperContainer<Container, InternalContainer>::unserialize(char const* str, size_t size)
{
	size_t uSize = _sparseArray.unserialize(str, size);
	std::memcpy(&_nbVoxels, &str[uSize], sizeof(_nbVoxels));
	uSize += sizeof(_nbVoxels);
	return uSize;
}

template <class Container, template <class...> class InternalContainer>
template <typename Iterator>
void SparseSuperContainer<Container, InternalContainer>::exploreVoxel(Iterator& it, std::function<void(Iterator const&)> const& predicate) const
{
    uint8_t& sx = std::get<0>(it.containerPosition[SUPERCONTAINER_ID]);
    uint8_t& sy = std::get<1>(it.containerPosition[SUPERCONTAINER_ID]);
    uint8_t& sz = std::get<2>(it.containerPosition[SUPERCONTAINER_ID]);
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
				auto container = _sparseArray.findData(sx, sy, sz);
				if (container)
				{
					container->exploreVoxel(it, predicate);
				}
			}
		}
	}
}

}