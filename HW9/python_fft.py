import matplotlib.pyplot as plt
import numpy as np

#dt = 1.0/10000.0 # 10kHz
#t = np.arange(0.0, 1.0, dt) # 10s
# a constant plus 100Hz and 1000Hz
#s = 4.0 * np.sin(2 * np.pi * 100 * t) + 0.25 * np.sin(2 * np.pi * 1000 * t) + 25

#gatting data from file
filename = 'sigD.csv' #change file name for other signals

#for moving average filter
#X= 10 

#for IIR filter
A = 0.95 #change for different cutoff frequencies, smaller is lower cutoff frequency
B = 1 - A

data = np.genfromtxt('sigD.csv', delimiter=',')  #change file name for other signals
t= data[:,0]
s= data[:,1]
Fs= len(t)/t[-1] # sample rate    

filtered = []

#----for average filter
#for i in range(len(s)):
#    if i < X:
#        avg = np.mean(s[0:i+1])
#    else:
#        avg = np.mean(s[i-X:i])
#    filtered.append(avg)

#----for IIR filter
for i in range(1, len(s)):
    new_value = A * filtered[i-1] + B * s[i]
    filtered.append(new_value)

filtered = np.array(filtered)

#For original FFT
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

#for filtered FFT 
Yf = np.fft.fft(filtered)/n # fft computing and normalization
Yf = Yf[range(int(n/2))]


fig, (ax1, ax2) = plt.subplots(2, 1)

ax1.plot(t,s,'b', label='Unfiltered')
ax1.plot(t,filtered,'r', label='Filtered')
ax1.set_xlabel('Time [s]')
ax1.set_ylabel('Amplitude')
ax1.set_title(filename + 'IIR Filter, A=' + str(A), ', B=' + str(B))
ax1.legend()

# plotting the fft
ax2.loglog(frq,abs(Y),'k', label='Unfiltered FFT') 
ax2.loglog(frq,abs(Yf),'r', label='Filtered FFT')
ax2.set_xlabel('Freq (Hz)')
ax2.set_ylabel('|Y(freq)|')
ax2.set_title(filename + ' FFT Comparison')
ax2.legend()

plt.tight_layout()
plt.show()