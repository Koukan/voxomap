namespace voxomap
{

template <class T_Area>
VoxelOctree<T_Area>::VoxelOctree()
{
	_cache.changeOctree(this);
}

template <class T_Area>
VoxelOctree<T_Area>::VoxelOctree(VoxelOctree<T_Area> const& other)
	: Octree(other), _activeUpdate(other._activeUpdate), _nbVoxels(other._nbVoxels)
{
	_cache.changeOctree(this);
}

template <class T_Area>
VoxelNode<T_Area>* VoxelOctree<T_Area>::push(VoxelNode<T_Area>& n)
{
	int			nbVoxel = n.getNbVoxel();
	auto		node = this->Octree::push(n);

	// update voxel number in voxel octree
	if (node == &n)
		_nbVoxels += nbVoxel;

	if (node->hasVoxel())
		this->callUpdate(*node);
	return node;
}

template <class T_Area>
std::unique_ptr<VoxelNode<T_Area>> VoxelOctree<T_Area>::pop(VoxelNode<T_Area>& node)
{
	this->removeOfCache(node);
	return this->Octree::pop(node);
}

template <class T_Area>
void VoxelOctree<T_Area>::clear()
{
	_nbVoxels = 0;
	_nodeCache.node = nullptr;
	_nodeCache.cache.clear();
	_cache.clear();
	this->Octree::clear();
	this->callUpdate(static_cast<VoxelNode<T_Area>&>(*this->_rootNode));
}

template <class T_Area>
typename T_Area::VoxelData* VoxelOctree<T_Area>::getVoxelAt(int x, int y, int z, VoxelNode<T_Area>** ret) const
{
	return _cache.getVoxel(x, y, z, ret);
}

template <class T_Area>
template <typename... Args>
typename T_Area::VoxelData* VoxelOctree<T_Area>::addVoxel(int x, int y, int z, Args&&... args)
{
	VoxelNode<T_Area>*	node = nullptr;
	auto				voxel = this->getVoxelAt(x, y, z, &node);

    if (voxel)
        return nullptr;
	if (!node)
		node = this->pushAreaNode(x, y, z);
	return node->addVoxel(x, y, z, std::forward<Args>(args)...);
}

template <class T_Area>
template <typename... Args>
typename T_Area::VoxelData* VoxelOctree<T_Area>::updateVoxel(int x, int y, int z, Args&&... args)
{
    VoxelNode<T_Area>*	node = nullptr;
    auto				voxel = this->getVoxelAt(x, y, z, &node);

    if (voxel)
        return node->updateVoxel(x, y, z, std::forward<Args>(args)...);
    return nullptr;
}

template <class T_Area>
template <typename... Args>
typename T_Area::VoxelData* VoxelOctree<T_Area>::putVoxel(int x, int y, int z, Args&&... args)
{
    VoxelNode<T_Area>*	node = nullptr;
    auto				voxel = this->getVoxelAt(x, y, z, &node);

    if (!node)
        node = this->pushAreaNode(x, y, z);
    return node->putVoxel(x, y, z, std::forward<Args>(args)...);
}

template <class T_Area>
bool VoxelOctree<T_Area>::removeVoxel(int x, int y, int z)
{
	auto    node = static_cast<VoxelNode<T_Area>*>(this->findNode(x & ~(T_Area::NB_VOXELS - 1), y & ~(T_Area::NB_VOXELS - 1), z & ~(T_Area::NB_VOXELS - 1), T_Area::NB_VOXELS));

	if (node && node->hasVoxel())
	{
        if (node->removeVoxel(x, y, z))
        {
            this->callUpdate(*node);
            return true;
        }
		return false;
	}
	return false;
}

template <class T_Area>
bool VoxelOctree<T_Area>::removeVoxel(int x, int y, int z, VoxelData& data)
{
	auto    node = static_cast<VoxelNode<T_Area>*>(this->findNode(x & ~(T_Area::NB_VOXELS - 1), y & ~(T_Area::NB_VOXELS - 1), z & ~(T_Area::NB_VOXELS - 1), T_Area::NB_VOXELS));

	if (node && node->hasVoxel())
	{
        if (node->removeVoxel(x, y, z, data))
        {
            this->callUpdate(*node);
            return true;
        }
		return false;
	}
	return VoxelData();
}

template <class T_Area>
unsigned int VoxelOctree<T_Area>::getAreaSize() const
{
	return T_Area::NB_VOXELS;
}

template <class T_Area>
void VoxelOctree<T_Area>::activeUpdate(bool active)
{
	_activeUpdate = active;
}

template <class T_Area>
void VoxelOctree<T_Area>::updateOctree(VoxelNode<T_Area>& node)
{
	if (_activeUpdate)
		this->callback(_updateNodeCallbacks, node);
}

template <class T_Area>
void VoxelOctree<T_Area>::removeOfCache(VoxelNode<T_Area> const& node)
{
	_cache.remove(node);
	if (_nodeCache.node == &node)
		_nodeCache.node = nullptr;
	_nodeCache.cache.erase(const_cast<VoxelNode<T_Area>*>(&node));
}

template <class T_Area>
void VoxelOctree<T_Area>::addUpdateCallback(std::string const& name, std::function<void(VoxelNode<T_Area>&)> const& func)
{
	this->_updateNodeCallbacks.emplace_back(func, name);
}

template <class T_Area>
void VoxelOctree<T_Area>::exploreVoxel(std::function<void(VoxelNode<T_Area> const&, VoxelData const&, uint8_t, uint8_t, uint8_t)> const& predicate) const
{
	this->_rootNode->exploreVoxel(predicate);
}

template <class T_Area>
void VoxelOctree<T_Area>::exploreVoxelArea(std::function<void(VoxelNode<T_Area> const&)> const& predicate) const
{
	this->_rootNode->exploreVoxelArea(predicate);
}

template <class T_Area>
unsigned int VoxelOctree<T_Area>::getNbVoxels() const
{
	return _nbVoxels;
}

template <class T_Area>
void VoxelOctree<T_Area>::setNbVoxels(unsigned int nbVoxels)
{
	_nbVoxels = nbVoxels;
}

template <class T_Area>
VoxelNode<T_Area>* VoxelOctree<T_Area>::pushAreaNode(int x, int y, int z)
{
	auto node = new VoxelNode<T_Area>(x & ~(T_Area::NB_VOXELS - 1), y & ~(T_Area::NB_VOXELS - 1), z & ~(T_Area::NB_VOXELS - 1), T_Area::NB_VOXELS);
	return this->Octree::push(*node);
}

template <class T_Area>
template <typename T, typename... Args>
void VoxelOctree<T_Area>::callback(CallbackList<T> const& list, Args&&... args)
{
	for (auto const& pair : list)
		pair.first(std::forward<Args>(args)...);
}

template <class T_Area>
void VoxelOctree<T_Area>::callUpdate(VoxelNode<T_Area>& node, std::array<VoxelNode<T_Area>*, 6> const& nodes)
{
	this->callUpdate(node);
	for (auto tmp : nodes)
		if (tmp)
			this->callUpdate(*tmp);
}

template <class T_Area>
void VoxelOctree<T_Area>::callUpdate(VoxelNode<T_Area>& node)
{
	this->callback(_updateNodeCallbacks, node);
	if (!_activeUpdate)
		return;

    if (&node != _nodeCache.node)
    {
        _nodeCache.node = &node;
		_nodeCache.cache.emplace(&node);
    }
}

}
