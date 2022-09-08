

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
  Radio2=[]
  Radio3=[]
  Throughput1 = []
  Throughput2 = []
  Throughput3 = []
  for line in f:
      s = line.strip()
      if s[0:6]=='Radio1':
        Radio.append(s[10:-1])
      if s[0:6]=='Radio2':
        Radio2.append(s[10:-1])
      if s[0:6]=='Radio3':
        Radio3.append(s[10:-1])
      if s[0:11]=='Throughput1':
        Throughput1.append(s[14:-1])
      if s[0:11]=='Throughput2':
        Throughput2.append(s[14:-1])
      if s[0:11]=='Throughput3':
        Throughput3.append(s[14:-1])
      

  f.close()
  x=[]
  y=[]
  x1=[]
  y1=[]
  x2=[]
  y2=[]
  x3=[]
  y3=[]
  x4=[]
  y4=[]
  x5=[]
  y5=[]
  x6=[]
  y6=[]
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
  for num in range(0,len(Radio2)):
      try:
        m=float(Radio2[num])
        y3.append(m)
        x3.append(num+1);
      except ValueError:
          pass
  for num in range(0,len(Radio3)):
      try:
        m=float(Radio3[num])
        y4.append(m)
        x4.append(num+1);
      except ValueError:
          pass
  for num in range(0,len(Throughput3)):
      try:
        m=float(Throughput3[num])
        y5.append(m)
        x5.append(num+1);
      except ValueError:
          pass
  
  return y,y1,y2,y3,y4,y5

#Get the cdf of the delay
def getavrage(y):
  
  return sum(y)/len(y)


[y1,y11,y12,y13,y14,y15]=readfile("/home/lab1112/ns3/ns-allinone-3.33/ns-3.33/CW7.dat")
[y2,y21,y22,y23,y24,y25]=readfile("/home/lab1112/ns3/ns-allinone-3.33/ns-3.33/CW15.dat")
[y3,y31,y32,y33,y34,y35]=readfile("/home/lab1112/ns3/ns-allinone-3.33/ns-3.33/CW31.dat")
[y4,y41,y42,y43,y44,y45]=readfile("/home/lab1112/ns3/ns-allinone-3.33/ns-3.33/CW63.dat")
[y5,y51,y52,y53,y54,y55]=readfile("/home/lab1112/ns3/ns-allinone-3.33/ns-3.33/CW127.dat")
[y6,y61,y62,y63,y64,y65]=readfile("/home/lab1112/ns3/ns-allinone-3.33/ns-3.33/CW255.dat")
[y7,y71,y72,y73,y74,y75]=readfile("/home/lab1112/ns3/ns-allinone-3.33/ns-3.33/CW511.dat")
[y8,y81,y82,y83,y84,y85]=readfile("/home/lab1112/ns3/ns-allinone-3.33/ns-3.33/CW1023.dat")

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
plt.xticks([0,1,2,3,4,5,6,7],['7','15','31','63','127','255','511','1023'])
plt.xlabel("CW Window for user2")
plt.ylabel("Throughput(Mbps)")
plt.legend()



[y1,y11,y12,y13,y14,y15]=readfile("/home/lab1112/ns3/ns-allinone-3.33/ns-3.33/CW7tcp.dat")
[y2,y21,y22,y23,y24,y25]=readfile("/home/lab1112/ns3/ns-allinone-3.33/ns-3.33/CW15tcp.dat")
[y3,y31,y32,y33,y34,y35]=readfile("/home/lab1112/ns3/ns-allinone-3.33/ns-3.33/CW31tcp.dat")
[y4,y41,y42,y43,y44,y45]=readfile("/home/lab1112/ns3/ns-allinone-3.33/ns-3.33/CW63tcp.dat")
[y5,y51,y52,y53,y54,y55]=readfile("/home/lab1112/ns3/ns-allinone-3.33/ns-3.33/CW127tcp.dat")
[y6,y61,y62,y63,y64,y65]=readfile("/home/lab1112/ns3/ns-allinone-3.33/ns-3.33/CW255tcp.dat")
[y7,y71,y72,y73,y74,y75]=readfile("/home/lab1112/ns3/ns-allinone-3.33/ns-3.33/CW511tcp.dat")
[y8,y81,y82,y83,y84,y85]=readfile("/home/lab1112/ns3/ns-allinone-3.33/ns-3.33/CW1023tcp.dat")

result1=[]
result1.append(getavrage(y1))
result1.append(getavrage(y2))
result1.append(getavrage(y3))
result1.append(getavrage(y4))
result1.append(getavrage(y5))
result1.append(getavrage(y6))
result1.append(getavrage(y7))
result1.append(getavrage(y8))
result2=[]
result2.append(getavrage(y13))
result2.append(getavrage(y23))
result2.append(getavrage(y33))
result2.append(getavrage(y43))
result2.append(getavrage(y53))
result2.append(getavrage(y63))
result2.append(getavrage(y73))
result2.append(getavrage(y83))
result3=[]
result3.append(getavrage(y14))
result3.append(getavrage(y24))
result3.append(getavrage(y34))
result3.append(getavrage(y44))
result3.append(getavrage(y54))
result3.append(getavrage(y64))
result3.append(getavrage(y74))
result3.append(getavrage(y84))
x1=[7,15,31,63,127,255,511,1023]
plt.figure()


plt.plot(x1, result1, label='User1 Ratio', linewidth=3, color='r', marker='o',
         markerfacecolor='blue', markersize=10)
plt.plot(x1, result2, label='User2 Ratio', linewidth=3, color='b', marker='x',
         markerfacecolor='red', markersize=10)
plt.plot(x1, result3, label='ACK Ratio', linewidth=3, color='g', marker='*',
         markerfacecolor='yellow', markersize=10)

plt.title("Probability")
plt.legend()
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
result1=[]
result1.append(getavrage(y15))
result1.append(getavrage(y25))
result1.append(getavrage(y35))
result1.append(getavrage(y45))
result1.append(getavrage(y55))
result1.append(getavrage(y65))
result1.append(getavrage(y75))
result1.append(getavrage(y85))
result4=[]
for num in range(0,8):
      result4.append(result2[num]+result3[num]+result1[num])

plt.plot(result2, label='Throughput1', linewidth=3, color='r', marker='o')
plt.plot(result3, label='Throughput2', linewidth=3, color='b', marker='x')
plt.plot(result1, label='Throughput AP', linewidth=3, color='g', marker='.')
plt.plot(result4, label='Total Throughput', linewidth=1, color='grey', marker='*')
plt.title("Throughput for 2 users")
plt.xticks([0,1,2,3,4,5,6,7],['7','15','31','63','127','255','511','1023'])
plt.xlabel("CW Window for user2")
plt.ylabel("Throughput(Mbps)")
plt.legend()






plt.show()



