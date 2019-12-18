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
VoxelArea<T_Area>::VoxelArea(iterator const& it)
{
    if (!it)
        return;
    _x = it.node->getX() + it.x;
    _y = it.node->getY() + it.y;
    _z = it.node->getZ() + it.z;
    _octree = static_cast<VoxelOctree<T_Area>*>(it.node->getOctree());
    _node = it.node;
}

template <class T_Area>
void VoxelArea<T_Area>::changeNode(VoxelNode<T_Area> const* node)
{
    _node = node;
}

template <class T_Area>
void VoxelArea<T_Area>::changeOctree(VoxelOctree<T_Area> const* octree)
{
    _octree = octree;
}

template <class T_Area>
void VoxelArea<T_Area>::changePosition(int x, int y, int z)
{
    _x = x;
    _y = y;
    _z = z;
}

template <class T_Area>
typename VoxelArea<T_Area>::iterator VoxelArea<T_Area>::findVoxel(int x, int y, int z)
{
    auto node = this->findVoxelNode(x, y, z);

    if (!node || !node->hasVoxel())
    {
        iterator it;
        it.node = node;
        return it;
    }

    x += _x;
    y += _y;
    z += _z;
    return node->findVoxel(x, y, z);
}

template <class T_Area>
VoxelNode<T_Area>* VoxelArea<T_Area>::findVoxelNode(int x, int y, int z)
{
    x += _x;
    y += _y;
    z += _z;
    int nx = x & VoxelNode<T_Area>::AREA_MASK;
    int ny = y & VoxelNode<T_Area>::AREA_MASK;
    int nz = z & VoxelNode<T_Area>::AREA_MASK;

    if (_node && _node->getX() == nx && _node->getY() == ny && _node->getZ() == nz)
    {
        return const_cast<VoxelNode<T_Area>*>(_node);
    }

    if (_octree)
    {
        auto node = _octree->findNode(nx, ny, nz, T_Area::NB_VOXELS);
        if (node)
        {
            _node = node;
            return node;
        }
    }
    return nullptr;
}

template <class T_Area>
std::vector<typename T_Area::iterator> VoxelArea<T_Area>::findNeighbors(float radius)
{
    std::vector<iterator> neighbors;


    return neighbors;
}


}