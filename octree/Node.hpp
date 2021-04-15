#ifndef _VOXOMAP_NODE_HPP_
#define _VOXOMAP_NODE_HPP_

#include <array>
#include <memory>

namespace voxomap
{

template <typename T_Node> class Octree;

/*! \class Node
    \ingroup Octree
    \brief Basic node of Octree
*/
template <typename T_Node>
class Node
{
public:
    /*!
        \brief Constructs Node with its properties
        \param x X coordinate
        \param y Y coordinate
        \param z Z coordinate
        \param size Size of the node
    */
    Node(int x, int y, int z, uint32_t size);
    /*!
        \brief Copy constructor
    */
    Node(Node const& other);
    /*!
        \brief Destructor method
    */
    virtual ~Node();

    /*!
        \brief Returns size of node
    */
    inline uint32_t         getSize() const;
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
    /*!
        \brief Returns the children array
    */
    inline std::array<T_Node*, 8> const& getChildren() const;
    /*!
        \brief Returns the parent node
    */
    inline T_Node*          getParent() const;
    /*!
        \brief Returns the octree
    */
    inline Octree<T_Node>*  getOctree() const;
    /*!
        \brief Check if node has children
        \return True if there is no children
    */
    inline bool             empty() const;
    /*!
        \brief Compare coordinate and size of the node
        \return True if \a this is similar to \a other
    */
    inline bool             operator==(Node const& other) const;
    /*!
        \brief Compare coordinate and size of the node
        \return True if \a this is different to \a other
    */
    inline bool             operator!=(Node const& other) const;

    /*!
        \brief Merge method of the node, do nothing
    */
    void                    merge(T_Node const& other);

    /*!
        \brief Find node
        \param x X coordinate
        \param y Y coordinate
        \param z Z coordinate
        \param size Size of the node
        \return True if \a this is different to \a other
    */
    T_Node*                 findNode(int x, int y, int z, uint32_t size) const;

    /*!
        \brief Get the child node that can contain the point at coordinate xyz
        \param x X coordinate
        \param y Y coordinate
        \param z Z coordinate
        \return The child node, nullptr if doesn't exist
    */
    inline T_Node*          getChild(int x, int y, int z) const;
    /*!
        \brief Get the child node position that can contain the point at coordinate xyz
        \param x X coordinate
        \param y Y coordinate
        \param z Z coordinate
        \return The index of children
    */
    inline int              getChildPos(int x, int y, int z) const;
    /*!
        \return The child id of \a this, inside the children array of the parent
    */
    inline uint8_t          getChildId() const;

    /*!
        \brief Check if box is inside the node
        \param x X coordinate of the box
        \param y Y coordinate of the box
        \param z Z coordinate of the box
        \param sx Size on x axis of the box
        \param sy Size on y axis of the box
        \param sz Size on z axis of the box
        \return True if the box is inside
    */
    template <typename T, typename T_Size>
    inline bool             isInside(T x, T y, T z, T_Size sx, T_Size sy, T_Size sz) const;
    /*!
        \brief Check if point is inside the node
        \param x X coordinate
        \param y Y coordinate
        \param z Z coordinate
        \return True if the point is inside
    */
    template <typename T>
    inline bool             isInside(T x, T y, T z) const;
    /*!
        \brief Check if box is inside the node
        \param x X coordinate
        \param y Y coordinate
        \param z Z coordinate
        \param size Size of the box
        \return True if the box is inside
    */
    template <typename T, typename T_Size>
    inline bool             isInside(T x, T y, T z, T_Size size) const;
    /*!
        \brief Check if node is inside the node
        \param node The node
        \return True if the node can be inside \a this
    */
    inline bool             isInside(T_Node& node) const;

protected:
    /*!
        \brief Change octree of the node and all its children
        \param octree Reference to the octree
    */
    void                    changeOctree(Octree<T_Node>& octree);
    /*!
        \brief Return the first child found in the children array
    */
    T_Node*                 getFirstChild() const;
    /*!
        \brief Is the root node that contains multi sign nodes
    */
    bool                    isNegPosRootNode() const;

    std::array<T_Node*, 8>  _children;          //!< Array of children
    T_Node*                 _parent = nullptr;  //!< Parent node
    Octree<T_Node>*         _octree = nullptr;  //!< Parent octree
    int                     _x = 0;             //!< X coordinate
    int                     _y = 0;             //!< Y coordinate
    int                     _z = 0;             //!< Z coordinate
    uint32_t                _size = 0;          //!< Size of the node
    uint8_t                 _childId = 0;       //!< Child id inside parent's children array
    uint8_t                 _nbChildren = 0;    //!< Number of node inside the children array
    friend Octree<T_Node>;
};

}

#include "Node.ipp"

#endif // _VOXOMAP_NODE_HPP_
