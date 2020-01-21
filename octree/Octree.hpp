#ifndef _VOXOMAP_OCTREE_HPP_
#define _VOXOMAP_OCTREE_HPP_

#include <cstdint>
#include <memory>
#include <vector>
#include <assert.h>

namespace voxomap
{

/*!
    \brief Octree container
*/
template <class T_Node>
class Octree
{
public:
    using Node = T_Node;

    /*!
        \brief Default constructor
    */
    Octree();
    Octree(Octree const& other);
    Octree(Octree&& other);
    virtual ~Octree() = default;
    Octree& operator=(Octree const& other);
    Octree& operator=(Octree&& other);

    /*!
        \brief Pushes \a node into the octree
    */
    virtual T_Node* push(T_Node& node);
    /*!
        \brief Removes \a node from the octree
    */
    virtual std::unique_ptr<T_Node> pop(T_Node& node);
    /*!
        \brief Search the node that corresponds to the parameters
        \param x X coordinate of the node
        \param y Y coordinate of the node
        \param z Z coordinate of the node
        \param size Size of the node
        \return Pointer to the node if it exists otherwise nullptr
    */
    T_Node*         findNode(int x, int y, int z, int size) const;

    /*!
        \brief Clear the octree
        Removes all nodes and all elements.
    */
    virtual void    clear();

    /*!
        \brief Getter of the root node
        \return The root node
    */
    T_Node*         getRootNode() const;

protected:
    uint8_t         findNodeNb(T_Node const& node) const;

    // Node method
    void            setChild(T_Node& parent, T_Node& child);
    void            setChild(T_Node& parent, T_Node& child, uint8_t childId);
    void            removeParent(T_Node& child);
    T_Node*         removeChild(T_Node& parent, uint8_t id);
    T_Node*         findParentNode(T_Node& parent, T_Node& node, uint8_t& childId) const;
    T_Node*         push(T_Node& parent, T_Node& node);
    T_Node*         push(T_Node& parent, T_Node& child, uint8_t childId);
    void            insertNode(T_Node& child, T_Node& newChild);
    void            merge(T_Node& currentNode, T_Node& newNode);

    std::unique_ptr<T_Node> _rootNode;    //!< Main node of the octree
};

}

#include "Octree.ipp"

#endif // _VOXOMAP_OCTREE_HPP_