#include <utility>
#include <algorithm>

#include <vector>
using std::vector;

#include <iostream>
using std::cout; using std::endl;

#include "server.h"
#include "graph.h"


//根据网络节点出度选定一个出度最大的集合当作最初初始服务器位置
void ServerSelector::Select(const Graph & g)
{
	// retrieve the outdegree of each node within graph
	int node_num = server_num + backup_num;
	int out_degree;
	Edge *edge = nullptr;
	vector<Node> nodes;
	for (int i = 0; i < node_num; i++) {
		out_degree = 0;
		edge = g.vertices[i];
		while (edge != nullptr) {
			out_degree += 1;
			edge = edge->next_edge;
		}
		nodes.emplace_back(i, out_degree);
	}

	// sort nodes by out_degree decrementally
	std::sort(nodes.begin(), nodes.end(), [] (const Node & a, const Node & b) {
		return a.out_degree > b.out_degree;
	});

	servers.insert(servers.end(), nodes.begin(), nodes.begin() + server_num);
	backups.insert(backups.end(), nodes.begin() + server_num, nodes.end());
	servers_copy = servers;
}


//分别给servers 和 backups 一个参考了节点出度比例的随机数，并进行替换
//server中节点出度分别为 [a,b,c,d] 替换a的概率p=1-(a-min(server)+1)/(sum(server)-(min(server-1))*4);
void ServerSelector::Mutate(void)
{
	// make a copy of servers before mutating
	// done within Select() method

	// retrieve the server with minimal out_degree
	// and the sum of out_degrees of all the servers
	int server_min_pos = 0, server_sum = 0, out_degree;
	for (int i = 0; i != server_num; ++i) {
		out_degree = servers[i].out_degree;
		server_sum += out_degree;
		if (servers[server_min_pos].out_degree > out_degree) {
			server_min_pos = i;
		}
	}

	vector<double> server_probabilities;
	double probability;
	int out_degree_min = servers[server_min_pos].out_degree;
	for (int i = 0; i != server_num; ++i) {
		probability  = (1 + rand() % 30) / 31.0;
		probability *= servers[i].out_degree - out_degree_min + 1;
		probability /= server_sum - server_num * (out_degree_min - 1);
		server_probabilities.push_back(probability);
	}

	
	auto iter_min = std::min_element(server_probabilities.begin(), server_probabilities.end());
	int server_substitute_pos = iter_min - server_probabilities.begin();
	//最可能被替换servers数组下标
	//float minpro_sev = rand() % 30 / (float) 31 * (servers[0].out_degree - (*minit).server_outdeg +1) / (servers_outdegsum-server_num*((*minit).server_outdeg-1))*1.0;
	//TODO: 如果severs 只有一个元素下面循环会不会越界
	

	int backup_min_pos = 0, backup_sum = 0;
	for (int i = 0; i != backup_num; ++i) {
		out_degree = backups[i].out_degree;
		backup_sum += out_degree;
		if (backups[backup_min_pos].out_degree > out_degree) {
			backup_min_pos = i;
		}
	}

	vector<double> backup_probabilities;
	out_degree_min = backups[backup_min_pos].out_degree;
	for (int i = 0; i != backup_num; ++i) {
		probability  = (1 + rand() % 30) / 31.0;
		probability *= backups[i].out_degree - out_degree_min + 1;
		probability /= backup_sum - backup_num * (out_degree_min - 1);
		server_probabilities.push_back(probability);
	}

	auto iter_max = std::max_element(backup_probabilities.begin(), backup_probabilities.end());
	int backup_substitute_pos = iter_max - backup_probabilities.begin();

	std::swap(servers[server_substitute_pos], backups[backup_substitute_pos]);
}

//从servers 中提取servers的ID
//返回值： vector<int> serversid
vector<int> ServerSelector::get_servers(void)
{
	vector<int> indices;
	for(const auto & server : servers) {
		indices.push_back(server.index);
	}
	return indices;
}

void ServerSelector::PrintServers(void)
{
	//cout << servers.size() << endl;
	cout << "id\tout_degree" << endl;
	for (const auto & server : servers) {
		cout << server.index << '\t' << server.out_degree << endl;
	}
}
