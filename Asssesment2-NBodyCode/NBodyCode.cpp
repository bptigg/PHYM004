
#include "Integrator.h"
#include "math.h"
#include "FileReader.h"

#define G 6.67430e-11
static double s_Epsilon = 0.0;

//This is file is the contains the main loop of the code found in the main function on line 111, it tells the integrators to do one more step and updates the write limit for the observor
//the two work functions are functions passed to other threads but  with objects attatched to them rather than just passing the function ptr's 
//Force calculation calculates the force on each body using the direct evauluation method

void work(Integrator& ig)
{
    ig.DoStep();
}

void WriteToFile(std::shared_ptr<EnergyObserver> ob)
{
    ob->WriteToFile();
}

Body::vec ForceCalculation(int CurrentBody, std::vector<std::shared_ptr<Body>> BodyVec)
{
    Body::vec acc = {0,0,0};

    auto force = [](std::shared_ptr<Body> b1, std::shared_ptr<Body> b2)
    {
        double epsilon = s_Epsilon;
        auto b2Vec = b2->GetGlobalPosition();
        auto b1Vec = b1->GetGlobalPosition();
        Body::vec rVec = {b2Vec.x - b1Vec.x, b2Vec.y - b1Vec.y, b2Vec.z - b1Vec.z};
        double r = std::sqrt(std::pow(rVec.x,2) + std::pow(rVec.y,2) + std::pow(rVec.z,2));

        //double force = G * ((b1->GetMass() * b2->GetMass()) / std::sqrt(std::pow(r,6) + std::pow(epsilon,2)));
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

int main(int argc, char* argv[])
{
    FileReader setup;
    std::string file = "";
    if(argc == 1)
        file = "setup.txt";
    else
        file = argv[1];
    setup.LoadFile(file);

    auto Bdat = setup.GetBodyData();
    auto Sdat = setup.GetSetupData();

    std::shared_ptr<IntergatorLockGuard> ILG = std::make_shared<IntergatorLockGuard>(Bdat.size());
    std::shared_ptr<OutputLockGuard> OLG = std::make_shared<OutputLockGuard>();
    std::shared_ptr<EnergyObserver> EOb = std::make_shared<EnergyObserver>(Bdat.size(),Sdat.steps);
    std::vector<Integrator> IntegratorVec = {};
    std::vector<std::thread> Threads = {};

    std::vector<std::shared_ptr<Body>> BVec = {};

    for(int i = 0; i < Bdat.size(); i++)
    {
        Body::vec pos = {Bdat[i].pos[0], Bdat[i].pos[1], Bdat[i].pos[2]};
        Body::vec vel = {Bdat[i].vel[0], Bdat[i].vel[1], Bdat[i].vel[2]};
        BVec.push_back(std::make_shared<Body>(pos,vel,Bdat[i].mass));
        IntegratorVec.push_back(Integrator({pos,vel,{0.0,0.0,0.0}}, ForceCalculation,Sdat.timestep, Sdat.timestep * Sdat.steps, i, ILG, EOb));
    }
    for(int i = 0; i < Bdat.size(); i++)
    {
        IntegratorVec[i].SetBodyVec(BVec);
    }

    //Body::vec pos1 = {0.0,0,0};
    //Body::vec vel1 = {0.000,0.000,0}; 
    //BVec.push_back(std::make_shared<Body>(pos1,vel1,2e10));
    //IntegratorVec.push_back(Integrator({pos1,vel1,{0.0,0.0,0.0}},ForceCalculation,0.1,1.0,0,ILG,EOb));
    //Body::vec pos2 = {10,0,0};
    //Body::vec vel2 = {0,0.5,0}; 
    //BVec.push_back(std::make_shared<Body>(pos2,vel2,1e5));
    //IntegratorVec.push_back(Integrator({pos2,vel2,{0.0,0.0,0.0}},ForceCalculation,0.1,1.0,1,ILG,EOb));
    //IntegratorVec[0].SetBodyVec(BVec);
    //IntegratorVec[1].SetBodyVec(BVec);

    EOb->AtttachLockGuard(OLG);
    EOb->AttachOutputFile(Sdat.OutputFile);
    s_Epsilon = Sdat.epsilon;
    EOb->SetEpsilon(s_Epsilon);

    //for(int i = 0; i < 2; i++)
    //{
    //    IntegratorVec.push_back(Integrator(0.01,1,ILG,i));
    //}
    std::thread FileWrite(WriteToFile, EOb);
    EOb->Reset();
    for(int e = 0; e < Sdat.steps; e++)
    {
        Threads.clear();
        ILG->Reset();
        for(int i = 0; i < Bdat.size(); i++)
        {
            std::thread t1(work, std::ref(IntegratorVec[i]));
            Threads.push_back(std::move(t1));
            //std::this_thread::sleep_for(std::chrono::microseconds(1));
        }
        for(int i = 0; i < Bdat.size(); i++)
        {
            Threads[i].join();
        }

        OLG->UpdateWriteLimit(e+1);
        EOb->Reset();
    }
    OLG->UpdateKill(true);
    FileWrite.join();
}