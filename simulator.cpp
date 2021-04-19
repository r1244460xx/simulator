#include "simulator.h"

int main () {
    Simulator simultor;
    
}

/*----------Server class-------------*/
int Server::get_intfc() {
    double k0;
    double k1;
    double k2;
    return k0 + k1*d_cpu/c_cpu + k2*d_mem/c_mem;
}

bool Server::deploy(Service ser) {
    if(d_cpu+ser.cpu<=c_cpu && d_mem+ser.mem<=c_mem) {
        d_cpu += ser.cpu;
        d_mem += ser.mem;
        service_list.push_back(ser);
        return true;
    }else{
        return false;
    }
    
    
}

/*----------Link class-------------*/
bool Link::deploy(Service ser) {
    if(d_bw+ser.thuput <= c_bw) {
        d_bw += ser.thuput;
        return true;
    }else {
        return false;
    }
}

/*----------Simulator class-------------*/
void Simulator::eval() {
    int tt_thuput = 0;
    for(int i=0; i<acc_set.size(); i++) {
        tt_thuput +=  acc_set[i].thuput;
    }
    cout << "Performance metric" << endl;
    cout << "Total throughput: " << tt_thuput << endl;
}