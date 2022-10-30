#ifndef NODE_H_
#define NODE_H_

#include "ops.h"

class Node
{
public:
	Node(int t_idx, float t_cost, float t_matrix[N][N], std::vector<int> t_explorable, float t_bound);
	Node(int t_idx, std::vector<int> t_explorable = {});
	~Node();
	int idx;
	float bound;
	float matrix[N][N];
	void addChild(Node* node);
	Node* getMinChild();
	Node* solveChildren(std::vector<int> data);
	void setMatrix(float t_matrix[N][N]);
	std::vector<int> getPath();

private:
	std::vector<int> explorable = {};
	std::vector<Node*> children = {};
};
#endif

