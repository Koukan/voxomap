#ifndef _VOXOMAP_RAY_HPP_
#define _VOXOMAP_RAY_HPP_

#include "Vector3.hpp"

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

    Vector3D    src;        //!< Position of the ray
    Vector3D    dir;        //!< Direction of the ray
};

inline Ray::Ray(Vector3D const &i_src, Vector3D const &i_dir)
    : src(i_src), dir(i_dir)
{
}

inline Ray Ray::getRay(Vector3D const &position, Vector3D const &target)
{
    return Ray(position, (target - position).normalize());
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
        return -1;
    float fPercent = static_cast<float>(vRotRay1.z / (vRotRay2.z - vRotRay1.z));
    Vector3D position = this->src - this->dir * fPercent;
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
    return -1;
}

}

#endif // _VOXOMAP_RAY_HPP_
