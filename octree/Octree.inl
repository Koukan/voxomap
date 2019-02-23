namespace voxomap
{

template <class T_Node>
Octree<T_Node>::Octree()
	: _rootNode(new T_Node(0, 0, 0, 0))
{
    _rootNode->_octree = this;
}

template <class T_Node>
Octree<T_Node>::Octree(Octree const& other)
	: _objectNodeSize(other._objectNodeSize)
{
    _rootNode.reset(new T_Node(*other._rootNode));
}

template <class T_Node>
uint8_t Octree<T_Node>::findNodeNb(T_Node const& node) const
{
	uint8_t signx = node._x >> 31 & 1;
	uint8_t signy = node._y >> 31 & 1;
	uint8_t signz = node._z >> 31 & 1;

	return signx + 2 * signy + 4 * signz;
}

template <class T_Node>
T_Node* Octree<T_Node>::findNode(int x, int y, int z, int size) const
{
    T_Node* tmp = this->_rootNode.get();

	do
	{
		tmp = tmp->getChild(x, y, z);
	} while (tmp && tmp->_size > size);
	if (tmp && tmp->_x == x && tmp->_y == y && tmp->_z == z && tmp->_size == size)
		return tmp;
	return nullptr;
}

template <class T_Node>
void Octree<T_Node>::clear()
{
	_rootNode.reset(new T_Node(0, 0, 0, 0));
}

template <class T_Node>
T_Node* Octree<T_Node>::getRootNode() const
{
	return _rootNode.get();
}

template <class T_Node>
unsigned int Octree<T_Node>::getObjectNodeSize() const
{
	return _objectNodeSize;
}

template <class T_Node>
void Octree<T_Node>::setObjectNodeSize(unsigned int size)
{
	_objectNodeSize = size;
}

template <class T_Node>
T_Node* Octree<T_Node>::push(T_Node& node)
{
	uint8_t nodeNb = this->findNodeNb(node);
    T_Node* tmp = _rootNode->_children[nodeNb];
    T_Node* ret = &node;

	if (tmp == nullptr)
	{
		this->setChild(*_rootNode, node, nodeNb);
	}
	else if (*tmp == node)
	{
		this->merge(*tmp, node);
		ret = tmp;
	}
    else if (tmp->isInside(node))
    {
        ret = this->push(*tmp, node);
    }
	else if (node.isInside(*tmp))
	{
        this->setChild(*_rootNode, node, nodeNb);
		tmp->_parent = nullptr;
		this->push(node, *tmp);
	}
    else
    {
        this->insertNode(*tmp, node);
    }
	ret->_octree = this;
	return ret;
}

template <class T_Node>
std::unique_ptr<T_Node> Octree<T_Node>::pop(T_Node& node)
{
	if (node._parent == nullptr)
		return nullptr;

	auto parent = node._parent;
	this->removeParent(node);
	if (parent->empty())
		this->pop(*parent);

	return std::unique_ptr<T_Node>(&node);
}


// Node method
template <class T_Node>
inline void	Octree<T_Node>::setChild(T_Node& parent, T_Node& child)
{
    int		i = parent.getChildPos(child._x, child._y, child._z);

    if (i >= 0 && i <= 7)
        this->setChild(parent, child, i);
}

template <class T_Node>
void Octree<T_Node>::setChild(T_Node& parent, T_Node& child, uint8_t childId)
{
    this->removeParent(child);
    if (!parent._children[childId])
        parent._nbChildren++;
    else
        parent._children[childId]->_parent = nullptr;
    parent._children[childId] = &child;
    child._parent = &parent;
    child._childId = childId;
}

template <class T_Node>
inline void Octree<T_Node>::removeParent(T_Node& child)
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

    while (tmp && (tmp->isInside(node) || tmp->_size == 0))
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
        this->setChild(parent, child, childId);
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
        this->insertNode(*parent._children[childId], child);
    return &child;
}

template <class T_Node>
void Octree<T_Node>::insertNode(T_Node& child, T_Node& newChild)
{
    T_Node& parent = *new T_Node(child._x, child._y, child._z, child._size);
	parent._octree = this;
    int& x = parent._x;
    int& y = parent._y;
    int& z = parent._z;
    int& size = parent._size;

    do {
        size <<= 1;
        x = x & ~(size - 1);
        y = y & ~(size - 1);
        z = z & ~(size - 1);
    } while (!parent.isInside(newChild));

    this->setChild(*child._parent, parent, child._childId);
    this->setChild(parent, child);
	this->setChild(parent, newChild);
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
                this->insertNode(*currentNode._children[i], *newNode._children[i]);
            newNode._children[i] = nullptr;
            --newNode._nbChildren;
        }
    }
}

}