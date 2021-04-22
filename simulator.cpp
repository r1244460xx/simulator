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

bool Service::is_satified() {
    if(e2e_delay <= d_delay) {
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
            cr = 3;
            node_delay = 0.1;
            break;
        case CC:
            s_cpu = 4;
            s_mem = 16;
            cr = 3;
            node_delay = 0.1;
            break;
    }
}

int Server::get_intfc() {

}

int Server::get_thuput(Service service) {
    return service.thuput * intfc;
}

void Server::deploy(Service ser) {
    
}

/*----------Link class-------------*/
void Link::deploy(Service ser) {

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

int Proposal::get_score(Service service, Server server) {

}