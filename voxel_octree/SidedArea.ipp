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


// SidedArea
template <template <class...> class T_Area, class T_Voxel>
inline uint16_t SidedArea<T_Area, T_Voxel>::getNbSide() const
{
    return _nbSides;
}

template <template <class...> class T_Area, class T_Voxel>
template <typename Iterator, typename... Args>
bool SidedArea<T_Area, T_Voxel>::addVoxel(Iterator& it, Args&&... args)
{
    if (!T_Area<VoxelData>::addVoxel(it, std::forward<Args>(args)...))
        return false;

    _nbSides += 6;
    this->removeSide(*it.node, *it.voxel, it.x, it.y, it.z);
    return true;
}

template <template <class...> class T_Area, class T_Voxel>
template <typename Iterator, typename... Args>
bool SidedArea<T_Area, T_Voxel>::updateVoxel(Iterator& it, Args&&... args)
{
    if (!T_Area<VoxelData>::updateVoxel(it, std::forward<Args>(args)...))
        return false;

    this->updateSide(*it.node, *it.voxel, it.x, it.y, it.z);
    return true;
}

template <template <class...> class T_Area, class T_Voxel>
template <typename Iterator, typename... Args>
void SidedArea<T_Area, T_Voxel>::putVoxel(Iterator& it, Args&&... args)
{
    auto voxel = this->findVoxel(it.x, it.y, it.z);

    if (voxel)
        this->updateVoxel(it, std::forward<Args>(args)...);
    else
        this->addVoxel(it, std::forward<Args>(args)...);
}

template <template <class...> class T_Area, class T_Voxel>
template <typename Iterator>
Iterator SidedArea<T_Area, T_Voxel>::removeVoxel(Iterator it, VoxelData* return_voxel)
{
    this->addSide(*it.node, it.x, it.y, it.z);
    return T_Area<VoxelData>::removeVoxel(it, return_voxel);
}

template <template <class...> class T_Area, class T_Voxel>
void SidedArea<T_Area, T_Voxel>::serialize(std::string& str) const
{
    str.append(reinterpret_cast<char const*>(&_nbSides), sizeof(_nbSides));
    T_Area<VoxelData>::serialize(str);
}

template <template <class...> class T_Area, class T_Voxel>
size_t SidedArea<T_Area, T_Voxel>::unserialize(char const* str, size_t size)
{
    if (size < sizeof(_nbSides))
        return 0;
    std::memcpy(&_nbSides, str, sizeof(_nbSides));
    size_t tmpsize = this->T_Area<VoxelData>::unserialize(str + sizeof(_nbSides), size - sizeof(_nbSides));
    return tmpsize ? sizeof(_nbSides) + tmpsize : 0;
}


// Side management
template <class T_Area>
inline static void addSide(T_Area& area, typename T_Area::VoxelData* voxel, SideEnum side)
{
    if (voxel)
    {
        *voxel &= ~side;
        ++area._nbSides;
    }
}

template <class T_Area>
inline static void removeSide(T_Area& a1, T_Area& a2, typename T_Area::VoxelData* v1, typename T_Area::VoxelData* v2, SideEnum f1, SideEnum f2)
{
    if (!v1 || !v2)
        return;

    if (v1->mergeSide(*v2))
    {
        *v1 |= f1;
        --a1._nbSides;
    }

    if (v2->mergeSide(*v1))
    {
        *v2 |= f2;
        --a2._nbSides;
    }
}

template <class T_Area>
inline static void updateSide(T_Area& a1, T_Area& a2, typename T_Area::VoxelData* v1, typename T_Area::VoxelData* v2, SideEnum f1, SideEnum f2)
{
    if (!v2)
        return;
    
    if (v1->mergeSide(*v2))
    {
        if ((*v1 & f1) == 0)
        {
            *v1 |= f1;
            --a1._nbSides;
        }
    }
    else if (*v1 & f1)
    {
        *v1 &= ~f1;
        ++a1._nbSides;
    }

    if (v2->mergeSide(*v1))
    {
        if ((*v2 & f2) == 0)
        {
            *v2 |= f2;
            --a2._nbSides;
        }
    }
    else if (*v2 & f2)
    {
        *v2 &= ~f2;
        ++a2._nbSides;
    }
}

