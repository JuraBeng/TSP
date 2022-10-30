#include "Node.h"
float Node::reduceMatrix()
{
	for (int i = 0; i < path.size(); i++)
	{
		ops::set_inf_row(distance_matrix, path[i]);
	}
	ops::set_inf_col(distance_matrix, idx);
	ops::set_mat_value(distance_matrix, idx, path[0], FLT_MAX);
	return  ops::reduce_matrix(distance_matrix);

}
Node::Node(std::vector<std::vector<float>>& t_distance_matrix, std::vector<int> t_explorable, std::vector<int> t_path, int t_idx, float upper_bound, int parent_idx)
{
	this->path = t_path;
	this->explorable = t_explorable;
	this->idx = t_idx;
	explorable.erase(std::remove(explorable.begin(), explorable.end(), idx), explorable.end());
	distance_matrix = t_distance_matrix;
	if(parent_idx == -1)
	{
		bound = upper_bound + ops::reduce_matrix(distance_matrix);
	}
	else
	{

		// compute cost and set all other things to node
		bound = reduceMatrix() + t_distance_matrix[parent_idx][idx] + upper_bound;
	}
}

Node::~Node()
{
}

std::pair<std::vector<int>, float> Node::computeTotalCost(std::vector<std::vector<float>> &t_distance_matrix)
{
	path.push_back(idx);
	path.push_back(path[0]);
	float total_cost = 0.0f;
	for (int i = 0; i < path.size()-1; i++)
	{
		total_cost += t_distance_matrix[path[i]][path[i + 1]];
	}
	return std::make_pair(path, total_cost);
}
