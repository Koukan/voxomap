#ifndef _VOXOMAP_OCTREE_HPP_
#define _VOXOMAP_OCTREE_HPP_

#include <cstdint>
#include <memory>
#include <vector>
#include <assert.h>

namespace voxomap
{

/*!
    \defgroup Octree Octree
    Classes use for define the octree
*/

/*! \class Octree
    \ingroup Octree
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
    /*!
        \brief Copy constructor
    */
    Octree(Octree const& other);
    /*!
        \brief Move constructor
    */
    Octree(Octree&& other);
    /*!
        \brief Default virtual destructor
    */
    virtual ~Octree() = default;
    /*!
        \brief Assignement operator
        \param other Right operand
        \return Reference to \a this
    */
    Octree& operator=(Octree const& other);
    /*!
        \brief Assignement move operator
        \param other Right operand
        \return Reference to \a this
    */
    Octree& operator=(Octree&& other);

    /*!
        \brief Pushes \a node into the octree
        \param node Node to push
    */
    virtual T_Node* push(T_Node& node);
    /*!
        \brief Removes \a node from the octree
        \return 
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
    // Node method
    /*!
        \brief Set \a child as child of \a parent
        \param parent Parent node
        \param child Child node
    */
    void            setChild(T_Node& parent, T_Node& child);
    /*!
        \brief Set \a child as child of \a parent
        \param parent Parent node
        \param child Child node
        \param childId Id of the child inside parent's children array
    */
    void            setChild(T_Node& parent, T_Node& child, uint8_t childId);
    /*!
        \brief Remove \a child from its parent and so from the octree
    */
    void            removeFromParent(T_Node& child);
    /*!
        \brief Remove the child with \a id from the \a parent node
        \return The removed node
    */
    T_Node*         removeChild(T_Node& parent, uint8_t id);
    /*!
        \brief Find node inside \a parent that can contain \a node
        \param parent Parent node
        \param node The new node
        \param childId Id of the node inside the found parent
        \return The found parent node, nullptr if not exist
    */
    T_Node*         findParentNode(T_Node& parent, T_Node& node, uint8_t& childId) const;
    /*!
        \brief Push \a node inside \a parent
        \param parent Parent node
        \param node Node to push
        \return Node added, can be different than \a node if a similar node already exist
    */
    T_Node*         push(T_Node& parent, T_Node& node);
    /*!
       \brief Push \a node inside \a parent
       \param parent Parent node
       \param child Child node
       \param childId Id of the child inside parent's children array
       \return Node added, can be different than \a node if a similar node already exist
    */
    T_Node*         push(T_Node& parent, T_Node& child, uint8_t childId);
    /*!
        \brief Create an intermediate node that can contain \a child and \a newChild and push it into octree
    */
    void            insertIntermediateNode(T_Node& child, T_Node& newChild);
    /*!
        \brief Merge two nodes
        \param currentNode Node already present in the octree
        \param newNode Node to merge inside
    */
    void            merge(T_Node& currentNode, T_Node& newNode);
    /*!
        \brief Remove useless intermediate node, intermediate node with only one child
        \param node The intermediate node
    */
    void            removeUselessIntermediateNode(T_Node& node);
    /*!
        \brief Compute the new coordinates and size of the NegPosRootNode
    */
    void            recomputeNegPosRootNode();
    /*!
        \brief Called when \a node is remove from the octree
    */
    virtual void    notifyNodeRemoving(T_Node& node);

    std::unique_ptr<T_Node> _rootNode;    //!< Main node of the octree
};

}

#include "Octree.ipp"

#endif // _VOXOMAP_OCTREE_HPP_