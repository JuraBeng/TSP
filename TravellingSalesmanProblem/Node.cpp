#include "Node.h"
float Node::reduceMatrix()
{
	return 0.0f;
}
Node::Node(float (& t_matrix)[N][N], std::vector<int> t_explorable, std::vector<int> t_path, int t_idx, float upper_bound, int parent_idx)
{
	this->path = t_path;
	this->explorable = t_explorable;
	this->idx = t_idx;
	explorable.erase(std::remove(explorable.begin(), explorable.end(), idx), explorable.end());
	memcpy(matrix, t_matrix, SIZE_BYTES);
	if(parent_idx == -1)
	{
		bound = upper_bound + ops::reduce_matrix(matrix);
	}
	else
	{
		for (int i = 0; i < path.size(); i++)
		{
			ops::set_inf_row(matrix, path[i]);
		}
		ops::set_inf_col(matrix, idx);
		ops::set_mat_value(matrix, idx, path[0], FLT_MAX);
		float reduction = ops::reduce_matrix(matrix);

		// compute cost and set all other things to node
		bound = reduction + t_matrix[parent_idx][idx] + upper_bound;
	}
}

Node::~Node()
{
}