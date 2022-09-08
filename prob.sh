#!/bin/bash

sudo ./waf --run 'scratch/prob.cc -CW=7' >CW7.dat
sudo ./waf --run 'scratch/prob.cc -CW=15' >CW15.dat
sudo ./waf --run 'scratch/prob.cc -CW=31'>CW31.dat
sudo ./waf --run 'scratch/prob.cc -CW=63' >CW63.dat
sudo ./waf --run 'scratch/prob.cc -CW=127 '>CW127.dat
sudo ./waf --run 'scratch/prob.cc -CW=255' >CW255.dat
sudo ./waf --run 'scratch/prob.cc -CW=511'>CW511.dat
sudo ./waf --run 'scratch/prob.cc -CW=1023'>CW1023.dat
sudo ./waf --run 'scratch/probtcp.cc -CW=7'>CW7tcp.dat
sudo ./waf --run 'scratch/probtcp.cc -CW=15'>CW15tcp.dat
sudo ./waf --run 'scratch/probtcp.cc -CW=31'>CW31tcp.dat
sudo ./waf --run 'scratch/probtcp.cc -CW=63'>CW63tcp.dat
sudo ./waf --run 'scratch/probtcp.cc -CW=127'>CW127tcp.dat
sudo ./waf --run 'scratch/probtcp.cc -CW=255'>CW255tcp.dat
sudo ./waf --run 'scratch/probtcp.cc -CW=511'>CW511tcp.dat
sudo ./waf --run 'scratch/probtcp.cc -CW=1023'>CW1023tcp.dat
