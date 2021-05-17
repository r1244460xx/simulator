#include "simulator.h"
ofstream brief, full;
int main () {
    brief.open("brief.csv", ios::out);
    full.open("full.txt", ios::out);
    for(int i=5; i<60; i++) {
        int num_req = i, num_mec = 4, num_cc = 1, seed = 1;
        //cout << "Insert the numbers of service, mec, cc services, seed" << endl;
        //cin >> num_req >> num_mec >> num_cc >> seed;
        cout << "\n\n**********The optimization of deployment**********" << endl;
        cout << "Number of requests: " << num_req << ", Number of MEC: " << num_mec <<\
        ", Number of CC: " << num_cc << endl;
        cout << "**********Start**********" << endl;
        Simulator simulator(num_req, num_mec, num_cc, seed);
        simulator.simulate();
        cout << "simulate over" << endl << endl;
        simulator.statistics();
        simulator.print();
        cout << endl;
    }
    
}
/*----------Service class-------------*/

Service::Service(int t) {
    switch(t) {
        case URLLC:
            type = t;
            d_sr = rand()%4 + 1;
            consumed_sr = rand()%d_sr + 1; 
            //thuput = d_sr * 2500;
            thuput = 1;
            d_delay = (rand()%6+5)/10.; //0.5~1ms
            data_rate_unit = 10;
            d_proc_delay = 0.5 - 0.1 * consumed_sr;
            break;

        case EMBB:
            type = t;
            d_sr = rand()%4+1;
            consumed_sr = rand()%d_sr+1;
            //thuput = d_sr*50;
            thuput = 1;
            d_delay = (rand()%401+100)/10.; //10ms ~ 50ms
            data_rate_unit = 1000;
            d_proc_delay = 5 - 1. * consumed_sr;
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
    return d_proc_delay/static_cast<double>(degraded_thuput) + 2 * propa_delay;
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
    if(used_sr <= s_sr) {
        return 1.;
    }else if(used_sr > s_sr && used_sr <= (s_sr*node_cr)) {
        return 1 - k1 * ((used_sr-s_sr) / (s_sr*(node_cr-1))); 
    }else {
        cout << id << " " << used_sr << " " << s_sr << endl;
        cout << "Deployment Error Exception" << endl;
        exit(1); 
    }
}

bool Server::avail(Service& service) {
    if((d_sr + service.d_sr) <= s_sr * node_cr) {
        return true;
    }else {
        return false;
    }
}

void Server::deploy(Service& service) {
    if((d_sr + service.d_sr) > (s_sr * node_cr)) {
        cout << service.id << " ";
        cout << "Available Error Exception" << endl;
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
    cout << "Server list: " << endl;
    for(int i=0; i<server_set.size(); i++) {
        cout << "\tServer id: " << server_set[i].id << ", Server type: " <<server_set[i].type <<\
        ", Server sr: " << server_set[i].s_sr << ", Propa delay: " << server_set[i].propa_delay << endl;
    }
    cout << endl;
    cout << "Request list: " << endl;
    for(int i=0; i<request_set.size(); i++) {
        cout << "\tRequest id: " << request_set[i].id << ", Request type: " << request_set[i].type <<\
        ", Request sr: " << request_set[i].d_sr << ", Request thuput: " << request_set[i].thuput <<\
        ", e2e delay: " <<request_set[i].d_delay << endl;    
    }
}

void Simulator::simulate() {
    for(int i=0; i<request_set.size(); i++) {
        vector<Server>::iterator iter = DTM::eval(request_set[i], server_set); //put into DTM evaluation
        // vector<Server>::iterator iter = server_set.begin() + rand()%5;
        if(iter->avail(request_set[i]) && iter!=server_set.end()) { //if all server are congestion, return end()
            iter->deploy(request_set[i]); //Exectue the deployment
        }
    }
}

void Simulator::statistics() {
    mtr.statistic(request_set, server_set);
}

void Simulator::print() {
    for(int i=0; i<server_set.size(); i++) {
        cout << endl;
        cout << "Server ID: " << server_set[i].id << endl;
        cout << "Server Type: " <<server_set[i].type << endl;
        cout << "Capacity: " << server_set[i].s_sr << endl;
        cout << "Allocated: " << server_set[i].d_sr << endl;
        cout << "Used: " << server_set[i].used_sr << endl;
        cout << "Interference: " << server_set[i].intfc << endl;
        cout << "Number of services: " << server_set[i].service_list.size() << endl;
        for(int j=0; j<server_set[i].service_list.size(); j++) {
            cout << "\tID: " << server_set[i].service_list[j].id << ", Type: " << server_set[i].service_list[j].type <<\
            ", Cost sr: " << server_set[i].service_list[j].consumed_sr << " / " << server_set[i].service_list[j].d_sr <<\
            ", Request E2E delay: " << server_set[i].service_list[j].d_delay <<\
            ", Ideal Thuput: " << server_set[i].service_list[j].thuput <<\
            ", Actual Thuput: " << server_set[i].service_list[j].degraded_thuput <<\
            ", E2E delay: " << server_set[i].service_list[j].e2e_delay << endl;
        }
    }
    cout << endl;
    mtr.print();
    cout << endl;
}

/*----------Metrics class-------------*/
void Metrics::statistic(vector<Service>& servcie_list, vector<Server>& server_list) {
    for(int i=0; i<servcie_list.size(); i++) {
        total_ideal_thuput += servcie_list[i].thuput;
        service_counter++;
        if(servcie_list[i].type == EMBB)
            embb_counter++;
        else if(servcie_list[i].type == URLLC)
            urllc_counter++;
    }
    int num = 1;
    for(int i=0; i<server_list.size(); i++) {
        for(int j=0; j<server_list[i].service_list.size(); j++) {
            if(server_list[i].service_list[j].type == URLLC) {
                avg_urllc_delay += server_list[i].service_list[j].e2e_delay;
            }else if(server_list[i].service_list[j].type == EMBB) {
                avg_embb_delay += server_list[i].service_list[j].e2e_delay;
            }
            if(server_list[i].service_list[j].e2e_delay <= server_list[i].service_list[j].d_delay) {
                satisfy_counter++;
                total_satisfy_thuput += server_list[i].service_list[j].degraded_thuput;
                cout << num++ << ". Service id: " << server_list[i].service_list[j].id << " is satisfied" << endl;
            }else {
                total_unsatisfy_thput += server_list[i].service_list[j].degraded_thuput;
                cout << num++ << ". Service id: " << server_list[i].service_list[j].id << " isn't satisfied" << endl;
            }
        }
    }
    acc_ratio = satisfy_counter / static_cast<double>(service_counter);
    avg_urllc_delay /= urllc_counter;
    avg_embb_delay /= embb_counter;
}

void Metrics::print() {
    cout << "Satisfying service rate: " << static_cast<double>(total_satisfy_thuput)/total_ideal_thuput*100. << endl;
    cout << "Unsatisfying service rate: " << static_cast<double>(total_unsatisfy_thput)/total_ideal_thuput*100. << endl;
    cout << "Dropped service rate: " << static_cast<double>(total_ideal_thuput-total_satisfy_thuput-total_unsatisfy_thput)/total_ideal_thuput*100. << endl;
    cout << "Average eMBB delay: " << avg_embb_delay << endl;
    cout << "Average Urllc delay: " << avg_urllc_delay << endl;
    cout << "Acceptance ratio: " << acc_ratio << endl;
    brief << static_cast<double>(total_satisfy_thuput)/total_ideal_thuput*100. << ",";
    brief << static_cast<double>(total_unsatisfy_thput)/total_ideal_thuput*100. << ",";
    brief << static_cast<double>(total_ideal_thuput-total_satisfy_thuput-total_unsatisfy_thput)/total_ideal_thuput*100. << ",";
    brief << avg_embb_delay << ",";
    brief << avg_urllc_delay << ",";
    brief << acc_ratio << ",";
    brief << endl;
}

/*-----------DTM class----------------*/

vector<Server>::iterator DTM::eval(Service& service, vector<Server>& server_set) {
    vector<int> score_table;
    vector<Data> data_table;
    cout << "\nService id: " << service.id << " parameter table: " << endl;
    for(int i=0; i<server_set.size(); i++) { //Fill the data_table
        Data data = get_data(service, server_set[i]);
        if(data.deployable)
            cout << "\tServer id: " << server_set[i].id << " are " << \
            data.delay << " / " << data.thuput << " / " << data.lost_thuput << endl;
        else
            cout << "\tService id: " << service.id << " on Server id: " << server_set[i].id << " isn't available." << endl;
        data_table.push_back(data);
    }
    normalization(data_table); //standardize the dataset
    cout << "Service id: " << service.id <<" data Score table: " <<endl;
    for(int i=0; i<data_table.size(); i++) {
        cout << "\tServer ID: " << data_table[i].server_id << ", delay score: " << data_table[i].delay_score << ", Thuput score: " << data_table[i].thuput_score \
        << ", Lost Thuput score: " << data_table[i].lost_thuput_score << endl;
    }
    int index = WAA(data_table); //get the highest score of server
    if(index == -1) {
        cout << "\tThis service is dropped" << endl;
        return server_set.end();
    }else {
        cout << "\tChoose Server id: " << (server_set.begin()+index)->id << endl;
        return server_set.begin() + index;
    }
        
}

Data DTM::get_data(Service& service, Server server) {
    Data data;
    if(!server.avail(service)) {
        data.deployable = false;
        return data;
    }else {
        double prev_intfc = server.intfc;
        server.deploy(service);
        data.server_id = server.id;
        data.delay =  server.service_list.back().d_delay - server.service_list.back().e2e_delay * reserved_factor;
        data.thuput = server.service_list.back().degraded_thuput;
        int total_us_thuput = 0;
        for(int i=0; i<server.service_list.size()-1; i++) {
            if(server.service_list[i].e2e_delay > server.service_list[i].d_delay) {
                total_us_thuput += (server.service_list[i].degraded_thuput - server.service_list[i].thuput * prev_intfc);
            }
        }
        data.lost_thuput = total_us_thuput;
        data.deployable = true;
        return data;
    }
    
}

void DTM::normalization(vector<Data>& data_table) {
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
        if(d_max != d_min) { //49~100
            positive_delay[i].delay_score = 49 * (1 - (positive_delay[i].delay-d_min)/(d_max-d_min)) + 51;
        }
        else {
            positive_delay[i].delay_score = 51; //將影響降到最低
        }
    }//get normalized delay score 51~100 
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
            negative_delay[i].delay_score = 49 * (negative_delay[i].delay-d_min)/(d_max-d_min) + 1;
        }else {
            negative_delay[i].delay_score = 1; //將影響降到最低
        }
    } //get normalized delay score 1~50 
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
            data_table[i].thuput_score = 99 * ((data_table[i].thuput-i_min) / (i_max-i_min)) + 1;
        }else {
            data_table[i].thuput_score = 1; //將影響降到最低
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
            data_table[i].lost_thuput_score = 99 * ((data_table[i].lost_thuput-i_min) / (i_max-i_min)) + 1;
        }else {
            data_table[i].lost_thuput_score = 1;
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
