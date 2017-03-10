/*
 * route_traffic.cpp
 *
 *  Created on: Mar 6, 2017
 *      Author: full
 */
#include <vector>
using std::vector;

#include <iostream>
using std::cout; using std::endl;

#include <algorithm>

#include "graph.h"
#include "route.h"

#define MAX_MEDIAN 2 //对同一个start_segment选择几条median_segment路线

void SortCustomers(vector<Customer> & customers)
{
	std::sort(customers.begin(), customers.end(), [](const Customer & a, const Customer & b){
		return a.demand < b.demand;
	});
}

void SortRoutes(vector<Route> & routes)
{
	std::sort(routes.begin(), routes.end(), [](const Route & a, const Route & b){
		return a.traffic < b.traffic;
	});
}

bool limit_all_segment_shared(const vector<vector<int>> & median_segments,
                              const vector<int> & limit_all_segment,
			      int & target)
{
	int route_num = median_segments.size(), node_num;
	for(int i = 0; i < route_num; i++) {
		node_num = median_segments[i].size();
		for(int j = 0; j < node_num - 1; j++) {
			if (median_segments[i][j]   == limit_all_segment[0]
			 && median_segments[i][j+1] == limit_all_segment[1]) {
				target = i;//记录哪一条路线有重复
				return true;
			}
		}
	}
	return false;
}

/*
 * 返回从服务器到消费者的最短路径
 * @cust: 单个消费者节点
 * @servers：服务器列表
 */
vector<Route> get_route(Graph & g, const Customer & cust, const vector<int> & servers)
{
	Edge *edge = g.vertices[cust.agency];
	vector<Route> routes;
	int server_num = servers.size();

	while (edge != nullptr) {
		int start_point = edge->index;    //代理上一个点
		// 入口代价（代理上一个点到代理的代价）和入口带宽
		int cost_entra  = g.get_edge(start_point, cust.agency)->unit_cost;
		int limit_entra = g.get_edge(start_point, cust.agency)->band_width;

		vector<vector<int>> median_segments;  //临时储存到每个服务器的路径
		vector<vector<int>> distance_bounds;
		vector<vector<int>> limit_all_segments;

		int push_num = 0;  //加到median_segments中的数据个数

		//bool is_server = false;  // whether 'start_point' is a server

		for (int i = 0; i < server_num; i++) {
			if (start_point == servers[i]) {  // if 'start_point' is a server
				//is_server = true;
				median_segments.push_back({ start_point });
				distance_bounds.push_back({ cost_entra, limit_entra });
				limit_all_segments.push_back({ start_point, cust.agency });
				push_num += 1;
				continue;
			}
			// 从服务器到代理上一个点的路径
			vector<int> nodes_on_path  = g.DijkstraShortestPath(servers[i], start_point);
			
			vector<int> limit_all_segment(2);
			vector<int> distance_bound = g.RetrieveDistanceBound(nodes_on_path, limit_all_segment);
			//获取全路径流量上限
			if (distance_bound[1] > limit_entra){
				limit_all_segment[0] = start_point;
				limit_all_segment[1] = cust.agency;
				distance_bound[1] = limit_entra;
			}
			// 全路径单价之和
			distance_bound[0] += cost_entra;

			// nodes_on_path 是否有不在端点的服务器
			bool is_there_server = false;

			// nodes_on_path 是否有不在端点的代理点
			bool is_there_agency = false;

			int node_num = nodes_on_path.size();
			for (int j = 0; j != server_num; ++j) {
				for (int k = 1; k != node_num; ++k) {
					if (nodes_on_path[k] == servers[j]) {
						is_there_server = true;
						break;
					}
					if (nodes_on_path[k] == cust.agency) {
						is_there_agency = true;
						break;
					}
				}
				if (is_there_server || is_there_agency) {
					break;
				}
			}
			if (is_there_server || is_there_agency) {
				continue;
			}

			//判断 limit_all_segment 是否被共享
			int target = -1;  //记录哪一条路线有重复
			if (limit_all_segment_shared(median_segments, limit_all_segment, target)) {
				if (distance_bound[0] < distance_bounds[target][0]) {
					distance_bounds[target] = distance_bound;
					median_segments[target] = nodes_on_path;
					limit_all_segments[target] = limit_all_segment;
				}
			} else {
				if (push_num < MAX_MEDIAN) {
					median_segments.push_back(nodes_on_path);
					distance_bounds.push_back(distance_bound);
					limit_all_segments.push_back(limit_all_segment);
					push_num += 1;
				} else {
					auto iter_max_distance = std::max_element(
						distance_bounds.begin(), distance_bounds.end(),
						[](const vector<int> & a, const vector<int> & b) {
							return a[0] < b[0];
					});
					auto index_max_distance = iter_max_distance - distance_bounds.begin();
					if ((*iter_max_distance)[0] > distance_bound[0]) {
						*iter_max_distance = distance_bound;
						median_segments[index_max_distance] = nodes_on_path;
						limit_all_segments[index_max_distance] = limit_all_segment;
					}
				}
			}
		}

		for (int i = 0; i < push_num; i++){
			Route route;
			route.start_segment[0]  = start_point;
			route.start_segment[1]  = cust.agency;
			route.limit_entra       = limit_entra;
			route.limit_all_segment = limit_all_segments[i];
			route.traffic           = distance_bounds[i][0];
			route.limit_all         = distance_bounds[i][1];
			route.median_segment    = median_segments[i];
			routes.push_back(route);
		}

		edge = edge->next_edge;
	}

	return routes;
}

