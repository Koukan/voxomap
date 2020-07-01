namespace voxomap
{

template <class T_Container>
VoxelOctree<T_Container>::VoxelOctree()
{
}

template <class T_Container>
VoxelOctree<T_Container>::VoxelOctree(VoxelOctree<T_Container> const& other)
    : Octree<VoxelNode<T_Container>>(other), _nbVoxels(other._nbVoxels)
{
}

template <class T_Container>
VoxelOctree<T_Container>::VoxelOctree(VoxelOctree<T_Container>&& other)
    : Octree<VoxelNode<T_Container>>(std::move(other)), _nbVoxels(other._nbVoxels)
{
}

template <class T_Container>
VoxelOctree<T_Container>& VoxelOctree<T_Container>::operator=(VoxelOctree<T_Container> const& other)
{
    this->Octree<VoxelNode<T_Container>>::operator=(other);
    _nbVoxels = other._nbVoxels;
}

template <class T_Container>
VoxelOctree<T_Container>& VoxelOctree<T_Container>::operator=(VoxelOctree<T_Container>&& other)
{
    this->Octree<VoxelNode<T_Container>>::operator=(std::move(other));
    _nbVoxels = other._nbVoxels;
}

template <class T_Container>
VoxelNode<T_Container>* VoxelOctree<T_Container>::push(VoxelNode<T_Container>& n)
{
    int nbVoxel = n.getNbVoxel();
    auto node = this->Octree<VoxelNode<T_Container>>::push(n);

    // update voxel number in voxel octree
    if (node == &n)
        _nbVoxels += nbVoxel;
    return node;
}

template <class T_Container>
std::unique_ptr<VoxelNode<T_Container>> VoxelOctree<T_Container>::pop(VoxelNode<T_Container>& node)
{
    this->removeOfCache(node);
    return this->Octree<VoxelNode<T_Container>>::pop(node);
}

template <class T_Container>
void VoxelOctree<T_Container>::clear()
{
    _nbVoxels = 0;
    _nodeCache = nullptr;
    this->Octree<VoxelNode<T_Container>>::clear();
}

template <class T_Container>
template <typename T>
typename T_Container::iterator VoxelOctree<T_Container>::findVoxel(T x, T y, T z)
{
    return this->_findVoxel<T_Container::NB_SUPERCONTAINER>(x, y, z);
}

template <class T_Container>
template <int NB_SUPERCONTAINER>
typename std::enable_if<(NB_SUPERCONTAINER == 0), typename T_Container::iterator>::type VoxelOctree<T_Container>::_findVoxel(int x, int y, int z)
{
    auto node = this->_findVoxelNode(x, y, z);

    iterator it;
    it.x = VoxelNode<T_Container>::findVoxelPosition(x);
    it.y = VoxelNode<T_Container>::findVoxelPosition(y);
    it.z = VoxelNode<T_Container>::findVoxelPosition(z);

    if (node)
        node->findVoxel(it);
    return it;
}

template <class T_Container>
template <int NB_SUPERCONTAINER>
typename std::enable_if<(NB_SUPERCONTAINER != 0), typename T_Container::iterator>::type VoxelOctree<T_Container>::_findVoxel(int x, int y, int z)
{
    auto node = this->_findVoxelNode(x, y, z);

    iterator it;
    it.x = VoxelNode<T_Container>::findVoxelPosition(x);
    it.y = VoxelNode<T_Container>::findVoxelPosition(y);
    it.z = VoxelNode<T_Container>::findVoxelPosition(z);

    for (int i = 0; i < T_Container::NB_SUPERCONTAINER; ++i)
    {
        std::get<0>(it.container_position[i]) = VoxelNode<T_Container>::findContainerPosition(x, i + 1);
        std::get<1>(it.container_position[i]) = VoxelNode<T_Container>::findContainerPosition(y, i + 1);
        std::get<2>(it.container_position[i]) = VoxelNode<T_Container>::findContainerPosition(z, i + 1);
    }

    if (node)
        node->findVoxel(it);
    return it;
}

template <class T_Container>
template <int NB_SUPERCONTAINER, typename T>
typename std::enable_if<std::is_floating_point<T>::value, typename T_Container::iterator>::type VoxelOctree<T_Container>::_findVoxel(T x, T y, T z)
{
    return this->_findVoxel<NB_SUPERCONTAINER>(
        static_cast<int>(std::floor(x)),
        static_cast<int>(std::floor(y)),
        static_cast<int>(std::floor(z))
    );
}

template <class T_Container>
template <typename T>
inline VoxelNode<T_Container>* VoxelOctree<T_Container>::findVoxelNode(T x, T y, T z) const
{
    return this->_findVoxelNode(x, y, z);
}

template <class T_Container>
VoxelNode<T_Container>* VoxelOctree<T_Container>::_findVoxelNode(int x, int y, int z) const
{
    x &= T_Container::COORD_MASK;
    y &= T_Container::COORD_MASK;
    z &= T_Container::COORD_MASK;

    if (_nodeCache && _nodeCache->getX() == x && _nodeCache->getY() == y && _nodeCache->getZ() == z)
        return _nodeCache;

    auto node = this->findNode(x, y, z, T_Container::NB_VOXELS);
    if (node)
        _nodeCache = node;
    return node;
}

template <class T_Container>
template <typename T>
typename std::enable_if<std::is_floating_point<T>::value, VoxelNode<T_Container>*>::type VoxelOctree<T_Container>::_findVoxelNode(T x, T y, T z) const
{
    return this->_findVoxelNode(
        static_cast<int>(std::floor(x)),
        static_cast<int>(std::floor(y)),
        static_cast<int>(std::floor(z))
    );
}

template <class T_Container>
template <typename T, typename... Args>
std::pair<typename T_Container::iterator, bool> VoxelOctree<T_Container>::addVoxel(T x, T y, T z, Args&&... args)
{
    auto it = this->findVoxel(x, y, z);

    if (it)
        return std::make_pair(it, false);
    if (!it.node)
        it.node = this->pushContainerNode(x, y, z);
    it.node->addVoxel(it, std::forward<Args>(args)...);
    return std::make_pair(it, true);
}

template <class T_Container>
template <typename T, typename... Args>
typename T_Container::iterator VoxelOctree<T_Container>::updateVoxel(T x, T y, T z, Args&&... args)
{
    return this->updateVoxel(this->findVoxel(x, y, z), std::forward<Args>(args)...);
}

template <class T_Container>
template <typename... Args>
typename T_Container::iterator VoxelOctree<T_Container>::updateVoxel(iterator it, Args&&... args)
{
    if (it)
    {
        it.node->updateVoxel(it, std::forward<Args>(args)...);
        return it;
    }
    return iterator();
}

template <class T_Container>
template <typename T, typename... Args>
typename T_Container::iterator VoxelOctree<T_Container>::putVoxel(T x, T y, T z, Args&&... args)
{
    auto it = this->findVoxel(x, y, z);

    if (!it.node)
        it.node = this->pushContainerNode(x, y, z);
    it.node->putVoxel(it, std::forward<Args>(args)...);
    return it;
}

template <class T_Container>
template <typename T, typename... Args>
bool VoxelOctree<T_Container>::removeVoxel(T x, T y, T z, Args&&... args)
{
    auto it = this->findVoxel(x, y, z);
    if (!it)
        return false;
	return it.node->removeVoxel(it, std::forward<Args>(args)...);
}

template <class T_Container>
template <typename... Args>
bool VoxelOctree<T_Container>::removeVoxel(iterator it, Args&&... args)
{
	if (it)
		return it.node->removeVoxel(it, std::forward<Args>(args)...);
    return false;
}

template <class T_Container>
unsigned int VoxelOctree<T_Container>::getAreaSize() const
{
    return T_Container::NB_VOXELS;
}

template <class T_Container>
void VoxelOctree<T_Container>::removeOfCache(VoxelNode<T_Container> const& node)
{
    if (_nodeCache == &node)
        _nodeCache = nullptr;
}

template <class T_Container>
void VoxelOctree<T_Container>::exploreVoxel(std::function<void(iterator const&)> const& predicate) const
{
    this->_rootNode->exploreVoxel(predicate);
}

template <class T_Container>
void VoxelOctree<T_Container>::exploreVoxelContainer(std::function<void(VoxelNode<T_Container> const&)> const& predicate) const
{
    const_cast<VoxelNode<T_Container> const*>(this->_rootNode.get())->exploreVoxelContainer(predicate);
}

template <class T_Container>
void VoxelOctree<T_Container>::exploreBoundingBox(BoundingBox<int> const& bounding_box,
                                             std::function<void(VoxelNode<T_Container>&)> const& in_predicate,
                                             std::function<void(VoxelNode<T_Container>&)> const& out_predicate)
{
    for (auto child : this->_rootNode->getChildren())
    {
        if (child)
        {
            child->exploreBoundingBox(bounding_box, in_predicate, out_predicate);
        }
    }
}

template <class T_Container>
unsigned int VoxelOctree<T_Container>::getNbVoxels() const
{
    return _nbVoxels;
}

template <class T_Container>
void VoxelOctree<T_Container>::setNbVoxels(unsigned int nbVoxels)
{
    _nbVoxels = nbVoxels;
}

template <class T_Container>
typename T_Container::iterator VoxelOctree<T_Container>::begin()
{
    return this->_rootNode->begin();
}

template <class T_Container>
typename T_Container::iterator VoxelOctree<T_Container>::end()
{
    return iterator();
}

template <class T_Container>
void VoxelOctree<T_Container>::serialize(std::string& str) const
{
    this->getRootNode()->serialize(str);
}

template <class T_Container>
size_t VoxelOctree<T_Container>::unserialize(char const* str, size_t strsize)
{
    return this->getRootNode()->unserialize(str, strsize);
}

template <class T_Container>
VoxelNode<T_Container>* VoxelOctree<T_Container>::pushContainerNode(int x, int y, int z)
{
    auto node = new VoxelNode<T_Container>(x & ~(T_Container::NB_VOXELS - 1), y & ~(T_Container::NB_VOXELS - 1), z & ~(T_Container::NB_VOXELS - 1), T_Container::NB_VOXELS);
    return this->Octree<VoxelNode<T_Container>>::push(*node);
}

template <class T_Container>
template <typename T>
typename std::enable_if<std::is_floating_point<T>::value, VoxelNode<T_Container>*>::type VoxelOctree<T_Container>::pushContainerNode(T x, T y, T z)
{
    return this->pushContainerNode(
        static_cast<int>(std::floor(x)),
        static_cast<int>(std::floor(y)),
        static_cast<int>(std::floor(z))
    );
}

}
