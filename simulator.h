#ifndef _SIMULATOR_H_
#define _SIMULATOR_H_

#include<bits/stdc++.h>
using namespace std;


class Service {
    public:
        int id = 0;
        int type = 0;
        int d_cpu = 0;
        int d_mem = 0;
        int thuput = 0;
        double e2e_delay = 0.;
        double d_delay = 0.;
        int d_bw = 0;
        bool accepted = false;
        Service(int t);
        double get_e2e_delay(double node_delay, double link_delay); //get measured e2e delay
        int get_thuput(double intfc);  //get interferenced thuput
        bool is_satified();

};

class Server {
    public:
        int id = 0;
        int d_mem = 0;
        int d_cpu = 0;
        int s_mem = 0;
        int s_cpu = 0;
        int s_bw = 0;
        int d_bw = 0;
        int type = 0;
        int node_cr = 0; //concentration rate
        int link_cr = 0;
        double intfc = 0.; //interference
        double node_delay = 0.; //rsp delay
        double link_delay = 0.;
        vector<Service> service_list; //deployed service
        Server(int t);
        double get_intfc(); //update intfc 
        double get_node_delay(); //update node delay
        double get_link_delay(); //update link delay
        bool avail(Service& ser); //Available for this deployment
        void deploy(Service& ser); //exec the deployment
};

class Metrics {
    public:
        int exec_time = 0;
        double acc_ratio = 0.;
        double avg_delay = 0.;
        int nor_thuput = 0;
        void statistic(vector<Server>& server_list);
        void print();
};

class Simulator {
    public:
        vector<Service> service_set;
        vector<Server> mec_set;
        vector<Server> cc_set;
        int sim_time = 0;
        int num_mec = 0;
        int num_cc = 0;
        int num_service = 0;
        Metrics mtr;
        Simulator();
        void init(int seed); //init parameter
        int event(); //randomly generate a event, return timestamp
        void arrive();  //arrive event
        void depart(); //depart event
        void deploy(); //Execute the deployment
        void statistics();
        void print(); //Print metrics
        
};

class Proposal {
    public:
    static vector<Server>::iterator eval(Service& service, vector<Server>& mec_set, vector<Server>& cc_set); //evaluate between CC and MEC choice, return chosed one
    static int get_score(Service& service, Server& server);
};

class AIA {
    public:
    static vector<Server>::iterator eval(Service& service, vector<Server>& mec_set, vector<Server>& cc_set); //evaluate between CC and MEC choice, return chosed one

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