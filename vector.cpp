#include "vector.hpp"

Vector2f::Vector2f()
{
    x = 0.0;
    y = 0.0;
};

Vector2f::Vector2f(float nx, float ny)
{
    x = nx;
    y = ny;
};

Vector2f::Vector2f(const Vector2f& vector)
{
    x = vector.x;
    y = vector.y;
};

Vector2f& Vector2f::operator=(const Vector2f& vector)
{
    x = vector.x;
    y = vector.y;
    return *this;
};

Vector2f& Vector2f::operator+=(const Vector2f& vector)
{
    x += vector.x;
    y += vector.y;
    return *this;
};

Vector2f& Vector2f::operator+=(const float scalar)
{
    x += scalar;
    y += scalar;
    return *this;
};

Vector2f& Vector2f::operator-=(const Vector2f& vector)
{
    x -= vector.x;
    y -= vector.y;
    return *this;
};

Vector2f& Vector2f::operator-=(const float scalar)
{
    x -= scalar;
    y -= scalar;
    return *this;
};

Vector2f& Vector2f::operator*=(const Vector2f& vector)
{
    x *= vector.x;
    y *= vector.y;
    return *this;
};

Vector2f& Vector2f::operator*=(const float scalar)
{
    x *= scalar;
    y *= scalar;
    return *this;
};

Vector2f& Vector2f::operator/=(const Vector2f& vector)
{
    x /= vector.x;
    y /= vector.y;
    return *this;
};

Vector2f& Vector2f::operator/=(const float scalar)
{
    x /= scalar;
    y /= scalar;
    return *this;
};

Vector2f& Vector2f::operator%=(const Vector2f& vector)
{
    x = fmod(x,vector.x);
    y = fmod(y,vector.y);
    return *this;
};

Vector2f& Vector2f::operator%=(const float scalar)
{
    x = fmod(x,scalar);
    y = fmod(y,scalar);
    return *this;
};

Vector2f& Vector2f::floor()
{
    x = ::floor(x);
    y = ::floor(y);
    return *this;
};

const Vector2f Vector2f::operator+(const Vector2f& vector) const
{
    return Vector2f(*this) += vector;
};

const Vector2f Vector2f::operator+(const float scalar) const
{
    return Vector2f(*this) += scalar;
};

const Vector2f Vector2f::operator-(const Vector2f& vector) const
{
    return Vector2f(*this) -= vector;
};

const Vector2f Vector2f::operator-(const float scalar) const
{
    return Vector2f(*this) -= scalar;
};

const Vector2f Vector2f::operator*(const Vector2f& vector) const
{
    return Vector2f(*this) *= vector;
};

const Vector2f Vector2f::operator*(const float scalar) const
{
    return Vector2f(*this) *= scalar;
};

const Vector2f Vector2f::operator/(const Vector2f& vector) const
{
    return Vector2f(*this) /= vector;
};

const Vector2f Vector2f::operator/(const float scalar) const
{
    return Vector2f(*this) /= scalar;
};

const Vector2f Vector2f::operator%(const Vector2f& vector) const
{
    return Vector2f(*this) %= vector;
};

const Vector2f Vector2f::operator%(const float scalar) const
{
    return Vector2f(*this) %= scalar;
};

bool Vector2f::operator==(const Vector2f& vector) const
{
    if(x == vector.x && y == vector.y)
    return true;
    return false;
};

bool Vector2f::operator!=(const Vector2f& vector) const
{
    if(x != vector.x || y != vector.y)
    return true;
    return false;
};

float Vector2f::magnitude() const
{
    return sqrtf(x*x+y*y);
};

float Vector2f::direction() const
{
    return atan2(y,x);
};

void Vector2f::setFromPolar(float mag,float dir)
{
    x = mag*cos(dir);
    y = mag*sin(dir);
};

float Vector2f::dot(const Vector2f& vector) const
{
    return x*vector.x+y*vector.y;
};

float Vector2f::cross(const Vector2f& vector) const
{
    return x*vector.y+y*vector.x;
};

void Vector2f::normalize()
{
    if(!(x == 0 && y == 0))
    {
        float normFactor = sqrt(x*x+y*y);
        x /= normFactor;
        y /= normFactor;
    }
};

Vector2f Vector2f::getNormalized() const
{
    Vector2f temp(*this);
    temp.normalize();
    return temp;
};

void Vector2f::set(float nx,float ny)
{
    x = nx;
    y = ny;
};

Vector3f::Vector3f()
{
    x = 0.0;
    y = 0.0;
    z = 0.0;
};

Vector3f::Vector3f(float nx, float ny, float nz)
{
    x = nx;
    y = ny;
    z = nz;
};

