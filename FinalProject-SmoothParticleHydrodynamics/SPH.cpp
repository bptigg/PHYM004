#include <iostream>

#include "Integrator.h"
#include "particle.h"

#define _USE_MATH_DEFINES
#include <cmath>

static std::vector<std::shared_ptr<Particle>> s_AllParticles;
static float s_h;

int DiracDelta(int a, int b)
{
    return a==b;
}

void KernalFunction(double q, double h, double& ReturnValue, double& derivative) //(pos of a - pos of b, smoothing length)
{
    if(q <= 1)
    {
        ReturnValue = 1 - (3.0/2.0 * std::pow(q,2)) + (3.0/4.0 * std::pow(q,4));
        derivative =  -3.0 * q + 3.0*std::pow(q,3);
    }
    else if(q <= 2)
    {
        ReturnValue = 0.25 * std::pow(2.0-q,3);
        derivative = 1.5 * std::pow(2.0 - q, 2);
    }
    else
    {
        ReturnValue = 0;
        derivative = 0;
    }

    ReturnValue = ReturnValue * (1.0/h);
    derivative = derivative * (1.0/std::pow(h,2)); 
}

void KernalEvaulation(int a)
{
    auto TargetParticle = s_AllParticles[a];
    std::vector<Kernal> KernalEvaulations;
    TargetParticle->ClearKernal();
    for(int b = 0; b < s_AllParticles.size(); b++)
    {
        double RadiusDiff = TargetParticle->GetX() - s_AllParticles[b]->GetX();
        double ReturnValue = 0;
        double DerivativeValue = 0;
        KernalFunction(RadiusDiff, s_h, ReturnValue, DerivativeValue);
        KernalEvaulations.push_back({ReturnValue, DerivativeValue});
    }
    TargetParticle->UpdateKernal(KernalEvaulations);
}

void EnergyEvaluation(int a)
{
    //In 1D
    //Pressure/Density * sum(mass of b * (velocity of a - velocity of b) * (kernal vector / magnitude of kernal)
    //auto Targ
}

void AccelerationEvaluation(int a)
{
    //In 1D
    //-1 * sum(mass of b * (Pressure of a/density of a^2 + Pressure of b/density of b^2) * (kernal vector / magnitude of kernal))
    auto TargetParticle = s_AllParticles[a];
    auto Kernal = TargetParticle->GetKernal();
    auto Pressure = TargetParticle->GetP();
    auto Density = TargetParticle->GetRho();
    double ValA = Pressure * (1 / std::pow(Density,2));
    double acceleration = 0.0;
    for(int b = 0; b < s_AllParticles.size(); b++)
    {
        double Mass = s_AllParticles[b]->GetMass();
        double ValB = s_AllParticles[b]->GetP() * (1 / std::pow(s_AllParticles[b]->GetRho(),2));
        double AccAB = Mass * (ValA + ValB) * Kernal[b].second * (Kernal[b].first / std::abs(Kernal[b].first)) * (DiracDelta(a,a) - DiracDelta(a,b));
        acceleration += AccAB;
    }

    TargetParticle->UpdateA(acceleration);
}

void DensityEvauluation(int a)
{
    //In ID
    //sum(mass of b * kernal function)
    auto TargetParticle = s_AllParticles[a];
    auto Kernal = TargetParticle->GetKernal();
    double density = 0.0;
    for(int b = 0; b < s_AllParticles.size(); b++)
    {
        double Mass = s_AllParticles[b]->GetMass();
        density += Kernal[b].first * Mass;
    }
    TargetParticle->UpdateRho(density);
}

int main()
{
    std::cin.get();
    return 0;
}