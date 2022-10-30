#include "ops.h"

/**
 * Pomocn� funkce pro redukci jednoho ��dku
 *
 * Prvn� for mi nalezne nejmen�� hodnotu v ��dku pro redukci
 *
 * Druh� for mi redukuje cel� ��dek nejmen�� hodnotou za p�edpokladu, �e nejmen�� hodnota nen� INF nebo 0
 *
 * \return nejmen�� hodnota pro redukci ��dku
 */
float ops::reduce_row(float(& dist_matrix)[N][N], int row)
{
	float min_val = FLT_MAX;
	for (int i = 0; i < N; i++)
	{
		if (dist_matrix[row][i] == FLT_MAX)
			continue;
		if (dist_matrix[row][i] <= min_val)
			min_val = dist_matrix[row][i];
	}
	if (min_val != 0.0f && min_val != FLT_MAX)
	{
		for (int i = 0; i < N; i++)
		{
			if (dist_matrix[row][i] == FLT_MAX)
				continue;
			dist_matrix[row][i] -= min_val;
		}

	}
	if (min_val == FLT_MAX)
		min_val = 0.0f;
	return min_val;
}

/**
 * Pomocn� funkce pro redukci jednoho sloupce
 *
 * Prvn� for mi nalezne nejmen�� hodnotu v sloupci pro redukci
 *
 * Druh� for mi redukuje cel� sloupec nejmen�� hodnotou za p�edpokladu, �e nejmen�� hodnota nen� INF nebo 0
 *
 * \return nejmen�� hodnota pro redukci sloupce
 */
float ops::reduce_col(float(& dist_matrix)[N][N], int col)
{
	float min_val = FLT_MAX;
	for (int i = 0; i < N; i++)
	{
		if (dist_matrix[i][col] == FLT_MAX)
			continue;
		if (dist_matrix[i][col] <= min_val)
			min_val = dist_matrix[i][col];
	}
	if (min_val != 0.0f && min_val != FLT_MAX)
	{
		for (int i = 0; i < N; i++)
		{
			if (dist_matrix[i][col] == FLT_MAX)
				continue;
			dist_matrix[i][col] -= min_val;
		}
	}
	if (min_val == FLT_MAX)
		min_val = 0.0f;
	return min_val;
}

/**
 * Funkce pro redukci cel� matice
 *
 * Prvn� for mi redukuje v�echny ��dky
 *
 * Druh� for mi redukuje v�echny sloupce
 *
 * \return Suma redukc� proveden�ch na sloupc�ch a ��dc�ch matice
 */
float ops::reduce_matrix(float(& dist_matrix)[N][N])
{
	float row_sum = 0.0f;
	float col_sum = 0.0f;
	for (int i = 0; i < N; i++)
	{
		row_sum += ops::reduce_row(dist_matrix, i);
	}
	for (int j = 0; j < N; j++)
	{
		col_sum += ops::reduce_col(dist_matrix, j);
	}
	return col_sum + row_sum;
}

/**
 * Funkce pro nastaven� sloupce na nekone�no
 */
void ops::set_inf_col(float(& dist_matrix)[N][N], int col)
{
	for (int i = 0; i < N; i++)
	{
		ops::set_mat_value(dist_matrix, i, col, FLT_MAX);
	}
}

/**
 * Funkce pro nastaven� ��dku na nekone�no
 */
void ops::set_inf_row(float(& dist_matrix)[N][N], int row)
{
	for (int i = 0; i < N; i++)
	{
		ops::set_mat_value(dist_matrix, row, i, FLT_MAX);
	}
}

/**
 * Pomocn� funkce pro nastaven� konkr�tn� hodnoty matice
 */
void ops::set_mat_value(float(& dist_matrix)[N][N], int row, int col, float val)
{
	dist_matrix[row][col] = val;
}
