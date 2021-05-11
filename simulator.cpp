#include "simulator.h"

int main () {
    int num_req = 60, num_mec = 4, num_cc = 1, seed = 1;
    //cout << "Insert the numbers of service, mec, cc services, seed" << endl;
    //cin >> num_req >> num_mec >> num_cc >> seed;
    Simulator simulator(num_req, num_mec, num_cc, seed);
    simulator.simulate();
    cout << "simulate over" << endl;
    
    //simulator.statistics();
    //cout << "statistics over" << endl;
    simulator.print();
}
/*----------Service class-------------*/

Service::Service(int t) {
    switch(t) {
        case URLLC:
            type = t;
            d_sr = rand()%4+1;
            thuput = d_sr*2500;
            d_delay = (rand()%11+10)/10.;
            data_rate_unit = 10;
            break;

        case EMBB:
            type = t;
            d_sr = rand()%4+1;
            thuput = d_sr*50;
            d_delay = (rand()%901+100)/10.;
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
            s_sr = 8;
            s_bw = 20000000; //kbps
            node_cr = 1.5;
            propa_delay = 0.; //random 0.1~0.5
            break;
        case CC:
            type = t;
            s_sr = 32;
            s_bw = 20000000;
            node_cr = 1.5;
            propa_delay = 45.;
            break;
    }
}

double Server::get_intfc() {
    double k0 = 0.0;
    double k1 = 0.8;
    if(d_sr<s_sr) {
        return 1.;
    }else if(d_sr>=s_sr && d_sr<=(s_sr*node_cr)) {
        return 1 - (k0 + k1*d_sr/(s_sr*node_cr)); 
    }else {
        cout << d_sr << " " << s_sr << endl;
        cout << "Not Allowed Deployment Error" << endl;
        exit(1); 
    }
}

void Server::deploy(Service& service) {
    if((d_sr + service.d_sr) > (s_sr * node_cr)) {
        cout << "No enough sr" << endl;
        return;
    }
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
    for(int i=0; i<num_req; i++) {
        int s = rand()%2;
        Service service(s);
        service.id = i;
        request_set.push_back(service);
    }
    double mec_delay[] = {0.25, 5., 15., 25.};
    for(int i=0; i<num_mec; i++) {
        Server s(MEC);
        s.id = id++;
        s.propa_delay = mec_delay[i];
        server_set.push_back(s);
    }
    for(int i=0; i<num_cc; i++) {
        Server s(CC);
        s.id = id++;
        server_set.push_back(s);
    }
    // for(int i=0; i<server_set.size(); i++) {
    //     cout << "Server id: " << server_set[i].id << ", Server type: " <<server_set[i].type <<\
    //     ", Server sr: " << server_set[i].s_sr << ", Propa delay: " << server_set[i].propa_delay << endl;
    // }    
}

void Simulator::simulate() {
    cout << "Service Size: " << request_set.size() << endl;
    cout << "Server Size: " << server_set.size() << endl;
    for(int i=0; i<request_set.size(); i++) {
        vector<Server>::iterator iter = DTM::eval(request_set[i], server_set); //put into DTM evaluation
        // vector<Server>::iterator iter = server_set.begin() + rand()%5;
        if(iter!=server_set.end()) { //if all server are congestion, return end()
            iter->deploy(request_set[i]); //Exectue the deployment
            cout << "Deployed Server id: " << iter->id << endl;
        }else {
            cout << "No server is appropriate for " << request_set[i].id << endl;
        }
        
       // cout << "Request id: " << request_set[i].id << ", Request type: " << request_set[i].type << ", Request sr: " << request_set[i].d_sr\
       // << ", Request thuput: " << request_set[i].thuput << ", e2e delay: " <<request_set[i].d_delay << endl;
    }
}

void Simulator::statistics() {
    mtr.statistic(server_set);
}

void Simulator::print() {
    //mtr.print();
    for(int i=0; i<server_set.size(); i++) {
        cout << "Consumed:" << server_set[i].d_sr << endl;
        for(int j=0; j<server_set[i].service_list.size(); j++) {
            cout << "\tID: " << server_set[i].service_list[j].id << "\tCost sr : " << server_set[i].service_list[j].d_sr << endl;
        }
    }
}

/*----------Metrics class-------------*/
void Metrics::statistic(vector<Server>& server_list) {
    int counter = 0;
    int urllc_counter = 0;
    int embb_counter = 0;
    int satisfy_counter = 0;
    for(int i=0; i<server_list.size(); i++) {
        cout << "Server id: " << server_list[i].id << " , size: " << server_list[i].service_list.size() << endl;
        for(int j=0; j<server_list[i].service_list.size(); j++) {
            cout << "\tService id: " << server_list[i].service_list[j].id;
            if(server_list[i].service_list[j].e2e_delay<=server_list[i].service_list[j].d_delay) {
                satisfy_counter++;
                total_thuput += server_list[i].service_list[j].degraded_thuput;
                if(server_list[i].service_list[j].type == URLLC) {
                    avg_urllc_delay += server_list[i].service_list[j].e2e_delay;
                }else if(server_list[i].service_list[j].type == EMBB) {
                    avg_embb_delay += server_list[i].service_list[j].e2e_delay;
                }
                cout << " is satisfied" << endl;
            }else {
                total_unsatisfy_thput += server_list[i].service_list[j].degraded_thuput;
                cout << " isn't satisfied" << endl;
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
        score_table.push_back(score); //Selectable >=0, not selectable < 0
        // cout << "Server id: " << server_set[i].id << ", score: " <<  score << endl;
    }
    for(int i=0; i<score_table.size(); i++) { 
        if(score_table[i]>=0 && score_table[i]>max) { //Block score <0, and find highest score
            max = score_table[i];
            max_index = i;
        }
    }
    assert(score_table.size()==server_set.size());
    if(max == -1) {
        return server_set.end();
    }else {
        cout << "Highest score :" << max << ", Chosen Server: " << (server_set.begin()+max_index)->id << endl;
        return server_set.begin()+max_index;
    }
        
}

int DTM::get_score(Service& service, Server server) {
    double r = 1.5;
    double D = 0, T = 0, M = 0;
    server.d_sr += service.d_sr;
    if(server.d_sr > server.s_sr*server.node_cr) {
        cout <<"\tService id: " << service.id << " on Server id: " << server.id << " has not enough space" << endl;
        return -1; //不能放就直接-1分
    }
    server.service_list.push_back(service); //能放就開始算分
    server.intfc = server.get_intfc();
    //Modify
    double psi = 0.3;
    double omega = 0.3;
    double tau = 0.3;
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
    //Modify END
    cout <<"\tService id: " << service.id << " on Server id: " << server.id <<": D: " << D << ", T: " << T << ", M: " << M << endl;
    if((D + T - M)<0)
        return 0;
    else
        return D + T - M;
}