namespace voxomap
{

template <class T_Voxel>
const typename ArrayArea<T_Voxel>::VoxelData ArrayArea<T_Voxel>::_emptyArea[NB_VOXELS][NB_VOXELS][NB_VOXELS];

template <class T_Voxel>
inline ArrayArea<T_Voxel>::ArrayArea()
{
    std::memcpy(this->area, _emptyArea, sizeof(_emptyArea));
}

template <class T_Voxel>
inline uint16_t ArrayArea<T_Voxel>::getNbVoxel() const
{
    return nbVoxels;
}

template <class T_Voxel>
inline typename ArrayArea<T_Voxel>::VoxelData* ArrayArea<T_Voxel>::getVoxel(uint8_t x, uint8_t y, uint8_t z)
{
    return (this->area[x][y][z]) ? &this->area[x][y][z] : nullptr;
}

template <class T_Voxel>
inline typename ArrayArea<T_Voxel>::VoxelData const* ArrayArea<T_Voxel>::getVoxel(uint8_t x, uint8_t y, uint8_t z) const
{
    return (this->area[x][y][z]) ? &this->area[x][y][z] : nullptr;
}

template <class T_Voxel>
template <typename T_Area, typename... Args>
typename ArrayArea<T_Voxel>::VoxelData* ArrayArea<T_Voxel>::addVoxel(VoxelNode<T_Area>& node, uint8_t x, uint8_t y, uint8_t z, Args&&... args)
{
    VoxelData& voxel = this->area[x][y][z];

    if (voxel)
        return nullptr;
    
    new (&voxel) VoxelData(std::forward<Args>(args)...);
    ++nbVoxels;
    return &voxel;
}

template <class T_Voxel>
template <typename T_Area, typename... Args>
typename ArrayArea<T_Voxel>::VoxelData* ArrayArea<T_Voxel>::updateVoxel(VoxelNode<T_Area>& node, uint8_t x, uint8_t y, uint8_t z, Args&&... args)
{
    VoxelData& voxel = this->area[x][y][z];

    if (!voxel)
        return nullptr;
    new (&voxel) VoxelData(std::forward<Args>(args)...);
    return &voxel;
}

template <class T_Voxel>
template <typename T_Area, typename... Args>
typename ArrayArea<T_Voxel>::VoxelData* ArrayArea<T_Voxel>::putVoxel(VoxelNode<T_Area>& node, uint8_t x, uint8_t y, uint8_t z, Args&&... args)
{
    if (this->area[x][y][z])
        return this->updateVoxel(node, x, y, z, std::forward<Args>(args)...);
    else
        return this->addVoxel(node, x, y, z, std::forward<Args>(args)...);
}

template <class T_Voxel>
template <typename T_Area>
bool ArrayArea<T_Voxel>::removeVoxel(VoxelNode<T_Area>& node, uint8_t x, uint8_t y, uint8_t z)
{
    VoxelData& voxel = this->area[x][y][z];

    if (!voxel)
        return false;
    --nbVoxels;
    new (&voxel) VoxelData();
    return true;
}

template <class T_Voxel>
template <typename T_Area>
bool ArrayArea<T_Voxel>::removeVoxel(VoxelNode<T_Area>& node, uint8_t x, uint8_t y, uint8_t z, VoxelData& data)
{
    VoxelData& voxel = this->area[x][y][z];

    if (!data)
        return false;
    data = voxel;
    --nbVoxels;
    new (&voxel) VoxelData();
    return true;
}

template <class T_Voxel>
inline void ArrayArea<T_Voxel>::serialize(std::string& str) const
{
    str.append(reinterpret_cast<char const*>(this), sizeof(*this));
}

template <class T_Voxel>
size_t ArrayArea<T_Voxel>::unserialize(char const* str, size_t size)
{
    if (size < sizeof(*this))
        return 0;
    std::memcpy(this, str, sizeof(*this));
    return sizeof(*this);
}

}