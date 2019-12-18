namespace voxomap
{

template <class T_Area>
VoxelOctree<T_Area>::VoxelOctree()
{
}

template <class T_Area>
VoxelOctree<T_Area>::VoxelOctree(VoxelOctree<T_Area> const& other)
    : Octree<VoxelNode<T_Area>>(other), _nbVoxels(other._nbVoxels)
{
}

template <class T_Area>
VoxelNode<T_Area>* VoxelOctree<T_Area>::push(VoxelNode<T_Area>& n)
{
    int nbVoxel = n.getNbVoxel();
    auto node = this->Octree<VoxelNode<T_Area>>::push(n);

    // update voxel number in voxel octree
    if (node == &n)
        _nbVoxels += nbVoxel;
    return node;
}

template <class T_Area>
std::unique_ptr<VoxelNode<T_Area>> VoxelOctree<T_Area>::pop(VoxelNode<T_Area>& node)
{
    this->removeOfCache(node);
    return this->Octree<VoxelNode<T_Area>>::pop(node);
}

template <class T_Area>
void VoxelOctree<T_Area>::clear()
{
    _nbVoxels = 0;
    _nodeCache = nullptr;
    this->Octree<VoxelNode<T_Area>>::clear();
}

template <class T_Area>
template <typename T>
typename T_Area::iterator VoxelOctree<T_Area>::findVoxel(T x, T y, T z)
{
    return this->_findVoxel(x, y, z);
}

template <class T_Area>
typename T_Area::iterator VoxelOctree<T_Area>::_findVoxel(int x, int y, int z)
{
    auto node = this->_findVoxelNode(x, y, z);

    iterator it;
    it.x = VoxelNode<T_Area>::findPosition(x);
    it.y = VoxelNode<T_Area>::findPosition(y);
    it.z = VoxelNode<T_Area>::findPosition(z);

    if (node)
        node->findVoxel(it);
    return it;
}

template <class T_Area>
template <typename T>
typename std::enable_if<std::is_floating_point<T>::value, typename T_Area::iterator>::type VoxelOctree<T_Area>::_findVoxel(T x, T y, T z)
{
    return this->_findVoxel(
        static_cast<int>(std::floor(x)),
        static_cast<int>(std::floor(y)),
        static_cast<int>(std::floor(z))
    );
}

template <class T_Area>
template <typename T>
inline VoxelNode<T_Area>* VoxelOctree<T_Area>::findVoxelNode(T x, T y, T z) const
{
    return this->_findVoxelNode(x, y, z);
}

template <class T_Area>
VoxelNode<T_Area>* VoxelOctree<T_Area>::_findVoxelNode(int x, int y, int z) const
{
    x &= VoxelNode<T_Area>::AREA_MASK;
    y &= VoxelNode<T_Area>::AREA_MASK;
    z &= VoxelNode<T_Area>::AREA_MASK;

    if (_nodeCache && _nodeCache->getX() == x && _nodeCache->getY() == y && _nodeCache->getZ() == z)
        return _nodeCache;

    auto node = this->findNode(x, y, z, T_Area::NB_VOXELS);
    if (node)
        _nodeCache = node;
    return node;
}

template <class T_Area>
template <typename T>
typename std::enable_if<std::is_floating_point<T>::value, VoxelNode<T_Area>*>::type VoxelOctree<T_Area>::_findVoxelNode(T x, T y, T z) const
{
    return this->_findVoxelNode(
        static_cast<int>(std::floor(x)),
        static_cast<int>(std::floor(y)),
        static_cast<int>(std::floor(z))
    );
}

template <class T_Area>
template <typename T, typename... Args>
std::pair<typename T_Area::iterator, bool> VoxelOctree<T_Area>::addVoxel(T x, T y, T z, Args&&... args)
{
    auto it = this->findVoxel(x, y, z);

    if (it)
        return std::make_pair(it, false);
    if (!it.node)
        it.node = this->pushAreaNode(x, y, z);
    it.node->addVoxel(it, std::forward<Args>(args)...);
    return std::make_pair(it, true);
}

template <class T_Area>
template <typename T, typename... Args>
typename T_Area::iterator VoxelOctree<T_Area>::updateVoxel(T x, T y, T z, Args&&... args)
{
    return this->updateVoxel(this->findVoxel(x, y, z), std::forward<Args>(args)...);
}

template <class T_Area>
template <typename... Args>
typename T_Area::iterator VoxelOctree<T_Area>::updateVoxel(iterator it, Args&&... args)
{
    if (it)
    {
        it.node->updateVoxel(it, std::forward<Args>(args)...);
        return it;
    }
    return iterator();
}

template <class T_Area>
template <typename T, typename... Args>
typename T_Area::iterator VoxelOctree<T_Area>::putVoxel(T x, T y, T z, Args&&... args)
{
    auto it = this->findVoxel(x, y, z);

    if (!it.node)
        it.node = this->pushAreaNode(x, y, z);
    it.node->putVoxel(it, std::forward<Args>(args)...);
    return it;
}

template <class T_Area>
template <typename T, typename... Args>
bool VoxelOctree<T_Area>::removeVoxel(T x, T y, T z, Args&&... args)
{
    auto it = this->findVoxel(x, y, z);
    if (!it)
        return false;
    this->removeVoxel(it, std::forward<Args>(args)...);
    return true;
}

template <class T_Area>
template <typename... Args>
typename VoxelOctree<T_Area>::iterator VoxelOctree<T_Area>::removeVoxel(iterator it, Args&&... args)
{
    if (it)
        return it.node->removeVoxel(it, std::forward<Args>(args)...);
    return iterator();
}

template <class T_Area>
unsigned int VoxelOctree<T_Area>::getAreaSize() const
{
    return T_Area::NB_VOXELS;
}

template <class T_Area>
void VoxelOctree<T_Area>::removeOfCache(VoxelNode<T_Area> const& node)
{
    if (_nodeCache == &node)
        _nodeCache = nullptr;
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
typename T_Area::iterator VoxelOctree<T_Area>::begin()
{
    return this->_rootNode->begin();
}

template <class T_Area>
typename T_Area::iterator VoxelOctree<T_Area>::end()
{
    return iterator();
}

template <class T_Area>
VoxelNode<T_Area>* VoxelOctree<T_Area>::pushAreaNode(int x, int y, int z)
{
    auto node = new VoxelNode<T_Area>(x & ~(T_Area::NB_VOXELS - 1), y & ~(T_Area::NB_VOXELS - 1), z & ~(T_Area::NB_VOXELS - 1), T_Area::NB_VOXELS);
    return this->Octree<VoxelNode<T_Area>>::push(*node);
}

template <class T_Area>
template <typename T>
typename std::enable_if<std::is_floating_point<T>::value, VoxelNode<T_Area>*>::type VoxelOctree<T_Area>::pushAreaNode(T x, T y, T z)
{
    return this->pushAreaNode(
        static_cast<int>(std::floor(x)),
        static_cast<int>(std::floor(y)),
        static_cast<int>(std::floor(z))
    );
}

}
