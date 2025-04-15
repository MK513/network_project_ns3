import matplotlib.pyplot as plt
 # Loadthedatafromtwodatafilesmanually
with open('./flow1.dat', 'r') as file:
    data1 = file.readlines()

flow1_time = []
flow1_value = []
for line in data1:
    parts= line.split()
    flow1_time.append(float(parts[0]))
    flow1_value.append(float(parts[1]))

with open('./flow2.dat', 'r') as file:
    data2 = file.readlines()

flow2_time = []
flow2_value = []
for line in data2:
    parts= line.split()
    flow2_time.append(float(parts[0]))
    flow2_value.append(float(parts[1]))
 # Plotbothflows
plt.figure(figsize=(10, 5))
plt.plot(flow1_time, flow1_value, label='Flow1', marker='o', linestyle='-')
plt.plot(flow2_time, flow2_value, label='Flow2', marker='o', linestyle='-')
 # Addingtitleand labels
plt.title('2020312852')
plt.xlabel('Time')
plt.ylabel('Value')
plt.legend() # Showlegend
# output_file= './week5_star.png' # Savetheplotasa.pngfile
# plt.savefig(output_file)