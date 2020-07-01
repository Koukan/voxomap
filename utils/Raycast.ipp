namespace voxomap
{

static int const gl_raycast_index[][9] = {
    { 7, 3, 6, 5, 2, 1, 4, 0, SideEnum::XPOS | SideEnum::YPOS | SideEnum::ZPOS }, // Right Top Back
    { 6, 2, 7, 4, 3, 0, 5, 1, SideEnum::XNEG | SideEnum::YPOS | SideEnum::ZPOS }, // Left Top Back
    { 5, 1, 7, 4, 3, 6, 0, 2, SideEnum::XPOS | SideEnum::YNEG | SideEnum::ZPOS }, // Right Bottom Back
    { 4, 5, 6, 0, 7, 2, 1, 3, SideEnum::XNEG | SideEnum::YNEG | SideEnum::ZPOS }, // Left Bottom Back
    { 3, 1, 2, 7, 0, 5, 6, 4, SideEnum::XPOS | SideEnum::YPOS | SideEnum::ZNEG }, // Right Top Front
    { 2, 0, 6, 3, 1, 7, 4, 5, SideEnum::XNEG | SideEnum::YPOS | SideEnum::ZNEG }, // Left Top Front
    { 1, 0, 5, 3, 2, 7, 4, 6, SideEnum::XPOS | SideEnum::YNEG | SideEnum::ZNEG }, // Right Bottom Front
    { 0, 1, 2, 4, 3, 6, 5, 7, SideEnum::XNEG | SideEnum::YNEG | SideEnum::ZNEG }  // Left Bottom Front
};

inline static int orderId(Vector3D const& direction)
{
    return (direction.x > 0) | ((direction.y > 0) << 1) | ((direction.z > 0) << 2);
}

template <class T_Container>
inline bool Raycast<T_Container>::Result::operator<(Result const& other) const
{
    return this->distance < other.distance;
}

template <class T_Container>
inline bool Raycast<T_Container>::Result::operator>(Result const& other) const
{
    return other < *this;
}

inline static double intersection(Vector3D const& corner, size_t size, Ray const& ray, SideEnum& side, SideEnum sideToCheck)
{
    double distance;

    if (sideToCheck & SideEnum::YPOS)
    {
        distance = ray.intersectPlane(Vector3D(corner.x, corner.y + size, corner.z), Vector3D(corner.x + size, corner.y + size, corner.z), Vector3D(corner + size));
        if (distance != -1)
        {
            side = SideEnum::YPOS;
            return distance;
        }
    }
    if (sideToCheck & SideEnum::YNEG)
    {
        distance = ray.intersectPlane(Vector3D(corner), Vector3D(corner.x + size, corner.y, corner.z), Vector3D(corner.x + size, corner.y, corner.z + size));
        if (distance != -1)
        {
            side = SideEnum::YNEG;
            return distance;
        }
    }
    if (sideToCheck & SideEnum::XNEG)
    {
        distance = ray.intersectPlane(Vector3D(corner), Vector3D(corner.x, corner.y + size, corner.z), Vector3D(corner.x, corner.y + size, corner.z + size));
        if (distance != -1)
        {
            side = SideEnum::XNEG;
            return distance;
        }
    }
    if (sideToCheck & SideEnum::XPOS)
    {
        distance = ray.intersectPlane(Vector3D(corner.x + size, corner.y, corner.z), Vector3D(corner.x + size, corner.y + size, corner.z), Vector3D(corner + size));
        if (distance != -1)
        {
            side = SideEnum::XPOS;
            return distance;
        }
    }
    if (sideToCheck & SideEnum::ZNEG)
    {
        distance = ray.intersectPlane(Vector3D(corner), Vector3D(corner.x + size, corner.y, corner.z), Vector3D(corner.x + size, corner.y + size, corner.z));
        if (distance != -1)
        {
            side = SideEnum::ZNEG;
            return distance;
        }
    }
    if (sideToCheck & SideEnum::ZPOS)
    {
        distance = ray.intersectPlane(Vector3D(corner.x, corner.y, corner.z + size), Vector3D(corner.x + size, corner.y, corner.z + size), Vector3D(corner + size));
        if (distance != -1)
        {
            side = SideEnum::ZPOS;
            return distance;
        }
    }
    return -1;
}

inline static double getDistance(Vector3I const& boxPosition, size_t boxSize, Ray const& ray, double distance, SideEnum& side, SideEnum checkSide)
{
    auto corner2 = boxPosition + boxSize;

    if (ray.src.x >= boxPosition.x && ray.src.x <= corner2.x &&
        ray.src.y >= boxPosition.y && ray.src.y <= corner2.y &&
        ray.src.z >= boxPosition.z && ray.src.z <= corner2.z)
        return 0;

    if ((ray.src.x < boxPosition.x && ray.dir.x < 0) || (ray.src.x > corner2.x && ray.dir.x > 0) ||
        (ray.src.y < boxPosition.y && ray.dir.y < 0) || (ray.src.y > corner2.y && ray.dir.y > 0) ||
        (ray.src.z < boxPosition.z && ray.dir.z < 0) || (ray.src.z > corner2.z && ray.dir.z > 0))
        return -1;

    double tmp = intersection(boxPosition, boxSize, ray, side, checkSide);
    return (tmp != -1 && (distance == -1 || tmp < distance)) ? tmp : -1;
}

template <typename, typename = void>
struct has_side : std::false_type {};

template <typename T>
struct has_side<T, std::void_t<decltype(&T::getSide)>> : std::is_same<uint8_t, decltype(std::declval<T>().getSide())>
{};

template <class T_Voxel>
typename std::enable_if<has_side<T_Voxel>::value, SideEnum>::type getSideToCheck(SideEnum sideToCheck, T_Voxel& voxel)
{
    return static_cast<SideEnum>(sideToCheck & ~voxel.getSide());
}

template <class T_Voxel>
typename std::enable_if<!has_side<T_Voxel>::value, SideEnum>::type getSideToCheck(SideEnum sideToCheck, T_Voxel&)
{
    return sideToCheck;
}

template <class T_Container>
bool Raycast<T_Container>::raycastVoxel(iterator& it, T_VoxelContainer const& container)
{
    auto voxel = container.findVoxel(it);
    if (!voxel)
        return false;
    it.voxel = const_cast<typename T_Container::VoxelData*>(voxel);

    SideEnum side;
    Vector3I corner;
    it.getVoxelPosition(corner.x, corner.y, corner.z);

    double computedDistance = getDistance(corner, 1, this->ray, this->maxDistance, side, getSideToCheck(_sideToCheck, *voxel));
    if (computedDistance == -1)
        return false;

    if (this->predicate && !this->predicate(it))
        return false;

    this->result.side = side;
    this->result.distance = std::sqrt(computedDistance);
    this->result.position = this->ray.src + this->ray.dir * this->result.distance;
    this->result.it = it;
    return true;
}

template <class T_Container>
bool Raycast<T_Container>::raycastContainer(iterator& it, T_VoxelContainer const& container, Vector3I const& boxPosition, size_t boxSize)
{
    if (boxSize == 1)
    {
        it.x = T_Node::findVoxelPosition(boxPosition.x);
        it.y = T_Node::findVoxelPosition(boxPosition.y);
        it.z = T_Node::findVoxelPosition(boxPosition.z);
        return this->raycastVoxel(it, container);
    }
    if (boxSize != T_Container::NB_VOXELS)
    {
        //if (_cache && !_cache->hasVoxel(node, boxPosition, boxSize))
        //    return false;

        Vector3I corner(it.node->getX() + boxPosition.x, it.node->getY() + boxPosition.y, it.node->getZ() + boxPosition.z);
        SideEnum side;
        if (getDistance(corner, boxSize, this->ray, this->maxDistance, side, _sideToCheck) == -1)
            return false;
    }

    boxSize /= 2;
    for (int i = 0; i < 8; ++i)
    {
        int index = gl_raycast_index[_sortingIndex][i];
        Vector3I offset(index & 1, (index >> 1) & 1, (index >> 2) & 1);
        offset = offset * boxSize + boxPosition;
        if (this->raycastContainer(it, container, offset, boxSize))
            return true;
    }
    return false;
}

template <class T_Container>
template <typename T>
typename std::enable_if<(T::NB_SUPERCONTAINER != 0), bool>::type Raycast<T_Container>::raycastContainer(iterator& it, T const& container, Vector3I const& boxPosition, size_t boxSize)
{
    if (boxSize == T::Container::NB_VOXELS)
    {
        uint8_t sx = T_Node::findContainerPosition(boxPosition.x, T::NB_SUPERCONTAINER);
        uint8_t sy = T_Node::findContainerPosition(boxPosition.y, T::NB_SUPERCONTAINER);
        uint8_t sz = T_Node::findContainerPosition(boxPosition.z, T::NB_SUPERCONTAINER);
        auto sub_container = container.findContainer(sx, sy, sz);
        if (!sub_container)
            return false;

        std::get<0>(it.container_position[T::SUPERCONTAINER_ID]) = sx;
        std::get<1>(it.container_position[T::SUPERCONTAINER_ID]) = sy;
        std::get<2>(it.container_position[T::SUPERCONTAINER_ID]) = sz;
        return this->raycastContainer(it, *sub_container, boxPosition, boxSize);
    }
    if (boxSize != T_Container::NB_VOXELS)
    {
        Vector3I corner(it.node->getX() + boxPosition.x, it.node->getY() + boxPosition.y, it.node->getZ() + boxPosition.z);
        SideEnum side;
        if (getDistance(corner, boxSize, this->ray, this->maxDistance, side, _sideToCheck) == -1)
            return false;
    }

    boxSize /= 2;
    for (int i = 0; i < 8; ++i)
    {
        int index = gl_raycast_index[_sortingIndex][i];
        Vector3I offset(index & 1, (index >> 1) & 1, (index >> 2) & 1);
        offset = offset * boxSize + boxPosition;
        if (this->raycastContainer(it, container, offset, boxSize))
            return true;
    }
    return false;
}

template <class T_Container>
bool Raycast<T_Container>::raycast(T_Node const& node)
{
    if (node.getSize() == T_Container::NB_VOXELS)
    {
        if (!node.hasVoxel())
            return false;
        iterator it;
        it.node = const_cast<T_Node*>(&node);
        return this->raycastContainer(it, *node.getVoxelContainer(), Vector3I(0, 0, 0), T_Container::NB_VOXELS);
    }
    
    SideEnum side;
    for (int i = 0; i < 8; ++i)
    {
        auto child = static_cast<T_Node*>(node.getChildren()[gl_raycast_index[_sortingIndex][i]]);
        if (child)
        {
            auto corner = Vector3I(child->getX(), child->getY(), child->getZ());
            if (getDistance(corner, child->getSize(), this->ray, this->maxDistance, side, _sideToCheck) != -1 &&
                this->execute(*child))
                return true;
        }
    }
    return false;
}

template <class T_Container>
inline bool Raycast<T_Container>::execute(T_Node const& node)
{
    _sortingIndex = orderId(this->ray.dir);
    _sideToCheck = static_cast<SideEnum>(gl_raycast_index[_sortingIndex][8]);
    return this->raycast(node);
}

template <class T_Container>
inline typename Raycast<T_Container>::Result Raycast<T_Container>::get(Ray const& ray, T_Node const& node, Predicate const& predicate, double maxDistance)
{
    Raycast raycast;

    raycast.ray = ray;
    raycast.predicate = predicate;
    raycast.maxDistance = (maxDistance > 0) ? maxDistance * maxDistance : -1;
    raycast.execute(node);
    return raycast.result;
}

template <class T_Container>
inline typename Raycast<T_Container>::Result Raycast<T_Container>::get(Ray const& ray, T_Node const& node, double maxDistance)
{
    return Raycast::get(ray, node, nullptr, maxDistance);
}

template <class T_Container>
inline typename Raycast<T_Container>::Result Raycast<T_Container>::get(Ray const& ray, T_Octree const& octree, Predicate const& predicate, double maxDistance)
{
    if (octree.getRootNode())
        return Raycast::get(ray, *octree.getRootNode(), predicate, maxDistance);
    return Raycast::Result();
}

template <class T_Container>
inline typename Raycast<T_Container>::Result Raycast<T_Container>::get(Ray const& ray, T_Octree const& octree, double maxDistance)
{
    if (octree.getRootNode())
        return Raycast::get(ray, *octree.getRootNode(), maxDistance);
    return Raycast::Result();
}

template <class T_Container>
inline typename Raycast<T_Container>::Result Raycast<T_Container>::get(Ray const& ray, T_Node const& node, Cache& cache, Predicate const& predicate, double maxDistance)
{
    Raycast raycast;

    raycast.ray = ray;
    raycast.predicate = predicate;
    raycast.maxDistance = (maxDistance > 0) ? maxDistance * maxDistance : -1;
    raycast._cache = &cache;
    raycast.execute(node);
    return raycast.result;
}

template <class T_Container>
inline typename Raycast<T_Container>::Result Raycast<T_Container>::get(Ray const& ray, T_Node const& node, Cache& cache, double maxDistance)
{
    return Raycast::get(ray, node, cache, nullptr, maxDistance);
}

template <class T_Container>
inline typename Raycast<T_Container>::Result Raycast<T_Container>::get(Ray const& ray, T_Octree const& octree, Cache& cache, Predicate const& predicate, double maxDistance)
{
    if (octree.getRootNode())
        return Raycast::get(ray, *octree.getRootNode(), cache, predicate, maxDistance);
    return Raycast::Result();
}

template <class T_Container>
inline typename Raycast<T_Container>::Result Raycast<T_Container>::get(Ray const& ray, T_Octree const& octree, Cache& cache, double maxDistance)
{
    if (octree.getRootNode())
        return Raycast::get(ray, *octree.getRootNode(), cache, maxDistance);
    return Raycast::Result();
}

template <class T_Container>
inline typename Raycast<T_Container>::Result Raycast<T_Container>::get(Ray const& ray, T_Node const* const* nodes, size_t nbNode, Cache& cache, Predicate const& predicate, double maxDistance)
{
    Raycast raycast;

    raycast.ray = ray;
    raycast.predicate = predicate;
    raycast.maxDistance = (maxDistance > 0) ? maxDistance * maxDistance : -1;
    raycast._cache = &cache;
    for (size_t i = 0; i < nbNode; ++i)
        raycast.execute(*nodes[i]);
    return raycast.result;
}


// Cache
template <class T_Container>
static bool hasVoxel(VoxelNode<T_Container> const& node, uint8_t bx, uint8_t by, uint8_t bz, uint8_t boxSize)
{
    for (uint8_t x = 0; x < boxSize; ++x)
    {
        for (uint8_t y = 0; y < boxSize; ++y)
        {
            for (uint8_t z = 0; z < boxSize; ++z)
            {
                if (node.getVoxelArea()->hasVoxel(x + bx, y + by, z + bz))
                    return true;
            }
        }
    }
    return false;
}

template <class T_Container>
bool Raycast<T_Container>::Cache::hasVoxel(T_Node const& node, Vector3I const& boxPosition, size_t boxSize) const
{
    auto it = _nodeCache.find(&node);

    if (it == _nodeCache.end())
        return false;
    if (boxSize == 2)
        return it->second.hasVoxelIn2.test(boxPosition.x / 2 + (boxPosition.y / 2) * 4 + (boxPosition.z / 2) * 16);
    return it->second.hasVoxelIn4.test(boxPosition.x / 4 + (boxPosition.y / 4) * 2 + (boxPosition.z / 4) * 4);
}

template <class T_Container>
bool Raycast<T_Container>::Cache::fillHasVoxel(T_Node const& node, Vector3I const& boxPosition, size_t boxSize)
{
    auto pair = _nodeCache.emplace(&node, NodeCache{});

    if (pair.second)
        this->fillCache(node, pair.first->second);
    if (boxSize == 2)
        return pair.first->second.hasVoxelIn2.test(boxPosition.x / 2 + (boxPosition.y / 2) * 4 + (boxPosition.z / 2) * 16);
    return pair.first->second.hasVoxelIn4.test(boxPosition.x / 4 + (boxPosition.y / 4) * 2 + (boxPosition.z / 4) * 4);
}

template <class T_Container>
void Raycast<T_Container>::Cache::fillCache(T_Node const& node, NodeCache& cache)
{
    for (uint8_t x = 0; x < T_Container::NB_VOXELS; x += 2)
    {
        for (uint8_t y = 0; y < T_Container::NB_VOXELS; y += 2)
        {
            for (uint8_t z = 0; z < T_Container::NB_VOXELS; z += 2)
            {
                if (voxomap::hasVoxel(node, x, y, z, 2))
                {
                    cache.hasVoxelIn2.set(x / 2 + (y / 2) * 4 + (z / 2) * 16, true);
                    cache.hasVoxelIn4.set(x / 4 + (y / 4) * 2 + (z / 4) * 4, true);
                }
            }
        }
    }
}

template <class T_Container>
void Raycast<T_Container>::Cache::fillCache(T_Node const& node)
{
    if (node.getVoxelContainer())
    {
        auto& cache = _nodeCache[&node];
        this->fillCache(node, cache);
    }
    else
    {
        for (auto child : node.getChildren())
        {
            if (child)
                this->fillCache(*child);
        }
    }
}

template <class T_Container>
void Raycast<T_Container>::Cache::fillCache(T_Octree const& octree)
{
    this->fillCache(*octree.getRootNode());
}

} // End namespace voxomap