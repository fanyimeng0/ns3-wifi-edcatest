

from ast import Delete
import os
from readline import append_history_file
from unittest import result
import matplotlib.pyplot as plt
import numpy as np

#Read data from datfile,return VI Ddelay#
def readfile(pathname):
  f=open(pathname)
  Throughput1 = []
  VIdelay = []
  for line in f:
      s = line.strip()
      if s[0:11]=='Throughput1':
        Throughput1.append(s[14:-1])
      if s[0:2]=='VI':
        VIdelay.append(s[10:-1])

      

  f.close()

  x1=[]
  y1=[]
  x=[]
  y=[]

  for num in range(0,len(Throughput1)):
      try:
        m=float(Throughput1[num])
        y1.append(m)
        x1.append(num+1);
      except ValueError:
          pass
  for num in range(0,len(VIdelay)):
      try:
        m=float(VIdelay[num])
        y.append(m)
        x.append(num+1);
      except ValueError:
          pass
  
  return y1,y

#Get the cdf of the delay
def getavrage(y):
  
  return sum(y)/len(y)

def caldel(lamda,rate):
    es=1500*8/rate/1000000

    return 1000*(lamda*es*es/2/(1-lamda*es)+es)



[y1,y11]=readfile("/home/lab1112/ns3/ns-allinone-3.33/ns-3.33/mcs0.dat")
[y2,y21]=readfile("/home/lab1112/ns3/ns-allinone-3.33/ns-3.33/mcs1.dat")
[y3,y31]=readfile("/home/lab1112/ns3/ns-allinone-3.33/ns-3.33/mcs2.dat")
[y4,y41]=readfile("/home/lab1112/ns3/ns-allinone-3.33/ns-3.33/mcs3.dat")
[y5,y51]=readfile("/home/lab1112/ns3/ns-allinone-3.33/ns-3.33/mcs4.dat")
[y6,y61]=readfile("/home/lab1112/ns3/ns-allinone-3.33/ns-3.33/mcs5.dat")
[y7,y71]=readfile("/home/lab1112/ns3/ns-allinone-3.33/ns-3.33/mcs6.dat")
[y8,y81]=readfile("/home/lab1112/ns3/ns-allinone-3.33/ns-3.33/mcs7.dat")
[y9,y91]=readfile("/home/lab1112/ns3/ns-allinone-3.33/ns-3.33/mcs8.dat")

result1=[]
result1.append(getavrage(y1))
result1.append(getavrage(y2))
result1.append(getavrage(y3))
result1.append(getavrage(y4))
result1.append(getavrage(y5))
result1.append(getavrage(y6))
result1.append(getavrage(y7))
result1.append(getavrage(y8))
plt.xticks([0,1,2,3,4,5,6,7,8],['VHT0','VHT1','VHT2','VHT3','VHT4','VHT5','VHT6','VHT7','VHT8'])


plt.plot(result1, label='Throughput', linewidth=3, color='r', marker='o',
         markerfacecolor='blue', markersize=20)


plt.xlabel("MCSValue")
plt.ylabel("Throughput(Mbps)")


plt.figure()
result2=[]
result2.append(getavrage(y11))
result2.append(getavrage(y21))
result2.append(getavrage(y31))
result2.append(getavrage(y41))
result2.append(getavrage(y51))
result2.append(getavrage(y61))
result2.append(getavrage(y71))
result2.append(getavrage(y81))
result2.append(getavrage(y91))
rate=[13,26,39,52,78,104,117,130,156]
y2=[]
y2.append(caldel(400,13))
y2.append(caldel(400,26))
y2.append(caldel(400,39))
y2.append(caldel(400,52))
y2.append(caldel(400,78))
y2.append(caldel(400,104))
y2.append(caldel(400,117))
y2.append(caldel(400,130))
y2.append(caldel(400,156))
print(y2)
plt.plot(result2, linewidth=3, color='r', marker='o')
plt.plot(y2,linewidth=2, color='b', marker='+')

plt.title("VIDelay")
plt.xticks([0,1,2,3,4,5,6,7,8],['VHT0','VHT1','VHT2','VHT3','VHT4','VHT5','VHT6','VHT7','VHT8'])
plt.xlabel("MCSValue")
plt.ylabel("Delay(ms)")

plt.legend()

plt.show()