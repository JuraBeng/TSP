#ifndef OPS_H_
#define OPS_H_
const int N = 11;
const int SIZE_BYTES = N * N * sizeof(float);
#include "stdafx.h"
namespace ops
{

	float reduce_row(std::vector<float> &distance_matrix , int row);

	float reduce_col(std::vector<float>& distance_matrix, int col);

	float reduce_matrix(std::vector<float>& distance_matrix);

	void set_inf_col(std::vector<float>& distance_matrix, int col);

	void set_inf_row(std::vector<float>& distance_matrix, int row);

	void set_mat_value(std::vector<float>& distance_matrix, int row, int col, float val);


}
#endif
