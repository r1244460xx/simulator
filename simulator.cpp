#include "simulator.h"
ofstream brief;
int main(int argc, char** argv) {
	brief.open("brief_CP.csv", ios::out);
	int init_request = 1;
	int max_request = 300;
	int init_server = 1;
	int max_server = 6;
	int seed = 5;
	if (argc == 6) {
		init_request = atoi(argv[1]);
		max_request = atoi(argv[2]);
		init_server = atoi(argv[3]);
		max_server = atoi(argv[4]);
		seed = atoi(argv[5]);
	}
	brief << "Satisfying service rate"
	      << ",Unsatisfying service rate"
	      << ",Dropped service rate";
	brief << ",Average eMBB delay"
	      << ",Average Urllc delay"
	      << ",Acceptance ratio" << endl;
	brief << init_request << "," << max_request << "," << init_server << "," << max_server << endl;
	for (int k = 2; k < 3; k++) {
		for (int j = init_server; j < max_server; j++) {
			for (int i = init_request; i < max_request; i++) {
				int num_req = i, num_mec = 4 * j, num_cc = j;
				cout << "\n\n**********The optimization of deployments**********"
				     << endl;
				cout << "Number of requests: " << num_req
				     << ", Number of MEC: " << num_mec << ", Number of CC: " << num_cc
				     << endl;
				cout << "**********Start**********" << endl;
				Simulator simulator(num_req, num_mec, num_cc, k);
				Simulator simulator2 = simulator;
				Simulator simulator3 = simulator;
				simulator.simulate();
				simulator2.simulate2();
				simulator3.simulate3();
				cout << "simulate over" << endl
				     << endl;
				simulator.statistics();
				simulator2.statistics();
				simulator3.statistics();
				cout << "CP result: " << endl;
				simulator.print();
				cout << endl
				     << "LD result: " << endl;
				simulator2.print();
				cout << endl
				     << "LI result: " << endl;
				simulator3.print();
				cout << endl;
			}
		}
	}
}
/*----------Service class-------------*/

Service::Service(int t) {
	switch (t) {
	case URLLC:
		type = t;
		d_sr = rand() % 4 + 1;
		consumed_sr = rand() % d_sr + 1;
		thuput = static_cast<double>(consumed_sr);
		d_delay = static_cast<double>(rand() % 20 + 1);  // 1 ~ 20ms demand e2e delay
		data_rate_unit = 10;
		d_proc_delay = 0.25 - 0.05 * consumed_sr;
		accepted = false;
		break;

	case EMBB:
		type = t;
		d_sr = rand() % 4 + 1;
		consumed_sr = rand() % d_sr + 1;
		thuput = static_cast<double>(consumed_sr);
		d_delay = static_cast<double>(rand() % 20 + 21);  // 21ms ~ 40ms demand e2e delay
		data_rate_unit = 1000;
		d_proc_delay = 12.5 - 2.5 * consumed_sr;  //20 times higher than urllc
		accepted = false;
		break;

	case MMTC:
		assert(t != MMTC);
		type = t;
		d_sr = rand() % 4 + 1;
		consumed_sr = rand() % d_sr + 1;
		thuput = rand() % 1000 + 1;
		d_delay = 1000.;
		data_rate_unit = 1;
		break;
	}
}

double Service::get_e2e_delay(double intfc, double propa_delay) {
	assert(intfc > 0.);
	return d_proc_delay / intfc + propa_delay;
}

double Service::get_thuput(double intfc) {
	assert(intfc > 0.);
	return intfc * thuput;
}

/*----------Server class-------------*/
Server::Server(int t) {
	switch (t) {
	case MEC:
		type = t;
		s_sr = 8;
		s_bw = 20000000;  // kbps
		node_cr = 2;
		propa_delay = static_cast<double>(rand() % 10) + 1.;  // random 1~10ms
		break;
	case CC:
		type = t;
		s_sr = 32;
		s_bw = 20000000;
		node_cr = 2;
		propa_delay = 15.;
		break;
	}
}

double Server::get_intfc() {
	double k1 = 0.9;
	assert(used_sr <= s_sr * node_cr);
	if (used_sr <= s_sr) {
		return 1.;
	} else {
		return 1. - k1 * ((used_sr - s_sr) / (s_sr * (node_cr - 1)));
	}
}

bool Server::avail(Service& service) {
	if ((d_sr + service.d_sr) <= s_sr * node_cr) {
		return true;
	} else {
		return false;
	}
}

