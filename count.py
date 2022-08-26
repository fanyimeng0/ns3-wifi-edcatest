import os
from readline import append_history_file
import matplotlib.pyplot as plt
import numpy as np

#Read data from datfile,return VI Ddelay#
def readfile(pathname):
  f=open(pathname)
  VIdelay = []
  VOdelay = []
  BEdelay = []
  BKdelay = []
  for line in f:
      s = line.strip()
      if s[0:2]=='VI':
        VIdelay.append(s[10:-1])
      if s[0:2]=='VO':
        VOdelay.append(s[10:-1])
      if s[0:2]=='BE':
        BEdelay.append(s[10:-1])
      if s[0:2]=='BK':
        BKdelay.append(s[10:-1])
  f.close()
  x=[]
  y=[]
  x1=[]
  y1=[]
  x2=[]
  y2=[]
  x3=[]
  y3=[]
  for num in range(0,len(VIdelay)):
      try:
        m=float(VIdelay[num])
        y.append(m)
        x.append(num+1);
      except ValueError:
          pass
  for num in range(0,len(VOdelay)):
      try:
        m=float(VOdelay[num])
        y1.append(m)
        x1.append(num+1);
      except ValueError:
          pass
  for num in range(0,len(BKdelay)):
      try:
        m=float(BKdelay[num])
        y2.append(m)
        x2.append(num+1);
      except ValueError:
          pass
  for num in range(0,len(BEdelay)):
      try:
        m=float(BEdelay[num])
        y3.append(m)
        x3.append(num+1);
      except ValueError:
          pass
  return y

#Get the cdf of the delay
def getcdf(y):
  hist, bin_edges = np.histogram(y,bins=1000)
  cdf = np.cumsum(hist/sum(hist))
  edges=np.delete(bin_edges,[0])
  return cdf,edges

y1=readfile("/home/mfy/ns-allinone-3.33/ns-3.33/delay523.dat")
y2=readfile("/home/mfy/ns-allinone-3.33/ns-3.33/delay524.dat")
y3=readfile("/home/mfy/ns-allinone-3.33/ns-3.33/delay525.dat")
y4=readfile("/home/mfy/ns-allinone-3.33/ns-3.33/delay526.dat")
y5=readfile("/home/mfy/ns-allinone-3.33/ns-3.33/delay5.dat")
y51=readfile("/home/mfy/ns-allinone-3.33/ns-3.33/delay51.dat")
plt.figure()
plt.subplot(1,2,1)
plt.scatter(range(0,len(y5)),y5)
plt.title("Packet delay")
plt.ylabel("delay(ms)")


[cdf1,edge1]=getcdf(y1)
[cdf2,edge2]=getcdf(y2)
[cdf3,edge3]=getcdf(y3)
[cdf4,edge4]=getcdf(y4)
[cdf5,edge5]=getcdf(y5)
[cdf51,edge51]=getcdf(y51)

plt.subplot(1,2,2)
plt.plot(edge1,cdf1,label='MCS = 3')
plt.plot(edge2,cdf2,color='y',label='MCS = 4')
plt.plot(edge3,cdf3,color='g',label='MCS = 5')
plt.plot(edge4,cdf4,color='r',label='MCS = 6')
plt.plot(edge5,cdf5,color='m',label='MCS = 8')
plt.xlim(0,40)
plt.title("CDF")
plt.legend()
plt.xlabel("delay(ms)")
plt.show()



