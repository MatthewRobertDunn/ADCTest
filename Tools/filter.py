import numpy as np
import scipy
from scipy.signal import remez, freqz
import matplotlib.pyplot as plt
print(f"SciPy version being used by script: {scipy.__version__}") # <--- ADDED THIS LINE


def plot_response(w, h, title):
    fig = plt.figure()
    ax = fig.add_subplot(111)
    ax.plot(w, 20*np.log10(np.abs(h)))
    ax.set_ylim(-40, 5)
    ax.grid(True)
    ax.set_xlabel('Frequency (Hz)')
    ax.set_ylabel('Gain (dB)')
    ax.set_title(title)

fs = 200000 #sampling rate
band = [10000, 12000]  # Desired pass band, Hz
trans_width = 3000    # Width of transition from pass to stop, Hz
numtaps = 65         # Size of the FIR filter.
edges = [0, band[0] - trans_width, band[0], band[1],
         band[1] + trans_width, 0.5*fs]
taps = remez(numtaps, edges, [0, 1, 0], fs=fs)

w, h = freqz(taps, [1], worN=100000, fs=fs)

# Compute response and normalize
w, h = freqz(taps, [1], worN=100000, fs=fs)

# Find max gain in the passband (you know your passband edges)
passband_indices = np.logical_and(w >= band[0], w <= band[1])
peak_gain = np.abs(h[passband_indices]).max()

# Normalize taps so passband peak gain == 1
taps /= peak_gain


# Compute response and graph
w, h = freqz(taps, [1], worN=100000, fs=fs)
plot_response(w, h, "Band-pass Filter")


# Assume taps is your array of floats in range [-1.0, 1.0)
# Scale to Q1.15 (signed 16-bit integer)
taps_q15 = np.round(taps * 32768).astype(np.int16)

print(*taps_q15, sep=", ") # <--- ADDED THIS LINEtaps, sep=", ")
plt.show()
