#include "deploy.h"
#include "strlib.h"
#include "lib_time.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include <string>
using std::string;

#include <vector>
using std::vector;

#include <iostream>
using std::cout; using std::endl;

#include "route.h"
#include "graph.h"
#include "server.h"

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
	
	// 边信息
	Graph g(node_num);
	g.CreateFromBuf(topo + 4, link_num);

	Graph g_copy = g;
	//g.Print();

	// 消费者
	vector<Customer> customers(customer_num);
	char **base_pos = topo + 4 + link_num + 1;
	vector<string> numbers;
	for(int i = 0; i < customer_num; i++) {
		string line(base_pos[i]);
		numbers = str_split(line, " ");
		customers[i].index  = std::stoi(numbers[0]);
		customers[i].agency = std::stoi(numbers[1]);
		customers[i].demand = std::stoi(numbers[2]);
	}

	SortCustomers(customers);  // 按需求降序排列

	auto customers_copy(customers);


	// 服务器集合
	//vector<int> servers{ 9, 20, 8, 2, 4, 40, 16 };

	int server_num = 4;
	ServerSelector selector(server_num, node_num);
	selector.Select(g);
	auto servers = selector.get_servers();
	selector.PrintServers();

	// 选路
	int total_traffic;
	vector<RouteTransfer> route_transfers;
	select_route(customers, servers, g, total_traffic, route_transfers);

	selector.Mutate();
	servers = selector.get_servers();
	select_route(customers_copy, servers, g_copy, total_traffic, route_transfers);


	// 需要输出的内容
	char * topo_file = (char *)"17\n\n0 8 0 20\n21 8 0 20\n9 11 1 13\n21 22 2 20\n23 22 2 8\n1 3 3 11\n24 3 3 17\n27 3 3 26\n24 3 3 10\n18 17 4 11\n1 19 5 26\n1 16 6 15\n15 13 7 13\n4 5 8 18\n2 25 9 15\n0 7 10 10\n23 24 11 23";

	// 直接调用输出文件的方法输出到指定文件中(ps请注意格式的正确性，如果有解，第一行只有一个数据；第二行为空；第三行开始才是具体的数据，数据之间用一个空格分隔开)
	write_result(topo_file, filename);

}
