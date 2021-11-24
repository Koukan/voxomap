#ifndef _VOXOMAP_RAY_HPP_
#define _VOXOMAP_RAY_HPP_

#include "Vector3.hpp"
#include <algorithm>

namespace voxomap
{

/*! \struct Ray
    \ingroup Utility
    \brief Represent a ray
*/
struct Ray
{
    /*!
        \brief Default constructor
    */
    Ray() = default;
    /*!
        \brief Default copy constructor
    */
    Ray(Ray const& ray) = default;
    /*!
        \brief Constructs Ray
        \param src Position of the ray
        \param dir Direction of the ray
    */
    Ray(Vector3D const &src, Vector3D const &dir);

    /*!
        \brief Create a ray
        \param position Position of the source
        \param target Position of the target
        \return The created ray
    */
    static Ray  getRay(Vector3D const &position, Vector3D const &target);
    /*!
        \brief Returns the distance between the ray source and the plane
        \return The square of the distance if it exist otherwise -1
    */
    double      intersectPlane(Vector3D const& p1, Vector3D const& p2, Vector3D const& p3) const;

    /*!
        \brief Check if there is an intersection with a AABox (Axis Aligned Box)
        \param boxMin Minimum position of the AABox
        \param boxMax Maximum position of the AABox
        \return True if there is an intersection
    */
    bool        intersectAABox(Vector3D const& boxMin, Vector3D const& boxMax) const;
    /*!
        \brief Check if there is an intersection with a AABox (Axis Aligned Box)
        \param x Minimum position of the AABox on x axis
        \param y Minimum position of the AABox on y axis
        \param z Minimum position of the AABox on z axis
        \param size Size of the AABox
        \return True if there is an intersection
    */
    bool        intersectAABox(double x, double y, double z, double size) const;

    /*!
        \brief Get the origin point of the ray
    */
    inline Vector3D const& getOrigin() const;
    /*!
        \brief Get the direction vector of the ray
    */
    inline Vector3D const& getDirection() const;
    /*!
        \brief Get the inverse direction vector of the ray
    */
    inline Vector3F const& getInverseDirection() const;

    /*!
        \brief Set the origin point of the ray
        \param src The origin position
    */
    inline void setOrigin(Vector3D const& src);
    /*!
        \brief Set the direction and inverse direction vector of the ray
        \param dir The direction vector
    */
    inline void setDirection(Vector3D const& dir);

