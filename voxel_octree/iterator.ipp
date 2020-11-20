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
inline bool container_iterator<T>::operator==(container_iterator const& other) const
{
	return this->node == other.node &&
		   this->voxel == other.voxel &&
		   this->voxelContainer == other.voxelContainer &&
		   this->x == other.x &&
		   this->y == other.y &&
		   this->z == other.z
		;
}

template <class T>
inline bool container_iterator<T>::operator!=(container_iterator const& other) const
{
	return !(*this == other);
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
		for (uint8_t iy = this->y; iy < VoxelContainer::NB_VOXELS; ++iy)
		{
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
inline void container_iterator<T>::getRelativeVoxelPosition(int& x, int& y, int& z) const
{
	x = this->x;
	y = this->y;
	z = this->z;
}

template <class T>
inline void container_iterator<T>::initPosition(int ix, int iy, int iz)
{
	this->x = ix & T::VOXEL_MASK;
	this->y = iy & T::VOXEL_MASK;
	this->z = iz & T::VOXEL_MASK;
}

template <class T>
inline int container_iterator<T>::getRelativeX() const
{
	return this->x;
}

template <class T>
inline int container_iterator<T>::getRelativeY() const
{
	return this->y;
}

template <class T>
inline int container_iterator<T>::getRelativeZ() const
{
	return this->z;
}

template <class T>
inline int container_iterator<T>::getAbsoluteX() const
{
	if (this->node)
		return this->node->getX() + this->x;
}

template <class T>
inline int container_iterator<T>::getAbsoluteY() const
{
	if (this->node)
		return this->node->getY() + this->y;
}

template <class T>
inline int container_iterator<T>::getAbsoluteZ() const
{
	if (this->node)
		return this->node->getZ() + this->z;
}

template <class T>
container_iterator<T> container_iterator<T>::findRelativeVoxel(int x, int y, int z)
{
	if (this->node)
	{
		int tx, ty, tz;
		this->getRelativeVoxelPosition(tx, ty, tz);
		return this->node->findRelativeVoxel(tx + x, ty + y, tz + z);
	}
	return container_iterator<T>();
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
		++containerPosition[0].z;	// Update the z position of last super container
		if (!this->findNextContainer(*this->node->getVoxelContainer()))
		{
			std::memset(&containerPosition[0], 0, sizeof(containerPosition));
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
inline bool supercontainer_iterator<T>::operator==(supercontainer_iterator const& other) const
{
	return this->container_iterator<T>::operator==(other) &&
		   std::memcmp(&this->containerPosition[0], &other.containerPosition[0], sizeof(containerPosition)) == 0
		;
}

template <class T>
inline bool supercontainer_iterator<T>::operator!=(supercontainer_iterator const& other) const
{
	return !(*this == other);
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
    for (uint8_t& x = containerPosition[T_Container::SUPERCONTAINER_ID].x; x < T_Container::NB_CONTAINERS; ++x)
    {
        uint8_t& y = containerPosition[T_Container::SUPERCONTAINER_ID].y;
        for (; y < T_Container::NB_CONTAINERS; ++y)
        {
            uint8_t& z = containerPosition[T_Container::SUPERCONTAINER_ID].z;
            for (; z < T_Container::NB_CONTAINERS; ++z)
            {
                auto tmp_container = container.findContainer(x, y, z);
                if (tmp_container && this->findNextContainer(*tmp_container))
                    return true;
                containerPosition[T_Container::SUPERCONTAINER_ID - 1].x = 0;
                containerPosition[T_Container::SUPERCONTAINER_ID - 1].y = 0;
                containerPosition[T_Container::SUPERCONTAINER_ID - 1].z = 0;
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
    for (uint8_t& x = containerPosition[T_Container::SUPERCONTAINER_ID].x; x < T_Container::NB_CONTAINERS; ++x)
    {
        uint8_t& y = containerPosition[T_Container::SUPERCONTAINER_ID].y;
        for (; y < T_Container::NB_CONTAINERS; ++y)
        {
            uint8_t& z = containerPosition[T_Container::SUPERCONTAINER_ID].z;
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
	this->findNextParentNode(node);
}

template <class T>
void supercontainer_iterator<T>::getVoxelPosition(int& x, int& y, int& z) const
{
	if (this->node)
	{
		this->getRelativeVoxelPosition(x, y, z);
		x += this->node->getX();
		y += this->node->getY();
		z += this->node->getZ();
	}
}

template <class T>
void supercontainer_iterator<T>::getRelativeVoxelPosition(int& x, int& y, int& z) const
{
	x = this->x;
	y = this->y;
	z = this->z;
	for (int i = 0; i < T::NB_SUPERCONTAINER; ++i)
	{
		int coef = (i + 1) * 3;
		x += this->containerPosition[i].x << coef;
		y += this->containerPosition[i].y << coef;
		z += this->containerPosition[i].z << coef;
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
		this->containerPosition[i].x = (ix >> coef) & T::CONTAINER_MASK;
		this->containerPosition[i].y = (iy >> coef) & T::CONTAINER_MASK;
		this->containerPosition[i].z = (iz >> coef) & T::CONTAINER_MASK;
	}
}

template <class T>
inline int supercontainer_iterator<T>::getRelativeX() const
{
	int x = this->x;
	for (int i = 0; i < T::NB_SUPERCONTAINER; ++i)
		x += this->containerPosition[i].x << ((i + 1) * 3);
	return x;
}

template <class T>
inline int supercontainer_iterator<T>::getRelativeY() const
{
	int y = this->y;
	for (int i = 0; i < T::NB_SUPERCONTAINER; ++i)
		y += this->containerPosition[i].y << ((i + 1) * 3);
	return y;
}

template <class T>
inline int supercontainer_iterator<T>::getRelativeZ() const
{
	int z = this->z;
	for (int i = 0; i < T::NB_SUPERCONTAINER; ++i)
		z += this->containerPosition[i].z << ((i + 1) * 3);
	return z;
}

template <class T>
inline int supercontainer_iterator<T>::getAbsoluteX() const
{
	return this->node ? this->node->getX() + this->getRelativeX() : 0;
}

template <class T>
inline int supercontainer_iterator<T>::getAbsoluteY() const
{
	return this->node ? this->node->getY() + this->getRelativeY() : 0;
}

template <class T>
inline int supercontainer_iterator<T>::getAbsoluteZ() const
{
	return this->node ? this->node->getZ() + this->getRelativeZ() : 0;
}

template <class T>
supercontainer_iterator<T> supercontainer_iterator<T>::findRelativeVoxel(int x, int y, int z)
{
	if (this->node)
	{
		int tx, ty, tz;
		this->getRelativeVoxelPosition(tx, ty, tz);
		return this->node->findRelativeVoxel(tx + x, ty + y, tz + z);
	}
	return supercontainer_iterator<T>();
}


} // namespace voxomap
