#ifndef _VOXOMAP_FACEAREA_HPP_
#define _VOXOMAP_FACEAREA_HPP_

#include <cstdint>
#include <utility>
#include <type_traits>
#include "VoxelNode.hpp"
#include "iterator.hpp"

namespace voxomap
{

enum FaceEnum : uint8_t
{
    TOP = 1,        //!< Top of the voxel
    BOTTOM = 2,        //!< Bottom of the voxel
    LEFT = 4,        //!< Left of the voxel
    RIGHT = 8,        //!< Right of the voxel
    FRONT = 16,        //!< Front of the voxel
    BACK = 32,        //!< Back of the voxel
    ALL = TOP | BOTTOM | LEFT | RIGHT | FRONT | BACK
};

template <class T_Voxel>
struct FaceVoxel : T_Voxel
{
    FaceVoxel() = default;
    template <typename T, typename = typename std::enable_if<!std::is_class<T>::value, int>::type>
    FaceVoxel(T arg);
    template <typename T, typename = typename std::enable_if<std::is_class<T>::value, int>::type>
    FaceVoxel(T const& arg);
    template <typename T, typename U, typename... Args>
    FaceVoxel(T&& arg_1, U&& arg_2, Args&&... args);
    FaceVoxel(FaceVoxel const& other) = default;
    FaceVoxel(FaceVoxel&& other) = default;
    template <typename... Args>
    FaceVoxel(FaceEnum face, Args&&... args);

    FaceVoxel& operator=(FaceVoxel const& other) = default;
    FaceVoxel& operator=(FaceVoxel&& other) = default;

    operator    bool() const;
    bool        operator==(FaceVoxel const& other) const;
    bool        operator!=(FaceVoxel const& other) const;
    bool        operator==(T_Voxel const& other) const;
    bool        operator!=(T_Voxel const& other) const;
    bool        operator==(uint8_t f) const;
    bool        operator!=(uint8_t f) const;
    uint8_t     operator&(uint8_t f) const;
    uint8_t     operator|(uint8_t f) const;
    uint8_t     operator&(FaceEnum f) const;
    uint8_t     operator|(FaceEnum f) const;
    FaceVoxel&  operator&=(uint8_t f);
    FaceVoxel&  operator|=(uint8_t f);
    uint8_t     getFace() const;

private:
    uint8_t _voxel_face = 0;
};

template <template <class...> class T_Area, class T_Voxel>
struct FaceArea : public T_Area<FaceVoxel<T_Voxel>>
{
    using VoxelData = FaceVoxel<T_Voxel>;
    using Area = T_Area<VoxelData>;
    using iterator = iterator<FaceArea<T_Area, T_Voxel>>;

    FaceArea() = default;
    FaceArea(FaceArea const& other) = default;
    FaceArea(FaceArea&& other) = default;

    void                init(VoxelNode<FaceArea> const&) {}
    inline uint16_t     getNbFace() const;

    template <typename Iterator, typename... Args>
    bool                addVoxel(Iterator& it, Args&&... args);
    template <typename Iterator, typename... Args>
    bool                updateVoxel(Iterator& it, Args&&... args);
    template <typename Iterator, typename... Args>
    void                putVoxel(Iterator& it, Args&&... args);
    template <typename Iterator>
    Iterator            removeVoxel(Iterator it, VoxelData* voxel = nullptr);

    void                serialize(std::string& str) const;
    size_t              unserialize(char const* str, size_t size);

private:
    // Face management
    template <typename... Args>
    bool updateVoxel(VoxelNode<FaceArea>& node, VoxelData& voxel, uint8_t x, uint8_t y, uint8_t z, Args&&... args);
    void addFace(VoxelNode<FaceArea>& node, uint8_t x, uint8_t y, uint8_t z);
    void removeFace(VoxelNode<FaceArea>& node, VoxelData& voxel, uint8_t x, uint8_t y, uint8_t z);
    void updateFace(VoxelNode<FaceArea>& node, VoxelData& voxel, uint8_t x, uint8_t y, uint8_t z);
    template <class Area_T> friend void addFace(Area_T& area, typename Area_T::VoxelData* voxel, FaceEnum face);
    template <class Area_T> friend void removeFace(Area_T& a1, Area_T& a2, typename Area_T::VoxelData* v1, typename Area_T::VoxelData* v2, FaceEnum f1, FaceEnum f2);
    template <class Area_T> friend void updateFace(Area_T& a1, Area_T& a2, typename Area_T::VoxelData* v1, typename Area_T::VoxelData* v2, FaceEnum f1, FaceEnum f2);

    uint16_t    _nbFaces = 0;
};

}

#include "FaceArea.ipp"

#endif // _VOXOMAP_FACEAREA_HPP_
