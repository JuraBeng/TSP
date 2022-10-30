#ifndef NODE_H_
#define NODE_H_
#include "ops.h"
class Node
{
public:
	Node(std::vector<std::vector<float>>& t_distance_matrix, std::vector<int> t_explorable, std::vector<int> t_path, int t_idx, float upper_bound, int parent_idx);
	~Node();
	std::pair<std::vector<int>, float> computeTotalCost(std::vector<std::vector<float>>& t_distance_matrix);
	int idx;
	float bound;
	float reduceMatrix();
	std::vector<int> path;
	std::vector<int> explorable;
	std::vector<std::vector<float>> distance_matrix = {};

};
#endif

