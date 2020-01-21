namespace voxomap
{

template <typename Scalar>
BoundingBox<Scalar>::BoundingBox(Vector3<Scalar> const& min, Vector3<Scalar> const& max)
    : minBounds(min), maxBounds(max)
{
}

template <typename Scalar>
BoundingBox<Scalar>::BoundingBox(Vector3<Scalar> const& min, Scalar sx, Scalar sy, Scalar sz)
    : minBounds(min), maxBounds(min.x + sx, min.y + sy, min.z + sz)
{
}

template <typename Scalar>
BoundingBox<Scalar>::BoundingBox(Scalar xmin, Scalar ymin, Scalar zmin, Scalar sx, Scalar sy, Scalar sz)
    : minBounds(xmin, ymin, zmin), maxBounds(xmin + sx, ymin + sy, zmin + sz)
{
}

template <typename Scalar>
template <typename T>
bool BoundingBox<Scalar>::isInside(Vector3<T> const& pt) const
{
    return this->isInside(pt.x, pt.y, pt.z);
}

template <typename Scalar>
template <typename T>
bool BoundingBox<Scalar>::isInside(T x, T y, T z) const
{
    return (x >= minBounds.x && x <= maxBounds.x &&
        y >= minBounds.y && y <= maxBounds.y &&
        z >= minBounds.z && z <= maxBounds.z);
}

template <typename Scalar>
template <typename T>
bool BoundingBox<Scalar>::intersect(BoundingBox<T> const& bounding) const
{
    if (maxBounds.x < bounding.minBounds.x || minBounds.x > bounding.maxBounds.x)
        return false;
    if (maxBounds.y < bounding.minBounds.y || minBounds.y > bounding.maxBounds.y)
        return false;
    if (maxBounds.z < bounding.minBounds.z || minBounds.z > bounding.maxBounds.z)
        return false;
    return true;
}

}