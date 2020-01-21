#ifndef _VOXOMAP_BOUNDING_BOX_HPP_
#define _VOXOMAP_BOUNDING_BOX_HPP_

#include "Vector3.hpp"

namespace voxomap
{
   template <typename Scalar>
   struct BoundingBox
   {
       BoundingBox() = default;
       BoundingBox(Vector3<Scalar> const& min, Vector3<Scalar> const& max);
       BoundingBox(Vector3<Scalar> const& min, Scalar sx, Scalar sy, Scalar sz);
       BoundingBox(Scalar xmin, Scalar ymin, Scalar zmin, Scalar sx, Scalar sy, Scalar sz);

       template <typename T>
       bool isInside(Vector3<T> const& pt) const;

       template <typename T>
       bool isInside(T x, T y, T z) const;

       template <typename T>
       bool intersect(BoundingBox<T> const& bounding) const;

   public:
       Vector3<Scalar> minBounds;
       Vector3<Scalar> maxBounds;
   };
}


#include "BoundingBox.ipp"

#endif