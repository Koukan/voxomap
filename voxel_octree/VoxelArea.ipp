namespace voxomap
{

template <class T_Container>
VoxelArea<T_Container>::VoxelArea(int x, int y, int z)
    : _x(x), _y(y), _z(z)
{
}

template <class T_Container>
VoxelArea<T_Container>::VoxelArea(VoxelNode<T_Container> const& node, int x, int y, int z)
    : _x(x), _y(y), _z(z), _octree(static_cast<VoxelOctree<T_Container>*>(node.getOctree())), _node(&node)
{
}

template <class T_Container>
VoxelArea<T_Container>::VoxelArea(VoxelOctree<T_Container> const& octree, int x, int y, int z)
    : _x(x), _y(y), _z(z), _octree(&octree), _node(nullptr)
{
}

template <class T_Container>
VoxelArea<T_Container>::VoxelArea(iterator const& it)
{
    if (!it)
        return;
    it.getVoxelPosition(_x, _y, _z);
    _octree = static_cast<VoxelOctree<T_Container>*>(it.node->getOctree());
    _node = it.node;
}

template <class T_Container>
void VoxelArea<T_Container>::changeNode(VoxelNode<T_Container> const* node)
{
    _node = node;
}

template <class T_Container>
void VoxelArea<T_Container>::changeOctree(VoxelOctree<T_Container> const* octree)
{
    _octree = octree;
}

template <class T_Container>
void VoxelArea<T_Container>::changePosition(int x, int y, int z)
{
    _x = x;
    _y = y;
    _z = z;
}

template <class T_Container>
typename VoxelArea<T_Container>::iterator VoxelArea<T_Container>::findVoxel(int x, int y, int z)
{
    iterator it;
    x += _x;
    y += _y;
    z += _z;
    it.initPosition(x, y, z);
    
    auto node = this->_findVoxelNode(x, y, z);
    if (!node || !node->hasVoxel())
    {
        it.node = node;
        return it;
    }

    node->findVoxel(it);
    return it;
}

template <class T_Container>
VoxelNode<T_Container>* VoxelArea<T_Container>::findVoxelNode(int x, int y, int z)
{
    x += _x;
    y += _y;
    z += _z;
    return this->_findVoxelNode(x, y, z);
}

template <class T_Container>
VoxelNode<T_Container>* VoxelArea<T_Container>::_findVoxelNode(int x, int y, int z)
{
    x &= T_Container::COORD_MASK;
    y &= T_Container::COORD_MASK;
    z &= T_Container::COORD_MASK;

    if (_node && _node->getX() == x && _node->getY() == y && _node->getZ() == z)
        return const_cast<VoxelNode<T_Container>*>(_node);

    if (_octree)
    {
        auto node = _octree->findNode(x, y, z, T_Container::NB_VOXELS);
        if (node)
        {
            _node = node;
            return node;
        }
    }
    return nullptr;
}

template <class T_Container>
std::vector<typename T_Container::iterator> VoxelArea<T_Container>::findNeighbors(float radius)
{
    std::vector<iterator> neighbors;


    return neighbors;
}


}