void Server::deploy(Service& service) {
	assert((d_sr + service.d_sr) <= s_sr * node_cr);
	d_sr += service.d_sr;
	used_sr += service.consumed_sr;
	service_list.push_back(service);
	intfc = get_intfc();  // update interference indicator
	for (int i = 0; i < service_list.size(); i++) {
		service_list[i].degraded_thuput = service_list[i].get_thuput(intfc);            // Update degraded thuput by new intfc
		service_list[i].e2e_delay = service_list[i].get_e2e_delay(intfc, propa_delay);  // Update e2e delay by new intfc
	}
	//d_bw += service_list.back().degraded_thuput *
	//        service_list.back().data_rate_unit;
}

/*----------Simulator class-------------*/
Simulator::Simulator(int num_req, int num_mec, int num_cc, int seed) {
	srand(seed);
	int id = 0;
	for (int i = 0; i < num_req; i++) {
		int s = rand() % 2;  //urllc or embb
		Service service(s);
		service.id = i;
		request_set.push_back(service);
	}
	for (int i = 0; i < num_mec; i++) {
		Server s(MEC);
		s.id = id++;
		server_set.push_back(s);
	}
	for (int i = 0; i < num_cc; i++) {
		Server s(CC);
		s.id = id++;
		server_set.push_back(s);
	}
	cout << "Request list: " << endl;
	for (int i = 0; i < request_set.size(); i++) {
		cout << "\tRequest id: " << request_set[i].id
		     << ", Request type: " << request_set[i].type
		     << ", Request sr: " << request_set[i].d_sr
		     << ", Consumed sr: " << request_set[i].consumed_sr
		     << ", Demand delay: " << request_set[i].d_delay << endl;
	}
	cout << endl;
	cout << "Server list: " << endl;
	for (int i = 0; i < server_set.size(); i++) {
		cout << "\tServer id: " << server_set[i].id
		     << ", Server type: " << server_set[i].type
		     << ", Server sr: " << server_set[i].s_sr
		     << ", Propa delay: " << server_set[i].propa_delay << endl;
	}
}

void Simulator::simulate() {
	for (int i = 0; i < request_set.size(); i++) {
		vector<Server>::iterator iter = DTM::eval(request_set[i], server_set);  // put into DTM evaluation
		if (iter->avail(request_set[i]) && iter != server_set.end()) {          // if all server are congestion, return end()
			iter->deploy(request_set[i]);                                       // Exectue the deployment
		} else {
			undeployed_service.push_back(request_set[i]);  //no enough space for this service
		}
	}
}

void Simulator::simulate2() {
	for (int i = 0; i < request_set.size(); i++) {
		vector<Server>::iterator iter = LD::eval(request_set[i], server_set);  // put into DTM evaluation
		if (iter->avail(request_set[i]) &&
		    iter != server_set.end()) {    // if all server are congestion, return end()
			iter->deploy(request_set[i]);  // Exectue the deployment
		} else {
			undeployed_service.push_back(request_set[i]);  //no enough space for this service
		}
	}
}

void Simulator::simulate3() {
	for (int i = 0; i < request_set.size(); i++) {
		vector<Server>::iterator iter = LI::eval(request_set[i], server_set);  // put into DTM evaluation
		if (iter->avail(request_set[i]) &&
		    iter != server_set.end()) {    // if all server are congestion, return end()
			iter->deploy(request_set[i]);  // Exectue the deployment
		} else {
			undeployed_service.push_back(request_set[i]);  //no enough space for this service
		}
	}
}

void Simulator::statistics() {
	mtr.statistic(request_set, server_set);
}

void Simulator::print() {
	for (int i = 0; i < server_set.size(); i++) {
		cout << endl;
		cout << "Server ID: " << server_set[i].id << endl;
		cout << "Server Type: " << server_set[i].type << endl;
		cout << "Capacity: " << server_set[i].s_sr << endl;
		cout << "Allocated: " << server_set[i].d_sr << endl;
		cout << "Used: " << server_set[i].used_sr << endl;
		cout << "Interference: " << server_set[i].intfc << endl;
		cout << "Number of services: " << server_set[i].service_list.size()
		     << endl;
		for (int j = 0; j < server_set[i].service_list.size(); j++) {
			cout << "\tID: " << server_set[i].service_list[j].id
			     << ", Type: " << server_set[i].service_list[j].type
			     << ", Cost sr: " << server_set[i].service_list[j].consumed_sr
			     << " / " << server_set[i].service_list[j].d_sr
			     << ", Request E2E delay: "
			     << server_set[i].service_list[j].d_delay
			     << ", Ideal Thuput: " << server_set[i].service_list[j].thuput
			     << ", Actual Thuput: "
			     << server_set[i].service_list[j].degraded_thuput
			     << ", E2E delay: " << server_set[i].service_list[j].e2e_delay
			     << endl;
		}
	}
	cout << endl;
	mtr.print();
	brief << endl;
	for (int i = 0; i < request_set.size(); i++) {
		if (request_set[i].accepted) {
			brief << "1";
		} else {
			brief << "0";
		}
	}
	brief << endl;
	cout << endl;
}

