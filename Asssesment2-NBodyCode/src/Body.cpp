#include "Body.h"

#include <math.h>

Body::vec Body::vec::operator+(vec pos2) //adds two vectors 
{
    Body::vec ReturnBody;
    ReturnBody.x = this->x + pos2.x;
    ReturnBody.y = this->y + pos2.y;
    ReturnBody.z = this->z + pos2.z;
    return ReturnBody;
}

Body::vec Body::vec::operator-(vec vec2) //subtracs two vectors
{
    Body::vec ReturnBody;
    ReturnBody.x = this->x - vec2.x;
    ReturnBody.y = this->y - vec2.y;
    ReturnBody.z = this->z - vec2.z;
    return ReturnBody;
}

Body::vec Body::vec::operator*(vec vec2) //element wise multiplication of two vectors 
{
    Body::vec ReturnBody;
    ReturnBody.x = this->x * vec2.x;
    ReturnBody.y = this->y * vec2.y;
    ReturnBody.z = this->z * vec2.z;
    return ReturnBody;
}

Body::vec Body::vec::operator*(double f) //element wise multiplication of a vector with a scaler 
{
    Body::vec ReturnBody;
    ReturnBody.x = this->x * f;
    ReturnBody.y = this->y * f;
    ReturnBody.z = this->z * f;
    return ReturnBody;
}

Body::vec Body::vec::operator+=(vec vec2) //adding to the current vector
{
    this->x += vec2.x;
    this->y += vec2.y;
    this->z += vec2.z;
}

double Body::vec::magnitude()
{
    return std::sqrt(std::pow(this->x,2) + std::pow(this->y,2) + std::pow(this->z,2));
}

Body::vec Body::vec::CrossProduct(vec vec2)
{
    vec result;
    double r1 = (this->y * vec2.z) - (this->z * vec2.y);
    double r2 = (this->z * vec2.x) - (this->x * vec2.z);
    double r3 = (this->x * vec2.y) - (this->y * vec2.x);
    result = {r1,r2,r3};
    return result;
}

std::ostream& operator<<(std::ostream& os, Body::vec &v) //printing vectors, usefull for when printing to a file 
{
    os << v.magnitude() << " " << v.x << " " << v.y << " " << v.z;
    return os;
}

Body::vec operator*(double f, Body::vec vec1) //calls the element wise multiplication 
{
    return vec1 * f;
}

Body::Body(Body::vec pos, Body::vec vel, double mass)
    :m_GlobalPosition(pos), m_Velocity(vel), m_Mass(mass)
{
}
