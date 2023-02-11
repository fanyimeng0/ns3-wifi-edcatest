# Ns3-wifi-edcatest

Table of Contents:
------------------

1) How to use this code
2) Main change in src file
3) Result analysis

1)How to use this code
-------------------------
This code is to simulate the relationship between contention window and transmitting performance  

This code is based on ns-3.33,so you are recommended to download soure file
from 
https://www.nsnam.org/releases/ns-3-33/

Some videos in youtube shows how to build and config ns3-platform.
After you have compiled the file, copy the file to src and scartch, recomplie

./waf --build
./waf --config

Some files use absolute path, you should change them to your own path
1.myonoffapplication.cc in src/applications  
2.cwshow.py  
3.count.py  

Then you can run the simulation by 

cd ns3/ns-allinone-3.33/ns-3.33/  
sudo ./waf --run scratch/'filename -parametername=parametervalue' >savefilename.dat (--vis)  
(--vis) will show the figure by using pyviz.

demo.cc still needs further changes.

Also you can run 


cd ns3/ns-allinone-3.33/ns-3.33/


./prob.sh

This command will run different situations,save them into different .dat file.

To analyse these results,we will use python to read these files.
Than run cwshow.py,you will get the figure like .

![97efa123e7ecf21a80c7c066fc4b91e](https://user-images.githubusercontent.com/81957249/189030109-4a5f29ca-4141-48e0-96c0-4c08fc3c9899.png)

2)Main change in src file
-------------------------

The main change concludes:  
1)myonfoff application:
Using a time list and packetsizelist to control the transmitting rhythm.  
![myonoffapplication](https://user-images.githubusercontent.com/81957249/189030742-2e9e6298-6c3d-4248-a0cf-48ae67315dd8.png)

2)Cancel the cw changes is sta-wifi-mac.cc  

3)Use mytag to sign special packet.  

3)(Unfinished) Control algorithms in channel-access-manager.cc

3)Result analysis
-------------------------
![2648cba2813fbe4e80d2ce527765dc3](https://user-images.githubusercontent.com/81957249/189031352-54689c26-ab72-45c6-ade1-b7bc8b7f04d6.png)

For UDP,only 2 nodes contention

![ea3ed140adf00b6b85e250ab0ce1321](https://user-images.githubusercontent.com/81957249/189031374-d4db04d8-538c-4b93-b873-3085f52ad467.png)

For TCP,the ack packet will influence the result.