template <template <class...> class T_Area, class T_Voxel>
void SidedArea<T_Area, T_Voxel>::addSide(VoxelNode<SidedArea>& node, uint8_t x, uint8_t y, uint8_t z)
{
    if (x > 0)
        voxomap::addSide(*this, this->findVoxel(x - 1, y, z), SideEnum::XPOS);
    else
    {
        auto tmp = node.findNode(node.getX() - Area::NB_VOXELS, node.getY(), node.getZ(), Area::NB_VOXELS);
        if (tmp && tmp->_area)
            voxomap::addSide(*tmp->_area, tmp->_area->findVoxel(Area::NB_VOXELS - 1, y, z), SideEnum::XPOS);
    }
    if (x < Area::NB_VOXELS - 1)
        voxomap::addSide(*this, this->findVoxel(x + 1, y, z), SideEnum::XNEG);
    else
    {
        auto tmp = node.findNode(node.getX() + Area::NB_VOXELS, node.getY(), node.getZ(), Area::NB_VOXELS);
        if (tmp && tmp->_area)
            voxomap::addSide(*tmp->_area, tmp->_area->findVoxel(0, y, z), SideEnum::XNEG);
    }
    if (y > 0)
        voxomap::addSide(*this, this->findVoxel(x, y - 1, z), SideEnum::YPOS);
    else
    {
        auto tmp = node.findNode(node.getX(), node.getY() - Area::NB_VOXELS, node.getZ(), Area::NB_VOXELS);
        if (tmp && tmp->_area)
            voxomap::addSide(*tmp->_area, tmp->_area->findVoxel(x, Area::NB_VOXELS - 1, z), SideEnum::YPOS);
    }
    if (y < Area::NB_VOXELS - 1)
        voxomap::addSide(*this, this->findVoxel(x, y + 1, z), SideEnum::YNEG);
    else
    {
        auto tmp = node.findNode(node.getX(), node.getY() + Area::NB_VOXELS, node.getZ(), Area::NB_VOXELS);
        if (tmp && tmp->_area)
            voxomap::addSide(*tmp->_area, tmp->_area->findVoxel(x, 0, z), SideEnum::YNEG);
    }
    if (z > 0)
        voxomap::addSide(*this, this->findVoxel(x, y, z - 1), SideEnum::ZPOS);
    else
    {
        auto tmp = node.findNode(node.getX(), node.getY(), node.getZ() - Area::NB_VOXELS, Area::NB_VOXELS);
        if (tmp && tmp->_area)
            voxomap::addSide(*tmp->_area, tmp->_area->findVoxel(x, y, Area::NB_VOXELS - 1), SideEnum::ZPOS);
    }
    if (z < Area::NB_VOXELS - 1)
        voxomap::addSide(*this, this->findVoxel(x, y, z + 1), SideEnum::ZNEG);
    else
    {
        auto tmp = node.findNode(node.getX(), node.getY(), node.getZ() + Area::NB_VOXELS, Area::NB_VOXELS);
        if (tmp && tmp->_area)
            voxomap::addSide(*tmp->_area, tmp->_area->findVoxel(x, y, 0), SideEnum::ZNEG);
    }
}

template <template <class...> class T_Area, class T_Voxel>
void SidedArea<T_Area, T_Voxel>::removeSide(VoxelNode<SidedArea>& node, VoxelData& voxel, uint8_t x, uint8_t y, uint8_t z)
{
    if (x > 0)
        voxomap::removeSide(*this, *this, &voxel, this->findVoxel(x - 1, y, z), SideEnum::XNEG, SideEnum::XPOS);
    else
    {
        auto tmp = node.findNode(node.getX() - Area::NB_VOXELS, node.getY(), node.getZ(), Area::NB_VOXELS);
        if (tmp && tmp->_area)
            voxomap::removeSide(*this, *tmp->_area, &voxel, tmp->_area->findVoxel(Area::NB_VOXELS - 1, y, z), SideEnum::XNEG, SideEnum::XPOS);
    }
    if (x < Area::NB_VOXELS - 1)
        voxomap::removeSide(*this, *this, &voxel, this->findVoxel(x + 1, y, z), SideEnum::XPOS, SideEnum::XNEG);
    else
    {
        auto tmp = node.findNode(node.getX() + Area::NB_VOXELS, node.getY(), node.getZ(), Area::NB_VOXELS);
        if (tmp && tmp->_area)
            voxomap::removeSide(*this, *tmp->_area, &voxel, tmp->_area->findVoxel(0, y, z), SideEnum::XPOS, SideEnum::XNEG);
    }
    if (y > 0)
        voxomap::removeSide(*this, *this, &voxel, this->findVoxel(x, y - 1, z), SideEnum::YNEG, SideEnum::YPOS);
    else
    {
        auto tmp = node.findNode(node.getX(), node.getY() - Area::NB_VOXELS, node.getZ(), Area::NB_VOXELS);
        if (tmp && tmp->_area)
            voxomap::removeSide(*this, *tmp->_area, &voxel, tmp->_area->findVoxel(x, Area::NB_VOXELS - 1, z), SideEnum::YNEG, SideEnum::YPOS);
    }
    if (y < Area::NB_VOXELS - 1)
        voxomap::removeSide(*this, *this, &voxel, this->findVoxel(x, y + 1, z), SideEnum::YPOS, SideEnum::YNEG);
    else
    {
        auto tmp = node.findNode(node.getX(), node.getY() + Area::NB_VOXELS, node.getZ(), Area::NB_VOXELS);
        if (tmp && tmp->_area)
            voxomap::removeSide(*this, *tmp->_area, &voxel, tmp->_area->findVoxel(x, 0, z), SideEnum::YPOS, SideEnum::YNEG);
    }
    if (z > 0)
        voxomap::removeSide(*this, *this, &voxel, this->findVoxel(x, y, z - 1), SideEnum::ZNEG, SideEnum::ZPOS);
    else
    {
        auto tmp = node.findNode(node.getX(), node.getY(), node.getZ() - Area::NB_VOXELS, Area::NB_VOXELS);
        if (tmp && tmp->_area)
            voxomap::removeSide(*this, *tmp->_area, &voxel, tmp->_area->findVoxel(x, y, Area::NB_VOXELS - 1), SideEnum::ZNEG, SideEnum::ZPOS);
    }
    if (z < Area::NB_VOXELS - 1)
        voxomap::removeSide(*this, *this, &voxel, this->findVoxel(x, y, z + 1), SideEnum::ZPOS, SideEnum::ZNEG);
    else
    {
        auto tmp = node.findNode(node.getX(), node.getY(), node.getZ() + Area::NB_VOXELS, Area::NB_VOXELS);
        if (tmp && tmp->_area)
            voxomap::removeSide(*this, *tmp->_area, &voxel, tmp->_area->findVoxel(x, y, 0), SideEnum::ZPOS, SideEnum::ZNEG);
    }
}

