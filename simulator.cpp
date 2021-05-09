#include "simulator.h"

int main () {
    int num_req = 1, num_mec = 1, num_cc = 1, seed = 0;
    cout << "Insert the numbers of service, mec, cc services, seed" << endl;
    cin >> num_req >> num_mec >> num_cc >> seed;
    Simulator simulator(num_req, num_mec, num_cc, seed);
    simulator.simulate();
    simulator.statistics();
    simulator.print();
}
/*----------Service class-------------*/

Service::Service(int t) {
    switch(t) {
        case URLLC:
            type = t;
            d_sr = 1;
            thuput = rand()%1000+1;
            d_delay = 2.;
            data_rate_unit = 10;
            break;

        case EMBB:
            type = t;
            d_sr = 2;
            thuput = rand()%1000+1;
            d_delay = 100.;
            data_rate_unit = 1000;
            break;

        case MMTC:
            type = t;
            d_sr = 1;
            thuput = rand()%1000+1;
            d_delay = 1000.;
            data_rate_unit = 1;
            break;
    }
}

double Service::get_e2e_delay(double propa_delay) {
    return static_cast<double>(1/degraded_thuput) + propa_delay;
}

int Service::get_thuput(double intfc) {
    return intfc * thuput;
}

/*----------Server class-------------*/
Server::Server(int t) {
    switch(t) {
        case MEC:
            type = t;
            s_sr = 4;
            s_bw = 20000000; //kbps
            node_cr = 3;
            propa_delay = static_cast<double>(rand()%5+1)/10.; //random 0.1~0.5
            break;
        case CC:
            type = t;
            s_sr = 16;
            s_bw = 20000000;
            node_cr = 3;
            propa_delay = 50.;
            break;
    }
}

double Server::get_intfc() {
    double k0 = 0.0;
    double k1 = 0.5;
    if(d_sr<s_sr) {
        return 1.;
    }else if(d_sr>=s_sr && d_sr<s_sr*node_cr) {
        return 1 - (k0 + k1*d_sr/s_sr); 
    }else {
        return 0.; //severe interference
    }
}

double Server::get_proc_delay() {
    double const_node_delay = 0.2;
    return const_node_delay / intfc;
}

bool Server::avail(Service& service) {
    if(type == MEC) {
        int sr = d_sr + service.d_sr;
        int bw = d_bw + service.d_bw;
        if(sr <= s_sr*node_cr && bw<=s_bw) {
            cout << service.id << " deployed on mec server " << id << " succeeded " <<endl;
            return true;
        }
        else {
            cout << service.id << " deployed on mec server " << id  << " failed " << endl;
        }
            return false;
    }else {
        cout << service.id << " deployed on cc server " << id << " succeeded " <<endl;
        return true;
    }
}

void Server::deploy(Service& service) {
    d_sr += service.d_sr;
    service_list.push_back(service);
    intfc = get_intfc(); //update interference indicator
    for(int i=0; i<service_list.size(); i++) {
        service_list[i].degraded_thuput = service_list[i].get_thuput(intfc); //Update degraded thuput by new intfc
        service_list[i].e2e_delay = service_list[i].get_e2e_delay(propa_delay); //Update e2e delay by new intfc
    }
    d_bw += service_list.back().degraded_thuput * service_list.back().data_rate_unit;
}

/*----------Simulator class-------------*/

Simulator::Simulator(int num_req, int num_mec, int num_cc, int seed) {
    srand(seed);
    int id = 0;
    for(int i=0; i<num_request; i++) {
        int s = rand()%2;
        Service service(s);
        service.id = i;
        request_set.push_back(service);
    }
    for(int i=0; i<num_mec; i++) {
        Server s(MEC);
        s.id = id++;
        server_set.push_back(s);
    }
    for(int i=0; i<num_cc; i++) {
        Server s(CC);
        s.id = id++;
        server_set.push_back(s);
    }    
}

void Simulator::simulate() {
    for(int i=0; i<request_set.size(); i++) {
        vector<Server>::iterator iter = DTM::eval(request_set[i], server_set); //put into DTM evaluation
        iter->deploy(request_set[i]); //Exectue the deployment
    }
}

void Simulator::statistics() {
    mtr.statistic(server_set);
}

void Simulator::print() {
    mtr.print();
}

/*----------Metrics class-------------*/
void Metrics::statistic(vector<Server>& server_list) {
    int counter = 0;
    int urllc_counter = 0;
    int embb_counter = 0;
    int satisfy_counter = 0;
    for(int i=0; i<server_list.size(); i++) {
        for(int j=0; j<server_list[i].service_list.size(); j++) {
            if(server_list[i].service_list[j].e2e_delay<=server_list[i].service_list[j].d_delay) {
                satisfy_counter++;
                total_thuput += server_list[i].service_list[j].degraded_thuput;
                if(server_list[i].service_list[j].type == URLLC) {
                    avg_urllc_delay += server_list[i].service_list[j].e2e_delay;
                }else if(server_list[i].service_list[j].type == EMBB) {
                    avg_embb_delay += server_list[i].service_list[j].e2e_delay;
                }
            }else {
                total_unsatisfy_thput += server_list[i].service_list[j].degraded_thuput;
            }
            counter++;
        }
    }
    acc_ratio = satisfy_counter / counter;
    avg_urllc_delay /= urllc_counter;
    avg_embb_delay /= embb_counter;
}

void Metrics::print() {
    cout << "Average eMBB delay: " << avg_embb_delay << endl;
    cout << "Average Urllc delay: " << avg_urllc_delay << endl;
    cout << "Maximum satisfying service rate: " << total_thuput << endl;
    cout << "Unsatisfying service rate: " << total_unsatisfy_thput << endl;
    cout << "Acceptance ratio: " << acc_ratio << endl;
}

/*-----------DTM class----------------*/
vector<Server>::iterator DTM::eval(Service& service, vector<Server>& server_set) {
    vector<int> score_table;
    int max = -1;
    int max_index = 0;
    for(int i=0; i<server_set.size(); i++) {
        int score = get_score(service, server_set[i]);
        score_table.push_back(score);
    }
    for(int i=0; i<score_table.size(); i++) { //Find highest score
        if(score_table[i]>max) {
            max = score_table[i];
            max_index = i;
        }
    }
    assert(score_table.size()==server_set.size());
    return server_set.begin()+max_index;
}

int DTM::get_score(Service& service, Server server) {
    double r = 2.;
    int D = 0, T = 0, M = 0;
    server.d_sr += service.d_sr;
    if(server.d_sr > server.s_sr) {
        return 0; //不能放就直接0分
    }
    server.service_list.push_back(service); //能放就開始算分
    server.intfc = server.get_intfc();
    double psi = 1.;
    double omega = 1.;
    double tau = 1.;
    for(int i=0; i<server.service_list.size(); i++) {
        server.service_list[i].degraded_thuput = server.service_list[i].get_thuput(server.intfc);
        server.service_list[i].e2e_delay = server.service_list[i].get_e2e_delay(server.propa_delay);
    }
    
    if(server.service_list.back().e2e_delay > server.service_list.back().d_delay) {
        D = 0;
    }else {
        D = psi / abs(server.service_list.back().d_delay - server.service_list.back().e2e_delay);
    }
    T = omega * server.service_list.back().degraded_thuput;
    
    double total_us_thuput = 0.;
    for(int i=0; i<server.service_list.size()-1; i++) {
        if(server.service_list[i].e2e_delay > server.service_list[i].d_delay) {
            total_us_thuput += server.service_list[i].degraded_thuput;
        }
    }
    M = tau * total_us_thuput;
    return D + T - M;
}