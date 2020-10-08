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

/*template <typename, typename = void>
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
}*/

template <class T_Voxel>
SideEnum getSideToCheck(SideEnum sideToCheck, T_Voxel&)
{
    return sideToCheck;
}

template <class T_Container>
bool Raycast<T_Container>::raycastVoxel(iterator& it, T_VoxelContainer const& container)
{
    Vector3I corner;
    it.getVoxelPosition(corner.x, corner.y, corner.z);

    auto voxel = container.findVoxel(it);
    if (!voxel)
        return false;
    it.voxel = const_cast<typename T_Container::VoxelData*>(voxel);

    double computedDistance = getDistance(corner, 1, this->ray, this->maxDistance, this->result.side, getSideToCheck(_sideToCheck, *voxel));
    if (computedDistance == -1)
        return false;

    if (this->predicate && !this->predicate(it))
        return false;

    this->result.distance = std::sqrt(computedDistance);
    this->result.position = this->ray.src + this->ray.dir * this->result.distance;
    this->result.it = it;
    return true;
}

template <class T_Container>
bool Raycast<T_Container>::raycastContainer(iterator& it, T_VoxelContainer const& container, Vector3I const& boxPosition, int boxSize)
{
    boxSize >>= 1;
    for (int i = 0; i < 8; ++i)
    {
        int index = gl_raycast_index[_sortingIndex][i];
        Vector3I newBoxPosition(index & 1, (index >> 1) & 1, (index >> 2) & 1);
        newBoxPosition.x = newBoxPosition.x * boxSize + boxPosition.x;
        newBoxPosition.y = newBoxPosition.y * boxSize + boxPosition.y;
        newBoxPosition.z = newBoxPosition.z * boxSize + boxPosition.z;

        if (boxSize == 1)
        {
            it.x = T_Node::findVoxelPosition(newBoxPosition.x);
            it.y = T_Node::findVoxelPosition(newBoxPosition.y);
            it.z = T_Node::findVoxelPosition(newBoxPosition.z);
            if (this->raycastVoxel(it, container))
                return true;
        }
        else
        {
            if (!_cache.empty() && !_cache.hasVoxel(*it.node, newBoxPosition, boxSize))
                continue;

            if (!this->ray.intersectAABox(it.node->getX() + newBoxPosition.x,
                                          it.node->getY() + newBoxPosition.y,
                                          it.node->getZ() + newBoxPosition.z,
                                          boxSize))
                continue;

            if (this->raycastContainer(it, container, newBoxPosition, boxSize))
                return true;
        }
    }
    return false;
}

template <class T_Container>
template <typename T>
bool Raycast<T_Container>::raycastContainer(iterator& it, T const& container, Vector3I const& boxPosition, int boxSize)
{
    boxSize >>= 1;
    for (int i = 0; i < 8; ++i)
    {
        int index = gl_raycast_index[_sortingIndex][i];
        Vector3I newBoxPosition(index & 1, (index >> 1) & 1, (index >> 2) & 1);
        newBoxPosition.x = newBoxPosition.x * boxSize + boxPosition.x;
        newBoxPosition.y = newBoxPosition.y * boxSize + boxPosition.y;
        newBoxPosition.z = newBoxPosition.z * boxSize + boxPosition.z;

        if (boxSize == T::Container::NB_VOXELS)
        {
            uint8_t sx = T_Node::findContainerPosition(newBoxPosition.x, T::NB_SUPERCONTAINER);
            uint8_t sy = T_Node::findContainerPosition(newBoxPosition.y, T::NB_SUPERCONTAINER);
            uint8_t sz = T_Node::findContainerPosition(newBoxPosition.z, T::NB_SUPERCONTAINER);

            auto sub_container = container.findContainer(sx, sy, sz);
            if (!sub_container)
                continue;

            if (!this->ray.intersectAABox(it.node->getX() + newBoxPosition.x,
                                          it.node->getY() + newBoxPosition.y,
                                          it.node->getZ() + newBoxPosition.z,
                                          boxSize))
                continue;

            _cache.hasVoxel(*it.node, boxPosition, boxSize);

            std::get<0>(it.container_position[T::SUPERCONTAINER_ID]) = sx;
            std::get<1>(it.container_position[T::SUPERCONTAINER_ID]) = sy;
            std::get<2>(it.container_position[T::SUPERCONTAINER_ID]) = sz;
            if (this->raycastContainer(it, *sub_container, newBoxPosition, boxSize))
                return true;
        }
        else
        {
            if (!_cache.empty() && !_cache.hasVoxel(*it.node, newBoxPosition, boxSize))
                continue;

            if (!this->ray.intersectAABox(it.node->getX() + newBoxPosition.x,
                                          it.node->getY() + newBoxPosition.y,
                                          it.node->getZ() + newBoxPosition.z,
                                          boxSize))
                continue;

            if (this->raycastContainer(it, container, newBoxPosition, boxSize))
                return true;
        }
    }
    return false;
}

