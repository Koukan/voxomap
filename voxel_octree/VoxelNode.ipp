namespace voxomap
{

template <class T_Container>
VoxelNode<T_Container>::VoxelNode(int x, int y, int z, uint32_t size)
  : P_Node(x, y, z, size)
{
}

template <class T_Container>
VoxelNode<T_Container>::VoxelNode(VoxelNode<T_Container> const& other)
  : P_Node(other)
{
    if (other._container)
        _container.reset(new T_Container(*other._container));
}

template <class T_Container>
typename T_Container::iterator VoxelNode<T_Container>::begin()
{
    iterator it;
    it.begin(*this);
    return it;
}

template <class T_Container>
typename T_Container::iterator VoxelNode<T_Container>::end()
{
    iterator it;
    it.end(*this);
    return it;
}

template <class T_Container>
inline unsigned int VoxelNode<T_Container>::getNbVoxel() const
{
    return (_container) ? _container->getNbVoxel() : 0;
}

template <class T_Container>
inline bool VoxelNode<T_Container>::hasVoxel() const
{
    return _container != nullptr;
}

template <class T_Container>
inline T_Container* VoxelNode<T_Container>::getVoxelContainer()
{
    return _container.get();
}

template <class T_Container>
inline T_Container const* VoxelNode<T_Container>::getVoxelContainer() const
{
    return _container.get();
}

template <class T_Container>
inline std::shared_ptr<T_Container> VoxelNode<T_Container>::getSharedVoxelContainer()
{
    return _container;
}

template <class T_Container>
inline void VoxelNode<T_Container>::setVoxelContainer(std::shared_ptr<T_Container> area)
{
    _container = area;
    _container->init(*this);
}

template <class T_Container>
typename T_Container::iterator VoxelNode<T_Container>::findVoxel(int x, int y, int z)
{
    iterator it;
    it.initPosition(x, y, z);

    if (this->getSize() != T_Container::NB_VOXELS || !this->isInside(x, y, z))
    {
        if (this->getOctree())
            return static_cast<VoxelOctree<T_Container>*>(this->getOctree())->findVoxel(x, y, z);
        else
            it.node = static_cast<VoxelNode<T_Container>*>(
              this->findNode(x & T_Container::COORD_MASK, y & T_Container::COORD_MASK, z & T_Container::COORD_MASK, T_Container::NB_VOXELS));
    }
    else
        it.node = this;

    if (it.node)
        it.node->findVoxel(it);
    return it;
}

template <class T_Container>
bool VoxelNode<T_Container>::findVoxel(iterator& it)
{
    it.node = this;
    if (_container != nullptr)
    {
        it.voxel = _container->findVoxel(it);
        return true;
    }
    return false;
}

template <class T_Container>
typename T_Container::iterator VoxelNode<T_Container>::findRelativeVoxel(int x, int y, int z) const
{
    iterator it;
    x += this->_x;
    y += this->_y;
    z += this->_z;
    it.initPosition(x, y, z);
    x &= T_Container::COORD_MASK;
    y &= T_Container::COORD_MASK;
    z &= T_Container::COORD_MASK;

    if (this->_x == x && this->_y == y && this->_z == z)
    {
        it.node = const_cast<VoxelNode<T_Container>*>(this);
        it.voxel = it.node->getVoxelContainer()->findVoxel(it);
    }
    else
    {
        it.node = this->findNode(x, y, z, T_Container::NB_VOXELS);
        if (it.node && it.node->hasVoxel())
            it.voxel = it.node->getVoxelContainer()->findVoxel(it);
    }

    return it;
}


template <class T_Container>
typename T_Container::iterator VoxelNode<T_Container>::findRelativeVoxel(NeighborAreaCache& neighbor_cache,
                                                               int x,
                                                               int y,
                                                               int z) const
{
    iterator it;
    x += this->_x;
    y += this->_y;
    z += this->_z;
    it.initPosition(x, y, z);
    x &= T_Container::COORD_MASK;
    y &= T_Container::COORD_MASK;
    z &= T_Container::COORD_MASK;

    if (this->_x == x && this->_y == y && this->_z == z)
        it.node = const_cast<VoxelNode<T_Container>*>(this);
    else
    {
        int ix = ((x - this->_x) / static_cast<int>(T_Container::NB_VOXELS)) + 1;
        int iy = ((y - this->_y) / static_cast<int>(T_Container::NB_VOXELS)) + 1;
        int iz = ((z - this->_z) / static_cast<int>(T_Container::NB_VOXELS)) + 1;

        if (ix >= 0 && ix <= 2 && iy >= 0 && iy <= 2 && iz >= 0 && iz <= 2)
        {
            auto& pair = neighbor_cache.neighbor_nodes[ix][iy][iz];
            if (pair.second == false)
            {
                auto node = this->findNode(x, y, z, T_Container::NB_VOXELS);
                pair.first = node;
                pair.second = true;
            }
            it.node = pair.first;
        }
        else
        {
            auto cache_it = neighbor_cache.nodes.find(Vector3I(x, y, z));
            if (cache_it == neighbor_cache.nodes.end())
            {
                auto node = this->findNode(x, y, z, T_Container::NB_VOXELS);
                cache_it = neighbor_cache.nodes.emplace(Vector3I(x, y, z), node).first;
            }
            it.node = cache_it->second;
        }
    }

    if (it.node && it.node->hasVoxel())
        it.voxel = it.node->getVoxelContainer()->findVoxel(it);
    return it;
}

template <class T_Container>
template <typename... Args>
bool VoxelNode<T_Container>::addVoxel(iterator& it, Args&&... args)
{
    if (!_container)
    {
        _container = std::make_shared<T_Container>();
        _container->init(*this);
    }
    else if (_container.use_count() > 1)
        this->copyOnWrite();

    return _container->addVoxel(it, std::forward<Args>(args)...);
}

template <class T_Container>
template <typename... Args>
bool VoxelNode<T_Container>::updateVoxel(iterator& it, Args&&... args)
{
    if (_container.use_count() > 1)
        this->copyOnWrite();
    return _container->updateVoxel(it, std::forward<Args>(args)...);
}

template <class T_Container>
template <typename... Args>
void VoxelNode<T_Container>::putVoxel(iterator& it, Args&&... args)
{
    if (!_container)
    {
        _container = std::make_shared<T_Container>();
        _container->init(*this);
    }
    else if (_container.use_count() > 1)
        this->copyOnWrite();

    _container->putVoxel(it, std::forward<Args>(args)...);
}

template <class T_Container>
template <typename... Args>
bool VoxelNode<T_Container>::removeVoxel(iterator const& it, Args&&... args)
{
    if (_container.use_count() > 1)
        this->copyOnWrite();
    bool return_value = _container->removeVoxel(it, std::forward<Args>(args)...);
    if (_container->getNbVoxel() == 0)
    {
        _container.reset();
        Node<VoxelNode<T_Container>>::_octree->pop(*this);
    }
    return return_value;
}

template <class T_Container>
void VoxelNode<T_Container>::exploreVoxel(std::function<void(iterator const&)> const& predicate) const
{
    if (_container)
    {
		iterator it;
		it.node = const_cast<VoxelNode<T_Container>*>(this);
		_container->exploreVoxel(it, predicate);
    }
    else
    {
        for (auto const child : this->_children)
        {
            if (child)
                static_cast<VoxelNode<T_Container> const*>(child)->exploreVoxel(predicate);
        }
    }
}

template <class T_Container>
void VoxelNode<T_Container>::exploreVoxelContainer(std::function<void(typename T_Container::VoxelContainer const&)> const& predicate) const
{
    if (_container)
    {
        _container->exploreVoxelContainer(predicate);
    }
    else
    {
        for (auto const child : this->_children)
        {
            if (child)
                static_cast<VoxelNode<T_Container> const*>(child)->exploreVoxelContainer(predicate);
        }
    }
}

template <class T_Container>
void VoxelNode<T_Container>::exploreVoxelNode(std::function<void(VoxelNode<T_Container> const&)> const& predicate) const
{
    if (_container)
        predicate(*this);
    else
    {
        for (auto const child : this->_children)
        {
            if (child)
                static_cast<VoxelNode<T_Container> const*>(child)->exploreVoxelNode(predicate);
        }
    }
}

template <class T_Container>
void VoxelNode<T_Container>::exploreVoxelNode(std::function<void(VoxelNode<T_Container>&)> const& predicate)
{
    if (_container)
        predicate(*this);
    else
    {
        for (auto child : this->_children)
        {
            if (child)
                static_cast<VoxelNode<T_Container>*>(child)->exploreVoxelNode(predicate);
        }
    }
}

template <class T_Container>
void VoxelNode<T_Container>::exploreBoundingBox(
  BoundingBox<int> const& bounding_box,
  std::function<void(VoxelNode<T_Container>&)> const& in_predicate,
  std::function<void(VoxelNode<T_Container>&)> const& out_predicate)
{
    BoundingBox<int> box(this->_x, this->_y, this->_z, this->_size, this->_size, this->_size);

    if (bounding_box.intersect(box))
    {
        if (_container && in_predicate)
        {
            in_predicate(*this);
        }

        for (auto child : this->_children)
        {
            if (child)
            {
                static_cast<VoxelNode<T_Container>*>(child)->exploreBoundingBox(bounding_box, in_predicate, out_predicate);
            }
        }
    }
    else if (out_predicate)
    {
        this->exploreVoxelNode(out_predicate);
    }
}

template <class T_Container>
void VoxelNode<T_Container>::merge(VoxelNode<T_Container>& node)
{
    if (node._container)
    {
        if (!_container)
        {
            _container = std::make_shared<T_Container>();
            _container->init(*this);
        }
        else if (_container.use_count() > 1)
            this->copyOnWrite();

        //for (uint8_t x = 0; x < T_Container::NB_VOXELS; ++x)
        //{
        //    for (uint8_t y = 0; y < T_Container::NB_VOXELS; ++y)
        //    {
        //        for (uint8_t z = 0; z < T_Container::NB_VOXELS; ++z)
        //        {
        //            auto voxel = node._container->findVoxel(x, y, z);
        //            if (voxel)
        //            {
        //                iterator tmp;
        //                tmp.x = x;
        //                tmp.y = y;
        //                tmp.z = z;
        //                tmp.node = this;
        //                this->addVoxel(tmp, *voxel);
        //            }
        //        }
        //    }
        //}
    }

    this->P_Node::merge(node);
}

template <class T_Container>
inline bool VoxelNode<T_Container>::empty() const
{
    return this->P_Node::empty() && (_container == nullptr || _container->getNbVoxel() == 0);
}

template <class T_Container>
void VoxelNode<T_Container>::copyOnWrite()
{
    auto tmp = _container;
    _container = std::make_shared<T_Container>(*tmp);
    _container->init(*this);
}

template <class T_Container>
uint32_t VoxelNode<T_Container>::serializeNode(std::string& str) const
{
    uint32_t nb_container = 0;

    if (_container && _container->getNbVoxel() > 0)
    {
        int pos[4];
        pos[0] = this->getX();
        pos[1] = this->getY();
        pos[2] = this->getZ();
        pos[3] = this->getSize();
        str.append(reinterpret_cast<char const*>(&pos), sizeof(pos));
        _container->serialize(str);
        ++nb_container;
    }

    for (auto child : this->_children)
    {
        if (child)
            nb_container += child->serializeNode(str);
    }
    return nb_container;
}

template <class T_Container>
inline void VoxelNode<T_Container>::serialize(std::string& str) const
{
    uint32_t total_size = 0;
    uint32_t nb_container = 0;
    size_t pos = str.size();

    str.append(reinterpret_cast<char*>(&total_size), sizeof(total_size));
    str.append(reinterpret_cast<char*>(&nb_container), sizeof(nb_container));
    nb_container = this->serializeNode(str);

    total_size = static_cast<uint32_t>(str.size() - pos);
    std::memcpy(&str[pos], &total_size, sizeof(total_size));
    pos += sizeof(total_size);
    std::memcpy(&str[pos], &nb_container, sizeof(nb_container));
}

template <class T_Container>
size_t VoxelNode<T_Container>::unserialize(VoxelOctree<T_Container>& octree, char const* str, size_t size)
{
    uint32_t total_size;
    size_t pos = 0;

    if (size < sizeof(total_size))
        return 0;
    std::memcpy(&total_size, str, sizeof(total_size));
    if (size < total_size)
        return 0;
    uint32_t nb_container;
    int position[4];

    pos += sizeof(total_size);
    std::memcpy(&nb_container, &str[pos], sizeof(nb_container));
    pos += sizeof(nb_container);

    for (uint32_t i = 0; i < nb_container; ++i)
    {
        std::memcpy(position, &str[pos], sizeof(position));
        pos += sizeof(position);
        auto node = new VoxelNode<T_Container>(position[0], position[1], position[2], position[3]);
        node->_container = std::make_shared<T_Container>();
        pos += node->_container->unserialize(&str[pos], size - pos);
        octree.push(*node);
    }
    return pos;
}

template <class T_Container>
inline int VoxelNode<T_Container>::findContainerPosition(int src, int container_id)
{
	return (src >> (3 * container_id)) & T_Container::CONTAINER_MASK;
}

template <class T_Container>
inline int VoxelNode<T_Container>::findVoxelPosition(int src)
{
    return src & T_Container::VOXEL_MASK;
}

template <class T_Container>
VoxelNode<T_Container>::NeighborAreaCache::NeighborAreaCache()
{
    std::memset(this->neighbor_nodes, 0, sizeof(this->neighbor_nodes));
}

} // namespace voxomap
