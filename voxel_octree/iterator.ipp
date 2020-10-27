namespace voxomap
{

template <class T>
container_iterator<T>& container_iterator<T>::operator++()
{
	if (!this->voxel)
		return *this;

	++this->z;
	if (!this->findNextVoxel(*this->voxelContainer))
	{
		this->x = 0;
		this->y = 0;
		this->z = 0;
		this->voxel = nullptr;
		this->voxelContainer = nullptr;
		this->findNextParentNode(*this->node);
	}
	return *this;
}

template <class T>
inline container_iterator<T> container_iterator<T>::operator++(int)
{
	container_iterator result = *this;
	++(*this);
	return result;
}

template <class T>
inline container_iterator<T>* container_iterator<T>::operator*()
{
	return this;
}

template <class T>
inline container_iterator<T>::operator bool() const
{
	return this->voxel != nullptr && this->node != nullptr;
}

template <class T>
void container_iterator<T>::findNextParentNode(VoxelNode<T>& i_node)
{
	auto parent = i_node.getParent();
	if (parent == nullptr)
	{
		this->node = nullptr;
		return;
	}

	for (uint8_t i = i_node.getChildId() + 1; i < 8; ++i)
	{
		if (!parent->getChildren()[i])
			continue;
		if (this->findNextChildNode(*parent->getChildren()[i]))
			return;
	}

	this->findNextParentNode(*parent);
}

template <class T>
bool container_iterator<T>::findNextChildNode(VoxelNode<T>& i_node)
{
	if (i_node.getSize() == T::NB_VOXELS)
	{
        if (this->findNextVoxel(*i_node.getVoxelContainer()))
        {
            this->node = &i_node;
            return true;
        }
	}
	else
	{
		for (uint8_t i = 0; i < 8; ++i)
		{
			auto child = i_node.getChildren()[i];
			if (child)
			{
				if (this->findNextChildNode(*child))
					return true;
			}
		}
	}
	return false;
}

template <class T>
bool container_iterator<T>::findNextVoxel(VoxelContainer& container)
{
	for (uint8_t ix = this->x; ix < VoxelContainer::NB_VOXELS; ++ix)
	{
        if (!container.hasVoxel(ix))
            continue;

		for (uint8_t iy = this->y; iy < VoxelContainer::NB_VOXELS; ++iy)
		{
            if (!container.hasVoxel(ix, iy))
                continue;

			for (uint8_t iz = this->z; iz < VoxelContainer::NB_VOXELS; ++iz)
			{
				this->voxel = container.findVoxel(ix, iy, iz);
				if (this->voxel)
				{
					this->x = ix;
					this->y = iy;
					this->z = iz;
					this->voxelContainer = &container;
					return true;
				}
			}
			this->z = 0;
		}
		this->y = 0;
	}
	return false;
}

template <class T>
inline void container_iterator<T>::begin(VoxelNode<T>& node)
{
    this->findNextChildNode(node);
}

template <class T>
inline void container_iterator<T>::end(VoxelNode<T>& node)
{
    this->findNextParentNode(node);
}

template <class T>
inline void container_iterator<T>::getVoxelPosition(int& x, int& y, int& z) const
{
	if (this->node)
	{
		x = this->node->getX() + this->x;
		y = this->node->getY() + this->y;
		z = this->node->getZ() + this->z;
	}
}

template <class T>
inline void container_iterator<T>::initPosition(int ix, int iy, int iz)
{
	this->x = ix & T::VOXEL_MASK;
	this->y = iy & T::VOXEL_MASK;
	this->z = iz & T::VOXEL_MASK;
}





// SuperContainerIterator
template <class T>
supercontainer_iterator<T>& supercontainer_iterator<T>::operator++()
{
	if (!this->voxel)
		return *this;

	++this->z;
	if (!this->container_iterator<T>::findNextVoxel(*container_iterator<T>::voxelContainer))
	{
		this->x = 0;
		this->y = 0;
		this->z = 0;
		this->voxel = nullptr;
		this->voxelContainer = nullptr;
		++std::get<2>(containerPosition[0]);	// Update the z position of last super container
		if (!this->findNextContainer(*this->node->getVoxelContainer()))
		{
			std::memset(containerPosition.data(), 0, sizeof(containerPosition));
			this->findNextParentNode(*this->node);
		}
	}
	return *this;
}

template <class T>
inline supercontainer_iterator<T> supercontainer_iterator<T>::operator++(int)
{
	supercontainer_iterator result = *this;
	++(*this);
	return result;
}

template <class T>
inline supercontainer_iterator<T>* supercontainer_iterator<T>::operator*()
{
    return this;
}

template <class T>
bool supercontainer_iterator<T>::findNextChildNode(VoxelNode<T>& i_node)
{
	if (i_node.getSize() == T::NB_VOXELS)
	{
        if (this->findNextContainer(*i_node.getVoxelContainer()))
        {
            this->node = &i_node;
            return true;
        }
	}
	else
	{
		for (uint8_t i = 0; i < 8; ++i)
		{
			auto child = i_node.getChildren()[i];
			if (child)
			{
				if (this->findNextChildNode(*child))
					return true;
			}
		}
	}
	return false;
}

template <class T>
void supercontainer_iterator<T>::findNextParentNode(VoxelNode<T>& i_node)
{
	auto parent = i_node.getParent();
	if (parent == nullptr)
	{
		this->node = nullptr;
		return;
	}

	for (uint8_t i = i_node.getChildId() + 1; i < 8; ++i)
	{
		if (!parent->getChildren()[i])
			continue;
		if (this->findNextChildNode(*parent->getChildren()[i]))
			return;
	}

	this->findNextParentNode(*parent);
}

template <class T>
template <class T_Container>
typename std::enable_if<(T_Container::NB_SUPERCONTAINER != 0 && T_Container::SUPERCONTAINER_ID != 0), bool>::type
supercontainer_iterator<T>::findNextContainer(T_Container& container)
{
    for (uint8_t& x = std::get<0>(containerPosition[T_Container::SUPERCONTAINER_ID]); x < T_Container::NB_CONTAINERS; ++x)
    {
        if (!container.hasContainer(x))
            continue;

        uint8_t& y = std::get<1>(containerPosition[T_Container::SUPERCONTAINER_ID]);
        for (; y < T_Container::NB_CONTAINERS; ++y)
        {
            if (!container.hasContainer(x, y))
                continue;

            uint8_t& z = std::get<2>(containerPosition[T_Container::SUPERCONTAINER_ID]);
            for (; z < T_Container::NB_CONTAINERS; ++z)
            {
                auto tmp_container = container.findContainer(x, y, z);
                if (tmp_container && this->findNextContainer(*tmp_container))
                    return true;
                std::get<0>(containerPosition[T_Container::SUPERCONTAINER_ID - 1]) = 0;
                std::get<1>(containerPosition[T_Container::SUPERCONTAINER_ID - 1]) = 0;
                std::get<2>(containerPosition[T_Container::SUPERCONTAINER_ID - 1]) = 0;
            }
            z = 0;
        }
        y = 0;
    }
    return false;
}

template <class T>
template <class T_Container>
typename std::enable_if<(T_Container::NB_SUPERCONTAINER != 0 && T_Container::SUPERCONTAINER_ID == 0), bool>::type
supercontainer_iterator<T>::findNextContainer(T_Container& container)
{
    for (uint8_t& x = std::get<0>(containerPosition[T_Container::SUPERCONTAINER_ID]); x < T_Container::NB_CONTAINERS; ++x)
    {
        if (!container.hasContainer(x))
            continue;

        uint8_t& y = std::get<1>(containerPosition[T_Container::SUPERCONTAINER_ID]);
        for (; y < T_Container::NB_CONTAINERS; ++y)
        {
            if (!container.hasContainer(x, y))
                continue;

            uint8_t& z = std::get<2>(containerPosition[T_Container::SUPERCONTAINER_ID]);
            for (; z < T_Container::NB_CONTAINERS; ++z)
            {
                auto tmp_container = container.findContainer(x, y, z);
                if (tmp_container)
                {
                    bool found = this->container_iterator<T>::findNextVoxel(*tmp_container);
                    if (found)
                        return true;
                    this->x = 0;
                    this->y = 0;
                    this->z = 0;
                }
            }
            z = 0;
        }
        y = 0;
    }
    return false;
}

template <class T>
template <class T_Container>
inline typename std::enable_if<(T_Container::NB_SUPERCONTAINER == 0), bool>::type
supercontainer_iterator<T>::findNextContainer(T_Container& container)
{
    return false;
}

template <class T>
inline void supercontainer_iterator<T>::begin(VoxelNode<T>& node)
{
	this->findNextChildNode(node);
}

template <class T>
inline void supercontainer_iterator<T>::end(VoxelNode<T>& node)
{
	this->container_iterator<T>::findNextParentNode(node);
}

template <class T>
void supercontainer_iterator<T>::getVoxelPosition(int& x, int& y, int& z) const
{
	if (this->node)
	{
		x = this->node->getX() + this->x;
		y = this->node->getY() + this->y;
		z = this->node->getZ() + this->z;
		for (int i = 0; i < T::NB_SUPERCONTAINER; ++i)
		{
			int coef = (i + 1) * 3;
			x += std::get<0>(this->containerPosition[i]) << coef;
			y += std::get<1>(this->containerPosition[i]) << coef;
			z += std::get<2>(this->containerPosition[i]) << coef;
		}
	}
}

template <class T>
void supercontainer_iterator<T>::initPosition(int ix, int iy, int iz)
{
	this->x = ix & T::VOXEL_MASK;
	this->y = iy & T::VOXEL_MASK;
	this->z = iz & T::VOXEL_MASK;
	for (int i = 0; i < T::NB_SUPERCONTAINER; ++i)
	{
		int coef = (i + 1) * 3;
		std::get<0>(this->containerPosition[i]) = (ix >> coef) & T::CONTAINER_MASK;
		std::get<1>(this->containerPosition[i]) = (iy >> coef) & T::CONTAINER_MASK;
		std::get<2>(this->containerPosition[i]) = (iz >> coef) & T::CONTAINER_MASK;
	}
}

} // namespace voxomap
