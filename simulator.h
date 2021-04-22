#ifndef _SIMULATOR_H_
#define _SIMULATOR_H_

#include<bits/stdc++.h>
using namespace std;


class Service {
    public:
        int type = 0;
        int d_cpu = 0;
        int d_mem = 0;
        int thuput = 0;
        double e2e_delay = 0.;
        double d_delay = 0.;
        int d_bw = 0;
        bool accepted = false;
        Service(int t);
        double get_e2e_delay();
        bool is_satified();

};

class Server {
    public:
        int d_mem = 0;
        int d_cpu = 0;
        int s_mem = 0;
        int s_cpu = 0;
        int cr = 0; //concentration rate
        int intfc = 0; //interference
        double node_delay = 0.; //rsp delay
        vector<Service> service_list; //deployed service
        Server(int t);
        int get_intfc(); //update intfc 
        double get_node_delay(); //update node delay;
        bool avail(Service ser); //Available for this deployment
        int get_thuput(Service ser);  //get interferenced thuput
        void deploy(Service ser); //exec the deployment
};

class Link {
    public:
        double link_delay = 0.;
        int s_bw = 0;
        int d_bw = 0;
        int cr; //concentration rate
        Link();
        double get_link_delay(); //update link delay
        bool avail(Service ser); //Available for this deployment
        void deploy(Service ser); //Execute the deployment
};

class Metrics {
    public:
        int exec_time = 0;
        double acc_ratio = 0.;
        double avg_delay = 0.;
        int nor_thuput = 0;
        void print();
};

class Simulator {
    public:
        vector<Service> service_set;
        vector<Server> mec_set;
        vector<Server> cc_set;
        Link ue_mec;
        Link ue_cc;
        int sim_time = 0;
        Metrics mtr;
        Simulator();
        void init(int seed); //init parameter
        int event(); //randomly generate a event, return timestamp
        void arrive();  //arrive event
        void depart(); //depart event
        void deploy(); //Execute the deployment
        void print(); //Print metrics

};

class Proposal {
    public:
    static vector<Server>::iterator eval(Service service, vector<Server>& mec_set, vector<Server>& cc_set); //evaluate between CC and MEC choice, return chosed one
    static int get_score(Service service, Server server);
};

class AIA {
    public:
    vector<Server>::iterator eval(Service service, vector<Server>& mec_set, vector<Server>& cc_set); //evaluate between CC and MEC choice, return chosed one

};

enum SERVER {
    MEC,
    CC,
};

enum EVENT {
    ARRIVE,
    DEPART,
};

enum SERVICE {
    URLLC,
    EMBB,
    MMTC,
};



#endif