#include "simulator.h"

int main () {
    Simulator simulator;
    int time;
    cout << "Insert simulation time: " << time << endl;
    cin >> time;
    simulator.init(time);
    while(time>0) {
        time -= simulator.event();
        //simulator.eval();
    }
    simulator.print();
}
/*----------Service class-------------*/
bool Service::is_satified() {
    if(e2e_delay<=d_delay) {
        return true;
    }else {
        return false;
    }
}

/*----------Server class-------------*/
int Server::get_intfc() {
    double k0;
    double k1;
    double k2;
    return k0 + k1*d_cpu/s_cpu + k2*d_mem/s_mem;
}

bool Server::deploy(Service ser) {
    if(d_cpu+ser.d_cpu<=s_cpu && d_mem+ser.d_mem<=s_mem) {
        d_cpu += ser.d_cpu;
        d_mem += ser.d_mem;
        service_list.push_back(ser);
        return true;
    }else{
        return false;
    }
    
    
}

/*----------Link class-------------*/
bool Link::deploy(Service ser) {
    if(d_bw+ser.thuput <= s_bw) {
        d_bw += ser.thuput;
        return true;
    }else {
        return false;
    }
}

/*----------Simulator class-------------*/
void Simulator::eval() {
    
}

int Simulator::event() {
    //arrive or depart 
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