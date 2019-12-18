namespace voxomap
{

template <class T_Area>
iterator<T_Area>& iterator<T_Area>::operator++()
{
    if (!this->voxel || !this->node)
        return *this;

    ++this->z;
    if (!this->findNextVoxel(*this->node))
    {
        this->x = 0;
        this->y = 0;
        this->z = 0;
        this->voxel = nullptr;
        this->findNextParentNode(*this->node);
    }

    return *this;
}

template <class T_Area>
iterator<T_Area> iterator<T_Area>::operator++(int)
{
    iterator result = *this;
    ++(*this);
    return result;
}

template <class T_Area>
iterator<T_Area>* iterator<T_Area>::operator*()
{
    return this;
}

template <class T_Area>
iterator<T_Area>::operator bool() const
{
    return this->voxel != nullptr && this->node != nullptr;
}

template <class T_Area>
T_Area& iterator<T_Area>::getArea() const
{
    return *this->node->_area;
}

template <class T_Area>
void iterator<T_Area>::begin(VoxelNode<T_Area>& node)
{
    this->findNextChildNode(node);
}

template <class T_Area>
void iterator<T_Area>::end(VoxelNode<T_Area>& node)
{
    this->findNextParentNode(node);
}

template <class T_Area>
void iterator<T_Area>::findNextParentNode(VoxelNode<T_Area>& node)
{
    auto parent = node.getParent();
    if (parent == nullptr)
    {
        this->node = nullptr;
        return;
    }

    for (uint8_t i = node.getChildId() + 1; i < 8; ++i)
    {
        if (!parent->getChildren()[i])
            continue;
        if (this->findNextChildNode(*parent->getChildren()[i]))
            return;
    }

    this->findNextParentNode(*parent);
}

template <class T_Area>
bool iterator<T_Area>::findNextChildNode(VoxelNode<T_Area>& node)
{
    if (node.getSize() == T_Area::NB_VOXELS)
    {
        if (this->findNextVoxel(node))
            return true;
    }
    else
    {
        for (uint8_t i = 0; i < 8; ++i)
        {
            auto child = node.getChildren()[i];
            if (child)
            {
                if (this->findNextChildNode(*child))
                    return true;
            }
        }
    }
    return false;
}

template <class T_Area>
bool iterator<T_Area>::findNextVoxel(VoxelNode<T_Area>& node)
{
    if (!node.hasVoxel())
        return false;

    for (uint8_t x = this->x; x < T_Area::NB_VOXELS; ++x)
    {
        for (uint8_t y = this->y; y < T_Area::NB_VOXELS; ++y)
        {
            for (uint8_t z = this->z; z < T_Area::NB_VOXELS; ++z)
            {
                this->voxel = node._area->findVoxel(x, y, z);
                if (this->voxel)
                {
                    this->x = x;
                    this->y = y;
                    this->z = z;
                    this->node = &node;
                    return true;
                }
            }
            this->z = 0;
        }
        this->y = 0;
    }
    return false;
}

}