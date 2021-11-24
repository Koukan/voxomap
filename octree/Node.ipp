#include <cstring>

namespace voxomap
{

template <class T_Node>
Node<T_Node>::Node(int x, int y, int z, uint32_t size)
        : _x(x), _y(y), _z(z), _size(size)
{
    std::memset(_children.data(), 0, sizeof(_children));
}

template <class T_Node>
Node<T_Node>::Node(Node const& other)
    : _x(other._x), _y(other._y), _z(other._z), _size(other._size)
    , _childId(other._childId), _nbChildren(other._nbChildren)
{
    std::memset(_children.data(), 0, sizeof(_children));

    for (size_t i = 0; i < 8; ++i)
    {
        if (other._children[i])
        {
#ifdef CACHEALLOCATOR
            _children[i] = CacheFriendlyAllocator<T_Node>::allocate(static_cast<T_Node*>(this), *other._children[i]);
#else
            _children[i] = new T_Node(*other._children[i]);
#endif
            _children[i]->_parent = static_cast<T_Node*>(this);
            _children[i]->_octree = _octree;
        }
    }
}

template <class T_Node>
Node<T_Node>::~Node()
{
    if (_parent && _parent->_children[_childId] == this)
        _parent->_children[_childId] = nullptr;
    for (size_t i = 0; i < 8; ++i)
    {
        if (_children[i])
        {
            _children[i]->_parent = nullptr;
#ifdef CACHEALLOCATOR
            CacheFriendlyAllocator<T_Node>::deallocate(_children[i]);
#else
            delete _children[i];
#endif

        }
    }
}

template <class T_Node>
inline uint32_t Node<T_Node>::getSize() const
{
    return _size;
}

template <class T_Node>
inline int Node<T_Node>::getX() const
{
    return _x;
}

template <class T_Node>
inline int Node<T_Node>::getY() const
{
    return _y;
}

template <class T_Node>
inline int Node<T_Node>::getZ() const
{
    return _z;
}

template <class T_Node>
inline int Node<T_Node>::getNbChildren() const
{
    return _nbChildren;
}

template <class T_Node>
inline std::array<T_Node*, 8> const& Node<T_Node>::getChildren() const
{
    return _children;
}

template <class T_Node>
inline T_Node* Node<T_Node>::getParent() const
{
    return _parent;
}

template <class T_Node>
inline Octree<T_Node>* Node<T_Node>::getOctree() const
{
    return _octree;
}

template <class T_Node>
inline bool Node<T_Node>::empty() const
{
    return _nbChildren == 0;
}

template <class T_Node>
inline bool Node<T_Node>::operator==(Node const& other) const
{
    return _x == other._x && _y == other._y && _z == other._z && _size == other._size;
}

template <class T_Node>
inline bool Node<T_Node>::operator!=(Node const& other) const
{
    return !(*this == other);
}

template <class T_Node>
void Node<T_Node>::merge(T_Node const&)
{
}

template <class T_Node>
T_Node* Node<T_Node>::findNode(int x, int y, int z, uint32_t size) const
{
    T_Node* tmp = static_cast<T_Node*>(const_cast<Node*>(this));

    if (size == 0)
        return nullptr;
    if (tmp->_x == x && tmp->_y == y && tmp->_z == z && tmp->_size == size)
        return tmp;
    while (tmp && tmp->_size != 0 && !tmp->isInside(x, y, z))
        tmp = tmp->_parent;
    while (tmp)
    {
        if (tmp->_size == size)
        {
            if (tmp->_x == x && tmp->_y == y && tmp->_z == z)
                return tmp;
            return nullptr;
        }
        tmp = tmp->getChild(x, y, z);
    }
    return nullptr;
}

template <class T_Node>
inline T_Node* Node<T_Node>::getChild(int x, int y, int z) const
{
    int i = this->getChildPos(x, y, z);
    return _children[i];
}

template <class T_Node>
inline int Node<T_Node>::getChildPos(int x, int y, int z) const
{
    if (Node<T_Node>::isNegPosRootNode(_x, _size))
        return ((x >> 31) & 1) | (((y >> 31) & 1) << 1) | (((z >> 31) & 1) << 2);

#ifndef NO_INTRINSIC
    #ifdef _WIN32
        int shift = _tzcnt_u32(_size >> 1);
    #else
        int shift = __builtin_ctz(_size >> 1);
    #endif
    return ((x >> shift) & 1) | (((y >> shift) & 1) << 1) | (((z >> shift) & 1) << 2);
#else
    int size = _size >> 1;
    return (x & size) / size | (((y & size) / size) << 1) | (((z & size) / size) << 2);
#endif
}

template <class T_Node>
inline uint8_t Node<T_Node>::getChildId() const
{
    return _childId;
}


template <class T_Node>
template <typename T, typename T_Size>
inline bool Node<T_Node>::isInside(T x, T y, T z, T_Size sx, T_Size sy, T_Size sz) const
{
    return (x >= _x && static_cast<uint32_t>(x + sx - _x) <= _size &&
        y >= _y && static_cast<uint32_t>(y + sy - _y) <= _size &&
        z >= _z && static_cast<uint32_t>(z + sz - _z) <= _size);
}

template <class T_Node>
template <typename T>
inline bool Node<T_Node>::isInside(T x, T y, T z) const
{
    return (x >= _x && static_cast<uint32_t>(x - _x) < _size &&
        y >= _y && static_cast<uint32_t>(y - _y) < _size &&
        z >= _z && static_cast<uint32_t>(z - _z) < _size);
}

template <class T_Node>
template <typename T, typename T_Size>
inline bool Node<T_Node>::isInside(T x, T y, T z, T_Size size) const
{
    return this->isInside(x, y, z, size, size, size);
}

template <class T_Node>
inline bool Node<T_Node>::isInside(T_Node& node) const
{
    return this->isInside(node._x, node._y, node._z, node._size, node._size, node._size);
}

template <class T_Node>
void Node<T_Node>::changeOctree(Octree<T_Node>& octree)
{
    _octree = &octree;

    for (auto child : _children)
    {
        if (child)
            child->changeOctree(octree);
    }
}

template <class T_Node>
inline T_Node* Node<T_Node>::getFirstChild() const
{
    for (auto child : _children)
    {
        if (child)
            return child;
    }
    return nullptr;
}

template <class T_Node>
bool Node<T_Node>::isNegPosRootNode(int x, uint32_t size)
{
    return x < 0 && static_cast<decltype(size)>(-x) < size;
}

}
