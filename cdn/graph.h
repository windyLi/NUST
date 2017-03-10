#ifndef GRAPH_H
#define GRAPH_H
#include <vector>
#include <stdio.h>

#include <limits>

#define INF std::numeric_limits<int>::max()

struct Edge {
        int  index;
        Edge *next_edge;
        int  band_width;
        int  unit_cost;
};

class Graph {
public:
	int vertex_num;
	int edge_num;
	std::vector<Edge*> vertices;

	Graph(int node_num) : vertex_num(node_num), edge_num(0)
	{
		for (int i = 0; i != node_num; ++i) {
			vertices.push_back(nullptr);
		}
	}
	inline int get_vertex_num(void) { return this->vertex_num; }
	inline int get_edge_num(void)   { return this->edge_num; }
	Edge * get_edge(int src, int dst);
	void CreateFromBuf(char ** buf, int line_num);
	void Print(void);
	std::vector<int> DijkstraShortestPath(int src, int dst);
	int Distance(const std::vector<int> & nodes_on_path);
	int Bound(const std::vector<int> & nodes_on_path);
	std::vector<int> BoundSegment(const std::vector<int> & nodes_on_path);
	std::vector<int> RetrieveDistanceBound(const std::vector<int> & nodes_on_path, std::vector<int> & bound_segment);
	~Graph();
};

#endif
