import json
import matplotlib.pyplot as plt
from matplotlib.widgets import Slider

with open(r"10minutes_fan_raw_data.json", "r") as f:
    data = json.load(f)

batch_data = data["sensor"]["batchAcceleration"]
merged_data = []
for key, entries in batch_data.items():
    merged_data.extend(entries)

merged_data.sort(key=lambda x: x["timestamp"])

timestamps = [entry["timestamp"] for entry in merged_data]
x_values = [entry["X"] for entry in merged_data]
y_values = [entry["Y"] for entry in merged_data]
z_values = [entry["Z"] for entry in merged_data]

fig, ax = plt.subplots(figsize=(14, 6))
plt.subplots_adjust(bottom=0.25)

line_x, = ax.plot([], [], label='X', color='r')
line_y, = ax.plot([], [], label='Y', color='g')
line_z, = ax.plot([], [], label='Z', color='b')

ax.set_xlabel("Timestamp")
ax.set_ylabel("Acceleration")
ax.set_title("Acceleration vs Time")
ax.grid(True)
ax.legend()

ax_slider = plt.axes([0.2, 0.1, 0.65, 0.03])
max_timestamp = max(timestamps)
slider = Slider(ax_slider, 'Max Timestamp', timestamps[0], max_timestamp, valinit=max_timestamp, valstep=1)

def update(val):
    max_time = slider.val
    indices = [i for i, t in enumerate(timestamps) if t <= max_time]
    
    if indices:
        last_index = indices[-1]
        line_x.set_data(timestamps[:last_index+1], x_values[:last_index+1])
        line_y.set_data(timestamps[:last_index+1], y_values[:last_index+1])
        line_z.set_data(timestamps[:last_index+1], z_values[:last_index+1])
        ax.set_xlim([timestamps[0], max_time])
        ax.set_ylim([
            min(x_values[:last_index+1] + y_values[:last_index+1] + z_values[:last_index+1]),
            max(x_values[:last_index+1] + y_values[:last_index+1] + z_values[:last_index+1])
        ])
        fig.canvas.draw_idle()

slider.on_changed(update)

update(max_timestamp)
plt.show()
