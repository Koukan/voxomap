namespace voxomap
{

static int const gl_raycast_index[][9] = {
	{ 7, 3, 6, 5, 2, 1, 4, 0, FaceEnum::RIGHT | FaceEnum::TOP    | FaceEnum::BACK },	// Right Top Back
	{ 6, 2, 7, 4, 3, 0, 5, 1, FaceEnum::LEFT  | FaceEnum::TOP    | FaceEnum::BACK },	// Left Top Back
	{ 5, 1, 7, 4, 3, 6, 0, 2, FaceEnum::RIGHT | FaceEnum::BOTTOM | FaceEnum::BACK },	// Right Bottom Back
	{ 4, 5, 6, 0, 7, 2, 1, 3, FaceEnum::LEFT  | FaceEnum::BOTTOM | FaceEnum::BACK },	// Left Bottom Back
	{ 3, 1, 2, 7, 0, 5, 6, 4, FaceEnum::RIGHT | FaceEnum::TOP    | FaceEnum::FRONT},	// Right Top Front
	{ 2, 0, 6, 3, 1, 7, 4, 5, FaceEnum::LEFT  | FaceEnum::TOP    | FaceEnum::FRONT},	// Left Top Front
	{ 1, 0, 5, 3, 2, 7, 4, 6, FaceEnum::RIGHT | FaceEnum::BOTTOM | FaceEnum::FRONT},	// Right Bottom Front
	{ 0, 1, 2, 4, 3, 6, 5, 7, FaceEnum::LEFT  | FaceEnum::BOTTOM | FaceEnum::FRONT}		// Left Bottom Front
};

inline static int orderId(Vector3D const& direction)
{
	return (direction.x > 0) | ((direction.y > 0) << 1) | ((direction.z > 0) << 2);
}

template <class T_Area>
inline bool Raycast<T_Area>::Result::operator<(Result const& other) const
{
	return this->distance < other.distance;
}

template <class T_Area>
inline bool Raycast<T_Area>::Result::operator>(Result const& other) const
{
	return other < *this;
}

inline static double intersection(Vector3D const& corner, size_t size, Ray const& ray, FaceEnum& face, FaceEnum faceToCheck)
{
	double distance;

	if (faceToCheck & FaceEnum::TOP)
	{
		distance = ray.intersectPlane(Vector3D(corner.x, corner.y + size, corner.z), Vector3D(corner.x + size, corner.y + size, corner.z), Vector3D(corner + size));
		if (distance != -1)
		{
			face = FaceEnum::TOP;
			return distance;
		}
	}
	if (faceToCheck & FaceEnum::BOTTOM)
	{
		distance = ray.intersectPlane(Vector3D(corner), Vector3D(corner.x + size, corner.y, corner.z), Vector3D(corner.x + size, corner.y, corner.z + size));
		if (distance != -1)
		{
			face = FaceEnum::BOTTOM;
			return distance;
		}
	}
	if (faceToCheck & FaceEnum::LEFT)
	{
		distance = ray.intersectPlane(Vector3D(corner), Vector3D(corner.x, corner.y + size, corner.z), Vector3D(corner.x, corner.y + size, corner.z + size));
		if (distance != -1)
		{
			face = FaceEnum::LEFT;
			return distance;
		}
	}
	if (faceToCheck & FaceEnum::RIGHT)
	{
		distance = ray.intersectPlane(Vector3D(corner.x + size, corner.y, corner.z), Vector3D(corner.x + size, corner.y + size, corner.z), Vector3D(corner + size));
		if (distance != -1)
		{
			face = FaceEnum::RIGHT;
			return distance;
		}
	}
	if (faceToCheck & FaceEnum::FRONT)
	{
		distance = ray.intersectPlane(Vector3D(corner), Vector3D(corner.x + size, corner.y, corner.z), Vector3D(corner.x + size, corner.y + size, corner.z));
		if (distance != -1)
		{
			face = FaceEnum::FRONT;
			return distance;
		}
	}
	if (faceToCheck & FaceEnum::BACK)
	{
		distance = ray.intersectPlane(Vector3D(corner.x, corner.y, corner.z + size), Vector3D(corner.x + size, corner.y, corner.z + size), Vector3D(corner + size));
		if (distance != -1)
		{
			face = FaceEnum::BACK;
			return distance;
		}
	}
	return -1;
}

inline static double getDistance(Vector3I const& boxPosition, size_t boxSize, Ray const& ray, double distance, FaceEnum& face, FaceEnum checkFace)
{
    auto corner2 = boxPosition + boxSize;

    if (ray.src.x >= boxPosition.x && ray.src.x < corner2.x &&
        ray.src.y >= boxPosition.y && ray.src.y < corner2.y &&
        ray.src.z >= boxPosition.z && ray.src.z < corner2.z)
        return 0;

    if ((ray.src.x < boxPosition.x && ray.dir.x < 0) || (ray.src.x > corner2.x && ray.dir.x > 0) ||
        (ray.src.y < boxPosition.y && ray.dir.y < 0) || (ray.src.y > corner2.y && ray.dir.y > 0) ||
        (ray.src.z < boxPosition.z && ray.dir.z < 0) || (ray.src.z > corner2.z && ray.dir.z > 0))
        return -1;

	double tmp = intersection(boxPosition, boxSize, ray, face, checkFace);
    return (tmp != -1 && (distance == -1 || tmp < distance)) ? tmp : -1;
}

template <class T_Area>
bool Raycast<T_Area>::raycastVoxel(VoxelNode<T_Area> const& node, Vector3I const& pos)
{
	auto voxel = node.getVoxel(pos.x, pos.y, pos.z);
    if (!voxel)
        return false;

    Vector3I corner(node.getX() + pos.x, node.getY() + pos.y, node.getZ() + pos.z);
	if (this->predicate && !this->predicate(*const_cast<VoxelData*>(voxel), corner))
		return false;

	FaceEnum face;

	double computedDistance = getDistance(corner, 1, this->ray, this->maxDistance, face, static_cast<FaceEnum>(_faceToCheck & ~voxel->getFace()));
	if (computedDistance == -1)
		return false;

	this->result.face = face;
    this->result.distance = ::sqrt(computedDistance);
    this->result.position = this->ray.src + this->ray.dir * this->result.distance;
    this->result.node = &node;
    this->result.voxel = voxel;
    this->result.voxelPosition = corner;
    return true;
}

template <class T_Area>
bool Raycast<T_Area>::raycastArea(VoxelNode<T_Area> const& node, Vector3I const& boxPosition, size_t boxSize)
{
    if (boxSize == 1)
        return this->raycastVoxel(node, boxPosition);

	Vector3I corner(node.getX() + boxPosition.x, node.getY() + boxPosition.y, node.getZ() + boxPosition.z);
    FaceEnum face;
	if (getDistance(corner, boxSize, this->ray, this->maxDistance, face, _faceToCheck) == -1)
		return false;

	boxSize /= 2;
	for (int i = 0; i < 8; ++i)
	{
		int index = gl_raycast_index[_sortingIndex][i];
		Vector3I offset(index & 1, (index >> 1) & 1, (index >> 2) & 1);
		offset = offset * boxSize + boxPosition;
		if (this->raycastArea(node, offset, boxSize))
			return true;
	}
	return false;
}

template <class T_Area>
bool Raycast<T_Area>::raycast(VoxelNode<T_Area> const& node)
{
    if (node.getSize() == T_Area::NB_VOXELS)
    {
        if (!node.hasVoxel())
            return false;
        return this->raycastArea(node, Vector3I(0, 0, 0), T_Area::NB_VOXELS);
    }
    
	FaceEnum face;
	for (int i = 0; i < 8; ++i)
	{
		auto child = static_cast<VoxelNode<T_Area>*>(node.getChildren()[gl_raycast_index[_sortingIndex][i]]);
        if (child)
        {
			auto corner = Vector3I(child->getX(), child->getY(), child->getZ());
            if (getDistance(corner, child->getSize(), this->ray, this->maxDistance, face, _faceToCheck) != -1 &&
				this->execute(*child))
                return true;
        }
	}
	return false;
}

template <class T_Area>
inline bool Raycast<T_Area>::execute(VoxelNode<T_Area> const& node)
{
    _sortingIndex = orderId(this->ray.dir);
    _faceToCheck = static_cast<FaceEnum>(gl_raycast_index[_sortingIndex][8]);
	return this->raycast(node);
}

template <class T_Area>
inline typename Raycast<T_Area>::Result Raycast<T_Area>::get(Ray const& ray, VoxelNode<T_Area> const& node, Predicate const& predicate, double maxDistance)
{
	Raycast raycast;

    raycast.ray = ray;
    raycast.predicate = predicate;
    raycast.maxDistance = (maxDistance > 0) ? maxDistance * maxDistance : -1;
    raycast.execute(node);
	return raycast.result;
}

template <class T_Area>
inline typename Raycast<T_Area>::Result Raycast<T_Area>::get(Ray const& ray, VoxelNode<T_Area> const& node, double maxDistance)
{
	return Raycast::get(ray, node, nullptr, maxDistance);
}

template <class T_Area>
inline typename Raycast<T_Area>::Result Raycast<T_Area>::get(Ray const& ray, VoxelOctree<T_Area> const& octree, Predicate const& predicate, double maxDistance)
{
    if (octree.getRootNode())
	    return Raycast::get(ray, *octree.getRootNode(), predicate, maxDistance);
    return Raycast::Result();
}

template <class T_Area>
inline typename Raycast<T_Area>::Result Raycast<T_Area>::get(Ray const& ray, VoxelOctree<T_Area> const& octree, double maxDistance)
{
    if (octree.getRootNode())
        return Raycast::get(ray, *octree.getRootNode(), maxDistance);
    return Raycast::Result();
}

} // End namespace voxomap