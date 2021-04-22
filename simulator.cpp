#include "simulator.h"

int main () {
    Simulator simulator;
    int time;
    cout << "Insert simulation time: " << time << endl;
    cin >> time;
    simulator.init(time);
    while(time>0) {
        time -= simulator.event();
    }
    simulator.print();
}
/*----------Service class-------------*/

Service::Service(int t) {
    switch(t) {
        case URLLC:
            thuput = rand()%9951+50;
            d_bw = 10000;
            d_delay = 1.;
            d_cpu = 1;
            d_mem = 2;
            type = t;
            break;

        case EMBB:
            thuput = rand()%150001+50000;
            d_bw = 200000;
            d_delay = 100.;
            d_cpu = 2;
            d_mem = 4;
            type = t;
            break;

        case MMTC:
            thuput = rand()%101+1;
            d_bw = 100;
            d_delay = 1000.;
            d_cpu = 1;
            d_mem = 1;
            type = t;
            break;
    }
}

int Service::get_thuput(double intfc) {
    return intfc * thuput;
}

bool Service::is_satified() {
    if(e2e_delay <= d_delay && thuput <= d_bw) {
        return true;
    }else {
        return false;
    }
}
/*----------Server class-------------*/
Server::Server(int t) {
    switch(t) {
        case MEC:
            s_cpu = 16;
            s_mem = 64;
            node_cr = 3;
            node_delay = 0.1;
            s_bw = 1000000;
            link_delay = 0.5;
            type = t;
            break;
        case CC:
            s_cpu = 4;
            s_mem = 16;
            node_cr = 3;
            node_delay = 0.1;
            s_bw = 1000000;
            link_delay = 50.;
            type = t;
            break;
    }
}

double Server::get_intfc() {
    double k0 = 0.0;
    double k1 = 0.5;
    double k2=0.5;
    return k0 + k1*d_cpu/s_cpu + k2*d_mem/s_mem;
}

double Server::get_node_delay() {
    double const_node_delay = 0.2;
    if(d_cpu<s_cpu && d_mem<s_mem) {
        return const_node_delay;
    }else if(d_cpu>=s_cpu && d_cpu<s_cpu*node_cr && d_mem>=s_mem && d_mem<s_mem*node_cr) {
        return const_node_delay * (1 + d_cpu/(s_cpu*node_cr));
    }else {
        return const_node_delay * (1 + d_cpu/s_cpu);
    }
}

double Server::get_link_delay() {
    double const_link_delay = 0.2;
    if(d_bw<s_bw) {
        return const_link_delay;
    }else if(d_bw>=s_bw && d_bw < s_bw*link_cr) {
        return const_link_delay * (1+ d_bw/(s_bw*link_cr));
    }else {
        return const_link_delay * (1+ d_bw/s_bw);
    }
}

bool Server::avail(Service ser) {
    if(type == MEC) {
        int cpu = (d_cpu + ser.d_cpu) * node_cr;
        int mem = (d_mem + ser.d_mem) * node_cr;
        int bw = (d_bw + ser.d_bw) * link_cr;
        if(cpu <= s_cpu && mem <= s_mem && bw<=s_bw) 
            return true;
        else
            return false;
    }else {
        return true;
    }
}

void Server::deploy(Service ser) {
    d_cpu += ser.d_cpu;
    d_mem += ser.d_mem;
    d_bw += ser.d_bw;
    intfc = get_intfc();
    node_delay = get_node_delay();
    link_delay = get_link_delay();
    ser.e2e_delay = node_delay + link_delay;
    ser.thuput = ser.get_thuput(intfc);
    service_list.push_back(ser);
}

/*----------Simulator class-------------*/
int Simulator::event() {
    int e = rand()%2;
    if(e == ARRIVE) {
        arrive();
    }else if(e == DEPART) {
        depart();
    }
    return rand()%10+1; //1~9 seconds
}

void Simulator::arrive() {
    int s = rand()%3;
    Service service(s);
    service_set.push_back(s);
    vector<Server>::iterator iter = Proposal::eval(service, mec_set, cc_set);
    if(iter->avail(service)) {
        iter->deploy(service);
    }
}

void Simulator::depart() {
    
}

void Simulator::init(int seed) {
    srand(seed);
}

void Simulator::print() {
    mtr.print();
}

/*----------Metrics class-------------*/
void Metrics::print() {
    cout << "Accepted ratio: " << acc_ratio << endl;
    cout << "Average delay: " << avg_delay << endl;
    cout << "Normalized throughput: " << nor_thuput << endl;
}

/*----------Proposal class-------------*/
vector<Server>::iterator Proposal::eval(Service service, vector<Server>& mec_set, vector<Server>& cc_set) {
    int mec_score = 0;
    int cc_score = 0;
    vector<Server>::iterator mec_iter = mec_set.begin();
    vector<Server>::iterator cc_iter = cc_set.begin();
    int mec_index = -1;
    int cc_index = -1;
    for(int i=0; i<mec_set.size(); i++) {  //Choose the best MEC server
        int score = get_score(service, mec_set[i]);
        if(score > mec_score) {
            mec_score = score;
            mec_index = i;
        }
    }
    mec_iter += mec_index;
    for(int i=0; i<cc_set.size(); i++) { //Choose the best CC server
        int score = get_score(service, cc_set[i]);
        if(score > cc_score) {
            cc_score = score;
            cc_index = i;
        }
    }
    cc_iter += cc_index;
    if(mec_score > cc_score) { //Compare MEC and CC server
        return mec_iter;
    }else {
        return cc_iter;
    }
}

int Proposal::get_score(Service service, Server server) { //Scoring mechanism

}