#ifndef _VOXOMAP_ITERATOR_HPP_
#define _VOXOMAP_ITERATOR_HPP_

#include <cstdint>

namespace voxomap
{

template <typename T_Area> class VoxelNode;

/*! \struct iterator
    \ingroup VoxelOctree
    \brief Iterator on VoxelOctree
*/
template <typename T_Area>
struct iterator
{
    using VoxelData = typename T_Area::VoxelData;

    VoxelNode<T_Area>* node = nullptr;  //!< Pointer on node
    VoxelData* voxel = nullptr;         //!< Pointer on voxel
    uint8_t x = 0;                      //!< x coordinate of \a voxel inside \a node
    uint8_t y = 0;                      //!< y coordinate of \a voxel inside \a node
    uint8_t z = 0;                      //!< z coordinate of \a voxel inside \a node

    /*!
        \brief Pre-increment the iterator
        \return Reference on \a this
    */
    iterator& operator++();
    /*!
        \brief Post increment the iterator
        \return Not incremented iterator
    */
    iterator operator++(int);
    /*!
        \brief Dereference iterator
    */
    iterator* operator*();
    /*!
        \brief Returns true if the iterator is valid
    */
    operator bool() const;
    /*!
        \brief Returns true if iterator are equal
    */
    bool operator==(iterator const& other) const;
    /*!
        \brief Returns true if iterator are different
    */
    bool operator!=(iterator const& other) const;
    /*!
        \brief Returns reference on area inside \a node
    */
    T_Area* getArea() const;

protected:
    /*!
        \brief Initialize \a this with the first voxel of \a node
    */
    void begin(VoxelNode<T_Area>& node);
    /*!
        \brief Initialize \a this with the next element after the last voxel of \a node
    */
    void end(VoxelNode<T_Area>& node);
    friend VoxelNode<T_Area>;

    /*!
        \brief Find the next voxel inside the parent node of \a node
    */
    void findNextParentNode(VoxelNode<T_Area>& node);
    /*!
        \brief Find the next voxel inside the next child node of \a node
        \return True if a voxel is found
    */
    bool findNextChildNode(VoxelNode<T_Area>& node);
    /*!
        \brief Find the next voxel inside the area of \a node
        \return True if a voxel is found
    */
    bool findNextVoxel(VoxelNode<T_Area>& node);
};

}

#include "iterator.ipp"

#endif // _VOXOMAP_ITERATOR_HPP_
