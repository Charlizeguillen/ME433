import matplotlib.pyplot as plt
import numpy as np

#dt = 1.0/10000.0 # 10kHz
#t = np.arange(0.0, 1.0, dt) # 10s
# a constant plus 100Hz and 1000Hz
#s = 4.0 * np.sin(2 * np.pi * 100 * t) + 0.25 * np.sin(2 * np.pi * 1000 * t) + 25

#gatting data from file for SigA 
data = np.genfromtxt('sigA.csv', delimiter=',')  #change file name for other signals
t= data[:,0]
s= data[:,1]
Fs= len(t)/t[-1] # sample rate    


#Fs = 10000 # sample rate
Ts = 1.0/Fs; # sampling interval
ts = np.arange(0,t[-1],Ts) # time vector
y = s # the data to make the fft from
n = len(y) # length of the signal

k = np.arange(n)
T = n/Fs
frq = k/T # two sides frequency range
frq = frq[range(int(n/2))] # one side frequency range

Y = np.fft.fft(y)/n # fft computing and normalization
Y = Y[range(int(n/2))]

fig, (ax1, ax2) = plt.subplots(2, 1)

ax1.plot(t,y,'b')
ax1.set_xlabel('Time [s]')
ax1.set_ylabel('Amplitude')
ax1.set_title('sigA Signal vs time')

# plotting the fft
ax2.loglog(frq,abs(Y),'b') 
ax2.set_xlabel('Freq (Hz)')
ax2.set_ylabel('|Y(freq)|')
ax2.set_title('sigA FFT')

plt.tight_layout()
plt.show()