/*
 * route_traffic.h
 *
 *  Created on: Mar 6, 2017
 *      Author: full
 */
#ifndef CDN_ROUTE_H_
#define CDN_ROUTE_H_

#include <vector>
#include "graph.h"

struct Customer {
	int index;   //消费者
	int agency;  //消费者相连的代理
	int demand;  //消费者需求
};

void SortCustomers(std::vector<Customer> &);

struct Route {
	int start_segment[2];                //路径开始段，由代理到它的上一个节点
	std::vector<int> median_segment;     //路径连接到服务器的中间段，从服务器到代理上一个点
	std::vector<int> limit_all_segment;  //路径流量上限段
	int traffic;      //整段路径的传输单价
	int limit_all;    //路径的流量上限
	int limit_entra;  //入口段的流量上限
};

struct RouteTransfer{
	std::vector<int> segment;  //传输路径
	int transfer;  //传输量
};


// struct Customer_route{
// 	int start_point;   //消费者节点
// 	int agency_point;  //消费者节点的代理节点
// 	std::vector<Route> Route;  //多条路径信息
// };

void PrintRoute(const Route & route);

void select_route(std::vector<Customer> & customers,
                  const std::vector<int> & servers,
		  Graph & g,
		  int & total_traffic,
		  std::vector<RouteTransfer> & route_transfers);

std::vector<Route> get_route(Graph & g, const Customer & cust, const std::vector<int> & servers);

void UpdateGraph(Graph & g, const Route & route, int transfer);

std::vector<int> get_route_traffic(Customer & cust, const Route & route, Graph & g);

void SortRoutes(std::vector<Route> & routes);

#endif /* CDN_ROUTE_H_ */
