#ifndef _VOXOMAP_VECTOR3_HPP_
#define _VOXOMAP_VECTOR3_HPP_

#include <cmath>
#include <iostream>

namespace voxomap
{

/*!
    \defgroup Utility Utility
    Utility classes of voxomap
*/

/*! \class Vector3
    \ingroup Utility
    \brief Utility class for manipulating 3-dimensional vectors
*/
template <typename Type>
class Vector3
{
public:
    static_assert(std::is_arithmetic<Type>::value, "Underlying type must be arithmetic");
    /*!
        \brief Constructs the vector from its coordinates
    */
    Vector3(Type i_x = 0, Type i_y = 0, Type i_z = 0)
        : x(i_x), y(i_y), z(i_z)
    {
    }
    /*!
        \brief Constructs the vector by copy
    */
    template <typename T>
    Vector3(Vector3<T> const& other)
        : x(static_cast<Type>(other.x)), y(static_cast<Type>(other.y)), z(static_cast<Type>(other.z))
    {
    }

    /*!
        \brief Copy right value
        \param right Right operand (a scalar value)
        \return Reference to left operand
    */
    template <typename T>
    Vector3<Type>& operator=(Vector3<T> const& right)
    {
        this->x = static_cast<Type>(right.x);
        this->y = static_cast<Type>(right.y);
        this->z = static_cast<Type>(right.z);
        return *this;
    }
    /*!
        \brief Affects \a right to \a x, \a y and \a z
        \param right Right operand (a scalar value)
        \return Reference to left operand
    */
    template <typename T>
    Vector3<Type>& operator=(T right)
    {
        this->x = static_cast<Type>(right);
        this->y = static_cast<Type>(right);
        this->z = static_cast<Type>(right);
        return *this;
    }
    /*!
        \brief Overload of operator +
        \param right Right operand (a vector)
        \return Memberwise addition of both vectors
    */
    template <typename T>
    Vector3<Type> operator+(Vector3<T> const& right) const
    {
        Vector3<Type> t = *this;
        t += right;
        return t;
    }
    /*!
        \brief Overload of operator +
        \param right Right operand (a scalar value)
        \return Memberwise addition by \a right
    */
    template <typename T>
    Vector3<Type> operator+(T right) const
    {
        Vector3<Type> result = *this;
        result += right;
        return result;
    }
    /*!
        \brief Overload of operator +=
        \param right Right operand (a vector)
        return Reference to left operand
    */
    template <typename T>
    Vector3<Type>& operator+=(Vector3<T> const& right)
    {
        this->x += static_cast<Type>(right.x);
        this->y += static_cast<Type>(right.y);
        this->z += static_cast<Type>(right.z);
        return *this;
    }
    /*!
        \brief Overload of operator +=.
        This operator performs a memberwise addition by \a right, and assigns the result to left operand
        \param right Right operand (a scalar value)
        \return Reference to left operand
    */
    template <typename T>
    Vector3<Type>& operator+=(T right)
    {
        this->x += static_cast<Type>(right);
        this->y += static_cast<Type>(right);
        this->z += static_cast<Type>(right);
        return *this;
    }
    /*!
        \brief Overload of unary operator -
        \return Memberwise opposite of the vector
    */
    Vector3<Type> operator-() const
    {
        Vector3<Type> res;

        res.x = -this->x;
        res.y = -this->y;
        res.z = -this->z;
        return res;
    }
    /*!
        \brief Overload of operator +
        \return Memberwise subtraction of both vectors
    */
    template <typename T>
    Vector3<Type> operator-(Vector3<T> const& right) const
    {
        Vector3<Type> t;
        t.x = static_cast<Type>(this->x - right.x);
        t.y = static_cast<Type>(this->y - right.y);
        t.z = static_cast<Type>(this->z - right.z);
        return t;
    }
    /*!
        \brief Overload of operator -
        \param right Right operand (a scalar value)
        \return Memberwise subtraction by \a right
    */
    template <typename T>
    Vector3<Type> operator-(T right) const
    {
        Vector3<Type> result = *this;
        result -= right;
        return result;
    }
    /*!
        \brief Overload of operator -=.
        This operator performs a memberwise subtraction of both vectors, and assigns the result to left operand
        \param right Right operand (a vector)
        \return Reference to left operand
    */
    template <typename T>
    Vector3<Type>& operator-=(Vector3<T> const& right)
    {
        this->x = static_cast<Type>(this->x - right.x);
        this->y = static_cast<Type>(this->y - right.y);
        this->z = static_cast<Type>(this->z - right.z);
        return *this;
    }
    /*!
        \brief Overload of operator -=.
        This operator performs a memberwise subtraction by \a right, and assigns the result to left operand
        \param value Right operand (a scalar value)
        \return Reference to left operand
    */
    template <typename T>
    Vector3<Type>& operator-=(T value)
    {
        this->x = static_cast<Type>(this->x - value);
        this->y = static_cast<Type>(this->y - value);
        this->z = static_cast<Type>(this->z - value);
        return *this;
    }
    /*!
        \brief Overload of binary operator *
        \param right Right operand (a scalar value)
        \return Memberwise multiplication by \a right
    */
    template <typename T>
    Vector3<Type> operator*(T right) const
    {
        Vector3<Type> result = *this;
        result *= right;
        return result;
    }
    /*!
        \brief Overload of operator *
        \param right Right operand (a vector)
        \return A cross product of both vectors
    */
    template <typename T>
    Vector3<Type> operator*(Vector3<T> const& right) const
    {
        Vector3<Type> t;

        t.x = static_cast<Type>(this->y * right.z - this->z * right.y);
        t.y = static_cast<Type>(this->z * right.x - this->x * right.z);
        t.z = static_cast<Type>(this->x * right.y - this->y * right.x);
        return t;
    }
    /*!
        \brief Overload of operator *=.
        This operator performs a memberwise multiplication by \a right, and assigns the result to left operand
        \param right Right operand (a scalar value)
        \return Reference to left operand
    */
    template <typename T>
    Vector3<Type>& operator*=(T right)
    {
        this->x = static_cast<Type>(this->x * right);
        this->y = static_cast<Type>(this->y * right);
        this->z = static_cast<Type>(this->z * right);
        return *this;
    }
    /*!
        \brief Overload of operator /
        \param right Right operand (a scalar value)
        \return Memberwise division by \a right
    */
    template <typename T>
    Vector3<Type> operator/(T right) const
    {
        Vector3<Type> result = *this;
        result /= right;
        return result;
    }
    /*!
        \brief Overload of binary operator /=
        \param right Right operand (a vector)
        \return Memberwise division by \a right
    */
    template <typename T>
    Vector3<Type>& operator/=(Vector3<T> const& right)
    {
        this->x = static_cast<Type>(this->x / right);
        this->y = static_cast<Type>(this->y / right);
        this->z = static_cast<Type>(this->z / right);
        return *this;
    }
    /*!
        \brief Overload of binary operator /=.
        This operator performs a memberwise division by \a right, and assigns the result to left operand
        \param right Right operand (a scalar value)
        \return Reference to left operand
    */
    template <typename T>
    Vector3<Type>& operator/=(T right)
    {
        if (right == 0)
            return *this;
        this->x = static_cast<Type>(this->x / right);
        this->y = static_cast<Type>(this->y / right);
        this->z = static_cast<Type>(this->z / right);
        return *this;
    }
    /*!
        \brief Overload of binary operator ==.
        This operator compares strict equality between two vectors
        \param right Right operand (a vector)
        \return True if left operand is equal to right
    */
    bool operator==(Vector3<Type> const& right) const
    {
        return !(*this != right);
    }
    /*!
        \brief Overload of binary operator !=.
        This operator compares strict difference between two vectors
        \param right Right operand (a vector)
        \return True if left operand is not equal to right
    */
    bool operator!=(Vector3<Type> const& right) const
    {
        return std::memcmp(this, &right, sizeof(*this)) != 0;
    }
    /*!
        \brief Overload of binary operator <.
        This operator compares strict difference between two vectors
        \param right Right operand (a vector)
    */
    bool operator<(Vector3<Type> const& right) const
    {
        return std::memcmp(this, &right, sizeof(*this)) < 0;
    }

