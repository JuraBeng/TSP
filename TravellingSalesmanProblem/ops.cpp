#include "ops.h"

/**
 * Pomocná funkce pro redukci jednoho øádku
 *
 * První for mi nalezne nejmenší hodnotu v øádku pro redukci
 *
 * Druhý for mi redukuje celý øádek nejmenší hodnotou za pøedpokladu, že nejmenší hodnota není INF nebo 0
 *
 * \return nejmenší hodnota pro redukci øádku
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
 * Pomocná funkce pro redukci jednoho sloupce
 *
 * První for mi nalezne nejmenší hodnotu v sloupci pro redukci
 *
 * Druhý for mi redukuje celý sloupec nejmenší hodnotou za pøedpokladu, že nejmenší hodnota není INF nebo 0
 *
 * \return nejmenší hodnota pro redukci sloupce
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
 * Funkce pro redukci celé matice
 *
 * První for mi redukuje všechny øádky
 *
 * Druhý for mi redukuje všechny sloupce
 *
 * \return Suma redukcí provedených na sloupcích a øádcích matice
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
 * Funkce pro nastavení sloupce na nekoneèno
 */
void ops::set_inf_col(float(& dist_matrix)[N][N], int col)
{
	for (int i = 0; i < N; i++)
	{
		ops::set_mat_value(dist_matrix, i, col, FLT_MAX);
	}
}

/**
 * Funkce pro nastavení øádku na nekoneèno
 */
void ops::set_inf_row(float(& dist_matrix)[N][N], int row)
{
	for (int i = 0; i < N; i++)
	{
		ops::set_mat_value(dist_matrix, row, i, FLT_MAX);
	}
}

/**
 * Pomocná funkce pro nastavení konkrétní hodnoty matice
 */
void ops::set_mat_value(float(& dist_matrix)[N][N], int row, int col, float val)
{
	dist_matrix[row][col] = val;
}