/*----------Metrics class-------------*/
void Metrics::statistic(vector<Service>& servcie_list,
                        vector<Server>& server_list) {
	for (int i = 0; i < servcie_list.size(); i++) {
		total_ideal_thuput += servcie_list[i].thuput;  //total_ideal_thuput = total_d_sr
		total_d_sr += servcie_list[i].consumed_sr;
	}
	service_counter = servcie_list.size();
	int num = 1;
	for (int i = 0; i < server_list.size(); i++) {
		for (int j = 0; j < server_list[i].service_list.size(); j++) {
			if (server_list[i].service_list[j].type == URLLC) {
				urllc_counter++;
				avg_urllc_delay += server_list[i].service_list[j].e2e_delay;
				avg_urllc_d_delay += server_list[i].service_list[j].d_delay;
			} else if (server_list[i].service_list[j].type == EMBB) {
				embb_counter++;
				avg_embb_delay += server_list[i].service_list[j].e2e_delay;
				avg_embb_d_delay += server_list[i].service_list[j].d_delay;
			}
			total_actual_thuput += server_list[i].service_list[j].degraded_thuput;
			if (server_list[i].service_list[j].e2e_delay <=
			    server_list[i].service_list[j].d_delay) {
				satisfy_counter++;
				total_satisfy_thuput +=
				    server_list[i].service_list[j].degraded_thuput;
				assert(server_list[i].service_list[j].id <= servcie_list.size());
				servcie_list[server_list[i].service_list[j].id].accepted = true;
				cout << num++
				     << ".\tService id: " << server_list[i].service_list[j].id
				     << ",\tService type: " << server_list[i].service_list[j].type << " is satisfied." << endl;
			} else {
				unsatisfy_counter++;
				total_unsatisfy_thput +=
				    server_list[i].service_list[j].degraded_thuput;
				cout << num++
				     << ".\tService id: " << server_list[i].service_list[j].id
				     << ",\tService type: " << server_list[i].service_list[j].type << " isn't satisfied." << endl;
			}
		}
	}
	assert(service_counter > 0);
	//assert(urllc_counter > 0);
	//assert(embb_counter > 0);
	satisfy_ratio = static_cast<double>(satisfy_counter) / service_counter * 100.;
	unsatisfy_ratio = static_cast<double>(unsatisfy_counter) / service_counter * 100.;
	drop_ratio = static_cast<double>(service_counter - satisfy_counter - unsatisfy_counter) / service_counter * 100.;
	if (urllc_counter > 0) {
		avg_urllc_delay /= urllc_counter;
		avg_urllc_d_delay /= urllc_counter;
	}
	if (embb_counter > 0) {
		avg_embb_delay /= embb_counter;
		avg_embb_d_delay /= embb_counter;
	}
}

void Metrics::print() {
	assert(total_ideal_thuput > 0);
	cout << "Satisfying service rate: "
	     << total_satisfy_thuput / total_ideal_thuput *
	            100.
	     << endl;
	cout << "Unsatisfying service rate: "
	     << static_cast<double>(total_unsatisfy_thput) / total_ideal_thuput *
	            100.
	     << endl;
	cout << "Dropped service rate: "
	     << total_ideal_thuput - total_satisfy_thuput -
	            total_unsatisfy_thput /
	                total_ideal_thuput * 100.
	     << endl;
	cout << "Average eMBB delay: " << avg_embb_delay << endl;
	cout << "Average Urllc delay: " << avg_urllc_delay << endl;
	cout << "Acceptance ratio: " << satisfy_ratio * 100. << endl;
	brief << satisfy_ratio << ",";
	brief << unsatisfy_ratio << ",";
	brief << drop_ratio << ",";
	brief << total_actual_thuput / total_ideal_thuput * 100. << ",";
	brief << total_satisfy_thuput / total_ideal_thuput * 100. << ",";
	brief << total_unsatisfy_thput / total_ideal_thuput * 100. << ",";
	brief << total_ideal_thuput << ",";
	brief << avg_embb_delay << ",";
	brief << avg_embb_d_delay << ",";
	brief << avg_urllc_delay << ",";
	brief << avg_urllc_d_delay << ",";
}