Vector3f::Vector3f(const Vector3f& vector)
{
    x = vector.x;
    y = vector.y;
    z = vector.z;
};

Vector3f& Vector3f::operator=(const Vector3f& vector)
{
    x = vector.x;
    y = vector.y;
    z = vector.z;
    return *this;
};

Vector3f& Vector3f::operator+=(const Vector3f& vector)
{
    x += vector.x;
    y += vector.y;
    z += vector.z;
    return *this;
};

Vector3f& Vector3f::operator+=(const float scalar)
{
    x += scalar;
    y += scalar;
    z += scalar;
    return *this;
};

Vector3f& Vector3f::operator-=(const Vector3f& vector)
{
    x -= vector.x;
    y -= vector.y;
    z -= vector.z;
    return *this;
};

Vector3f& Vector3f::operator-=(const float scalar)
{
    x -= scalar;
    y -= scalar;
    z -= scalar;
    return *this;
};

Vector3f& Vector3f::operator*=(const Vector3f& vector)
{
    x *= vector.x;
    y *= vector.y;
    z *= vector.z;
    return *this;
};

Vector3f& Vector3f::operator*=(const float scalar)
{
    x *= scalar;
    y *= scalar;
    z *= scalar;
    return *this;
};

Vector3f& Vector3f::operator/=(const Vector3f& vector)
{
    x /= vector.x;
    y /= vector.y;
    z /= vector.z;
    return *this;
};

Vector3f& Vector3f::operator/=(const float scalar)
{
    x /= scalar;
    y /= scalar;
    z /= scalar;
    return *this;
};

Vector3f& Vector3f::operator%=(const Vector3f& vector)
{
    x = fmod(x,vector.x);
    y = fmod(y,vector.y);
    z = fmod(z,vector.z);
    return *this;
};

Vector3f& Vector3f::operator%=(const float scalar)
{
    x = fmod(x,scalar);
    y = fmod(y,scalar);
    z = fmod(z,scalar);
    return *this;
};

Vector3f& Vector3f::floor()
{
    x = ::floor(x);
    y = ::floor(y);
    z = ::floor(z);
    return *this;
};

const Vector3f Vector3f::operator+(const Vector3f& vector) const
{
    return Vector3f(*this) += vector;
};

const Vector3f Vector3f::operator+(const float scalar) const
{
    return Vector3f(*this) += scalar;
};

const Vector3f Vector3f::operator-(const Vector3f& vector) const
{
    return Vector3f(*this) -= vector;
};

const Vector3f Vector3f::operator-(const float scalar) const
{
    return Vector3f(*this) -= scalar;
};

const Vector3f Vector3f::operator*(const Vector3f& vector) const
{
    return Vector3f(*this) *= vector;
};

const Vector3f Vector3f::operator*(const float scalar) const
{
    return Vector3f(*this) *= scalar;
};

const Vector3f Vector3f::operator/(const Vector3f& vector) const
{
    return Vector3f(*this) /= vector;
};

const Vector3f Vector3f::operator/(const float scalar) const
{
    return Vector3f(*this) /= scalar;
};

const Vector3f Vector3f::operator%(const Vector3f& vector) const
{
    return Vector3f(*this) %= vector;
};

const Vector3f Vector3f::operator%(const float scalar) const
{
    return Vector3f(*this) %= scalar;
};

bool Vector3f::operator==(const Vector3f& vector) const
{
    if(x == vector.x && y == vector.y && z == vector.z)
    return true;
    return false;
};

bool Vector3f::operator!=(const Vector3f& vector) const
{
    if(x != vector.x || y != vector.y || z != vector.z)
    return true;
    return false;
};

float Vector3f::magnitude() const
{
    return sqrtf(x*x+y*y+z*z);
};

float Vector3f::dot(const Vector3f& vector) const
{
    return x*vector.x+y*vector.y+z*vector.z;
};

Vector3f Vector3f::cross(const Vector3f& vector) const
{
    return Vector3f(y*vector.z-vector.y*z, z*vector.x- vector.z*x, x*vector.y - vector.x*y);
};

void Vector3f::normalize()
{
    if(!(x == 0 && y == 0))
    {
        float normFactor = sqrt(x*x+y*y+z*z);
        x /= normFactor;
        y /= normFactor;
        z /= normFactor;
    }
};

Vector3f Vector3f::getNormalized() const
{
    Vector3f temp(*this);
    temp.normalize();
    return temp;
};

void Vector3f::set(float nx,float ny, float nz)
{
    x = nx;
    y = ny;
    z = nz;
};
