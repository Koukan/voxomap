namespace voxomap
{

template <class T_Voxel, template<class...> class T_Container>
inline uint16_t SparseContainer<T_Voxel, T_Container>::getNbVoxel() const
{
    return _sparseArray.getNbData();
}

template <class T_Voxel, template<class...> class T_Container>
inline bool SparseContainer<T_Voxel, T_Container>::hasVoxel(uint8_t x) const
{
    return _sparseArray.hasData(x);
}

template <class T_Voxel, template<class...> class T_Container>
inline bool SparseContainer<T_Voxel, T_Container>::hasVoxel(uint8_t x, uint8_t y) const
{
    return _sparseArray.hasData(x, y);
}

template <class T_Voxel, template<class...> class T_Container>
inline bool SparseContainer<T_Voxel, T_Container>::hasVoxel(uint8_t x, uint8_t y, uint8_t z) const
{
    return _sparseArray.hasData(x, y, z);
}

template <class T_Voxel, template<class...> class T_Container>
inline T_Voxel* SparseContainer<T_Voxel, T_Container>::findVoxel(uint8_t x, uint8_t y, uint8_t z)
{
    return _sparseArray.findData(x, y, z);
}

template <class T_Voxel, template<class...> class T_Container>
inline T_Voxel const* SparseContainer<T_Voxel, T_Container>::findVoxel(uint8_t x, uint8_t y, uint8_t z) const
{
    return _sparseArray.findData(x, y, z);
}

template <class T_Voxel, template<class...> class T_Container>
template <typename Iterator>
inline T_Voxel* SparseContainer<T_Voxel, T_Container>::findVoxel(Iterator& it)
{
    it.voxelContainer = static_cast<decltype(it.voxelContainer)>(this);
    return this->findVoxel(it.x, it.y, it.z);
}

template <class T_Voxel, template<class...> class T_Container>
template <typename Iterator>
inline T_Voxel const* SparseContainer<T_Voxel, T_Container>::findVoxel(Iterator& it) const
{
    it.voxelContainer = static_cast<decltype(it.voxelContainer)>(const_cast<SparseContainer<T_Voxel, T_Container>*>(this));
    return this->findVoxel(it.x, it.y, it.z);
}

template <class T_Voxel, template<class...> class T_Container>
template <typename Iterator, typename... Args>
inline bool SparseContainer<T_Voxel, T_Container>::addVoxel(Iterator& it, Args&&... args)
{
    it.voxelContainer = static_cast<decltype(it.voxelContainer)>(const_cast<SparseContainer<T_Voxel>*>(this));
    return _sparseArray.addData(it.x, it.y, it.z, it.voxel, std::forward<Args>(args)...);
}

template <class T_Voxel, template<class...> class T_Container>
template <typename Iterator, typename... Args>
inline bool SparseContainer<T_Voxel, T_Container>::updateVoxel(Iterator& it, Args&&... args)
{
    it.voxelContainer = static_cast<decltype(it.voxelContainer)>(const_cast<SparseContainer<T_Voxel>*>(this));
    return _sparseArray.updateData(it.x, it.y, it.z, it.voxel, std::forward<Args>(args)...);
}

template <class T_Voxel, template<class...> class T_Container>
template <typename Iterator, typename... Args>
inline void SparseContainer<T_Voxel, T_Container>::putVoxel(Iterator& it, Args&&... args)
{
    it.voxelContainer = static_cast<decltype(it.voxelContainer)>(const_cast<SparseContainer<T_Voxel>*>(this));
    _sparseArray.putData(it.x, it.y, it.z, it.voxel, std::forward<Args>(args)...);
}

template <class T_Voxel, template<class...> class T_Container>
template <typename Iterator>
inline bool SparseContainer<T_Voxel, T_Container>::removeVoxel(Iterator const& it, VoxelData* voxel)
{
    return _sparseArray.removeData(it.x, it.y, it.z, voxel);
}

template <class T_Voxel, template<class...> class T_Container>
template <typename Iterator>
void SparseContainer<T_Voxel, T_Container>::exploreVoxel(Iterator& it, std::function<void(Iterator const&)> const& predicate) const
{
    for (it.x = 0; it.x < NB_VOXELS; ++it.x)
    {
        if (!this->hasVoxel(it.x))
            continue;

        for (it.y = 0; it.y < NB_VOXELS; ++it.y)
        {
            if (!this->hasVoxel(it.x, it.y))
                continue;

            for (it.z = 0; it.z < NB_VOXELS; ++it.z)
            {
                it.voxel = const_cast<T_Voxel*>(this->findVoxel(it.x, it.y, it.z));
                if (it.voxel)
                    predicate(it);
            }
        }
    }
}

template <class T_Voxel, template<class...> class T_Container>
inline void SparseContainer<T_Voxel, T_Container>::serialize(std::string& str) const
{
    _sparseArray.serialize(str);
}

template <class T_Voxel, template<class...> class T_Container>
inline size_t SparseContainer<T_Voxel, T_Container>::unserialize(char const* str, size_t size)
{
    return _sparseArray.unserialize(str, size);
}

}