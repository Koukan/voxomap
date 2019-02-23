#ifndef _VOXOMAP_VECTOR3_HPP_
#define _VOXOMAP_VECTOR3_HPP_

#include <cmath>
#include <iostream>

namespace voxomap
{

/*!
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
	Vector3(Type x = 0, Type y = 0, Type z = 0)
		: x(x), y(y), z(z)
	{
	}
	/*!
		\brief Constructs the vector by copy
	*/
	template <typename T>
	Vector3(Vector3<T> const& other)
		: x(other.x), y(other.y), z(other.z)
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
		this->x = right.x;
		this->y = right.y;
		this->z = right.z;
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
		this->x = right;
		this->y = right;
		this->z = right;
		return *this;
	}
	/*!
		\brief Overload of binary operator +
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
		\brief Overload of binary operator +
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
		\brief Overload of binary operator +=
		\param value Right operand (a vector)
		return Reference to left operand
	*/
	template <typename T>
	Vector3<Type>& operator+=(Vector3<T> const& right)
	{
		this->x += right.x;
		this->y += right.y;
		this->z += right.z;
		return *this;
	}
	/*!
		\brief Overload of binary operator +=
		This operator performs a memberwise addition by \a right, and assigns the result to left operand
		\param right Right operand (a scalar value)
		\return Reference to left operand
	*/
	template <typename T>
	Vector3<Type>& operator+=(T right)
	{
		x += right;
		y += right;
		z += right;
		return *this;
	}
	/*!
		\brief Overload of unary operator -
		\return Memberwise opposite of the vector
	*/
	Vector3<Type> operator-() const
	{
		Vector3<Type> res;

		res.x = -x;
		res.y = -y;
		res.z = -z;
		return res;
	}
	/*!
		\brief Overload of binary operator +
		\return Memberwise subtraction of both vectors
	*/
	template <typename T>
	Vector3<Type> operator-(Vector3<T> const& right) const
	{
		Vector3<Type> t;
		t.x = x - right.x;
		t.y = y - right.y;
		t.z = z - right.z;
		return t;
	}
	/*!
		\brief Overload of binary operator -
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
		\brief Overload of binary operator -=
		This operator performs a memberwise subtraction of both vectors, and assigns the result to left operand
		\param right Right operand (a vector)
		\return Reference to left operand
	*/
	template <typename T>
	Vector3<Type>& operator-=(Vector3<T> const& right)
	{
		this->x -= right.x;
		this->y -= right.y;
		this->z -= right.z;
		return *this;
	}
	/*!
		\brief Overload of binary operator -=
		This operator performs a memberwise subtraction by \a right, and assigns the result to left operand
		\param right Right operand (a scalar value)
		\return Reference to left operand
	*/
	template <typename T>
	Vector3<Type>& operator-=(T value)
	{
		x -= value;
		y -= value;
		z -= value;
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
		\brief Overload of binary operator *
		\param right Right operand (a vector)
		\return A cross product of both vectors
	*/
	template <typename T>
	Vector3<Type> operator*(Vector3<T> const& right) const
	{
		Vector3<Type> t;

		t.x = y * right.z - z * right.y;
		t.y = z * right.x - x * right.z;
		t.z = x * right.y - y * right.x;
		return t;
	}
	/*!
		\brief Overload of binary operator *=
		This operator performs a memberwise multiplication by \a right, and assigns the result to left operand
		\param right Right operand (a scalar value)
		\return Reference to left operand
	*/
	template <typename T>
	Vector3<Type>& operator*=(T right)
	{
		this->x *= right;
		this->y *= right;
		this->z *= right;
		return *this;
	}
	/*!
		\brief Overload of binary operator /
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
	template <typename T>
	Vector3<Type>& operator/=(Vector3<T> const& right)
	{
		this->x /= right.x;
		this->y /= right.y;
		this->z /= right.z;
		return *this;
	}
	/*!
		\brief Overload of binary operator /=
		This operator performs a memberwise division by \a right, and assigns the result to left operand
		\param right Right operand (a scalar value)
		\return Reference to left operand
	*/
	template <typename T>
	Vector3<Type>& operator/=(T right)
	{
		if (right == 0)
			return *this;
		this->x /= right;
		this->y /= right;
		this->z /= right;
		return *this;
	}
	/*!
		\brief Overload of binary operator ==
		This operator compares strict equality between two vectors
		\param right Right operand (a vector)
		\return True if left operand is equal to right
	*/
	bool operator==(Vector3<Type> const& right) const
	{
		return !(*this != right);
	}
	/*!
		\brief Overload of binary operator !=
		This operator compares strict difference between two vectors
		\param right Right operand (a vector)
		\return True if left operand is not equal to right
	*/
	bool operator!=(Vector3<Type> const& right) const
	{
		return ::memcmp(this, &right, sizeof(*this)) != 0;
	}
	bool operator<(Vector3<Type> const& right) const
	{
		return ::memcmp(this, &right, sizeof(*this)) < 0;
	}

	Vector3<Type>& operator&=(int mask)
	{
		this->x &= mask;
		this->y &= mask;
		this->z &= mask;
		return *this;
	}

	Vector3<Type> operator&(int mask) const
	{
		Vector3<Type> copy(*this);
		copy &= mask;
		return copy;
	}

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
		return ::sqrt(x * x + y * y + z * z);
	}

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
	*/
	void set(Type x, Type y, Type z)
	{
		this->x = x;
		this->y = y;
		this->z = z;
	}

	// vector 2d functions
	Vector3<Type>& rotate2D(float angle)
	{
		auto cosa = cos(angle);
		auto sina = sin(angle);
		auto px = this->x * cosa - this->y * sina;
		this->y = this->x * sina + this->y * cosa;
		this->x = px;
		return *this;
	}

	template<typename T>
	Type dotProduct2D(Vector3<T> const& other) const
	{
		return this->x * other.x + this->y * other.y;
	}

	template<typename T>
	Type det2D(Vector3<T> const& other) const
	{
		return this->x * other.y - this->y * other.x;
	}

	template<typename T>
	float getAngle2D(Vector3<T> const& other) const
	{
		return atan2(this->det2D(other), this->dotProduct2D(other));
	}

	Type		x;		//!< X coordinate of the vector
	Type		y;		//!< Y coordinate of the vector
	Type		z;		//!< Z coordinate of the vector
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
	\class Vector3

	Vector3 is a simple class that defines a mathematical
	vector with three coordinates (x, y and z). It can be used to
	represent anything that has three dimensions: a size, a point,
	a velocity, etc.

	The Vector3 class has a small and simple interface, its x and y members
	can be accessed directly.

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
