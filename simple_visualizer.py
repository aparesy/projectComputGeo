import matplotlib.pyplot as plt

def draw_input_from_file(f):
    with open(f) as st:
        lines = st.readlines()
        x = [float(line.split()[0]) for line in lines]
        y = [float(line.split()[1]) for line in lines]
        plt.plot(x,y, "ob")

def draw_output(f = 'output.txt'):
    with open(f) as st:
        lines = st.readlines()
        x = [float(line.split()[0]) for line in lines]
        y = [float(line.split()[1]) for line in lines]
        plt.plot(x,y, "or")
        print(len(x))
        for i in range(len(x)-1):
            plt.plot([x[i], x[i+1]], [y[i], y[i+1]], "r")
        plt.plot([x[-1], x[0]], [y[-1], y[0]], "r")

plt.xlabel("x")
plt.ylabel("y")
plt.title("Convex hull by sweeping")
draw_input_from_file('datasetB.txt')
draw_output()
plt.show()
