import numpy as np
from matplotlib import pyplot as plt

file = "ShockTubeResults7"
EP = np.loadtxt(file + "_ENERGY_MOMENTUM.txt")
X = np.loadtxt(file + "_POSITION.txt")
D = np.loadtxt(file + "_DENSITY.txt")
V = np.loadtxt(file + "_VELOCITY.txt")
U = np.loadtxt(file + "_ENERGY_POSITION.txt")

data = [EP,X,D,V,U]
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

Change = [[]]
Value = KineticEnergy[0]
for i in range(1,len(KineticEnergy)):
    Change[0].append(KineticEnergy[i] - Value)
    Value = KineticEnergy[i]
Change.append([])
Value = ThermalEnergy[0]
for i in range(1,len(ThermalEnergy)):
    Change[1].append(ThermalEnergy[i] - Value)
    Value = ThermalEnergy[i]
Change.append([])
Value = Momentum[0]
for i in range(1,len(Momentum)):
    Change[2].append(Momentum[i] - Value)
    Value = Momentum[i]
Change.append([])
for i in range(0,len(Change[0])):
    Change[3].append(Change[1][i] + Change[0][i])


plt.plot(Change[0], label = "KE")
plt.plot(Change[1], label = "U")
plt.plot(Change[3], label = "Total Energy")
plt.xlabel("Steps")
plt.ylabel(r'$\Delta E$')
plt.legend()
plt.show()

plt.plot(Change[2])
plt.show()


x = []
rho = []
velocity = []
U = []

for i in range(0,len(ModifiedData[1][len(ModifiedData[1])-1])):
    x.append(ModifiedData[1][len(ModifiedData[1])-1][i])
    rho.append(ModifiedData[2][len(ModifiedData[2])-1][i])
    velocity.append(ModifiedData[3][len(ModifiedData[2])-1][i])
    U.append(ModifiedData[4][len(ModifiedData[2])-1][i])
    #x.append(ModifiedData[1][0][i])
    #rho.append(ModifiedData[2][0][i])
    #velocity.append(ModifiedData[3][0][i])
    #U.append(ModifiedData[4][0][i])

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



print(EP)