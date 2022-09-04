

from ast import Delete
import os
from readline import append_history_file
from unittest import result
import matplotlib.pyplot as plt
import numpy as np

#Read data from datfile,return VI Ddelay#
def readfile(pathname):
  f=open(pathname)
  Radio=[]
  Throughput1 = []
  Throughput2 = []
  for line in f:
      s = line.strip()
      if s[0:2]=='Ra':
        Radio.append(s[10:-1])
      if s[0:11]=='Throughput1':
        Throughput1.append(s[14:-1])
      if s[0:11]=='Throughput2':
        Throughput2.append(s[14:-1])

  f.close()
  x=[]
  y=[]
  x1=[]
  y1=[]
  x2=[]
  y2=[]
  for num in range(0,len(Radio)):
      try:
        m=float(Radio[num])
        y.append(m)
        x.append(num+1);
      except ValueError:
          pass
  for num in range(0,len(Throughput1)):
      try:
        m=float(Throughput1[num])
        y1.append(m)
        x1.append(num+1);
      except ValueError:
          pass
  for num in range(0,len(Throughput2)):
      try:
        m=float(Throughput2[num])
        y2.append(m)
        x2.append(num+1);
      except ValueError:
          pass
  y2=np.delete(y2,[0])
  y1=np.delete(y1,[0])
  return y,y1,y2

#Get the cdf of the delay
def getavrage(y):
  
  return sum(y)/len(y)

[y1,y11,y12]=readfile("/home/lab1112/ns3/ns-allinone-3.33/ns-3.33/CW7.dat")
[y2,y21,y22]=readfile("/home/lab1112/ns3/ns-allinone-3.33/ns-3.33/CW15.dat")
[y3,y31,y32]=readfile("/home/lab1112/ns3/ns-allinone-3.33/ns-3.33/CW31.dat")
[y4,y41,y42]=readfile("/home/lab1112/ns3/ns-allinone-3.33/ns-3.33/CW63.dat")
[y5,y51,y52]=readfile("/home/lab1112/ns3/ns-allinone-3.33/ns-3.33/CW127.dat")
[y6,y61,y62]=readfile("/home/lab1112/ns3/ns-allinone-3.33/ns-3.33/CW255.dat")
[y7,y71,y72]=readfile("/home/lab1112/ns3/ns-allinone-3.33/ns-3.33/CW511.dat")
[y8,y81,y82]=readfile("/home/lab1112/ns3/ns-allinone-3.33/ns-3.33/CW1023.dat")
print(y12)

result1=[]
result1.append(getavrage(y1))
result1.append(getavrage(y2))
result1.append(getavrage(y3))
result1.append(getavrage(y4))
result1.append(getavrage(y5))
result1.append(getavrage(y6))
result1.append(getavrage(y7))
result1.append(getavrage(y8))
x1=[7,15,31,63,127,255,511,1023]
plt.figure()


plt.plot(x1, result1, label='weight changes', linewidth=3, color='r', marker='o',
         markerfacecolor='blue', markersize=20)

plt.title("Probability for user1")
plt.xlabel("CW Window for user2")

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
result3=[]
result3.append(getavrage(y12))
result3.append(getavrage(y22))
result3.append(getavrage(y32))
result3.append(getavrage(y42))
result3.append(getavrage(y52))
result3.append(getavrage(y62))
result3.append(getavrage(y72))
result3.append(getavrage(y82))
result4=[]
for num in range(0,8):
      result4.append(result2[num]+result3[num])

plt.plot(result2, label='Throughput1', linewidth=3, color='r', marker='o')
plt.plot(result3, label='Throughput2', linewidth=3, color='b', marker='x')
plt.plot(result4, label='Total Throughput', linewidth=1, color='grey', marker='*')
plt.title("Throughput for 2 users")
plt.xlabel("CW Window for user2")
plt.ylabel("Throughput(Mbps)")
plt.legend()






plt.show()



