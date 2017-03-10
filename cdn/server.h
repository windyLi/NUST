/*
 * SelectLoc.h
 *
 *  Created on: Mar 7, 2017
 *      Author: full
 */

#ifndef CDN_SERVER_H_
#define CDN_SEEVER_H_

#include "graph.h"
#include <vector>
#include <iostream>

struct Node {
	int index;
	int out_degree;

	Node(int id, int od) : index(id), out_degree(od) {}
};

class ServerSelector {

private:
	int server_num;
	int backup_num;
	std::vector<Node> servers;
	std::vector<Node> servers_copy;
	std::vector<Node> backups;
public:
	//初始化服务器数量
	ServerSelector(int server_num, int node_num) 
		: server_num(server_num), backup_num(node_num - server_num) {}

	//选择服务器位置, 写入servers, 返回备用服务器集合
	void Select(const Graph & g);

	//挑选备用服务器节点，（随机）替换已有服务器节点
	void Mutate(void);

	std::vector<int> get_servers(void);

	void PrintServers(void);
};

#endif /* CDN_SELECTLOC_H_ */
