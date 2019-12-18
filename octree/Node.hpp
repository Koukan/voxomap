#ifndef _VOXOMAP_NODE_HPP_
#define _VOXOMAP_NODE_HPP_

#include <array>
#include <memory>

namespace voxomap
{

template <typename T_Node> class Octree;

/*!
    \brief Basic node of Octree
*/
template <typename T_Node>
class Node
{
public:
    /*!
        \brief Constructs Node with its properties
    */
    Node(int x, int y, int z, int size);
    Node(Node const& other);
    ~Node();

    /*!
        \brief Returns size of node
    */
    inline int              getSize() const;
    /*!
        \brief Returns x position of the node
    */
    inline int              getX() const;
    /*!
        \brief Returns y position of the node
    */
    inline int              getY() const;
    /*!
        \brief Returns z position of the node
    */
    inline int              getZ() const;
    /*!
        \brief Returns the number of children, [0, 8]
    */
    inline int              getNbChildren() const;

    inline std::array<T_Node*, 8> const& getChildren() const;

    inline T_Node*          getParent() const;

    inline Octree<T_Node>*  getOctree() const;

    inline bool             empty() const;

    inline bool             operator==(Node const& other) const;

    inline bool             operator!=(Node const& other) const;

    void                    merge(T_Node const& other);

    T_Node*                 findNode(int x, int y, int z, int size) const;

    inline T_Node*          getChild(int x, int y, int z) const;

    inline int              getChildPos(int x, int y, int z) const;

    inline uint8_t          getChildId() const;

    template <typename T>
    inline bool             isInside(T x, T y, T z, T sx, T sy, T sz) const;
    template <typename T>
    inline bool             isInside(T x, T y, T z) const;
    template <typename T>
    inline bool             isInside(T x, T y, T z, T size) const;
    inline bool             isInside(T_Node& node) const;

protected:
    std::array<T_Node*, 8>  _children;
    T_Node*                 _parent = nullptr;
    Octree<T_Node>*         _octree = nullptr;
    int                     _x = 0;
    int                     _y = 0;
    int                     _z = 0;
    int                     _size = 0;
    uint8_t                 _childId = 0;
    uint8_t                 _nbChildren = 0;
    friend Octree<T_Node>;
};

}

#include "Node.ipp"

#endif // _VOXOMAP_NODE_HPP_
