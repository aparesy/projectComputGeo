import matplotlib.pyplot as plt

def draw_input_from_file(f, ax):
    with open(f) as st:
        lines = st.readlines()
        x = [float(line.split()[0]) for line in lines]
        y = [float(line.split()[1]) for line in lines]
        ax.plot(x, y, "ob")

def draw_output(ax, f='resultsC.txt'):
    with open(f) as st:
        lines = st.readlines()
        x = [float(line.split()[0]) for line in lines]
        y = [float(line.split()[1]) for line in lines]
        ax.plot(x, y, "or")
        print(len(x))
        for i in range(len(x)-1):
            ax.plot([x[i], x[i+1]], [y[i], y[i+1]], "r")
        ax.plot([x[-1], x[0]], [y[-1], y[0]], "r")

# Create 2x2 subplot grid
fig, axes = plt.subplots(2, 2, figsize=(10, 10))

datasets = ['datasetA.txt', 'alt.txt', 'datasetC.txt', 'datasetD.txt']
results = ['resultsA.txt', 'alt.txt', 'resultsC.txt', 'resultsD.txt']

for ax, data, result in zip(axes.flatten(), datasets, results):
    draw_input_from_file(data, ax)
    draw_output(ax, result)
    ax.set_xlabel("x")
    ax.set_ylabel("y")
    ax.set_title(f"Convex hull for {data}")

plt.tight_layout()
plt.show()