template <class T_Container>
bool Raycast<T_Container>::raycast(T_Node const& node)
{
    if (node.getVoxelContainer())
    {
        if (!node.hasVoxel())
            return false;
        iterator it;
        it.node = const_cast<T_Node*>(&node);
        return this->raycastContainer(it, *node.getVoxelContainer(), Vector3I(0, 0, 0), T_Container::NB_VOXELS);
    }
    else
    {
        for (int i = 0; i < 8; ++i)
        {
            auto child = node.getChildren()[gl_raycast_index[_sortingIndex][i]];
            if (child)
            {
                if (this->ray.intersectAABox(child->getX(), child->getY(), child->getZ(), child->getSize()) && this->raycast(*child))
                    return true;
            }
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
    raycast._cache = cache;
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
    raycast._cache = cache;
    for (size_t i = 0; i < nbNode; ++i)
        raycast.execute(*nodes[i]);
    return raycast.result;
}


// Cache
template <class T_Container>
Raycast<T_Container>::Cache::Cache()
{
    _nodeCache = std::make_shared<std::unordered_map<T_Node const*, NodeCache>>();
}

template <class T_Container>
Raycast<T_Container>::Cache::Cache(Cache const& other)
    : _nodeCache(other._nodeCache)
{
}

template <class T_Container>
bool Raycast<T_Container>::Cache::PresenceCache::hasVoxel(Cache& cache, Vector3I const& boxPosition, int boxSize) const
{
    assert(boxSize > 1 && boxSize <= 8);

    switch (boxSize)
    {
    case 2:
        return this->presence[(boxPosition.x >> 2 & 1) | (boxPosition.y >> 1 & 2) | (boxPosition.z & 4)]
            >> ((boxPosition.x >> 1 & 1) | (boxPosition.y & 2) | ((boxPosition.z & 2) << 1)) & 1;
    case 4:
        return this->presence[(boxPosition.x >> 2 & 1) | (boxPosition.y >> 1 & 2) | (boxPosition.z & 4)] != 0;
    default:
        return this->hasVoxel();
    }
}

template <class T_Container>
template <class T_SubContainer>
bool Raycast<T_Container>::Cache::ContainerPresenceCache<T_SubContainer>::hasVoxel(Cache& cache, Vector3I const& boxPosition, int boxSize) const
{
    assert(boxSize <= T_SubContainer::NB_VOXELS);

    switch (boxSize)
    {
    case 8:
        return this->containerPresence[boxPosition.x >> 3 & 7][boxPosition.y >> 3 & 7][boxPosition.z >> 3 & 7].hasVoxel();
    case 16:
        return this->presence[(boxPosition.x >> 5 & 1) | (boxPosition.y >> 4 & 2) | (boxPosition.z >> 3 & 4)]
            >> ((boxPosition.x >> 4 & 1) | (boxPosition.y >> 3 & 2) | (boxPosition.z >> 2 & 4)) & 1;
    case 32:
        return this->presence[(boxPosition.x >> 5 & 1) | (boxPosition.y >> 4 & 2) | (boxPosition.z >> 3 & 4)] != 0;
    default:
        return this->containerPresence[boxPosition.x >> 3 & 7][boxPosition.y >> 3 & 7][boxPosition.z >> 3 & 7].hasVoxel(cache, boxPosition, boxSize);
    }
}

template <class T_Container>
template <class T_SubContainer>
bool Raycast<T_Container>::Cache::SuperContainerPresenceCache<T_SubContainer>::hasVoxel(Cache& cache, Vector3I const& boxPosition, int boxSize) const
{
    assert(boxSize <= T_SubContainer::NB_VOXELS);

    const int bshift = T_SubContainer::NB_SUPERCONTAINER * 3;
    const int bshift1 = bshift + 1;
    const int bshift2 = bshift + 2;
    const int bshift_1 = bshift - 1;
    switch (boxSize)
    {
    case T_SubContainer::NB_VOXELS / 8:
        return this->containerPresence[boxPosition.x >> bshift & 7][boxPosition.y >> bshift & 7][boxPosition.z >> bshift & 7] != nullptr;
    case T_SubContainer::NB_VOXELS / 4:
        return this->presence[(boxPosition.x >> bshift2 & 1) | (boxPosition.y >> bshift1 & 2) | (boxPosition.z >> bshift & 4)]
            >> ((boxPosition.x >> bshift1 & 1) | (boxPosition.y >> bshift & 2) | (boxPosition.z >> bshift_1 & 4)) & 1;
    case T_SubContainer::NB_VOXELS / 2:
        return this->presence[(boxPosition.x >> bshift2 & 1) | (boxPosition.y >> bshift1 & 2) | (boxPosition.z >> bshift & 4)] != 0;
    default:
        return this->containerPresence[boxPosition.x >> bshift & 7][boxPosition.y >> bshift & 7][boxPosition.z >> bshift & 7]->hasVoxel(cache, boxPosition, boxSize);
    }
}

template <class T_SubContainer>
static bool hasVoxel(T_SubContainer const& container, uint8_t bx, uint8_t by, uint8_t bz, uint8_t boxSize)
{
    for (uint8_t x = 0; x < boxSize; ++x)
    {
        for (uint8_t y = 0; y < boxSize; ++y)
        {
            for (uint8_t z = 0; z < boxSize; ++z)
            {
                if (container.hasVoxel(x + bx, y + by, z + bz))
                    return true;
            }
        }
    }
    return false;
}

template <class T_Container>
bool Raycast<T_Container>::Cache::hasVoxel(T_Node const& node, Vector3I const& boxPosition, int boxSize)
{
    if (_lastNode == &node)
        return _lastCache ? _lastCache->hasVoxel(*this, boxPosition, boxSize) : false;

    _lastNode = &node;
    auto it = _nodeCache->find(&node);
    if (it == _nodeCache->end())
    {
        _lastCache = nullptr;
        return false;
    }
    _lastCache = &it->second;
    return it->second.hasVoxel(*this, boxPosition, boxSize);
}

template <class T_Container>
bool Raycast<T_Container>::Cache::fillHasVoxel(T_Node const& node, Vector3I const& boxPosition, int boxSize)
{
    if (_lastNode == &node)
        return _lastCache ? _lastCache->hasVoxel(*this, boxPosition, boxSize) : false;

    _lastNode = &node;
    auto pair = _nodeCache->emplace(&node, NodeCache{});
    if (pair.second)
        this->fillCache(node, pair.first->second);
    _lastCache = &pair.first->second;
    return pair.first->second.hasVoxel(*this, boxPosition, boxSize);
}

template <class T_Container>
template <class T_SubContainer, typename T_Cache>
void Raycast<T_Container>::Cache::fillCache(T_SubContainer const& container, T_Cache& cache)
{
    for (uint8_t x = 0; x < T_SubContainer::NB_CONTAINERS; ++x)
    {
        for (uint8_t y = 0; y < T_SubContainer::NB_CONTAINERS; ++y)
        {
            for (uint8_t z = 0; z < T_SubContainer::NB_CONTAINERS; ++z)
            {
                if (container.hasContainer(x, y, z))
                {
                    cache.presence[(x >> 2) | (y >> 1 & 2) | (z & 4)] |=
                        1 << ((x >> 1 & 1) | (y & 2) | ((z & 2) << 1));

                    if (!cache.containerPresence[x][y][z])
                        cache.containerPresence[x][y][z].reset(new typename T_Cache::SubCache);
                    this->fillCache(*container.findContainer(x, y, z), *cache.containerPresence[x][y][z]);
                }
            }
        }
    }
}

template <class T_Container>
template <class T_SubContainer>
void Raycast<T_Container>::Cache::fillCache(T_SubContainer const& container, ContainerPresenceCache<T_SubContainer>& cache)
{
    for (uint8_t x = 0; x < T_SubContainer::NB_CONTAINERS; ++x)
    {
        for (uint8_t y = 0; y < T_SubContainer::NB_CONTAINERS; ++y)
        {
            for (uint8_t z = 0; z < T_SubContainer::NB_CONTAINERS; ++z)
            {
                if (container.hasContainer(x, y, z))
                {
                    cache.presence[(x >> 2) | (y >> 1 & 2) | (z & 4)] |=
                        1 << ((x >> 1 & 1) | (y & 2) | ((z & 2) << 1));

                    this->fillCache(*container.findContainer(x, y, z), cache.containerPresence[x][y][z]);
                }
            }
        }
    }
}

template <class T_Container>
template <class T_SubContainer>
void Raycast<T_Container>::Cache::fillCache(T_SubContainer const& container, PresenceCache& cache)
{
    for (uint8_t x = 0; x < T_SubContainer::NB_VOXELS; x += 2)
    {
        for (uint8_t y = 0; y < T_SubContainer::NB_VOXELS; y += 2)
        {
            for (uint8_t z = 0; z < T_SubContainer::NB_VOXELS; z += 2)
            {
                if (voxomap::hasVoxel(container, x, y, z, 2))
                {
                    cache.presence[(x >> 2) | (y >> 1 & 2) | (z & 4)] |=
                        1 << ((x >> 1 & 1) | (y & 2) | ((z & 2) << 1));
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
        auto& cache = (*_nodeCache)[&node];
        this->fillCache(*node.getVoxelContainer(), cache);
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