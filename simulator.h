#ifndef _SIMULATOR_H_
#define _SIMULATOR_H_

#include<bits/stdc++.h>
using namespace std;

class Service {
    public:
        int d_cpu;
        int d_mem;
        int thuput;
        int e2e_delay;
        int d_delay;
        int d_bw;
        bool accepted;
        int get_e2e_delay();
        bool is_satified();

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
        int s_mem;
        int s_cpu;
        int intfc; //interference
        int proc_delay;
        vector<Service> service_list;
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

class Cluster  {
    public:
        vector<Server> server_list;
        Link *front = NULL;
        Link *rear = NULL;
};

class Edge: public Cluster {
    public:
        vector<MEC> mec_list;
};

class Core: public Cluster {
    public:
        vector<CC> cc_list;
};

class Link {
    public:
        int prop_delay;
        int s_bw;
        int d_bw;
        Cluster *front = NULL;
        Cluster *rear = NULL;
        Link();
        bool deploy(Service ser);
};

class Metrics {
    public:
        int exec_time;
        int acc_ratio;
        int avg_delay;
        int nor_thuput;
        void print();
};

class Simulator {
    public:
        vector<MEC> mec_set;
        vector<CC> cc_set;
        vector<Service> req_set;
        vector<Link> link_set;
        vector<Service> acc_set;
        Link ue_mec;
        Link mec_cc;
        int sim_time;
        Metrics mtr;
        Simulator();
        void eval();
        void deploy(Service service);
        void arrive();
        void depart();
        void init(int seed);
        int event();
        void print();

};

class Proposal {
    
};

class AIA {

};








#endif