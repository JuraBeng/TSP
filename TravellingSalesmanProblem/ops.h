#ifndef OPS_H_
#define OPS_H_

#ifndef N
#define N 13
#endif
#ifndef SIZE_BYTES
#define SIZE_BYTES N*N*sizeof(float)
#endif
namespace ops
{

	float reduce_row(float(&dist_matrix)[N][N], int row);

	float reduce_col(float(&dist_matrix)[N][N], int col);

	float reduce_matrix(float(&dist_matrix)[N][N]);

	void set_inf_row_col(float(&dist_matrix)[N][N], int row, int col);

	void set_inf_col(float(&dist_matrix)[N][N], int col);

	void set_inf_row(float(&dist_matrix)[N][N], int row);

	void set_mat_value(float(&dist_matrix)[N][N], int row, int col, float val);


}
#endif
