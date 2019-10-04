namespace voxomap
{

template <class T_Area>
VoxelNode<T_Area>::VoxelNode(int x, int y, int z, int size)
	: P_Node(x, y, z, size)
{
}

template <class T_Area>
VoxelNode<T_Area>::VoxelNode(VoxelNode<T_Area> const& other)
	: P_Node(other)
{
}

template <class T_Area>
inline unsigned int VoxelNode<T_Area>::getNbVoxel() const
{
	return (area) ? area->getNbVoxel() : 0;
}

template <class T_Area>
inline bool VoxelNode<T_Area>::hasVoxel() const
{
	return this->area != nullptr;
}

template <class T_Area>
inline T_Area const* VoxelNode<T_Area>::getVoxelArea() const
{
	return this->area.get();
}

template <class T_Area>
inline typename T_Area::VoxelData* VoxelNode<T_Area>::getVoxel(int x, int y, int z)
{
    return this->area->getVoxel(findPosition(x), findPosition(y), findPosition(z));
}

template <class T_Area>
inline typename T_Area::VoxelData const* VoxelNode<T_Area>::getVoxel(int x, int y, int z) const
{
    return this->area->getVoxel(findPosition(x), findPosition(y), findPosition(z));
}

template <class T_Area>
typename T_Area::VoxelData* VoxelNode<T_Area>::getVoxelAt(int x, int y, int z, VoxelNode** ret) const
{
	VoxelNode<T_Area>*	tmp;
	int					nx = x & AREA_MASK;
	int					ny = y & AREA_MASK;
	int					nz = z & AREA_MASK;

	if (this->getSize() != T_Area::NB_VOXELS || !this->isInside(x, y, z))
	{
		if (this->_octree)
			return static_cast<VoxelOctree<T_Area>*>(this->_octree)->getVoxelAt(x, y, z, ret);
		else
		{
			tmp = static_cast<VoxelNode<T_Area>*>(this->findNode(nx, ny, nz, T_Area::NB_VOXELS));
			if (tmp == nullptr)
				return nullptr;
		}
	}
	else
		tmp = const_cast<VoxelNode<T_Area>*>(this);
	if (ret != nullptr)
		*ret = tmp;
	if (tmp->area == nullptr)
		return nullptr;

	return tmp->area->getVoxel(findPosition(x), findPosition(y), findPosition(z));
}

template <class T_Area>
inline bool VoxelNode<T_Area>::getVoxelAt(int x, int y, int z, VoxelData*& voxel, VoxelNode** ret) const
{
	voxel = this->getVoxelAt(x, y, z, ret);
	return voxel != nullptr;
}

template <class T_Area>
template <typename... Args>
typename T_Area::VoxelData* VoxelNode<T_Area>::addVoxel(int x, int y, int z, Args&&... args)
{
    if (!area)
        area = std::make_shared<T_Area>();
	else
		this->copyOnWrite();

    auto voxel = area->addVoxel(*this, findPosition(x), findPosition(y), findPosition(z), std::forward<Args>(args)...);
    if (voxel)
        static_cast<VoxelOctree<T_Area>*>(this->_octree)->updateOctree(*this);
    return voxel;
}

template <class T_Area>
template <typename... Args>
typename T_Area::VoxelData* VoxelNode<T_Area>::updateVoxel(int x, int y, int z, Args&&... args)
{
    if (!area)
        return nullptr;
	else
		this->copyOnWrite();

    auto voxel = area->updateVoxel(*this, findPosition(x), findPosition(y), findPosition(z), std::forward<Args>(args)...);
    if (voxel)
        static_cast<VoxelOctree<T_Area>*>(this->_octree)->updateOctree(*this);
    return voxel;
}

template <class T_Area>
template <typename... Args>
typename T_Area::VoxelData* VoxelNode<T_Area>::putVoxel(int x, int y, int z, Args&&... args)
{
    if (!area)
        area = std::make_shared<T_Area>();
	else
		this->copyOnWrite();

    auto voxel = area->putVoxel(*this, findPosition(x), findPosition(y), findPosition(z), std::forward<Args>(args)...);
    if (voxel)
        static_cast<VoxelOctree<T_Area>*>(this->_octree)->updateOctree(*this);
    return voxel;
}

template <class T_Area>
bool VoxelNode<T_Area>::removeVoxel(int x, int y, int z)
{
    if (!area)
        return false;

    this->copyOnWrite();
    if (!area->removeVoxel(*this, findPosition(x), findPosition(y), findPosition(z)))
        return false;
    static_cast<VoxelOctree<T_Area>*>(this->_octree)->updateOctree(*this);
    return true;
}

template <class T_Area>
bool VoxelNode<T_Area>::removeVoxel(int x, int y, int z, VoxelData& data)
{
    if (!area)
        return false;

    this->copyOnWrite();
    if (!area->removeVoxel(*this, findPosition(x), findPosition(y), findPosition(z), data))
        return false;
    static_cast<VoxelOctree<T_Area>*>(this->_octree)->updateOctree(*this);
    return true;
}

template <class T_Area>
void VoxelNode<T_Area>::exploreVoxel(std::function<void(VoxelNode<T_Area> const&, typename T_Area::VoxelData const&, uint8_t, uint8_t, uint8_t)> const& predicate) const
{
	if (this->area)
	{
		for (uint8_t x = 0; x < T_Area::NB_VOXELS; ++x)
		{
			for (uint8_t y = 0; y < T_Area::NB_VOXELS; ++y)
			{
				for (uint8_t z = 0; z < T_Area::NB_VOXELS; ++z)
				{
					auto voxel = this->area->getVoxel(x, y, z);
					if (voxel)
						predicate(*this, *voxel, x, y, z);
				}
			}
		}
	}

	for (auto const child : this->_children)
	{
		if (child)
			static_cast<VoxelNode<T_Area> const*>(child)->exploreVoxel(predicate);
	}
}

template <class T_Area>
void VoxelNode<T_Area>::exploreVoxelArea(std::function<void(VoxelNode<T_Area> const&)> const& predicate) const
{
    if (area)
        predicate(*this);

    for (auto const child : this->_children)
    {
        if (child)
            static_cast<VoxelNode<T_Area> const*>(child)->exploreVoxelArea(predicate);
    }
}

template <class T_Area>
void VoxelNode<T_Area>::merge(VoxelNode<T_Area>& node)
{
	auto octree = static_cast<VoxelOctree<T_Area>*>(this->getOctree());

	if (node.area)
	{
		if (!this->area)
			this->area = std::make_shared<T_Area>();
		else
			this->copyOnWrite();

		for (int x = 0; x < T_Area::NB_VOXELS; ++x)
		{
			for (int y = 0; y < T_Area::NB_VOXELS; ++y)
			{
				for (int z = 0; z < T_Area::NB_VOXELS; ++z)
				{
					auto voxel = node.area->getVoxel(x, y, z);
					if (voxel)
						this->addVoxel(x, y, z, *voxel);
				}
			}
		}
	}

	this->P_Node::merge(node);
}

template <class T_Area>
inline bool VoxelNode<T_Area>::empty() const
{
	return this->Node<VoxelNode<T_Area>>::empty() && (this->area == nullptr || this->area->getNbVoxel() == 0);
}

template <class T_Area>
void VoxelNode<T_Area>::copyOnWrite()
{
	if (this->area.use_count() > 1)
	{
        auto tmp = this->area;
		this->area = std::make_shared<T_Area>(*tmp);
	}
}

template <class T_Area>
void VoxelNode<T_Area>::serializeNode(VoxelNode const& node, std::string& str) const
{
    int				nbVoxelList = 0;

    if (node.area)
    {
        int			pos[4];
        pos[0] = node.getX();
        pos[1] = node.getY();
        pos[2] = node.getZ();
        pos[3] = node.getSize();
        str.append(reinterpret_cast<char const *>(&pos), sizeof(pos));
        node.area->serialize(str);
    }

    for (auto child : this->_children)
    {
        if (child)
            this->serializeNode(static_cast<VoxelNode&>(*child), str);
    }
}

template <class T_Area>
inline void VoxelNode<T_Area>::serialize(std::string& str) const
{
	this->serializeNode(*this, str);
}

static inline int myread(void* dest, void const* src, int size)
{
	::memcpy(dest, src, size);
	return size;
}

template <class T_Area>
VoxelNode<T_Area>* VoxelNode<T_Area>::unserialize(char const* str, size_t strsize)
{
	//if (strsize < sizeof(uint32_t))
		return nullptr;
	/*VoxelNode	*parent = nullptr;
    size_t  	position = 0;
	int			pos[4];

	while ((position + sizeof(pos)) <= strsize)
	{
		position += myread(pos, &str[position], sizeof(pos));
		auto node = new VoxelNode<T_Area>(pos[0], pos[1], pos[2], pos[3]);
		if ((position + sizeof(T_Area)) > strsize)
			return parent;

		node->area = std::make_shared<T_Area>();
        auto size = node->area->unserialize(&str[position], strsize - position);
        if (size == 0)
            return parent;
        position += size;

		if (parent)
			parent = static_cast<VoxelNode*>(parent->forcedPush(*node));
		else
			parent = node;
	}
	return parent;
    */
}

}