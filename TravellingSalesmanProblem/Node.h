#ifndef NODE_H_
#define NODE_H_
#include "ops.h"
class Node
{
public:
	Node(float (&t_matrix)[N][N], std::vector<int> t_explorable, std::vector<int> t_path, int t_idx, float upper_bound, int parent_idx);
	~Node();
	int idx;
	float bound;
	float reduceMatrix();
	std::vector<int> path;
	std::vector<int> explorable;
	float matrix[N][N];

};
#endif