    /*!
        \brief Execute a binary mask on each member of the vector
        \param mask The binary mask
        \return Reference to this operand
    */
    Vector3<Type>& operator&=(int mask)
    {
        this->x &= mask;
        this->y &= mask;
        this->z &= mask;
        return *this;
    }

    /*!
        \brief Execute a binary mask on each member of the vector
        \param mask The binary mask
        \return The resulting vector
    */
    Vector3<Type> operator&(int mask) const
    {
        Vector3<Type> copy(*this);
        copy &= mask;
        return copy;
    }

    /*!
        \brief Cast operator on another vector type
        \return The resulting vector
    */
    template <typename T>
    operator Vector3<T>()
    {
        Vector3<T> tmp;
        tmp.x = this->x;
        tmp.y = this->y;
        tmp.z = this->z;
        return tmp;
    }

    /*!
        \brief Calculates the dot/scalar product of the vectors
        \return dot product of both vector
    */
    template <typename T>
    Type dotProduct(Vector3<T> const& v) const
    {
        return x * v.x + y * v.y + z * v.z;
    }
    /*!
        \brief Calculates the dot/scalar product of the vectors
        \return vector wich contain dot product of vectors
    */
    Vector3<Type> dotProduct(Vector3<Type> const& first, Vector3<Type> const& second, Vector3<Type> const& third) const
    {
        return { this->dotProduct(first), this->dotProduct(second), this->dotProduct(third) };
    }

