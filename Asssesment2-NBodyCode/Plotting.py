import pandas as pd
import matplotlib.pyplot as plt
from scipy import constants
import math

def main():
    #filename = "yingyangresult.txt"
    #filename = "Butterfly3result2.txt"
    filename = "EarthSunMoonResults.txt"
    file = open(filename, "r")
    param = []

    for i in range(0,2):
        line = file.readline()
        value = False
        buffer = ""
        for c in line:
            if(c == ':'):
                value = True
                continue
            if(c == '\n'):
                param.append(int(buffer))
                buffer = ""
                value = False
            if(value):
                buffer += c

    file.close()
    data = pd.read_csv(filename, sep = " ", skiprows=2)
    df = pd.DataFrame(data)
    print(df)
    print(df.columns)

    timesteps = []
    ts = df.get("timestep(s)")
    for i in range(0,ts.size):
        timesteps.append(ts[i])
    energy = []
    es = df.get("energy")
    for i in range(0,es.size):
        energy.append(es[i])
    AngularMomentum = []
    As = df.get("angular_momentum_magnitude")
    AsL = [df.get("L_x"), df.get("L_y"), df.get("L_z")]
    for i in range(0,As.size):
        AngularMomentum.append(As[i])

    AngularMomentumComponents = [[],[],[]]

    a  = 0
    for Asi in AsL:
        for i in range(0,Asi.size):
            AngularMomentumComponents[a].append(Asi[i])
        a += 1

    positions = []
    for i in range(0,param[0]):
        positions.append([])
        posr = df.get("body" + str(i) + "_radius")
        posx = df.get("body" + str(i) + ".x")
        posy = df.get("body" + str(i) + ".y")
        posz = df.get("body" + str(i) + ".z")

        for e in range(0,posx.size):
            positions[i].append([posr[e],posx[e], posy[e], posz[e]])
        
    ax = plt.figure().add_subplot(projection = '3d')

    rad = []
    rad_index = []

    for i in range(0,param[0]):
        r = [e[0] for e in positions[i]]
        x = [e[1] for e in positions[i]]
        y = [e[2] for e in positions[i]]
        z = [e[3] for e in positions[i]]

        x_max = max(x)
        y_max = max(y)
        z_max = max(z)
        print(str(x_max) + "," + str(y_max) + "," + str(z_max))
        print(str(x.index(x_max)) + "," + str(y.index(y_max)) + "," + str(z.index(z_max)))

        r_max = max(r, key = abs)
        r_min = min(r, key = abs)
        print(str(r_max) + "," + str(r_min))
        ax.plot(x,y,z, lw=1)

        if(param[0] != 2):
            continue
        if(i == 0):
            index = r.index(r_max)
            ax.plot([0,x[index]],[0,y[index]],[0,z[index]], lw = 1, label = r_max, linestyle = ':')
            index = r.index(r_min)
            ax.plot([0,x[index]],[0,y[index]],[0,z[index]], lw = 1, label = r_min, linestyle = ':')
            rad_index = [r.index(r_max), r.index(r_min)]
            rad = [r_max, r_min]
        if(i == 1):
            rad[0] = rad[0] - r[0]
            rad[1] = rad[1] - r[1]

    
    if(param[0] == 2):
        u1 = -0.5 * constants.G * ((5.9722e24 * 1.9891e30)/rad[0])
        u2 = -0.5 * constants.G * ((5.9722e24 * 1.9891e30)/rad[1])

        e1 = energy[rad_index[0]] - u1
        e2 = energy[rad_index[1]] - u2

        k1 = math.sqrt((e1 * 2)/5.9722e24)
        k2 = math.sqrt((e2 * 2)/5.9722e24)

        print("Total= ", energy[rad_index[0]], " grav= ", u1, " kinetic= ", e1, " velocity= ", k1)
        print("Total= ", energy[rad_index[0]], " grav= ", u2, " kinetic= ", e2, " velocity= ", k2)


    
    ax.legend(loc = 'upper right')
    ax.set_xlabel("X Axis")
    ax.set_ylabel("Y Axis")
    ax.set_zlabel("Z Axis")

    change = 0
    for i in range(0,len(energy)-1):
        change += energy[i+1] - energy[i]

    energyax = plt.figure().add_subplot(111)
    energyax.plot(timesteps,energy,lw = 1,label = "Change in energy: " + str(change))
    energyax.set_xlabel("Time(s)")
    energyax.set_ylabel("Energy(J)")
    energyax.legend(loc = 'upper right')

    residulals = []
    a = 0
    for Axi in AngularMomentumComponents:
        current_val = Axi[0]
        residulal = 0
        for i in range(0,len(Axi)):
            residulal = residulal + math.pow((Axi[i] - current_val),2)
        residulals.append(residulal)
        a += 1
    
    residulal = 0
    for i in range(0,len(AngularMomentum)):
        residulal = residulal + math.pow((AngularMomentum[i] - AngularMomentum[0]),2)
    
    AngularMomentumax = plt.figure().add_subplot(111)
    AngularMomentumax.plot(timesteps,AngularMomentum, lw = 1, label = "Magnitude, Residuals squared = " + str(residulal))
    AngularMomentumax.plot(timesteps,AngularMomentumComponents[0], lw = 1, label = "Lx, Residuals squared = " + str(residulals[0]))
    AngularMomentumax.plot(timesteps,AngularMomentumComponents[1], lw = 1, label = "Ly, Residuals squared = " + str(residulals[1]))
    AngularMomentumax.plot(timesteps,AngularMomentumComponents[2], lw = 1, label = "Lz, Residuals squared = " + str(residulals[2]))
    AngularMomentumax.set_xlabel("Time(s)")
    AngularMomentumax.set_ylabel("Angualar Momentum (kgm^2/s)")
    AngularMomentumax.legend(loc = 'upper right')


    plt.show()


    
    #print(timesteps)
    



main()