/*-----------DTM class----------------*/

vector<Server>::iterator DTM::eval(Service& service,
                                   vector<Server>& server_set) {
	vector<int> score_table;
	vector<Data> data_table;
	cout << "\nService id: " << service.id << " parameter table: " << endl;
	for (int i = 0; i < server_set.size(); i++) {      // Fill the data_table
		Data data = get_data(service, server_set[i]);  //Evaluation of deploying this service on this server
		if (data.deployable) {
			cout << "\tServer id: " << server_set[i].id << " are " << data.delay
			     << " / " << data.thuput << " / " << data.lost_thuput << endl;
			data_table.push_back(data);
		} else {
			cout << "\tService id: " << service.id << ", Service type: " << service.type
			     << " on Server id: " << server_set[i].id << ", Server type: " << server_set[i].type
			     << " isn't available." << endl;
		}
	}
	for (int i = 0; i < data_table.size(); i++) {
		assert(data_table[i].deployable == true);
	}
	normalization(data_table);  // normalize the columns of the data table
	cout << "Service id: " << service.id << " data Score table: " << endl;
	for (int i = 0; i < data_table.size(); i++) {
		cout << "\tServer ID: " << data_table[i].server_id
		     << ", Delay score: " << data_table[i].delay_score
		     << ", Thuput score: " << data_table[i].thuput_score
		     << ", Lost Thuput score: " << data_table[i].lost_thuput_score
		     << endl;
	}
	int index = WAA(data_table);  // get the weighted highest score server
	if (index == -1) {
		cout << "\tThis service is dropped" << endl;
		return server_set.end();
	} else {
		cout << "\tChoose Server id: " << (server_set.begin() + index)->id
		     << endl;
		return server_set.begin() + index;
	}
}

Data DTM::get_data(Service& service, Server server) {
	Data data;
	if (!server.avail(service)) {
		data.deployable = false;
		return data;
	} else {
		double prev_intfc = server.intfc;
		double r = 1.;  //reserved factor
		server.deploy(service);
		data.server_id = server.id;
		data.delay =
		    server.service_list.back().d_delay -
		    server.service_list.back().e2e_delay * r;  // * reserved_factor;
		//cout << "||e2e delay: " << server.service_list.back().e2e_delay << "||"
		//     << endl;
		data.thuput = server.service_list.back().degraded_thuput;
		int total_us_thuput = 0;
		for (int i = 0; i < server.service_list.size() - 1; i++) {
			if (server.service_list[i].e2e_delay >
			    server.service_list[i].d_delay) {
				total_us_thuput += (server.service_list[i].degraded_thuput -
				                    server.service_list[i].thuput * prev_intfc);
			}
		}
		data.lost_thuput = total_us_thuput;
		data.deployable = true;
		return data;
	}
}

