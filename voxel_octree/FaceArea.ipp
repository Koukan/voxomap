namespace voxomap
{

template <class T_Voxel>
template <typename T, typename>
FaceVoxel<T_Voxel>::FaceVoxel(T arg)
    : T_Voxel(arg)
{
}

template <class T_Voxel>
template <typename T, typename>
FaceVoxel<T_Voxel>::FaceVoxel(T const& arg)
    : T_Voxel(arg)
{
}

template <class T_Voxel>
template <typename T, typename U, typename... Args>
FaceVoxel<T_Voxel>::FaceVoxel(T&& arg_1, U&& arg_2, Args&&... args)
    : T_Voxel(std::forward<T>(arg_1), std::forward<U>(arg_2), std::forward<Args>(args)...)
{
}

template <class T_Voxel>
inline FaceVoxel<T_Voxel>::operator bool() const
{
    return static_cast<T_Voxel const&>(*this);
}

template <class T_Voxel>
inline bool FaceVoxel<T_Voxel>::operator==(FaceVoxel const& other) const
{
    return static_cast<T_Voxel const&>(*this) == static_cast<T_Voxel const&>(other);
}

template <class T_Voxel>
inline bool FaceVoxel<T_Voxel>::operator!=(FaceVoxel const& other) const
{
    return !(*this == other);
}

template <class T_Voxel>
inline bool FaceVoxel<T_Voxel>::operator==(T_Voxel const& other) const
{
    return static_cast<T_Voxel const&>(*this) == other;
}

template <class T_Voxel>
inline bool FaceVoxel<T_Voxel>::operator!=(T_Voxel const& other) const
{
    return !(*this == other);
}

template <class T_Voxel>
inline bool FaceVoxel<T_Voxel>::operator==(uint8_t f) const
{
    return _voxel_face == f;
}

template <class T_Voxel>
inline bool FaceVoxel<T_Voxel>::operator!=(uint8_t f) const
{
    return _voxel_face != f;
}

template <class T_Voxel>
inline uint8_t FaceVoxel<T_Voxel>::operator&(uint8_t f) const
{
    return _voxel_face & f;
}

template <class T_Voxel>
inline uint8_t FaceVoxel<T_Voxel>::operator|(uint8_t f) const
{
    return _voxel_face | f;
}

template <class T_Voxel>
inline uint8_t FaceVoxel<T_Voxel>::operator&(FaceEnum f) const
{
    return _voxel_face & f;
}

template <class T_Voxel>
inline uint8_t FaceVoxel<T_Voxel>::operator|(FaceEnum f) const
{
    return _voxel_face | f;
}

template <class T_Voxel>
inline FaceVoxel<T_Voxel>& FaceVoxel<T_Voxel>::operator&=(uint8_t f)
{
    _voxel_face &= f;
    return *this;
}

template <class T_Voxel>
inline FaceVoxel<T_Voxel>& FaceVoxel<T_Voxel>::operator|=(uint8_t f)
{
    _voxel_face |= f;
    return *this;
}

template <class T_Voxel>
inline uint8_t FaceVoxel<T_Voxel>::getFace() const
{
    return _voxel_face;
}


// FaceArea
template <template <class...> class T_Area, class T_Voxel>
inline uint16_t FaceArea<T_Area, T_Voxel>::getNbFace() const
{
    return _nbFaces;
}

template <template <class...> class T_Area, class T_Voxel>
template <typename Iterator, typename... Args>
bool FaceArea<T_Area, T_Voxel>::addVoxel(Iterator& it, Args&&... args)
{
    if (!T_Area<VoxelData>::addVoxel(it, std::forward<Args>(args)...))
        return false;

    _nbFaces += 6;
    this->removeFace(*it.node, *it.voxel, it.x, it.y, it.z);
    return true;
}

template <template <class...> class T_Area, class T_Voxel>
template <typename Iterator, typename... Args>
bool FaceArea<T_Area, T_Voxel>::updateVoxel(Iterator& it, Args&&... args)
{
    if (!T_Area<VoxelData>::updateVoxel(it, std::forward<Args>(args)...))
        return false;

    this->updateFace(*it.node, *it.voxel, it.x, it.y, it.z);
    return true;
}

template <template <class...> class T_Area, class T_Voxel>
template <typename Iterator, typename... Args>
void FaceArea<T_Area, T_Voxel>::putVoxel(Iterator& it, Args&&... args)
{
    auto voxel = this->findVoxel(it.x, it.y, it.z);

    if (voxel)
        this->updateVoxel(it, std::forward<Args>(args)...);
    else
        this->addVoxel(it, std::forward<Args>(args)...);
}

template <template <class...> class T_Area, class T_Voxel>
template <typename Iterator>
Iterator FaceArea<T_Area, T_Voxel>::removeVoxel(Iterator it, VoxelData* return_voxel)
{
    this->addFace(*it.node, it.x, it.y, it.z);
    return T_Area<VoxelData>::removeVoxel(it, return_voxel);
}

template <template <class...> class T_Area, class T_Voxel>
void FaceArea<T_Area, T_Voxel>::serialize(std::string& str) const
{
    str.append(reinterpret_cast<char const*>(&_nbFaces), sizeof(_nbFaces));
    T_Area<VoxelData>::serialize(str);
}

template <template <class...> class T_Area, class T_Voxel>
size_t FaceArea<T_Area, T_Voxel>::unserialize(char const* str, size_t size)
{
    if (size < sizeof(_nbFaces))
        return 0;
    std::memcpy(&_nbFaces, str, sizeof(_nbFaces));
    size_t tmpsize = this->T_Area<VoxelData>::unserialize(str + sizeof(_nbFaces), size - sizeof(_nbFaces));
    return tmpsize ? sizeof(_nbFaces) + tmpsize : 0;
}


// Face management
template <class T_Area>
inline static void addFace(T_Area& area, typename T_Area::VoxelData* voxel, FaceEnum face)
{
    if (voxel)
    {
        *voxel &= ~face;
        ++area._nbFaces;
    }
}

template <class T_Area>
inline static void removeFace(T_Area& a1, T_Area& a2, typename T_Area::VoxelData* v1, typename T_Area::VoxelData* v2, FaceEnum f1, FaceEnum f2)
{
    if (!v1 || !v2)
        return;

    if (v1->mergeFace(*v2))
    {
        *v1 |= f1;
        --a1._nbFaces;
    }

    if (v2->mergeFace(*v1))
    {
        *v2 |= f2;
        --a2._nbFaces;
    }
}

template <class T_Area>
inline static void updateFace(T_Area& a1, T_Area& a2, typename T_Area::VoxelData* v1, typename T_Area::VoxelData* v2, FaceEnum f1, FaceEnum f2)
{
    if (!v2)
        return;
    
    if (v1->mergeFace(*v2))
    {
        if ((*v1 & f1) == 0)
        {
            *v1 |= f1;
            --a1._nbFaces;
        }
    }
    else if (*v1 & f1)
    {
        *v1 &= ~f1;
        ++a1._nbFaces;
    }

    if (v2->mergeFace(*v1))
    {
        if ((*v2 & f2) == 0)
        {
            *v2 |= f2;
            --a2._nbFaces;
        }
    }
    else if (*v2 & f2)
    {
        *v2 &= ~f2;
        ++a2._nbFaces;
    }
}

template <template <class...> class T_Area, class T_Voxel>
void FaceArea<T_Area, T_Voxel>::addFace(VoxelNode<FaceArea>& node, uint8_t x, uint8_t y, uint8_t z)
{
    if (x > 0)
        voxomap::addFace(*this, this->findVoxel(x - 1, y, z), FaceEnum::RIGHT);
    else
    {
        auto tmp = node.findNode(node.getX() - Area::NB_VOXELS, node.getY(), node.getZ(), Area::NB_VOXELS);
        if (tmp && tmp->_area)
            voxomap::addFace(*tmp->_area, tmp->_area->findVoxel(Area::NB_VOXELS - 1, y, z), FaceEnum::RIGHT);
    }
    if (x < Area::NB_VOXELS - 1)
        voxomap::addFace(*this, this->findVoxel(x + 1, y, z), FaceEnum::LEFT);
    else
    {
        auto tmp = node.findNode(node.getX() + Area::NB_VOXELS, node.getY(), node.getZ(), Area::NB_VOXELS);
        if (tmp && tmp->_area)
            voxomap::addFace(*tmp->_area, tmp->_area->findVoxel(0, y, z), FaceEnum::LEFT);
    }
    if (y > 0)
        voxomap::addFace(*this, this->findVoxel(x, y - 1, z), FaceEnum::TOP);
    else
    {
        auto tmp = node.findNode(node.getX(), node.getY() - Area::NB_VOXELS, node.getZ(), Area::NB_VOXELS);
        if (tmp && tmp->_area)
            voxomap::addFace(*tmp->_area, tmp->_area->findVoxel(x, Area::NB_VOXELS - 1, z), FaceEnum::TOP);
    }
    if (y < Area::NB_VOXELS - 1)
        voxomap::addFace(*this, this->findVoxel(x, y + 1, z), FaceEnum::BOTTOM);
    else
    {
        auto tmp = node.findNode(node.getX(), node.getY() + Area::NB_VOXELS, node.getZ(), Area::NB_VOXELS);
        if (tmp && tmp->_area)
            voxomap::addFace(*tmp->_area, tmp->_area->findVoxel(x, 0, z), FaceEnum::BOTTOM);
    }
    if (z > 0)
        voxomap::addFace(*this, this->findVoxel(x, y, z - 1), FaceEnum::BACK);
    else
    {
        auto tmp = node.findNode(node.getX(), node.getY(), node.getZ() - Area::NB_VOXELS, Area::NB_VOXELS);
        if (tmp && tmp->_area)
            voxomap::addFace(*tmp->_area, tmp->_area->findVoxel(x, y, Area::NB_VOXELS - 1), FaceEnum::BACK);
    }
    if (z < Area::NB_VOXELS - 1)
        voxomap::addFace(*this, this->findVoxel(x, y, z + 1), FaceEnum::FRONT);
    else
    {
        auto tmp = node.findNode(node.getX(), node.getY(), node.getZ() + Area::NB_VOXELS, Area::NB_VOXELS);
        if (tmp && tmp->_area)
            voxomap::addFace(*tmp->_area, tmp->_area->findVoxel(x, y, 0), FaceEnum::FRONT);
    }
}

template <template <class...> class T_Area, class T_Voxel>
void FaceArea<T_Area, T_Voxel>::removeFace(VoxelNode<FaceArea>& node, VoxelData& voxel, uint8_t x, uint8_t y, uint8_t z)
{
    if (x > 0)
        voxomap::removeFace(*this, *this, &voxel, this->findVoxel(x - 1, y, z), FaceEnum::LEFT, FaceEnum::RIGHT);
    else
    {
        auto tmp = node.findNode(node.getX() - Area::NB_VOXELS, node.getY(), node.getZ(), Area::NB_VOXELS);
        if (tmp && tmp->_area)
            voxomap::removeFace(*this, *tmp->_area, &voxel, tmp->_area->findVoxel(Area::NB_VOXELS - 1, y, z), FaceEnum::LEFT, FaceEnum::RIGHT);
    }
    if (x < Area::NB_VOXELS - 1)
        voxomap::removeFace(*this, *this, &voxel, this->findVoxel(x + 1, y, z), FaceEnum::RIGHT, FaceEnum::LEFT);
    else
    {
        auto tmp = node.findNode(node.getX() + Area::NB_VOXELS, node.getY(), node.getZ(), Area::NB_VOXELS);
        if (tmp && tmp->_area)
            voxomap::removeFace(*this, *tmp->_area, &voxel, tmp->_area->findVoxel(0, y, z), FaceEnum::RIGHT, FaceEnum::LEFT);
    }
    if (y > 0)
        voxomap::removeFace(*this, *this, &voxel, this->findVoxel(x, y - 1, z), FaceEnum::BOTTOM, FaceEnum::TOP);
    else
    {
        auto tmp = node.findNode(node.getX(), node.getY() - Area::NB_VOXELS, node.getZ(), Area::NB_VOXELS);
        if (tmp && tmp->_area)
            voxomap::removeFace(*this, *tmp->_area, &voxel, tmp->_area->findVoxel(x, Area::NB_VOXELS - 1, z), FaceEnum::BOTTOM, FaceEnum::TOP);
    }
    if (y < Area::NB_VOXELS - 1)
        voxomap::removeFace(*this, *this, &voxel, this->findVoxel(x, y + 1, z), FaceEnum::TOP, FaceEnum::BOTTOM);
    else
    {
        auto tmp = node.findNode(node.getX(), node.getY() + Area::NB_VOXELS, node.getZ(), Area::NB_VOXELS);
        if (tmp && tmp->_area)
            voxomap::removeFace(*this, *tmp->_area, &voxel, tmp->_area->findVoxel(x, 0, z), FaceEnum::TOP, FaceEnum::BOTTOM);
    }
    if (z > 0)
        voxomap::removeFace(*this, *this, &voxel, this->findVoxel(x, y, z - 1), FaceEnum::FRONT, FaceEnum::BACK);
    else
    {
        auto tmp = node.findNode(node.getX(), node.getY(), node.getZ() - Area::NB_VOXELS, Area::NB_VOXELS);
        if (tmp && tmp->_area)
            voxomap::removeFace(*this, *tmp->_area, &voxel, tmp->_area->findVoxel(x, y, Area::NB_VOXELS - 1), FaceEnum::FRONT, FaceEnum::BACK);
    }
    if (z < Area::NB_VOXELS - 1)
        voxomap::removeFace(*this, *this, &voxel, this->findVoxel(x, y, z + 1), FaceEnum::BACK, FaceEnum::FRONT);
    else
    {
        auto tmp = node.findNode(node.getX(), node.getY(), node.getZ() + Area::NB_VOXELS, Area::NB_VOXELS);
        if (tmp && tmp->_area)
            voxomap::removeFace(*this, *tmp->_area, &voxel, tmp->_area->findVoxel(x, y, 0), FaceEnum::BACK, FaceEnum::FRONT);
    }
}

template <template <class...> class T_Area, class T_Voxel>
void FaceArea<T_Area, T_Voxel>::updateFace(VoxelNode<FaceArea>& node, VoxelData& voxel, uint8_t x, uint8_t y, uint8_t z)
{
    if (x > 0)
        voxomap::updateFace(*this, *this, &voxel, this->findVoxel(x - 1, y, z), FaceEnum::LEFT, FaceEnum::RIGHT);
    else
    {
        auto tmp = node.findNode(node.getX() - Area::NB_VOXELS, node.getY(), node.getZ(), Area::NB_VOXELS);
        if (tmp && tmp->_area)
            voxomap::updateFace(*this, *tmp->_area, &voxel, tmp->_area->findVoxel(Area::NB_VOXELS - 1, y, z), FaceEnum::LEFT, FaceEnum::RIGHT);
    }
    if (x < Area::NB_VOXELS - 1)
        voxomap::updateFace(*this, *this, &voxel, this->findVoxel(x + 1, y, z), FaceEnum::RIGHT, FaceEnum::LEFT);
    else
    {
        auto tmp = node.findNode(node.getX() + Area::NB_VOXELS, node.getY(), node.getZ(), Area::NB_VOXELS);
        if (tmp && tmp->_area)
            voxomap::updateFace(*this, *tmp->_area, &voxel, tmp->_area->findVoxel(0, y, z), FaceEnum::RIGHT, FaceEnum::LEFT);
    }
    if (y > 0)
        voxomap::updateFace(*this, *this, &voxel, this->findVoxel(x, y - 1, z), FaceEnum::BOTTOM, FaceEnum::TOP);
    else
    {
        auto tmp = node.findNode(node.getX(), node.getY() - Area::NB_VOXELS, node.getZ(), Area::NB_VOXELS);
        if (tmp && tmp->_area)
            voxomap::updateFace(*this, *tmp->_area, &voxel, tmp->_area->findVoxel(x, Area::NB_VOXELS - 1, z), FaceEnum::BOTTOM, FaceEnum::TOP);
    }
    if (y < Area::NB_VOXELS - 1)
        voxomap::updateFace(*this, *this, &voxel, this->findVoxel(x, y + 1, z), FaceEnum::TOP, FaceEnum::BOTTOM);
    else
    {
        auto tmp = node.findNode(node.getX(), node.getY() + Area::NB_VOXELS, node.getZ(), Area::NB_VOXELS);
        if (tmp && tmp->_area)
            voxomap::updateFace(*this, *tmp->_area, &voxel, tmp->_area->findVoxel(x, 0, z), FaceEnum::TOP, FaceEnum::BOTTOM);
    }
    if (z > 0)
        voxomap::updateFace(*this, *this, &voxel, this->findVoxel(x, y, z - 1), FaceEnum::FRONT, FaceEnum::BACK);
    else
    {
        auto tmp = node.findNode(node.getX(), node.getY(), node.getZ() - Area::NB_VOXELS, Area::NB_VOXELS);
        if (tmp && tmp->_area)
            voxomap::updateFace(*this, *tmp->_area, &voxel, tmp->_area->findVoxel(x, y, Area::NB_VOXELS - 1), FaceEnum::FRONT, FaceEnum::BACK);
    }
    if (z < Area::NB_VOXELS - 1)
        voxomap::updateFace(*this, *this, &voxel, this->findVoxel(x, y, z + 1), FaceEnum::BACK, FaceEnum::FRONT);
    else
    {
        auto tmp = node.findNode(node.getX(), node.getY(), node.getZ() + Area::NB_VOXELS, Area::NB_VOXELS);
        if (tmp && tmp->_area)
            voxomap::updateFace(*this, *tmp->_area, &voxel, tmp->_area->findVoxel(x, y, 0), FaceEnum::BACK, FaceEnum::FRONT);
    }
}

}