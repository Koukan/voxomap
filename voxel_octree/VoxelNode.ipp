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
    if (other._area)
        _area.reset(new T_Area(*other._area));
}

template <class T_Area>
typename T_Area::iterator VoxelNode<T_Area>::begin()
{
    iterator it;
    it.begin(*this);
    return it;
}

template <class T_Area>
typename T_Area::iterator VoxelNode<T_Area>::end()
{
    iterator it;
    it.end(*this);
    return it;
}

template <class T_Area>
inline unsigned int VoxelNode<T_Area>::getNbVoxel() const
{
    return (_area) ? _area->getNbVoxel() : 0;
}

template <class T_Area>
inline bool VoxelNode<T_Area>::hasVoxel() const
{
    return _area != nullptr;
}

template<class T_Area>
inline T_Area* VoxelNode<T_Area>::getVoxelArea()
{
    return _area.get();
}

template <class T_Area>
inline T_Area const* VoxelNode<T_Area>::getVoxelArea() const
{
    return _area.get();
}

template <class T_Area>
inline std::shared_ptr<T_Area> VoxelNode<T_Area>::getSharedVoxelArea()
{
    return _area;
}

template <class T_Area>
inline void VoxelNode<T_Area>::setVoxelArea(std::shared_ptr<T_Area> area)
{
    _area = area;
}

template <class T_Area>
typename T_Area::iterator VoxelNode<T_Area>::findVoxel(int x, int y, int z)
{
    iterator it;

    if (this->getSize() != T_Area::NB_VOXELS || !this->isInside(x, y, z))
    {
        if (this->getOctree())
            return static_cast<VoxelOctree<T_Area>*>(this->getOctree())->findVoxel(x, y, z);
        else
            it.node = static_cast<VoxelNode<T_Area>*>(
              this->findNode(x & AREA_MASK, y & AREA_MASK, z & AREA_MASK, T_Area::NB_VOXELS));
    }
    else
        it.node = this;

    it.x = findPosition(x);
    it.y = findPosition(y);
    it.z = findPosition(z);
    if (it.node)
        it.node->findVoxel(it);
    return it;
}

template <class T_Area>
bool VoxelNode<T_Area>::findVoxel(iterator& it)
{
    it.node = this;
    if (_area != nullptr)
    {
        it.voxel = _area->findVoxel(it.x, it.y, it.z);
        return true;
    }
    return false;
}

template <class T_Area>
template <typename... Args>
bool VoxelNode<T_Area>::addVoxel(iterator& it, Args&&... args)
{
    if (it.node != this)
        return false;

    if (!_area)
    {
        _area = std::make_shared<T_Area>();
        _area->init(*this);
    }
    else
        this->copyOnWrite();

    return _area->addVoxel(it, std::forward<Args>(args)...);
}

template <class T_Area>
template <typename... Args>
bool VoxelNode<T_Area>::updateVoxel(iterator& it, Args&&... args)
{
    if (!it || it.node != this)
        return false;

    this->copyOnWrite();
    return _area->updateVoxel(it, std::forward<Args>(args)...);
}

template <class T_Area>
template <typename... Args>
void VoxelNode<T_Area>::putVoxel(iterator& it, Args&&... args)
{
    if (!_area)
    {
        _area = std::make_shared<T_Area>();
        _area->init(*this);
    }
    else
        this->copyOnWrite();

    _area->putVoxel(it, std::forward<Args>(args)...);
}

template <class T_Area>
template <typename... Args>
typename VoxelNode<T_Area>::iterator VoxelNode<T_Area>::removeVoxel(iterator it, Args&&... args)
{
    if (!it || it.node != this)
        return iterator();

    this->copyOnWrite();
    return _area->removeVoxel(it, std::forward<Args>(args)...);
}

template <class T_Area>
void VoxelNode<T_Area>::exploreVoxel(std::function<void(VoxelNode<T_Area> const&, typename T_Area::VoxelData const&, uint8_t, uint8_t, uint8_t)> const& predicate) const
{
    if (_area)
    {
        for (uint8_t x = 0; x < T_Area::NB_VOXELS; ++x)
        {
            for (uint8_t y = 0; y < T_Area::NB_VOXELS; ++y)
            {
                for (uint8_t z = 0; z < T_Area::NB_VOXELS; ++z)
                {
                    auto voxel = _area->findVoxel(x, y, z);
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
    if (_area)
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
    if (node._area)
    {
        if (!_area)
        {
            _area = std::make_shared<T_Area>();
            _area->init(*this);
        }
        else
            this->copyOnWrite();

        for (uint8_t x = 0; x < T_Area::NB_VOXELS; ++x)
        {
            for (uint8_t y = 0; y < T_Area::NB_VOXELS; ++y)
            {
                for (uint8_t z = 0; z < T_Area::NB_VOXELS; ++z)
                {
                    auto voxel = node._area->findVoxel(x, y, z);
                    if (voxel)
                    {
                        iterator tmp;
                        tmp.x = x;
                        tmp.y = y;
                        tmp.z = z;
                        tmp.node = this;
                        this->addVoxel(tmp, *voxel);
                    }
                }
            }
        }
    }

    this->P_Node::merge(node);
}

template <class T_Area>
inline bool VoxelNode<T_Area>::empty() const
{
    return this->P_Node::empty() && (_area == nullptr || _area->getNbVoxel() == 0);
}

template <class T_Area>
void VoxelNode<T_Area>::copyOnWrite()
{
    if (_area.use_count() > 1)
    {
        auto tmp = _area;
        _area = std::make_shared<T_Area>(*tmp);
        _area->init(*this);
    }
}

template <class T_Area>
void VoxelNode<T_Area>::serializeNode(VoxelNode const& node, std::string& str) const
{
    int nbVoxelList = 0;

    if (node._area)
    {
        int pos[4];
        pos[0] = node.getX();
        pos[1] = node.getY();
        pos[2] = node.getZ();
        pos[3] = node.getSize();
        str.append(reinterpret_cast<char const*>(&pos), sizeof(pos));
        node._area->serialize(str);
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
    std::memcpy(dest, src, size);
    return size;
}

template <class T_Area>
VoxelNode<T_Area>* VoxelNode<T_Area>::unserialize(char const* str, size_t strsize)
{
    // if (strsize < sizeof(uint32_t))
    return nullptr;
    /*VoxelNode    *parent = nullptr;
    size_t      position = 0;
    int            pos[4];

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

template <class T_Area>
inline int VoxelNode<T_Area>::findPosition(int src)
{
    return src & VOXEL_MASK;
}

} // namespace voxomap
