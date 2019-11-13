namespace voxomap
{

template <class T_Area>
VoxelArea<T_Area>::VoxelArea(int x, int y, int z)
    : _x(x), _y(y), _z(z)
{
}

template <class T_Area>
VoxelArea<T_Area>::VoxelArea(VoxelNode<T_Area> const& node, int x, int y, int z)
    : _x(x), _y(y), _z(z), _octree(static_cast<VoxelOctree<T_Area>*>(node.getOctree())), _node(&node)
{
}

template <class T_Area>
VoxelArea<T_Area>::VoxelArea(VoxelOctree<T_Area> const& octree, int x, int y, int z)
    : _x(x), _y(y), _z(z), _octree(&octree), _node(nullptr)
{
}

template <class T_Area>
void        VoxelArea<T_Area>::changeNode(VoxelNode<T_Area> const* node)
{
    _node = node;
}

template <class T_Area>
void        VoxelArea<T_Area>::changeOctree(VoxelOctree<T_Area> const* octree)
{
    if (_octree == octree)
        return;
    _octree = octree;
    _nodeCache.clear();
}

template <class T_Area>
void VoxelArea<T_Area>::changePosition(int x, int y, int z)
{
    _x = x;
    _y = y;
    _z = z;
}

template <class T_Area>
void VoxelArea<T_Area>::add(VoxelNode<T_Area> const& node)
{
    if (node.getOctree() != _octree || node.getSize() != T_Area::NB_VOXELS)
        return;

    _nodeCache[std::make_tuple(node.getX(), node.getY(), node.getZ())] = &node;
}

template <class T_Area>
void VoxelArea<T_Area>::remove(int x, int y, int z)
{
    _nodeCache.erase(std::make_tuple(x, y, z));
    if (_node && _node->getX() == x && _node->getY() == y && _node->getZ() == z)
        _node = nullptr;
}

template <class T_Area>
void VoxelArea<T_Area>::remove(VoxelNode<T_Area> const& node)
{
    if (node.getOctree() != _octree)
        return;

    if (node.getSize() == T_Area::NB_VOXELS)
        this->remove(node.getX(), node.getY(), node.getZ());
    else
    {
        for (size_t i = 0; i < 8; ++i)
        {
            if (node.getChildren()[i])
                this->remove(*node.getChildren()[i]);
        }
    }
}

template <class T_Area>
void VoxelArea<T_Area>::clear()
{
    _node = nullptr;
    _nodeCache.clear();
}

template <class T_Area>
typename T_Area::VoxelData* VoxelArea<T_Area>::getVoxel(int x, int y, int z, VoxelNode<T_Area>** ret)
{
    x += _x;
    y += _y;
    z += _z;
    VoxelNode<T_Area>* node;

    if (ret)
        *ret = nullptr;
    int        nx = x & VoxelNode<T_Area>::AREA_MASK;
    int        ny = y & VoxelNode<T_Area>::AREA_MASK;
    int        nz = z & VoxelNode<T_Area>::AREA_MASK;
    
    if (_node && _node->getX() == nx && _node->getY() == ny && _node->getZ() == nz)
        node = const_cast<VoxelNode<T_Area>*>(_node);
    else
    {
        auto it = _nodeCache.find(std::make_tuple(nx, ny, nz));
        if (it == _nodeCache.end())
        {
            /*if (_node)
                node = _node->findNode(nx, ny, nz, T_Area::NB_VOXELS);
            else*/ if (_octree)
                node = _octree->findNode(nx, ny, nz, T_Area::NB_VOXELS);
            else
                return nullptr;
            if (node == nullptr)
                return nullptr;
            _nodeCache[std::make_tuple(nx, ny, nz)] = node;
        }
        else
            node = const_cast<VoxelNode<T_Area>*>(it->second);
        _node = node;
    }
    if (node == nullptr)
        return nullptr;
    if (ret)
        *ret = node;
    if (!node->hasVoxel())
        return nullptr;
    x &= T_Area::NB_VOXELS - 1;
    y &= T_Area::NB_VOXELS - 1;
    z &= T_Area::NB_VOXELS - 1;
    return node->getVoxel(x, y, z);
}

}