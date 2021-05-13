#include "simulator.h"

int main () {
    int num_req = 30, num_mec = 4, num_cc = 1, seed = 1;
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
            consumed_sr = rand()%d_sr+1;
            thuput = d_sr*2500;
            d_delay = (rand()%11+10)/10.;
            data_rate_unit = 10;
            break;

        case EMBB:
            type = t;
            d_sr = rand()%4+1;
            consumed_sr = rand()%d_sr+1;
            thuput = d_sr*50;
            d_delay = (rand()%901+100)/10.;
            data_rate_unit = 1000;
            break;

        case MMTC:
            type = t;
            d_sr = rand()%4+1;
            consumed_sr = rand()%d_sr+1;
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
            node_cr = 2;
            propa_delay = 0.; //random 0.1~0.5
            break;
        case CC:
            type = t;
            s_sr = 32;
            s_bw = 20000000;
            node_cr = 2;
            propa_delay = 45.;
            break;
    }
}

double Server::get_intfc() {
    double k1 = 0.9;
    if(used_sr<=s_sr) {
        return 1.;
    }else if(used_sr>s_sr && used_sr<=(s_sr*node_cr)) {
        return 1 - k1 * ((used_sr-s_sr) / (s_sr*(node_cr-1))); 
    }else {
        cout << used_sr << " " << s_sr << endl;
        cout << "Deployment Error Exception" << endl;
        exit(1); 
    }
}

bool Server::avail(Service& service) {
    if((d_sr+service.d_sr)<=s_sr*node_cr) {
        return true;
    }else {
        return false;
    }
}

