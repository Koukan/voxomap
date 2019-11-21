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
template <typename Iterator, typename... Args>
bool ArrayArea<T_Voxel>::addVoxel(Iterator& it, Args&&... args)
{
    VoxelData& voxel = this->area[it.x][it.y][it.z];

    it.voxel = &voxel;
    if (voxel)
        return false;
    new (&voxel) VoxelData(std::forward<Args>(args)...);
    ++nbVoxels;
    return true;
}

template <class T_Voxel>
template <typename Iterator, typename... Args>
bool ArrayArea<T_Voxel>::updateVoxel(Iterator& it, Args&&... args)
{
    VoxelData& voxel = this->area[it.x][it.y][it.z];

    if (!voxel)
        return false;
    new (&voxel) VoxelData(std::forward<Args>(args)...);
    it.voxel = &voxel;
    return true;
}

template <class T_Voxel>
template <typename Iterator, typename... Args>
bool ArrayArea<T_Voxel>::putVoxel(Iterator& it, Args&&... args)
{
    if (this->area[it.x][it.y][it.z])
        return this->updateVoxel(it, std::forward<Args>(args)...);
    else
        return this->addVoxel(it, std::forward<Args>(args)...);
}

template <class T_Voxel>
template <typename Iterator>
bool ArrayArea<T_Voxel>::removeVoxel(Iterator& it, VoxelData* return_voxel)
{
    VoxelData& voxel = this->area[it.x][it.y][it.z];

    if (!voxel)
        return false;
    if (return_voxel)
        *return_voxel = voxel;
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