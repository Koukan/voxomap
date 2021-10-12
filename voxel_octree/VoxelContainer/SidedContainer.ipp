namespace voxomap
{

template <class T_Voxel>
template <typename T, typename>
SidedVoxel<T_Voxel>::SidedVoxel(T arg)
    : T_Voxel(arg)
{
}

template <class T_Voxel>
template <typename T, typename>
SidedVoxel<T_Voxel>::SidedVoxel(T const& arg)
    : T_Voxel(arg)
{
}

template <class T_Voxel>
template <typename T, typename U, typename... Args>
SidedVoxel<T_Voxel>::SidedVoxel(T&& arg_1, U&& arg_2, Args&&... args)
    : T_Voxel(std::forward<T>(arg_1), std::forward<U>(arg_2), std::forward<Args>(args)...)
{
}
template <class T_Voxel>
template <typename... Args>
SidedVoxel<T_Voxel>::SidedVoxel(SideEnum side, Args&&... args)
    : T_Voxel(std::forward<Args>(args)...), _voxel_side(side)
{
}

template <class T_Voxel>
inline SidedVoxel<T_Voxel>::operator bool() const
{
    return static_cast<T_Voxel const&>(*this);
}

template <class T_Voxel>
inline bool SidedVoxel<T_Voxel>::operator==(SidedVoxel const& other) const
{
    return static_cast<T_Voxel const&>(*this) == static_cast<T_Voxel const&>(other);
}

template <class T_Voxel>
inline bool SidedVoxel<T_Voxel>::operator!=(SidedVoxel const& other) const
{
    return !(*this == other);
}

template <class T_Voxel>
inline bool SidedVoxel<T_Voxel>::operator==(T_Voxel const& other) const
{
    return static_cast<T_Voxel const&>(*this) == other;
}

template <class T_Voxel>
inline bool SidedVoxel<T_Voxel>::operator!=(T_Voxel const& other) const
{
    return !(*this == other);
}

template <class T_Voxel>
inline bool SidedVoxel<T_Voxel>::operator==(uint8_t f) const
{
    return _voxel_side == f;
}

template <class T_Voxel>
inline bool SidedVoxel<T_Voxel>::operator!=(uint8_t f) const
{
    return _voxel_side != f;
}

template <class T_Voxel>
inline uint8_t SidedVoxel<T_Voxel>::operator&(uint8_t f) const
{
    return _voxel_side & f;
}

template <class T_Voxel>
inline uint8_t SidedVoxel<T_Voxel>::operator|(uint8_t f) const
{
    return _voxel_side | f;
}

template <class T_Voxel>
inline uint8_t SidedVoxel<T_Voxel>::operator&(SideEnum f) const
{
    return _voxel_side & f;
}

template <class T_Voxel>
inline uint8_t SidedVoxel<T_Voxel>::operator|(SideEnum f) const
{
    return _voxel_side | f;
}

template <class T_Voxel>
inline SidedVoxel<T_Voxel>& SidedVoxel<T_Voxel>::operator&=(uint8_t f)
{
    _voxel_side &= f;
    return *this;
}

template <class T_Voxel>
inline SidedVoxel<T_Voxel>& SidedVoxel<T_Voxel>::operator|=(uint8_t f)
{
    _voxel_side |= f;
    return *this;
}

template <class T_Voxel>
inline uint8_t SidedVoxel<T_Voxel>::getSide() const
{
    return _voxel_side;
}


// SidedContainer
template <template <class...> class T_Container, class T_Voxel>
inline uint16_t SidedContainer<T_Container, T_Voxel>::getNbSide() const
{
    return _nbSides;
}

template <template <class...> class T_Container, class T_Voxel>
template <typename Iterator, typename... Args>
int SidedContainer<T_Container, T_Voxel>::addVoxel(Iterator& it, Args&&... args)
{
    if (!T_Container<VoxelData>::addVoxel(it, std::forward<Args>(args)...))
        return 0;

    _nbSides += 6;
    this->removeSide(it);
    return 1;
}

template <template <class...> class T_Container, class T_Voxel>
template <typename Iterator, typename... Args>
int SidedContainer<T_Container, T_Voxel>::updateVoxel(Iterator& it, Args&&... args)
{
    if (!T_Container<VoxelData>::updateVoxel(it, std::forward<Args>(args)...))
        return 0;

    this->updateSide(it);
    return 1;
}

template <template <class...> class T_Container, class T_Voxel>
template <typename Iterator, typename... Args>
int SidedContainer<T_Container, T_Voxel>::putVoxel(Iterator& it, Args&&... args)
{
    auto voxel = this->findVoxel(it.x, it.y, it.z);

    if (voxel)
    {
        this->updateVoxel(it, std::forward<Args>(args)...);
        return 0;
    }
    else
    {
        return this->addVoxel(it, std::forward<Args>(args)...);
    }
}

template <template <class...> class T_Container, class T_Voxel>
template <typename Iterator>
int SidedContainer<T_Container, T_Voxel>::removeVoxel(Iterator const& it, VoxelData* return_voxel)
{
    if (!T_Container<VoxelData>::removeVoxel(it, return_voxel))
        return 0;

    this->addSide(it);
    return 1;
}

template <template <class...> class T_Container, class T_Voxel>
void SidedContainer<T_Container, T_Voxel>::exploreVoxelContainer(std::function<void(SidedContainer const&)> const& predicate) const
{
    predicate(*this);
}


template <template <class...> class T_Container, class T_Voxel>
void SidedContainer<T_Container, T_Voxel>::serialize(std::string& str) const
{
    str.append(reinterpret_cast<char const*>(&_nbSides), sizeof(_nbSides));
	T_Container<VoxelData>::serialize(str);
}

template <template <class...> class T_Container, class T_Voxel>
size_t SidedContainer<T_Container, T_Voxel>::unserialize(char const* str, size_t size)
{
    if (size < sizeof(_nbSides))
        return 0;
    std::memcpy(&_nbSides, str, sizeof(_nbSides));
    size_t tmpsize = this->T_Container<VoxelData>::unserialize(str + sizeof(_nbSides), size - sizeof(_nbSides));
    return tmpsize ? sizeof(_nbSides) + tmpsize : 0;
}


// Side management
template <class Iterator>
static void addSide(Iterator const& otherIt, SideEnum side)
{
    if (otherIt.voxel)
    {
        *otherIt.voxel &= ~side;
        ++otherIt.voxelContainer->_nbSides;
    }
}

template <class Iterator>
static void removeSide(Iterator const& currentIt, Iterator const& otherIt, SideEnum f1, SideEnum f2)
{
    if (!currentIt.voxel || !otherIt.voxel)
        return;

    if (currentIt.voxel->mergeSide(*otherIt.voxel))
    {
        *currentIt.voxel |= f1;
        --currentIt.voxelContainer->_nbSides;
    }

    if (otherIt.voxel->mergeSide(*currentIt.voxel))
    {
        *otherIt.voxel |= f2;
        --otherIt.voxelContainer->_nbSides;
    }
}

template <class Iterator>
static void updateSide(Iterator const& currentIt, Iterator const& otherIt, SideEnum f1, SideEnum f2)
{
    if (!otherIt.voxel)
        return;
    
    if (currentIt.voxel->mergeSide(*otherIt.voxel))
    {
        if ((*currentIt.voxel & f1) == 0)
        {
            *currentIt.voxel |= f1;
            --currentIt.voxelContainer->_nbSides;
        }
    }
    else if (*currentIt.voxel & f1)
    {
        *currentIt.voxel &= ~f1;
        ++currentIt.voxelContainer->_nbSides;
    }

    if (otherIt.voxel->mergeSide(*currentIt.voxel))
    {
        if ((*otherIt.voxel & f2) == 0)
        {
            *otherIt.voxel |= f2;
            --otherIt.voxelContainer->_nbSides;
        }
    }
    else if (*otherIt.voxel & f2)
    {
        *otherIt.voxel &= ~f2;
        ++otherIt.voxelContainer-> _nbSides;
    }
}

template <class Node>
static typename Node::iterator findRelativeVoxel(Node& node, int x, int y, int z)
{
    typename Node::iterator it;
    it.initPosition(x, y, z);
    x &= Node::Container::COORD_MASK;
    y &= Node::Container::COORD_MASK;
    z &= Node::Container::COORD_MASK;

    if (node.getX() == x && node.getY() == y && node.getZ() == z)
    {
        it.node = &node;
        it.voxel = it.node->getVoxelContainer()->findVoxel(it);
    }
    else
    {
        it.node = node.findNode(x, y, z, Node::Container::NB_VOXELS);
        if (it.node)
            it.voxel = it.node->getVoxelContainer()->findVoxel(it);
    }

    return it;
}

template <template <class...> class T_Container, class T_Voxel>
template <typename Iterator>
void SidedContainer<T_Container, T_Voxel>::addSide(Iterator const& it)
{
    int x, y, z;
    it.getVoxelPosition(x, y, z);

    Iterator otherIt;
    otherIt.voxelContainer = this;
    if (it.z > 0)
    {
        otherIt.voxel = this->findVoxel(it.x, it.y, it.z - 1);
        voxomap::addSide(otherIt, SideEnum::ZPOS);
    }
    else
        voxomap::addSide(findRelativeVoxel(*it.node, x, y, z - 1), SideEnum::ZPOS);

    if (it.z < NB_VOXELS - 1)
    {
        otherIt.voxel = this->findVoxel(it.x, it.y, it.z + 1);
        voxomap::addSide(otherIt, SideEnum::ZNEG);
    }
    else
        voxomap::addSide(findRelativeVoxel(*it.node, x, y, z + 1), SideEnum::ZNEG);

    if (it.y > 0)
    {
        otherIt.voxel = this->findVoxel(it.x, it.y - 1, it.z);
        voxomap::addSide(otherIt, SideEnum::YPOS);
    }
    else
        voxomap::addSide(findRelativeVoxel(*it.node, x, y - 1, z), SideEnum::YPOS);

    if (it.y < NB_VOXELS - 1)
    {
        otherIt.voxel = this->findVoxel(it.x, it.y + 1, it.z);
        voxomap::addSide(otherIt, SideEnum::YNEG);
    }
    else
        voxomap::addSide(findRelativeVoxel(*it.node, x, y + 1, z), SideEnum::YNEG);

    if (it.x > 0)
    {
        otherIt.voxel = this->findVoxel(it.x - 1, it.y, it.z);
        voxomap::addSide(otherIt, SideEnum::XPOS);
    }
    else
        voxomap::addSide(findRelativeVoxel(*it.node, x - 1, y, z), SideEnum::XPOS);

    if (it.x < NB_VOXELS - 1)
    {
        otherIt.voxel = this->findVoxel(it.x + 1, it.y, it.z);
        voxomap::addSide(otherIt, SideEnum::XNEG);
    }
    else
        voxomap::addSide(findRelativeVoxel(*it.node, x + 1, y, z), SideEnum::XNEG);
}

template <template <class...> class T_Container, class T_Voxel>
template <typename Iterator>
void SidedContainer<T_Container, T_Voxel>::removeSide(Iterator const& it)
{
    int x, y, z;
    it.getVoxelPosition(x, y, z);

    Iterator otherIt;
    otherIt.voxelContainer = this;
    if (it.z > 0)
    {
        otherIt.voxel = this->findVoxel(it.x, it.y, it.z - 1);
        voxomap::removeSide(it, otherIt, SideEnum::ZNEG, SideEnum::ZPOS);
    }
    else
        voxomap::removeSide(it, findRelativeVoxel(*it.node, x, y, z - 1), SideEnum::ZNEG, SideEnum::ZPOS);

    if (it.z < NB_VOXELS - 1)
    {
        otherIt.voxel = this->findVoxel(it.x, it.y, it.z + 1);
        voxomap::removeSide(it, otherIt, SideEnum::ZNEG, SideEnum::ZPOS);
    }
    else
        voxomap::removeSide(it, findRelativeVoxel(*it.node, x, y, z + 1), SideEnum::ZPOS, SideEnum::ZNEG);

    if (it.y > 0)
    {
        otherIt.voxel = this->findVoxel(it.x, it.y - 1, it.z);
        voxomap::removeSide(it, otherIt, SideEnum::YNEG, SideEnum::YPOS);
    }
    else
        voxomap::removeSide(it, findRelativeVoxel(*it.node, x, y - 1, z), SideEnum::YNEG, SideEnum::YPOS);

    if (it.y < NB_VOXELS - 1)
    {
        otherIt.voxel = this->findVoxel(it.x, it.y + 1, it.z);
        voxomap::removeSide(it, otherIt, SideEnum::YPOS, SideEnum::YNEG);
    }
    else
        voxomap::removeSide(it, findRelativeVoxel(*it.node, x, y + 1, z), SideEnum::YPOS, SideEnum::YNEG);

    if (it.x > 0)
    {
        otherIt.voxel = this->findVoxel(it.x - 1, it.y, it.z);
        voxomap::removeSide(it, otherIt, SideEnum::XNEG, SideEnum::XPOS);
    }
    else
        voxomap::removeSide(it, findRelativeVoxel(*it.node, x - 1, y, z), SideEnum::XNEG, SideEnum::XPOS);

    if (it.x < NB_VOXELS - 1)
    {
        otherIt.voxel = this->findVoxel(it.x + 1, it.y, it.z);
        voxomap::removeSide(it, otherIt, SideEnum::XPOS, SideEnum::XNEG);
    }
    else
        voxomap::removeSide(it, findRelativeVoxel(*it.node, x + 1, y, z), SideEnum::XPOS, SideEnum::XNEG);
}

template <template <class...> class T_Container, class T_Voxel>
template <typename Iterator>
void SidedContainer<T_Container, T_Voxel>::updateSide(Iterator const& it)
{
    int x, y, z;
    it.getVoxelPosition(x, y, z);

    Iterator otherIt;
    otherIt.voxelContainer = this;
    if (it.z > 0)
    {
        otherIt.voxel = this->findVoxel(it.x, it.y, it.z - 1);
        voxomap::updateSide(it, otherIt, SideEnum::ZNEG, SideEnum::ZPOS);
    }
    else
        voxomap::updateSide(it, findRelativeVoxel(*it.node, x, y, z - 1), SideEnum::ZNEG, SideEnum::ZPOS);

    if (it.z < NB_VOXELS - 1)
    {
        otherIt.voxel = this->findVoxel(it.x, it.y, it.z + 1);
        voxomap::updateSide(it, otherIt, SideEnum::ZPOS, SideEnum::ZNEG);
    }
    else
        voxomap::updateSide(it, findRelativeVoxel(*it.node, x, y, z + 1), SideEnum::ZPOS, SideEnum::ZNEG);

    if (it.y > 0)
    {
        otherIt.voxel = this->findVoxel(it.x, it.y - 1, it.z);
        voxomap::updateSide(it, otherIt, SideEnum::YNEG, SideEnum::YPOS);
    }
    else
        voxomap::updateSide(it, findRelativeVoxel(*it.node, x, y - 1, z), SideEnum::YNEG, SideEnum::YPOS);

    if (it.y < NB_VOXELS - 1)
    {
        otherIt.voxel = this->findVoxel(it.x, it.y + 1, it.z);
        voxomap::updateSide(it, otherIt, SideEnum::YPOS, SideEnum::YNEG);
    }
    else
        voxomap::updateSide(it, findRelativeVoxel(*it.node, x, y + 1, z), SideEnum::YPOS, SideEnum::YNEG);

    if (it.x > 0)
    {
        otherIt.voxel = this->findVoxel(it.x - 1, it.y, it.z);
        voxomap::updateSide(it, otherIt, SideEnum::XNEG, SideEnum::XPOS);
    }
    else
        voxomap::updateSide(it, findRelativeVoxel(*it.node, x - 1, y, z), SideEnum::XNEG, SideEnum::XPOS);

    if (it.x < NB_VOXELS - 1)
    {
        otherIt.voxel = this->findVoxel(it.x + 1, it.y, it.z);
        voxomap::updateSide(it, otherIt, SideEnum::XPOS, SideEnum::XNEG);
    }
    else
        voxomap::updateSide(it, findRelativeVoxel(*it.node, x + 1, y, z), SideEnum::XPOS, SideEnum::XNEG);
}

}