template <template <class...> class T_Area, class T_Voxel>
void SidedArea<T_Area, T_Voxel>::updateSide(VoxelNode<SidedArea>& node, VoxelData& voxel, uint8_t x, uint8_t y, uint8_t z)
{
    if (x > 0)
        voxomap::updateSide(*this, *this, &voxel, this->findVoxel(x - 1, y, z), SideEnum::XNEG, SideEnum::XPOS);
    else
    {
        auto tmp = node.findNode(node.getX() - Area::NB_VOXELS, node.getY(), node.getZ(), Area::NB_VOXELS);
        if (tmp && tmp->_area)
            voxomap::updateSide(*this, *tmp->_area, &voxel, tmp->_area->findVoxel(Area::NB_VOXELS - 1, y, z), SideEnum::XNEG, SideEnum::XPOS);
    }
    if (x < Area::NB_VOXELS - 1)
        voxomap::updateSide(*this, *this, &voxel, this->findVoxel(x + 1, y, z), SideEnum::XPOS, SideEnum::XNEG);
    else
    {
        auto tmp = node.findNode(node.getX() + Area::NB_VOXELS, node.getY(), node.getZ(), Area::NB_VOXELS);
        if (tmp && tmp->_area)
            voxomap::updateSide(*this, *tmp->_area, &voxel, tmp->_area->findVoxel(0, y, z), SideEnum::XPOS, SideEnum::XNEG);
    }
    if (y > 0)
        voxomap::updateSide(*this, *this, &voxel, this->findVoxel(x, y - 1, z), SideEnum::YNEG, SideEnum::YPOS);
    else
    {
        auto tmp = node.findNode(node.getX(), node.getY() - Area::NB_VOXELS, node.getZ(), Area::NB_VOXELS);
        if (tmp && tmp->_area)
            voxomap::updateSide(*this, *tmp->_area, &voxel, tmp->_area->findVoxel(x, Area::NB_VOXELS - 1, z), SideEnum::YNEG, SideEnum::YPOS);
    }
    if (y < Area::NB_VOXELS - 1)
        voxomap::updateSide(*this, *this, &voxel, this->findVoxel(x, y + 1, z), SideEnum::YPOS, SideEnum::YNEG);
    else
    {
        auto tmp = node.findNode(node.getX(), node.getY() + Area::NB_VOXELS, node.getZ(), Area::NB_VOXELS);
        if (tmp && tmp->_area)
            voxomap::updateSide(*this, *tmp->_area, &voxel, tmp->_area->findVoxel(x, 0, z), SideEnum::YPOS, SideEnum::YNEG);
    }
    if (z > 0)
        voxomap::updateSide(*this, *this, &voxel, this->findVoxel(x, y, z - 1), SideEnum::ZNEG, SideEnum::ZPOS);
    else
    {
        auto tmp = node.findNode(node.getX(), node.getY(), node.getZ() - Area::NB_VOXELS, Area::NB_VOXELS);
        if (tmp && tmp->_area)
            voxomap::updateSide(*this, *tmp->_area, &voxel, tmp->_area->findVoxel(x, y, Area::NB_VOXELS - 1), SideEnum::ZNEG, SideEnum::ZPOS);
    }
    if (z < Area::NB_VOXELS - 1)
        voxomap::updateSide(*this, *this, &voxel, this->findVoxel(x, y, z + 1), SideEnum::ZPOS, SideEnum::ZNEG);
    else
    {
        auto tmp = node.findNode(node.getX(), node.getY(), node.getZ() + Area::NB_VOXELS, Area::NB_VOXELS);
        if (tmp && tmp->_area)
            voxomap::updateSide(*this, *tmp->_area, &voxel, tmp->_area->findVoxel(x, y, 0), SideEnum::ZPOS, SideEnum::ZNEG);
    }
}

}