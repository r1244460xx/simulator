#ifndef _SIMULATOR_H_
#define _SIMULATOR_H_

#include<bits/stdc++.h>
using namespace std;


class Service {
    public:
        int id = 0;
        int type = 0;
        int d_sr = 0; //demand system resource
        int consumed_sr = 0;
        int d_bw = 0; //demand link resource
        int thuput = 0; //service rate
        int degraded_thuput = 0;
        double d_proc_delay = 0.;
        double d_delay = 0.; //required e2e delay
        double e2e_delay = 0.; //measured e2e delay
        int data_rate_unit = 0; //data rate unit
        bool accepted = false;
        Service(int t);
        double get_e2e_delay(double propa_delay); //get measured e2e delay
        int get_thuput(double intfc);  //get interferenced thuput
        bool is_satified();

};

class Server {
    public:
        int id = 0;
        int type = 0;
        int s_sr = 0;
        int d_sr = 0;
        int used_sr = 0;
        int s_bw = 0;
        int d_bw = 0;
        double node_cr = 0; //concentration rate
        double intfc = 0.; //interference
        double propa_delay = 0.;
        vector<Service> service_list; //deployed service
        Server(int t);
        double get_intfc(); //update intfc 
        bool avail(Service& ser);
        void deploy(Service& ser); //execute the deployment
};

class Metrics {
    public:
        int exec_time = 0;
        double acc_ratio = 0.;
        double avg_urllc_delay = 0.;
        double avg_embb_delay = 0.;
        int service_counter = 0;
        int satisfy_counter = 0;
        int urllc_counter = 0;
        int embb_counter = 0;
        int total_satisfy_thuput = 0;
        int total_ideal_thuput = 0;
        int total_unsatisfy_thput = 0;
        void statistic(vector<Service>& servcie_list, vector<Server>& server_list);
        void print();
};

class Simulator {
    public:
        vector<Service> request_set; //生成初始request set會複製給server set的service list, 所以不會有degraded thuput與e2e delay
        vector<Server> server_set;
        int sim_time = 0;
        int num_mec = 0; //Number of MEC servers 
        int num_cc = 0; //Number of CC servers 
        int num_request = 0; //Number of service request
        Metrics mtr;
        Simulator(int num_req, int num_mec, int num_cc, int seed);
        void simulate(); //Pop all requests into servers
        void statistics();
        void print(); //Print metrics
};

class Data {
    public:
        double delay = 0.;
        int thuput = 0;
        int lost_thuput = 0;
        int delay_score = 0;
        int thuput_score = 0;
        int lost_thuput_score = 0;
        int server_id = 0;
        bool deployable = false;
        double D = 0;
        double T = 0;
        double M = 0;
};

class DTM {
    public:
    static constexpr double psi = 1; 
    static constexpr double omega = 1;
    static constexpr double tau = 1;
    static constexpr double reserved_factor = 1.;
    static vector<Server>::iterator eval(Service& service, vector<Server>& server_set); //evaluate between CC and MEC choice, return chosed one
    static Data get_data(Service& service, Server server);
    static void normalization(vector<Data>& data_table);
    static int WAA(vector<Data>& data_table);
};



enum SERVER {
    MEC,
    CC,
};

enum SERVICE {
    URLLC,
    EMBB,
    MMTC,
};

#endif