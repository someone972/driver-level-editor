#ifndef VECTOR_HPP
#define VECTOR_HPP
#include <cmath>

class Vector4f
{
    public:
        float x,y,z,w;
};

class Vector2i
{
    public:
        int x,y;
};

class Vector2f
{
    public:
        Vector2f();
        Vector2f(float nx,float ny);
        Vector2f(const Vector2f& vector);

        Vector2f& operator=(const Vector2f& vector);
        Vector2f& operator+=(const Vector2f& vector);
        Vector2f& operator+=(const float scalar);
        Vector2f& operator-=(const Vector2f& vector);
        Vector2f& operator-=(const float scalar);
        Vector2f& operator*=(const Vector2f& vector);
        Vector2f& operator*=(const float scalar);
        Vector2f& operator/=(const Vector2f& vector);
        Vector2f& operator/=(const float scalar);
        Vector2f& operator%=(const Vector2f& vector);
        Vector2f& operator%=(const float scalar);
        Vector2f& floor();
        const Vector2f operator+(const Vector2f& vector) const;
        const Vector2f operator+(const float scalar) const;
        const Vector2f operator-(const Vector2f& vector) const;
        const Vector2f operator-(const float scalar) const;
        const Vector2f operator*(const Vector2f& vector) const;
        const Vector2f operator*(const float scalar) const;
        const Vector2f operator/(const Vector2f& vector) const;
        const Vector2f operator/(const float scalar) const;
        const Vector2f operator%(const Vector2f& vector) const;
        const Vector2f operator%(const float scalar) const;
        bool operator==(const Vector2f& vector) const;
        bool operator!=(const Vector2f& vector) const;

        float magnitude() const;
        float direction() const;
        void setFromPolar(float mag,float dir);
        float dot(const Vector2f& vector) const;
        float cross(const Vector2f& vector) const;
        void normalize();
        void set(float nx,float ny);
        Vector2f getNormalized() const;

        float x,y;
};

class Vector3f
{
    public:
        Vector3f();
        Vector3f(float nx,float ny, float nz);
        Vector3f(const Vector3f& vector);

        Vector3f& operator=(const Vector3f& vector);
        Vector3f& operator+=(const Vector3f& vector);
        Vector3f& operator+=(const float scalar);
        Vector3f& operator-=(const Vector3f& vector);
        Vector3f& operator-=(const float scalar);
        Vector3f& operator*=(const Vector3f& vector);
        Vector3f& operator*=(const float scalar);
        Vector3f& operator/=(const Vector3f& vector);
        Vector3f& operator/=(const float scalar);
        Vector3f& operator%=(const Vector3f& vector);
        Vector3f& operator%=(const float scalar);
        Vector3f& floor();
        const Vector3f operator+(const Vector3f& vector) const;
        const Vector3f operator+(const float scalar) const;
        const Vector3f operator-(const Vector3f& vector) const;
        const Vector3f operator-(const float scalar) const;
        const Vector3f operator*(const Vector3f& vector) const;
        const Vector3f operator*(const float scalar) const;
        const Vector3f operator/(const Vector3f& vector) const;
        const Vector3f operator/(const float scalar) const;
        const Vector3f operator%(const Vector3f& vector) const;
        const Vector3f operator%(const float scalar) const;
        bool operator==(const Vector3f& vector) const;
        bool operator!=(const Vector3f& vector) const;

        float magnitude() const;
        float dot(const Vector3f& vector) const;
        Vector3f cross(const Vector3f& vector) const;
        void normalize();
        void set(float nx,float ny,float nz);
        Vector3f getNormalized() const;

        float x,y,z;
};

class Vector3s
{
    public:
        short x,y,z,padding;
};

class Vector2s
{
    public:
        short x,y;
};

#endif
