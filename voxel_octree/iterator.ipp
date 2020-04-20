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
bool iterator<T_Area>::operator==(iterator const& other) const
{
    return this->node == other.node && this->voxel == other.voxel && this->x == other.x
           && this->y == other.y && this->z == other.z;
}

template <class T_Area>
bool iterator<T_Area>::operator!=(iterator const& other) const
{
    return !(*this == other);
}

template <class T_Area>
T_Area* iterator<T_Area>::getArea() const
{
    return this->node->getVoxelArea();
}

template <class T_Area>
void iterator<T_Area>::begin(VoxelNode<T_Area>& i_node)
{
    this->findNextChildNode(i_node);
}

template <class T_Area>
void iterator<T_Area>::end(VoxelNode<T_Area>& i_node)
{
    this->findNextParentNode(i_node);
}

template <class T_Area>
void iterator<T_Area>::findNextParentNode(VoxelNode<T_Area>& i_node)
{
    auto parent = i_node.getParent();
    if (parent == nullptr)
    {
        this->node = nullptr;
        return;
    }

    for (uint8_t i = i_node.getChildId() + 1; i < 8; ++i)
    {
        if (!parent->getChildren()[i])
            continue;
        if (this->findNextChildNode(*parent->getChildren()[i]))
            return;
    }

    this->findNextParentNode(*parent);
}

template <class T_Area>
bool iterator<T_Area>::findNextChildNode(VoxelNode<T_Area>& i_node)
{
    if (i_node.getSize() == T_Area::NB_VOXELS)
    {
        if (this->findNextVoxel(i_node))
            return true;
    }
    else
    {
        for (uint8_t i = 0; i < 8; ++i)
        {
            auto child = i_node.getChildren()[i];
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
bool iterator<T_Area>::findNextVoxel(VoxelNode<T_Area>& i_node)
{
    if (!i_node.hasVoxel())
        return false;

    for (uint8_t ix = this->x; ix < T_Area::NB_VOXELS; ++ix)
    {
        for (uint8_t iy = this->y; iy < T_Area::NB_VOXELS; ++iy)
        {
            for (uint8_t iz = this->z; iz < T_Area::NB_VOXELS; ++iz)
            {
                this->voxel = i_node.getVoxelArea()->findVoxel(ix, iy, iz);
                if (this->voxel)
                {
                    this->x = ix;
                    this->y = iy;
                    this->z = iz;
                    this->node = &i_node;
                    return true;
                }
            }
            this->z = 0;
        }
        this->y = 0;
    }
    return false;
}

} // namespace voxomap
