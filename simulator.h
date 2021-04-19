#ifndef _SIMULATOR_H_
#define _SIMULATOR_H_

#include<bits/stdc++.h>
using namespace std;

class Service {
    public:
        int cpu;
        int mem;
        int thuput;
        int e2e_delay;

};

class URLLC : public Service {
    public:
        URLLC();
};

class EMBB : public Service {
    public:
        EMBB();
};

class Server {
    public:
        int d_mem;
        int d_cpu;
        int c_mem;
        int c_cpu;
        int intfc; //interference
        int proc_delay;
        vector<Service> ser_list;
        int get_intfc();
        bool deploy(Service ser);

};

class MEC : public Server {
    public:
        MEC();
};

class CC: public Server {
    public:
        CC();
};

class Link {
    public:
        int prop_delay;
        int c_bw;
        int d_bw;
        Link();
        bool deploy(Service ser);
};

class Simulator {
    public:
        int throughput;
        vector<MEC> mec_set;
        vector<CC> cc_set;
        vector<Service> req_set;
        vector<Link> link_set;
        vector<Service> acc_set;
        Link ue_mec;
        Link mec_cc;
        int sim_time;
        Simulator();
        void start(int time);
        void eval();
};












#endif