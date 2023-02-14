#!/bin/bash

sudo ./waf --run 'scratch/model20.cc -phymode=0' >mcs0.dat
sudo ./waf --run 'scratch/model20.cc -phymode=1' >mcs1.dat
sudo ./waf --run 'scratch/model20.cc -phymode=2' >mcs2.dat
sudo ./waf --run 'scratch/model20.cc -phymode=3' >mcs3.dat
sudo ./waf --run 'scratch/model20.cc -phymode=4' >mcs4.dat
sudo ./waf --run 'scratch/model20.cc -phymode=5' >mcs5.dat
sudo ./waf --run 'scratch/model20.cc -phymode=6' >mcs6.dat
sudo ./waf --run 'scratch/model20.cc -phymode=7' >mcs7.dat
sudo ./waf --run 'scratch/model20.cc -phymode=8' >mcs8.dat
