#ifndef _VOXOMAP_BOUNDING_BOX_HPP_
#define _VOXOMAP_BOUNDING_BOX_HPP_

#include "Vector3.hpp"

namespace voxomap
{

/*! \struct BoundingBox
    \ingroup Utility
    \brief Represent an axis aligned bounding box
*/
template <typename Scalar>
struct BoundingBox
{
    /*!
        \brief Default constructor
    */
    BoundingBox() = default;
    /*!
        \brief Constructs BoundingBox
        \param min Minimum xyz coordinates
        \param max Maximum xyz coordinates
    */
    BoundingBox(Vector3<Scalar> const& min, Vector3<Scalar> const& max);
    /*!
        \brief Constructs BoundingBox
        \param min Minimum xyz coordinates
        \param sx Size of box in x
        \param sy Size of box in y
        \param sz Size of box in z
    */
    BoundingBox(Vector3<Scalar> const& min, Scalar sx, Scalar sy, Scalar sz);
    /*!
        \brief Constructs BoundingBox
        \param xmin Minimum x coordinate
        \param ymin Minimum y coordinate
        \param zmin Minimum z coordinate
        \param sx Size of box in x
        \param sy Size of box in y
        \param sz Size of box in z
    */
    BoundingBox(Scalar xmin, Scalar ymin, Scalar zmin, Scalar sx, Scalar sy, Scalar sz);

    /*!
        \brief Check if the point \a pt is inside the bounding box or not
        \param pt Position of the point
        \return True if it's inside
    */
    template <typename T>
    bool isInside(Vector3<T> const& pt) const;

    /*!
        \brief Check if the point is inside the bounding box or not
        \param x X position of the point
        \param y Y position of the point
        \param z Z position of the point
        \return True if it's inside
    */
    template <typename T>
    bool isInside(T x, T y, T z) const;

    /*!
        \brief Check if \a this intersect the \a bounding_box
        \param bounding_box The other bounding box
        \return True if intersect
    */
    template <typename T>
    bool intersect(BoundingBox<T> const& bounding_box) const;

public:
    Vector3<Scalar> minBounds;  //!< Minimum xyz coordinates
    Vector3<Scalar> maxBounds;  //!< Maximum xyz coordinates
};

}

#include "BoundingBox.ipp"

#endif