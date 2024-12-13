import pandas as pd

def main():
    filename = "test.txt"
    file = open(filename, "r")
    lines = []
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

main()