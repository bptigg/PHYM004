import numpy as np
from matplotlib import pyplot as plt

file = "ShockTubeResults13"
EP = np.loadtxt(file + "_ENERGY_MOMENTUM.txt")
X = np.loadtxt(file + "_POSITION.txt")
D = np.loadtxt(file + "_DENSITY.txt")
V = np.loadtxt(file + "_VELOCITY.txt")
U = np.loadtxt(file + "_ENERGY_POSITION.txt")
P = np.loadtxt(file + "_PRESSURE_POSITION.txt")

timestep = 0.00005
time = []

for i in EP:
    time.append(i[0] * timestep)

data = [EP,X,D,V,U,P]
ModifiedData = []
for d in data:
    temp = []
    for i in d:
        temp.append(i[1:len(i)])
    ModifiedData.append(temp)

Momentum = []
KineticEnergy = []
ThermalEnergy = []

for d in ModifiedData[0]:
    KineticEnergy.append(d[0])
    ThermalEnergy.append(d[1])
    Momentum.append(d[2])

Energy = []
for i in range(0,len(KineticEnergy)):
    Energy.append(KineticEnergy[i] + ThermalEnergy[i])
plt.plot(time, Energy)
plt.xlabel("Time(s)")
plt.ylabel(r'$E$')
plt.show()

Change = [[]]
Value = KineticEnergy[0]
for i in range(1,len(KineticEnergy)):
    Change[0].append((KineticEnergy[i] - Value)*(1))
    Value = KineticEnergy[i]
Change.append([])
Value = ThermalEnergy[0]
for i in range(1,len(ThermalEnergy)):
    Change[1].append((ThermalEnergy[i] - Value)*(1))
    Value = ThermalEnergy[i]
Change.append([])
Value = Momentum[0]
for i in range(1,len(Momentum)):
    Change[2].append(Momentum[i] - Value)
    Value = Momentum[i]
Change.append([])
for i in range(0,len(Change[0])):
    Change[3].append(Change[1][i] + Change[0][i])


plt.plot(time[1:len(Change[0])+1], Change[0], label = "KE")
plt.plot(time[1:len(Change[0])+1], Change[1], label = "U")
plt.plot(time[1:len(Change[0])+1], Change[3], label = "Total Energy")
plt.xlabel("Time(s)")
plt.ylabel(r'$\Delta E$')
plt.legend()
plt.show()

plt.plot(time[1:len(Change[0])+1], Change[2])
plt.xlabel("Time(s)")
plt.ylabel("Change in Momentum")
plt.show()

plt.plot(time, Momentum)
plt.xlabel("Time(s)")
plt.ylabel("Momentum")
plt.show()


x = []
rho = []
velocity = []
U = []
P = []

for i in range(0,len(ModifiedData[1][len(ModifiedData[1])-1])):
    x.append(ModifiedData[1][len(ModifiedData[1])-1][i])
    rho.append(ModifiedData[2][len(ModifiedData[2])-1][i])
    velocity.append(ModifiedData[3][len(ModifiedData[2])-1][i])
    U.append(ModifiedData[4][len(ModifiedData[2])-1][i])
    P.append(ModifiedData[5][len(ModifiedData[2])-1][i])
    #x.append(ModifiedData[1][0][i])
    #rho.append(ModifiedData[2][0][i])
    #velocity.append(ModifiedData[3][0][i])
    #U.append(ModifiedData[4][0][i])
    #P.append(ModifiedData[5][0][i])

y = np.zeros(len(x))
plt.scatter(x,y)
plt.show()

plt.scatter(x,rho)
plt.xlabel("Position")
plt.ylabel("Density")
plt.show()

plt.scatter(x,velocity)
plt.xlabel("Position")
plt.ylabel("Velocity")
plt.show()

plt.scatter(x,U)
plt.xlabel("Position")
plt.ylabel("Internal Energy")
plt.show()

plt.scatter(x,P)
plt.xlabel("Position")
plt.ylabel("Pressure")
plt.show()



print(EP)