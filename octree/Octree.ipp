namespace voxomap
{

template <class T_Node>
Octree<T_Node>::Octree()
{
    this->clear();
}

template <class T_Node>
Octree<T_Node>::Octree(Octree const& other)
{
    if (other._rootNode)
    {
        _rootNode.reset(new T_Node(*other._rootNode));
        _rootNode->changeOctree(*this);
    }
    else
        _rootNode = nullptr;
}

template <class T_Node>
Octree<T_Node>::Octree(Octree&& other)
    : _rootNode(std::move(other._rootNode))
{
    if (_rootNode)
        _rootNode->changeOctree(*this);
}

template <class T_Node>
Octree<T_Node>& Octree<T_Node>::operator=(Octree const& other)
{
    if (other._rootNode)
    {
        _rootNode.reset(new T_Node(*other._rootNode));
        _rootNode->changeOctree(*this);
    }
    else
        _rootNode = nullptr;
    return *this;
}

template <class T_Node>
Octree<T_Node>& Octree<T_Node>::operator=(Octree&& other)
{
    _rootNode = std::move(other._rootNode);
    if (_rootNode)
        _rootNode->changeOctree(*this);
    return *this;
}

template <class T_Node>
T_Node* Octree<T_Node>::findNode(int x, int y, int z, int size) const
{
    if (_rootNode)
    {
        T_Node* tmp = _rootNode.get();

        while (tmp && tmp->_size > size)
            tmp = tmp->getChild(x, y, z);
        if (tmp && tmp->_x == x && tmp->_y == y && tmp->_z == z && tmp->_size == size)
            return tmp;
    }
    return nullptr;
}

template <class T_Node>
void Octree<T_Node>::clear()
{
    _rootNode = nullptr;
}

template <class T_Node>
T_Node* Octree<T_Node>::getRootNode() const
{
    return _rootNode.get();
}

template <class T_Node>
T_Node* Octree<T_Node>::push(T_Node& node)
{
    if (!_rootNode)
    {
        _rootNode.reset(&node);
        node._octree = this;
        return &node;
    }
    if (*_rootNode == node)
    {
        this->merge(*_rootNode, node);
        return _rootNode.get();
    }
    if (_rootNode->isInside(node))
    {
        return this->push(*_rootNode, node);
    }
    if (node.isInside(*_rootNode))
    {
        auto child = _rootNode.release();
        _rootNode.reset(&node);
        this->push(node, *child);
    }
    else
        this->insertIntermediateNode(*_rootNode, node);
    return &node;
}

template <class T_Node>
std::unique_ptr<T_Node> Octree<T_Node>::pop(T_Node& node)
{
    assert(node._octree == this && "Node does not belong to this octree.");
    if (node._parent == nullptr)
    {
        assert(&node == _rootNode.get() && "Node has no parent and is not the root node.");
        return std::move(_rootNode);
    }

    auto parent = node._parent;
    this->removeFromParent(node);
    this->notifyNodeRemoving(node);
    if (parent->empty())
        this->pop(*parent);
    else if (parent->getNbChildren() == 1)
        this->removeUselessIntermediateNode(*parent);
    else if (parent->isNegPosRootNode())
        this->recomputeNegPosRootNode();

    return std::unique_ptr<T_Node>(&node);
}


// Node method
template <class T_Node>
inline void Octree<T_Node>::setChild(T_Node& parent, T_Node& child)
{
    int i = parent.getChildPos(child._x, child._y, child._z);

    if (i >= 0 && i <= 7)
        this->setChild(parent, child, i);
}

template <class T_Node>
void Octree<T_Node>::setChild(T_Node& parent, T_Node& child, uint8_t childId)
{
    this->removeFromParent(child);
    if (!parent._children[childId])
        parent._nbChildren++;
    else
        parent._children[childId]->_parent = nullptr;
    parent._children[childId] = &child;
    child._parent = &parent;
    child._childId = childId;
    child._octree = parent._octree;
}

template <class T_Node>
inline void Octree<T_Node>::removeFromParent(T_Node& child)
{
    if (child._parent != nullptr)
        this->removeChild(*child._parent, child._childId);
}

template <class T_Node>
T_Node* Octree<T_Node>::removeChild(T_Node& parent, uint8_t id)
{
    if (!parent._children[id])
        return nullptr;
    auto child = parent._children[id];
    parent._nbChildren--;
    parent._children[id] = nullptr;
    child->_parent = nullptr;
    return child;
}

template <class T_Node>
T_Node* Octree<T_Node>::findParentNode(T_Node& parent, T_Node& node, uint8_t& childId) const
{
    T_Node* tmp = const_cast<T_Node*>(&parent);
    T_Node* prev = nullptr;

    while (tmp && tmp->isInside(node))
    {
        prev = tmp;
        childId = tmp->getChildPos(node._x, node._y, node._z);
        tmp = tmp->_children[childId];
    }
    return prev;
}

template <class T_Node>
T_Node* Octree<T_Node>::push(T_Node& parent, T_Node& node)
{
    uint8_t childId = 0;
    auto tmp = this->findParentNode(parent, node, childId);

    return (tmp) ? this->push(*tmp, node, childId) : nullptr;
}

template <class T_Node>
T_Node* Octree<T_Node>::push(T_Node& parent, T_Node& child, uint8_t childId)
{
    if (parent._children[childId] == nullptr)
    {
        this->setChild(parent, child, childId);
    }
    else if (*parent._children[childId] == child)
    {
        this->merge(*parent._children[childId], child);
        return parent._children[childId];
    }
    else if (child.isInside(*parent._children[childId]))
    {
        this->push(child, *parent._children[childId]);
        this->setChild(parent, child, childId);
    }
    else
        this->insertIntermediateNode(*parent._children[childId], child);
    return &child;
}

template <class T_Node>
void Octree<T_Node>::insertIntermediateNode(T_Node& child, T_Node& newChild)
{
    // child is a NegPosRootNode, we have to enlarge it
    if (child.isNegPosRootNode())
    {
        assert(&child == _rootNode.get() && "The node should be the root node.");
        do {
            child._x <<= 1;
            child._y <<= 1;
            child._z <<= 1;
            child._size <<= 1;
        } while (!child.isInside(newChild));

        this->push(child, newChild);
        return;
    }

    T_Node* parent = new T_Node(child._x, child._y, child._z, child._size);
    parent->_octree = this;

    // If the two nodes have coordinates with different signs
    // We must create a NegPosRootNode
    if ((child._x < 0) != (newChild._x < 0) ||
        (child._y < 0) != (newChild._y < 0) ||
        (child._z < 0) != (newChild._z < 0))
    {
        parent->_x = -parent->_size;
        parent->_y = -parent->_size;
        parent->_z = -parent->_size;
        parent->_size <<= 1;
        while (!parent->isInside(child) || !parent->isInside(newChild))
        {
            parent->_x <<= 1;
            parent->_y <<= 1;
            parent->_z <<= 1;
            parent->_size <<= 1;
        }

        _rootNode.release();
        _rootNode.reset(parent);
    }
    else
    {
        // Find coordinate and size of intermediate node that can contain child and newChild
        do {
            parent->_size <<= 1;
            parent->_x = parent->_x & ~(parent->_size - 1);
            parent->_y = parent->_y & ~(parent->_size - 1);
            parent->_z = parent->_z & ~(parent->_size - 1);
        } while (!parent->isInside(newChild));

        // Insert new intermediate node in place of child
        if (child._parent)
            this->setChild(*child._parent, *parent, child._childId);
        else // child is the root node, replace it by the new intermediate node
        {
            assert(&child == _rootNode.get() && "The node should be the root node.");
            _rootNode.release();
            _rootNode.reset(parent);
        }
    }

    this->setChild(*parent, child);
    this->setChild(*parent, newChild);
}

template <class T_Node>
void Octree<T_Node>::merge(T_Node& currentNode, T_Node& newNode)
{
    currentNode.merge(newNode);

    for (int i = 0; i < 8; ++i)
    {
        if (newNode._children[i])
        {
            if (currentNode._children[i] == nullptr)
                this->setChild(currentNode, *newNode._children[i], i);
            else if (*currentNode._children[i] == *newNode._children[i])
                this->merge(*currentNode._children[i], *newNode._children[i]);
            else if (currentNode._children[i]->isInside(*newNode._children[i]))
                this->push(*currentNode._children[i], *newNode._children[i]);
            else if (newNode._children[i]->isInside(*currentNode._children[i]))
            {
                auto oldChildNode = currentNode._children[i];
                currentNode._children[i]->_parent = nullptr;
                currentNode._children[i] = nullptr;
                this->setChild(currentNode, *newNode._children[i], i);

                uint8_t childId = 0;
                auto parentNode = this->findParentNode(currentNode, *oldChildNode, childId);
                assert(!parentNode && !"Octree::merge: invalid node, missing parent.");
                auto newChildNode = parentNode->_children[childId];
                newChildNode->_parent = nullptr;
                parentNode->_children[childId] = nullptr;
                this->setChild(*parentNode, *oldChildNode, childId);
                this->push(*parentNode, *newChildNode, childId);
            }
            else
                this->insertIntermediateNode(*currentNode._children[i], *newNode._children[i]);
            newNode._children[i] = nullptr;
            --newNode._nbChildren;
        }
    }
}

template <class T_Node>
void Octree<T_Node>::removeUselessIntermediateNode(T_Node& node)
{
    assert(node._nbChildren == 1 && "Node must only have one child.");

    node._nbChildren = 0;
    if (!node.empty())
    {
        node._nbChildren = 1;
        return;
    }

    this->notifyNodeRemoving(node);
    T_Node* child = node.getFirstChild();
    child->_parent = node._parent;
    ::memset(node._children.data(), 0, sizeof(node._children));
    if (node._parent)
    {
        child->_childId = node._childId;
        node._parent->_children[child->_childId] = child;
        node._parent = nullptr;
        delete &node;
        if (child->_parent->isNegPosRootNode())
            this->recomputeNegPosRootNode();
    }
    else
    {
        assert(&node == _rootNode.get() && "Parent node has no parent and is not the root node.");
        _rootNode.reset(child);
    }
}

template <class T_Node>
void Octree<T_Node>::recomputeNegPosRootNode()
{
    static auto checkChildrenInside = [](T_Node& node) {
        for (auto child : node._children)
        {
            if (child && !node.isInside(*child))
                return false;
        }
        return true;
    };

    uint8_t nb = _rootNode->_nbChildren;
    _rootNode->_nbChildren = 0;
    if (_rootNode->empty())
    {
        // Reduce size of the node and check if all it's children are always inside it
        do
        {
            _rootNode->_x >>= 1;
            _rootNode->_y >>= 1;
            _rootNode->_z >>= 1;
            _rootNode->_size >>= 1;
        } while (checkChildrenInside(*_rootNode));

        // Reset to last values where all children are inside
        _rootNode->_x <<= 1;
        _rootNode->_y <<= 1;
        _rootNode->_z <<= 1;
        _rootNode->_size <<= 1;
    }
    _rootNode->_nbChildren = nb;
}

template <class T_Node>
void Octree<T_Node>::notifyNodeRemoving(T_Node& node)
{
}

}