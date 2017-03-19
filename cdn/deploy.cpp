#include "deploy.h"
#include "lib_time.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <util.h>
#include <string>
#include <vector>
#include <iostream>
#include <cmath>
using std::exp;
#include <map>
using namespace std;


#include "server.h"
#include <MCMF.h>
#define SETZR(a) memset(a,0,sizeof(a))

void PrintPath(const vector<int> & nodes_on_path)
{
	for (auto & index : nodes_on_path) {
		cout << index << " ";
	}
	cout << endl;
}

void PrintCustomer(const Customer & customer)
{
	cout << "index: "  << customer.index << endl;
	cout << "demand: " << customer.demand << endl;
}

//你要完成的功能总入口
void deploy_server(char * topo[MAX_EDGE_NUM], int line_num, char * filename)
{

	int node_num, link_num, customer_num, server_unit_cost;

	string line(topo[0]);
	vector<string> str_vec = str_split(line, " ");

	node_num = std::stoi(str_vec[0]);
	link_num = std::stoi(str_vec[1]);
	customer_num = std::stoi(str_vec[2]);
	server_unit_cost = std::atoi(topo[2]);

	cout << "number of nodes:\t" << node_num << endl;
	cout << "number of links:\t" << link_num << endl;
	cout << "number of customers:\t" << customer_num << endl;
	cout << "cost of a server:\t" << server_unit_cost << endl;
	
	int server_num = 26;
	NetGraph g(node_num);
	ServerSelector selector(server_num, node_num);
	selector.CreateFromBuf(topo + 4, g, link_num);
	//customer
	vector<Customer> customers(customer_num);
	map<int, int> agency_map;
	char **base_pos = topo + 4 + link_num + 1;
	vector<string> numbers;
	for(int i = 0; i < customer_num; i++) {
		string line(base_pos[i]);
		numbers = str_split(line, " ");
		customers[i].index  = std::stoi(numbers[0]);
		customers[i].agency = std::stoi(numbers[1]);
		customers[i].demand = std::stoi(numbers[2]);
		agency_map[customers[i].agency] = customers[i].index;
	}

//**************************************************************************************
//	// 服务器集合

//	vector<int> servers{ 37, 35, 14, 44, 38, 22};
    selector.GenEvalueSheet(g, selector.evalue_sheet, agency_map);
	selector.Select(selector.evalue_sheet);
	vector<int> servers = selector.get_servers();
 	selector.PrintServers();

 	//******************************************************
 	//寻路
 	int m,n,s,t;
	int pointer[10000];
	SETZR(pointer);
	vector<vector<int>> paths;
	bool find_so = true;

	int customer_demand = dataLoad(topo,servers,line_num, m,n,s,t,pointer);

	int cost = findCost(paths, pointer, n,  s,t);

	cout<<customer_demand<<endl;
	for (auto i=0;i<paths.size();i++){
		customer_demand-=paths[i][0];
		cout<<customer_demand<<endl;
	}
	if (customer_demand!=0){
		cout<<customer_demand<<endl;
		cout<<"no solution!!"<<endl;
		find_so = false;
	}

//	// 选路
//	int cur_cost;
//	int pre_cost = INF;
//	int best_cost = INF;
//	vector<vector<int>> best_path;
//	vector<int> best_servers;
//	//TODO: T与delta的选择应该 考虑 de/t之后的值，确保能有多少方案可以变异。同时T的detla次方衰减到t_time 需要的次数与问题规模相匹配； 要考虑备选服务器数量。需要迭代次数与备选服务器数量近似。
//
//	int de = -1;
//	double T = 1000, delta = 0.98, T_time = 998;
//	while (T > T_time) {
//		cost = findCost(paths, pointer, n,  s,t);
//		de = pre_traffic - total_traffic;
//
//		if(de > 0){
//			//接受该次改变，servers为上次mutate后的servers。
//			best_traffic = total_traffic;
//			best_route = route_transfers;
//			best_servers = selector.servers;
//			//pre_servers = selector.servers;
//		}
//		else{
//			if(exp(de/T) < (1 + rand() % 9999) / 10000.0){
//				//计算退火概率，以一定机率改变selector.servers为best_servers;
//				selector.Rollback();
//			}
//		}
//		pre_traffic = total_traffic;
//		selector.Mutate();
//		servers = selector.get_servers();
//		//g = g_copy;
//		customers = customers_copy;
////		T *= delta;
//		T--;
//	}


//	select_route(customers_copy, servers, g_copy, total_traffic, route_transfers);



	string output = pathString(paths, agency_map, find_so);
	char * topo_file = new char[output.size() + 1];
	std::copy(output.begin(), output.end(), topo_file);
	topo_file[output.size()] = '\0'; // don't forget the terminating 0

	 //直接调用输出文件的方法输出到指定文件中(ps请注意格式的正确性，如果有解，第一行只有一个数据；第二行为空；第三行开始才是具体的数据，数据之间用一个空格分隔开)
	write_result(topo_file, filename);
	delete[] topo_file;
}