void DTM::normalization(vector<Data>& data_table) {
	vector<Data> negative_delay;
	vector<Data> positive_delay;
	for (int i = 0; i < data_table.size(); i++) {
		if (data_table[i].delay < 0.) {
			negative_delay.push_back(data_table[i]);
		} else {
			positive_delay.push_back(data_table[i]);
		}
	}
	double d_max = -1.;
	double d_min = 100000.;
	for (int i = 0; i < positive_delay.size(); i++) {  //get positive highest and lowest value
		if (positive_delay[i].delay > d_max) {
			d_max = positive_delay[i].delay;
		}
		if (positive_delay[i].delay < d_min) {
			d_min = positive_delay[i].delay;
		}
	}
	for (int i = 0; i < positive_delay.size(); i++) {
		if (d_max != d_min) {  // 49~100
			positive_delay[i].delay_score =
			    49 * (1 - (positive_delay[i].delay - d_min) / (d_max - d_min)) +
			    51;
		} else {
			positive_delay[i].delay_score = 51;  //將影響降到最低
		}
	}  // get normalized delay score 51~100

	d_max = -100000.;
	d_min = 1;
	for (int i = 0; i < negative_delay.size(); i++) {
		if (negative_delay[i].delay > d_max) {
			d_max = negative_delay[i].delay;
		}
		if (negative_delay[i].delay < d_min) {
			d_min = negative_delay[i].delay;
		}
	}
	for (int i = 0; i < negative_delay.size(); i++) {
		if (d_max != d_min) {
			negative_delay[i].delay_score =
			    49 * (negative_delay[i].delay - d_min) / (d_max - d_min) + 1;
		} else {
			negative_delay[i].delay_score = 1;  //將影響降到最低
		}
	}  // get normalized delay score 1~50

	negative_delay.insert(negative_delay.end(), positive_delay.begin(),
	                      positive_delay.end());
	data_table.clear();
	data_table.insert(data_table.end(), negative_delay.begin(), negative_delay.end());
	assert(data_table.size() == negative_delay.size());

	d_max = -10000.;
	d_min = 100000.;
	for (int i = 0; i < data_table.size(); i++) {  //get highest and lowest thuput
		if (data_table[i].thuput > d_max) {
			d_max = data_table[i].thuput;
		}
		if (data_table[i].thuput < d_min) {
			d_min = data_table[i].thuput;
		}
	}
	for (int i = 0; i < data_table.size(); i++) {
		if (d_max != d_min) {
			data_table[i].thuput_score =
			    99 * ((data_table[i].thuput - d_min) / (d_max - d_min)) + 1;
		} else {
			data_table[i].thuput_score = 1;  //將影響降到最低
		}
	}

	d_max = -10000;
	d_min = 10000;
	for (int i = 0; i < data_table.size(); i++) {
		if (data_table[i].lost_thuput > d_max) {
			d_max = data_table[i].lost_thuput;
		}
		if (data_table[i].lost_thuput < d_min) {
			d_min = data_table[i].lost_thuput;
		}
	}
	for (int i = 0; i < data_table.size(); i++) {
		if (d_max != d_min) {
			data_table[i].lost_thuput_score =
			    99 * ((data_table[i].lost_thuput - d_min) / (d_max - d_min)) +
			    1;
		} else {
			data_table[i].lost_thuput_score = 1;
		}
	}
}

int DTM::WAA(vector<Data>& data_table) {  // Weighted_arithmetic_average
	if (!data_table.size()) {
		return -1;
	}
	int highest_index = -1;
	int max = -10000;
	for (int i = 0; i < data_table.size(); i++) {
		int total = psi * data_table[i].delay_score +
		            omega * data_table[i].thuput_score -  //0;
		            3 * tau * data_table[i].lost_thuput_score;
		if (total > max) {
			max = total;
			highest_index = i;
		}
	}
	assert(highest_index != -1);
	return data_table[highest_index].server_id;
}

/*-----------Lowest delay class----------------*/
vector<Server>::iterator LD::eval(Service& service,
                                  vector<Server>& server_set) {
	vector<double> delay_table;
	double min = 10000.;
	int min_index = -1;
	for (int i = 0; i < server_set.size(); i++) {  // Fill the delay_table
		if (server_set[i].avail(service)) {
			Server server = server_set[i];
			server.deploy(service);
			delay_table.push_back(server.service_list.back().e2e_delay);
		} else {
			delay_table.push_back(10000.);
		}
	}
	assert(server_set.size() == delay_table.size());
	for (int i = 0; i < delay_table.size(); i++) {
		if (delay_table[i] < 10000.) {
			if (min > delay_table[i]) {
				min = delay_table[i];
				min_index = i;
			}
		}
	}
	if (min_index == -1) {
		return server_set.end();
	} else {
		return server_set.begin() + min_index;
	}
}

/*-----------Lowest interference class----------------*/
vector<Server>::iterator LI::eval(Service& service,
                                  vector<Server>& server_set) {
	vector<Server> candidate1;
	vector<Server> candidate2;
	for (int i = 0; i < server_set.size(); i++) {
		if (server_set[i].avail(service)) {
			Server server = server_set[i];
			server.deploy(service);
			if (server.service_list.back().e2e_delay <= server.service_list.back().d_delay) {
				candidate1.push_back(server);
			} else {
				candidate2.push_back(server);
			}
		}
	}
	if (candidate1.size() > 0) {
		double max_intfc = 0.;
		int max_index = -1;
		for (int i = 0; i < candidate1.size(); i++) {
			if (candidate1[i].intfc > max_intfc) {
				max_intfc = candidate1[i].intfc;
				max_index = i;
			}
		}
		return server_set.begin() + candidate1[max_index].id;
	} else if (candidate2.size() > 0) {
		double max_intfc = 0.;
		int max_index = -1;
		for (int i = 0; i < candidate2.size(); i++) {
			if (candidate2[i].intfc > max_intfc) {
				max_intfc = candidate2[i].intfc;
				max_index = i;
			}
		}
		return server_set.begin() + candidate2[max_index].id;
	} else {
		return server_set.end();
	}
}