    /*!
        \brief Calculates the length of the vector
        \return The length of the vector
    */
    Type length() const
    {
        return std::sqrt(x * x + y * y + z * z);
    }
    /*!
        \brief Calculates the square length of the vector
        \return The square length of the vector
    */
    Type squaredLength() const
    {
        return x * x + y * y + z * z;
    }

    /*!
        \brief Normalizes the vector
        \return Reference
    */
    Vector3<Type>& normalize()
    {
        *this /= length();
        return *this;
    }
    /*!
        \brief Sets the values of the vector
        \param i_x x value
        \param i_y y value
        \param i_z z value
    */
    void set(Type i_x, Type i_y, Type i_z)
    {
        this->x = i_x;
        this->y = i_y;
        this->z = i_z;
    }

    Type        x;        //!< X coordinate of the vector
    Type        y;        //!< Y coordinate of the vector
    Type        z;        //!< Z coordinate of the vector
};

using Vector3D = Vector3<double>;
using Vector3I = Vector3<int>;
using Vector3F = Vector3<float>;

} // End namespace voxomap

/*!
    \relates Vector3
    \brief Overload of operator <<
    This operand write on the \a os stream
    \param os Left operand (std::ostream)
    \param vector Right operand (a vector)
    \return Reference to \a os
*/
template <typename Type>
std::ostream& operator<<(std::ostream& os, voxomap::Vector3<Type> const& vector)
{
    return os << vector.x << " " << vector.y << " " << vector.z;
}

/*!
    \example Vector3

    Vector3 is a simple class that defines a mathematical
    vector with three coordinates (x, y and z). It can be used to
    represent anything that has three dimensions: a size, a point,
    a velocity, etc.

    The Vector3 class has a small and simple interface, its x, y and z members can be accessed directly.

    Usage example:
    \code
    Vector3 v1(16.5, 24, -8.2);
    v1.x = 18.2f;
    double y = v1.y;
    double z = v1.z;

    Vector3 v2 = v1 * 5;
    Vector3 v3;
    v3 = v1 + v2;

    std::cout << "v1 = " << v1 << std::endl;
    std::cout << "v2 = " << v2 << std::endl;
    if (v2 != v3)
        std::cout << "v3 = " << v3 << std::endl;
    \endcode

    Output:
    \code
    v1 = 18.2 24 -8.2
    v2 = 91 120 -41
    v3 = 109.2 144 -49.2
    \endcode
*/

#endif // _VOXOMAP_VECTOR3_HPP_
