class Vector2 {
public:
    float x, y;

    // Constructors
    Vector2() : x(0), y(0) {}
    Vector2(float x, float y) : x(x), y(y) {}

    // Operator overloading
    Vector2 operator+(const Vector2 &v) const {
        return Vector2(x + v.x, y + v.y);
    }

    Vector2 operator-(const Vector2 &v) const {
        return Vector2(x - v.x, y - v.y);
    }

    Vector2 operator*(float scalar) const {
        return Vector2(x * scalar, y * scalar);
    }

    Vector2 operator/(float scalar) const {
        return Vector2(x / scalar, y / scalar);
    }

    // Dot product
    float dot(const Vector2 &v) const {
        return x * v.x + y * v.y;
    }

    // Length of the vector
    float length() const {
        return std::sqrt(x * x + y * y);
    }

    // Normalize the vector
    Vector2 normalized() const {
        float len = length();
        return Vector2(x / len, y / len);
    }
};