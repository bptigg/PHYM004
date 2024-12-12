
#include "Integrator.h"
#include "math.h"

#define G 6.67430e-11

void work(Integrator& ig)
{
    ig.DoStep();
}

Body::vec ForceCalculation(int CurrentBody, std::vector<std::shared_ptr<Body>> BodyVec)
{
    Body::vec acc = {0,0,0};

    auto force = [](std::shared_ptr<Body> b1, std::shared_ptr<Body> b2)
    {
        double epsilon = 1e-5;
        auto b2Vec = b2->GetGlobalPosition();
        auto b1Vec = b1->GetGlobalPosition();
        Body::vec rVec = {b2Vec.x - b1Vec.x, b2Vec.y - b1Vec.y, b2Vec.z - b1Vec.z};
        double r = std::sqrt(std::pow(rVec.x,2) + std::pow(rVec.y,2) + std::pow(rVec.z,2));

        double force = G * ((b1->GetMass() * b2->GetMass()) / (std::pow(r,3) + epsilon));
        
        Body::vec ForceVector = force * rVec;
        return ForceVector;
    };

    for (auto a = BodyVec.begin(); a != BodyVec.end(); a++)
    {
        if(a - BodyVec.begin() == CurrentBody)
        {
            continue;
        }

        acc += force(BodyVec[CurrentBody], (*a));
    }
    acc = acc * (1 / BodyVec[CurrentBody]->GetMass());


    return acc;
}

int main()
{
    std::shared_ptr<IntergatorLockGuard> ILG = std::make_shared<IntergatorLockGuard>(2);
    std::shared_ptr<EnergyObserver> EOb = std::make_shared<EnergyObserver>(2);
    std::vector<Integrator> IntegratorVec = {};
    std::vector<std::thread> Threads = {};

    std::vector<std::shared_ptr<Body>> BVec = {};
    
    Body::vec pos1 = {0.0,0,0};
    Body::vec vel1 = {0.000,0.000,0}; 
    BVec.push_back(std::make_shared<Body>(pos1,vel1,2e10));
    IntegratorVec.push_back(Integrator({pos1,vel1,{0.0,0.0,0.0}},ForceCalculation,0.1,1.0,0,ILG,EOb));
    Body::vec pos2 = {10,0,0};
    Body::vec vel2 = {0,0.5,0}; 
    BVec.push_back(std::make_shared<Body>(pos2,vel2,1e5));
    IntegratorVec.push_back(Integrator({pos2,vel2,{0.0,0.0,0.0}},ForceCalculation,0.1,1.0,1,ILG,EOb));
    IntegratorVec[0].SetBodyVec(BVec);
    IntegratorVec[1].SetBodyVec(BVec);

    //for(int i = 0; i < 2; i++)
    //{
    //    IntegratorVec.push_back(Integrator(0.01,1,ILG,i));
    //}
    for(int e = 0; e < 100000; e++)
    {
        Threads.clear();
        ILG->Reset();
        for(int i = 0; i < 2; i++)
        {
            std::thread t1(work, std::ref(IntegratorVec[i]));
            Threads.push_back(std::move(t1));
            std::this_thread::sleep_for(std::chrono::microseconds(1));
        }
        for(int i = 0; i < 2; i++)
        {
            Threads[i].join();
        }

        EOb->CalculateEnergy();
        EOb->CalculateAngularMomentum();
        EOb->Reset();
    }
}