/*
 * 计算单条路径传输的代价 更新邻接表 和 消费者需求
 * @route： 服务器到消费者的路径
 */
vector<int> get_route_cost(Customer & cust, const Route & route, Graph & g)
{
	vector<int> cost_transfer(2);

	// 数据传输量 消费者需求和路径带宽上限的较小值
	int transfer = (route.limit_all < cust.demand) ? route.limit_all : cust.demand;

	cust.demand -= transfer;          // 更新消费者需求
	UpdateGraph(g, route, transfer);  // 更新邻接表

	int cost = route.traffic * transfer;  // 传输代价

	cost_transfer[0] = cost;
	cost_transfer[1] = transfer;
	return cost_transfer;
}

/* 在选完路径输送流量之后，更新邻接表中的带宽信息
 * @g：邻接表
 * @route：路径信息
 * @transfer: 传输量
 * */
void UpdateGraph(Graph & g, const Route & route, int transfer)
{
	Edge *edge = g.get_edge(route.start_segment[0], route.start_segment[1]);
	edge->band_width -= transfer;
	int median_edge_num = route.median_segment.size() - 1;
	if (median_edge_num != 0) {
		for (int i = 0; i != median_edge_num; ++i) {
			edge = g.get_edge(route.median_segment[i], route.median_segment[i+1]);
			edge->band_width -= transfer;
		}
	}
}

void PrintRoute(const Route & route)
{
	for (auto i : route.median_segment) {
		cout << i << " to ";
	}
	cout << route.start_segment[1] << endl;
	cout << "cost: " << route.traffic << endl;
	cout << "limit all: " << route.limit_all << endl;
	cout << "limit entra: " << route.limit_entra << endl;
}

/* 选择路径并计算代价
 * @total_traffic : 总的传输代价
 * @route_transfer: 总的传输路径
 **/
void select_route(vector<Customer> & customers, const vector<int> & servers,
                  Graph & g, int & total_traffic, vector<RouteTransfer> & route_transfers)
{
	for (auto & customer : customers) {
		cout << "------------------" << endl;
		cout << "demand: " << customer.demand << endl;
		cout << endl;

		vector<Route> routes = get_route(g, customer, servers);
		SortRoutes(routes);
		
		int route_num = routes.size();

		vector<int> limit_all_segment(2), distance_bound;

		for (int i = 0; i != route_num; ) {

			if (i > 0) {  // after selecting a route, update limits of other routes
				routes[i].limit_entra = g.get_edge(routes[i].start_segment[0], customer.agency)->band_width;
				if (routes[i].median_segment.size() == 1) {
					routes[i].limit_all = routes[i].limit_entra;
					routes[i].limit_all_segment = routes[i].limit_all_segment;
				} else {
					distance_bound = g.RetrieveDistanceBound(routes[i].median_segment, limit_all_segment);
					if (routes[i].limit_entra > distance_bound[1]) {
						routes[i].limit_all = distance_bound[1];
						routes[i].limit_all_segment = limit_all_segment;
					} else {
						routes[i].limit_all = routes[i].limit_entra;
						routes[i].limit_all_segment[0] = routes[i].start_segment[0];
						routes[i].limit_all_segment[1] = customer.agency;
					}
				}
			}

			if (routes[i].limit_all == 0) {
				i += 1;
				continue;
			}

			vector<int> cost_transfer = get_route_cost(customer, routes[i], g);
			
			total_traffic += cost_transfer[0];
			RouteTransfer route_transfer;
			
			route_transfer.segment = routes[i].median_segment;
			route_transfer.segment.push_back(routes[i].start_segment[1]);
			route_transfer.transfer = cost_transfer[1];
			route_transfers.push_back(route_transfer);
			i += 1;

			for (const auto & j : route_transfer.segment){
				cout << j << " to ";
			}
			cout << "customer "          << customer.index   << endl;
			cout << "transfer: "         << cost_transfer[1] << endl;
			cout << "remaining demand: " << customer.demand  << endl;
			cout << "cost: "             << cost_transfer[0] << endl;

			cout << endl;
			if (customer.demand == 0) {
				break;
			}
		}
	}
}

