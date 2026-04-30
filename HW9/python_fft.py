import matplotlib.pyplot as plt
import numpy as np

#dt = 1.0/10000.0 # 10kHz
#t = np.arange(0.0, 1.0, dt) # 10s
# a constant plus 100Hz and 1000Hz
#s = 4.0 * np.sin(2 * np.pi * 100 * t) + 0.25 * np.sin(2 * np.pi * 1000 * t) + 25

#gatting data from file
filename = 'sigC.csv' #change file 

#for moving average filter
#X= 10 

#for IIR filter
A = 0.9
B = 0.1

data = np.genfromtxt(filename, delimiter=',')  
t= data[:,0]
s= data[:,1]
Fs= len(t)/t[-1] # sample rate 
print('Sample rate: ' + str(Fs) + ' Hz')   

#filtered = []
h = np.array([0.000550283536446716,
    0.000669295562481363,
    0.000725049084885430,
    0.000695746364512743,
    0.000553181037731246,
    0.000271460844301492,
    -0.000160118396348936,
    -0.000723290750324895,
    -0.001360327841953853,
    -0.001971027653998972,
    -0.002420289983048358,
    -0.002557336588935020,
    -0.002244973835174325,
    -0.001394546873944762,
    0.000000000000000002,
    0.001836701746656515,
    0.003896222513431482,
    0.005854090584586322,
    0.007316683424242810,
    0.007878835452694664,
    0.007196428844128102,
    0.005063438453233345,
    0.001480445790163525,
    -0.003298766762015542,
    -0.008753984670937607,
    -0.014125947667245113,
    -0.018485698539391957,
    -0.020840478812437453,
    -0.020264235026079800,
    -0.016035646196701506,
    -0.007763768307303201,
    0.004518449922367797,
    0.020309223323408971,
    0.038651901165652057,
    0.058210123107134283,
    0.077397576433913304,
    0.094547499875283325,
    0.108100711754370901,
    0.116787530891422020,
    0.119779116385585874,
    0.116787530891422034,
    0.108100711754370915,
    0.094547499875283325,
    0.077397576433913304,
    0.058210123107134283,
    0.038651901165652057,
    0.020309223323408974,
    0.004518449922367797,
    -0.007763768307303203,
    -0.016035646196701506,
    -0.020264235026079797,
    -0.020840478812437453,
    -0.018485698539391950,
    -0.014125947667245110,
    -0.008753984670937609,
    -0.003298766762015542,
    0.001480445790163525,
    0.005063438453233348,
    0.007196428844128102,
    0.007878835452694667,
    0.007316683424242815,
    0.005854090584586324,
    0.003896222513431484,
    0.001836701746656515,
    0.000000000000000002,
    -0.001394546873944764,
    -0.002244973835174325,
    -0.002557336588935024,
    -0.002420289983048359,
    -0.001971027653998974,
    -0.001360327841953853,
    -0.000723290750324895,
    -0.000160118396348936,
    0.000271460844301492,
    0.000553181037731246,
    0.000695746364512743,
    0.000725049084885430,
    0.000669295562481363,
    0.000550283536446716])   # paste from website
filtered = np.convolve(s, h, mode='same')

#----for average filter
#for i in range(len(s)):
#    if i < X:
#        avg = np.mean(s[0:i+1])
#    else:
#        avg = np.mean(s[i-X:i])
#    filtered.append(avg)

#----for IIR filter
#filtered.append(s[0]) #initial value 
#for i in range(1, len(s)):
#    new_value = A * filtered[i-1] + B * s[i]
#    filtered.append(new_value)


#filtered = np.array(filtered)

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

ax1.plot(t,s,'k', label='Unfiltered')
ax1.plot(t,filtered,'r', label='Filtered')
ax1.set_xlabel('Time [s]')
ax1.set_ylabel('Amplitude')
ax1.set_title(f'{filename} FIR (Hamming), taps={len(h)}, fc=150 Hz, bw=100 Hz')
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