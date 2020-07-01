namespace voxomap
{

template <class T>
container_iterator<T>& container_iterator<T>::operator++()
{
	if (!this->voxel)
		return *this;

	++this->z;
	if (!this->findNextVoxel(*voxel_container))
	{
		this->x = 0;
		this->y = 0;
		this->z = 0;
		this->voxel = nullptr;
		this->voxel_container = nullptr;
		this->findNextParentNode(*this->node);
	}
	return *this;
}

template <class T>
container_iterator<T> container_iterator<T>::operator++(int)
{
	container_iterator result = *this;
	++(*this);
	return result;
}

template <class T>
container_iterator<T>* container_iterator<T>::operator*()
{
	return this;
}

template <class T>
container_iterator<T>::operator bool() const
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
					this->voxel_container = &container;
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
void container_iterator<T>::begin(VoxelNode<T>& node)
{
    this->findNextChildNode(node);
}

template <class T>
void container_iterator<T>::end(VoxelNode<T>& node)
{
    this->findNextParentNode(node);
}

template <class T>
void container_iterator<T>::getVoxelPosition(int& x, int& y, int& z) const
{
    if (this->voxel)
    {
        x = this->node->getX() + this->x;
        y = this->node->getY() + this->y;
        z = this->node->getZ() + this->z;
    }
}




// SuperContainerIterator
template <class T>
supercontainer_iterator<T>& supercontainer_iterator<T>::operator++()
{
	if (!this->voxel)
		return *this;

	++this->z;
	if (!this->container_iterator<T>::findNextVoxel(*container_iterator<T>::voxel_container))
	{
		this->x = 0;
		this->y = 0;
		this->z = 0;
		this->voxel = nullptr;
		this->voxel_container = nullptr;
		++std::get<2>(container_position[0]);	// Update the z position of last super container
		if (!this->findNextContainer(*this->node->getVoxelContainer()))
		{
			std::memset(container_position.data(), 0, sizeof(container_position));
			this->findNextParentNode(*this->node);
		}
	}
	return *this;
}

template <class T>
supercontainer_iterator<T> supercontainer_iterator<T>::operator++(int)
{
	supercontainer_iterator result = *this;
	++(*this);
	return result;
}

template <class T>
supercontainer_iterator<T>* supercontainer_iterator<T>::operator*()
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
    for (uint8_t& x = std::get<0>(container_position[T_Container::SUPERCONTAINER_ID]); x < T_Container::NB_CONTAINERS; ++x)
    {
        if (!container.hasContainer(x))
            continue;

        uint8_t& y = std::get<1>(container_position[T_Container::SUPERCONTAINER_ID]);
        for (; y < T_Container::NB_CONTAINERS; ++y)
        {
            if (!container.hasContainer(x, y))
                continue;

            uint8_t& z = std::get<2>(container_position[T_Container::SUPERCONTAINER_ID]);
            for (; z < T_Container::NB_CONTAINERS; ++z)
            {
                auto tmp_container = container.findContainer(x, y, z);
                if (tmp_container && this->findNextContainer(*tmp_container))
                    return true;
                std::get<0>(container_position[T_Container::SUPERCONTAINER_ID - 1]) = 0;
                std::get<1>(container_position[T_Container::SUPERCONTAINER_ID - 1]) = 0;
                std::get<2>(container_position[T_Container::SUPERCONTAINER_ID - 1]) = 0;
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
    for (uint8_t& x = std::get<0>(container_position[T_Container::SUPERCONTAINER_ID]); x < T_Container::NB_CONTAINERS; ++x)
    {
        if (!container.hasContainer(x))
            continue;

        uint8_t& y = std::get<1>(container_position[T_Container::SUPERCONTAINER_ID]);
        for (; y < T_Container::NB_CONTAINERS; ++y)
        {
            if (!container.hasContainer(x, y))
                continue;

            uint8_t& z = std::get<2>(container_position[T_Container::SUPERCONTAINER_ID]);
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
typename std::enable_if<(T_Container::NB_SUPERCONTAINER == 0), bool>::type
supercontainer_iterator<T>::findNextContainer(T_Container& container)
{
    return false;
}

template <class T>
void supercontainer_iterator<T>::begin(VoxelNode<T>& node)
{
	this->findNextChildNode(node);
}

template <class T>
void supercontainer_iterator<T>::end(VoxelNode<T>& node)
{
	this->container_iterator<T>::findNextParentNode(node);
}

template <class T>
void supercontainer_iterator<T>::getVoxelPosition(int& x, int& y, int& z) const
{
    x = this->node->getX() + this->x;
    y = this->node->getY() + this->y;
    z = this->node->getZ() + this->z;
    for (int i = 0; i < T::NB_SUPERCONTAINER; ++i)
    {
        int coef = (i + 1) * 3;
        x += std::get<0>(container_position[i]) << coef;
        y += std::get<1>(container_position[i]) << coef;
        z += std::get<2>(container_position[i]) << coef;
    }
}

} // namespace voxomap
