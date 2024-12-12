//Definition of the Body object
#pragma once
class Body
{
public:
    struct vec
    {
        double x;
        double y;
        double z;

        vec operator+(vec vec2);
        vec operator-(vec vec2);
        vec operator*(vec vec2); //not the cross product but element multiplication
        vec operator*(double f);
        vec operator+=(vec vec2);

        double magnitude();
        vec CrossProduct(vec vec2);
    };

private:
    vec m_GlobalPosition;
    vec m_Velocity;

    double m_Mass;
public:
    vec GetGlobalPosition() { return m_GlobalPosition; };
    vec GetVelocity() { return m_Velocity; };

    double GetMass() {return m_Mass;};

    void SetGlobalPositon(Body::vec pos) { this->m_GlobalPosition = pos;};
    void setVelocity(Body::vec vel) { this->m_Velocity = vel;}

    Body(Body::vec, Body::vec, double);
};

Body::vec operator*(double f, Body::vec vec1);