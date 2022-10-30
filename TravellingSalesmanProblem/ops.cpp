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
float ops::reduce_row(std::vector<std::vector<float>>& distance_matrix, int row)
{
	float min_val = FLT_MAX;
	for (int i = 0; i < distance_matrix.size(); i++)
	{
		if (distance_matrix[row][i] == FLT_MAX)
			continue;
		if (distance_matrix[row][i] <= min_val)
			min_val = distance_matrix[row][i];
	}
	if (min_val != 0.0f && min_val != FLT_MAX)
	{
		for (int i = 0; i < distance_matrix.size(); i++)
		{
			if (distance_matrix[row][i] == FLT_MAX)
				continue;
			distance_matrix[row][i] -= min_val;
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
float ops::reduce_col(std::vector<std::vector<float>>& distance_matrix, int col)
{
	float min_val = FLT_MAX;
	for (int i = 0; i < distance_matrix.size(); i++)
	{
		if (distance_matrix[i][col] == FLT_MAX)
			continue;
		if (distance_matrix[i][col] <= min_val)
			min_val = distance_matrix[i][col];
	}
	if (min_val != 0.0f && min_val != FLT_MAX)
	{
		for (int i = 0; i < distance_matrix.size(); i++)
		{
			if (distance_matrix[i][col] == FLT_MAX)
				continue;
			distance_matrix[i][col] -= min_val;
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
float ops::reduce_matrix(std::vector<std::vector<float>>& distance_matrix)
{
	float row_sum = 0.0f;
	float col_sum = 0.0f;
	for (int i = 0; i < distance_matrix.size(); i++)
	{
		row_sum += ops::reduce_row(distance_matrix, i);
	}
	for (int j = 0; j < distance_matrix.size(); j++)
	{
		col_sum += ops::reduce_col(distance_matrix, j);
	}
	return col_sum + row_sum;
}

/**
 * Funkce pro nastavení sloupce na nekoneèno
 */
void ops::set_inf_col(std::vector<std::vector<float>>& distance_matrix, int col)
{
	for (int i = 0; i < distance_matrix.size(); i++)
	{
		ops::set_mat_value(distance_matrix, i, col, FLT_MAX);
	}
}

/**
 * Funkce pro nastavení øádku na nekoneèno
 */
void ops::set_inf_row(std::vector<std::vector<float>>& distance_matrix, int row)
{
	for (int i = 0; i < distance_matrix.size(); i++)
	{
		ops::set_mat_value(distance_matrix, row, i, FLT_MAX);
	}
}

/**
 * Pomocná funkce pro nastavení konkrétní hodnoty matice
 */
void ops::set_mat_value(std::vector<std::vector<float>>& distance_matrix, int row, int col, float val)
{
	distance_matrix[row][col] = val;
}