    Vector3D    src;        //!< Position of the ray
    Vector3D    dir;        //!< Direction of the ray
    Vector3F    inv_dir;    //!< Inverse direction of the ray
};

inline Ray::Ray(Vector3D const &i_src, Vector3D const &i_dir)
    : src(i_src), dir(i_dir)
{
    inv_dir.x = 1.f / float(dir.x);
    inv_dir.y = 1.f / float(dir.y);
    inv_dir.z = 1.f / float(dir.z);
}

inline Ray Ray::getRay(Vector3D const &position, Vector3D const &target)
{
    return Ray(position, (target - position).normalize());
}

inline Vector3D const& Ray::getOrigin() const
{
    return src;
}

inline Vector3D const& Ray::getDirection() const
{
    return dir;
}

inline Vector3F const& Ray::getInverseDirection() const
{
    return inv_dir;
}

inline void Ray::setOrigin(Vector3D const& src)
{
    this->src = src;
}

inline void Ray::setDirection(Vector3D const& dir)
{
    this->dir = dir;
    this->dir.normalize();
    this->inv_dir.x = 1.f / float(this->dir.x);
    this->inv_dir.y = 1.f / float(this->dir.y);
    this->inv_dir.z = 1.f / float(this->dir.z);
}

inline double Ray::intersectPlane(Vector3D const& p1, Vector3D const& p2, Vector3D const& p3) const
{
    Vector3D v1 = p2 - p1;
    Vector3D v2 = p3 - p1;
    Vector3D v3 = v1 * v2;
    Vector3D tmp = this->src - p1;
    Vector3D vRotRay1(v1.dotProduct(tmp), v2.dotProduct(tmp), v3.dotProduct(tmp));
    tmp += this->dir;
    Vector3D vRotRay2(v1.dotProduct(tmp), v2.dotProduct(tmp), v3.dotProduct(tmp));
    if (vRotRay1.z == vRotRay2.z)
        return -1.0;
    double fPercent = vRotRay1.z / (vRotRay2.z - vRotRay1.z);
    Vector3D position = (this->src - this->dir) * fPercent;
    Vector3D pA(std::min(p1.x, std::min(p2.x, p3.x)), std::min(p1.y, std::min(p2.y, p3.y)), std::min(p1.z, std::min(p2.z, p3.z)));
    Vector3D pB(std::max(p1.x, std::max(p2.x, p3.x)), std::max(p1.y, std::max(p2.y, p3.y)), std::max(p1.z, std::max(p2.z, p3.z)));
    pA -= 0.001;
    pB += 0.001;
    if (position.x >= pA.x && position.x <= pB.x && position.y >= pA.y && position.y <= pB.y && position.z >= pA.z && position.z <= pB.z &&
        ((this->dir.x <= 0 && position.x <= this->src.x) || (this->dir.x > 0 && position.x > this->src.x)) &&
        ((this->dir.y <= 0 && position.y <= this->src.y) || (this->dir.y > 0 && position.y > this->src.y)) &&
        ((this->dir.z <= 0 && position.z <= this->src.z) || (this->dir.z > 0 && position.z > this->src.z)))
    {
        tmp = position - this->src;
        return tmp.x * tmp.x + tmp.y * tmp.y + tmp.z * tmp.z;
    }
    return -1.0;
}

inline bool Ray::intersectAABox(Vector3D const& boxMin, Vector3D const& boxMax) const
{
    float t1 = static_cast<float>((boxMin.x - this->src.x) * inv_dir.x);
    float t2 = static_cast<float>((boxMax.x - this->src.x) * inv_dir.x);

    std::pair<float, float> min_max = std::minmax(t1, t2);
    float tmin = min_max.first;
    float tmax = min_max.second;

    t1 = static_cast<float>((boxMin.y - this->src.y) * inv_dir.y);
    t2 = static_cast<float>((boxMax.y - this->src.y) * inv_dir.y);

    min_max = std::minmax(t1, t2);
    tmin = std::max(tmin, min_max.first);
    tmax = std::min(tmax, min_max.second);

    t1 = static_cast<float>((boxMin.z - this->src.z) * inv_dir.z);
    t2 = static_cast<float>((boxMax.z - this->src.z) * inv_dir.z);

    min_max = std::minmax(t1, t2);
    tmin = std::max(tmin, min_max.first);
    tmax = std::min(tmax, min_max.second);

    return tmax >= 0 && tmax >= tmin;
}

inline bool Ray::intersectAABox(double x, double y, double z, double size) const
{
    x -= this->src.x;
    y -= this->src.y;
    z -= this->src.z;

    float t1 = float(x) * inv_dir.x;
    float t2 = float(x + size) * inv_dir.x;

    float tmin;
    float tmax;
    std::tie(tmin, tmax) = std::minmax(t1, t2);

    float min;
    float max;
    t1 = float(y) * inv_dir.y;
    t2 = float(y + size) * inv_dir.y;

    std::tie(min, max) = std::minmax(t1, t2);
    tmin = std::max(tmin, min);
    tmax = std::min(tmax, max);

    t1 = float(z) * inv_dir.z;
    t2 = float(z + size) * inv_dir.z;

    std::tie(min, max) = std::minmax(t1, t2);
    tmin = std::max(tmin, min);
    tmax = std::min(tmax, max);

    return tmax >= 0 && tmax >= tmin;
}

}

#endif // _VOXOMAP_RAY_HPP_