void Server::deploy(Service& service) {
    if((d_sr + service.d_sr) > (s_sr * node_cr)) {
        cout << "Available Exception" << endl;
        exit(1);
    }
    d_sr += service.d_sr;
    used_sr += service.consumed_sr;
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
        if(iter!=server_set.end() && iter->avail(request_set[i])) { //if all server are congestion, return end()
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
        cout << "Allocated: " << server_set[i].d_sr << endl;
        cout << "Used: " << server_set[i].used_sr << endl;
        cout << "Number of services: " << server_set[i].service_list.size() << endl;
        for(int j=0; j<server_set[i].service_list.size(); j++) {
            cout << "\tID: " << server_set[i].service_list[j].id << "\tCost sr : " \
            << server_set[i].service_list[j].consumed_sr  << " // " << server_set[i].service_list[j].d_sr << endl;
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
    vector<Data> data_table;
    for(int i=0; i<server_set.size(); i++) { //Fill the data_table
        Data data = get_data(service, server_set[i]);
        cout << "\tService id: " << service.id << " on Server id: " << server_set[i].id << " parameters are " << \
        data.delay << " " << data.thuput << " " << data.lost_thuput << endl;
        data_table.push_back(data);
    }
    standardization(data_table); //standardize the dataset
    cout << endl;
    cout << "Data Score table: " <<endl;
    for(int i=0; i<data_table.size(); i++) {
        cout << "ID: " <<data_table[i].server_id << ": Delay score: " << data_table[i].delay_score << ", Thuput score: " << data_table[i].thuput_score \
        << ", Lost Thuput score: " << data_table[i].lost_thuput_score << endl;
    }
    int index = WAA(data_table); //get the highest score of server
    // for(int i=0; i<score_table.size(); i++) { 
    //     if(score_table[i]>=0 && score_table[i]>max) { //Block score <0, and find highest score
    //         max = score_table[i];
    //         max_index = i;
    //     }
    // }
    // assert(score_table.size()==server_set.size());
    if(index == -1) {
        return server_set.end();
    }else {
        cout << "Highest score :" << index << ", Chosen Server: " << (server_set.begin()+index)->id << endl;
        return server_set.begin() + index;
    }
        
}

Data DTM::get_data(Service& service, Server server) {
    Data data;
    if(!server.avail(service)) {
        cout <<"\tService id: " << service.id << " on Server id: " << server.id << " has not enough space" << endl;
        data.deployable = false;
        return data;
    }else {
        server.deploy(service);
        data.server_id = server.id;
        data.delay =  server.service_list.back().d_delay - server.service_list.back().e2e_delay * reserved_factor;
        data.thuput = server.service_list.back().degraded_thuput;
        int total_us_thuput = 0;
        for(int i=0; i<server.service_list.size()-1; i++) {
            if(server.service_list[i].e2e_delay > server.service_list[i].d_delay) {
                total_us_thuput += 1;
            }
        }
        data.lost_thuput = total_us_thuput;
        data.deployable = true;
        return data;
    }
    
}

void DTM::standardization(vector<Data>& data_table) {
    vector<Data>::iterator iter = data_table.begin();
    while(iter!=data_table.end()) { //Delete not available server
        if(!(iter->deployable))
            data_table.erase(iter);
        else
            iter++;
    }
    vector<Data> negative_delay;
    vector<Data> positive_delay;
    for(int i=0; i<data_table.size(); i++) {
        if(data_table[i].delay<0.) {
            negative_delay.push_back(data_table[i]);
        }else {
            positive_delay.push_back(data_table[i]);
        }
    }
    double d_max = -1.;
    double d_min = 100000.;
    for(int i=0; i<positive_delay.size(); i++) {
        if(positive_delay[i].delay>d_max) {
            d_max = positive_delay[i].delay;
        }
        if(positive_delay[i].delay<d_min) {
            d_min = positive_delay[i].delay;
        }
    }
    
    for(int i=0; i<positive_delay.size(); i++) {
        if(d_max != d_min) {
            positive_delay[i].delay_score = 100 * (1 - (positive_delay[i].delay-d_min)/(d_max-d_min));
        }
        else {
            positive_delay[i].delay_score = 100;
        }
    }//get normalized delay score 0~100 
    d_max = -100000.;
    d_min = 1;
    for(int i=0; i<negative_delay.size(); i++) {
        if(negative_delay[i].delay>d_max) {
            d_max = negative_delay[i].delay;
        }
        if(negative_delay[i].delay<d_min) {
            d_min = negative_delay[i].delay;
        }
    }
    for(int i=0; i<negative_delay.size(); i++) {
        if(d_max != d_min) {
            negative_delay[i].delay_score = -99 * (1 - (negative_delay[i].delay-d_min)/(d_max-d_min)) - 1;
        }else {
            negative_delay[i].delay_score = -1;
        }
    } //get normalized delay score -1~-100
    negative_delay.insert(negative_delay.end(), positive_delay.begin(), positive_delay.end());
    data_table = negative_delay;
    
    int i_max = -10000;
    int i_min = 100000;
    for(int i=0; i<data_table.size(); i++) {
        if(data_table[i].thuput > i_max) {
            i_max = data_table[i].thuput;
        }
        if(data_table[i].thuput < i_min) {
            i_min = data_table[i].thuput;
        }
    }
    for(int i=0; i<data_table.size(); i++) {
        if(i_max!=i_min) {
            data_table[i].thuput_score = 100*((data_table[i].thuput-i_min) / (i_max-i_min));
        }else {
            data_table[i].thuput_score = 100;
        }
    }
    
    i_max = -10000;
    i_min = 10000;
    for(int i=0; i<data_table.size(); i++) {
        if(data_table[i].lost_thuput>i_max) {
            i_max = data_table[i].lost_thuput;
        }
        if(data_table[i].lost_thuput<i_min) {
            i_min = data_table[i].lost_thuput;
        }
    }
    for(int i=0; i<data_table.size(); i++) {
        if(i_max != i_min) {
            data_table[i].lost_thuput_score = 100*((data_table[i].lost_thuput-i_min) / (i_max-i_min));
            data_table[i].lost_thuput_score = 0;
        }else {
            data_table[i].lost_thuput_score = 0;
        }
    }
}

int DTM::WAA(vector<Data>& data_table) { //Weighted_arithmetic_average
    if(!data_table.size()) {
        return -1;
    } 
    int highest_index = -1;
    int max = -10000;
    for(int i=0; i<data_table.size(); i++) {
        int total = psi*data_table[i].delay_score + omega*data_table[i].thuput_score - tau*data_table[i].lost_thuput_score;
        if(total>max) {
            max = total;
            highest_index = i;
        }
    }
    return data_table[highest_index].server_id;
}

int DTM::get_score(Service& service, Server server) { //depracated
    double r = 1.5;
    double D = 0, T = 0, M = 0;
    if(!server.avail(service)) {
        cout <<"\tService id: " << service.id << " on Server id: " << server.id << " has not enough space" << endl;
        return -1; //不能放就直接-1分
    }
    server.deploy(service);
    //Weighted
    double psi = 0.5; 
    double omega = 0.5;
    double tau = 0.3;
    //Normalization, standardization
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
    //M = tau * total_us_thuput;
    M = 0;
    //Modify END
    cout <<"\tService id: " << service.id << " on Server id: " << server.id <<": D: " << D << ", T: " << T << ", M: " << M << endl;
    if((D + T - M)<0)
        return 0;
    else
        return D + T - M;
}