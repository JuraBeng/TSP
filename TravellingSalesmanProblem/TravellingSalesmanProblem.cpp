#include <queue>
#include "Node.h"

using namespace std;
const int N = 13;
vector<double> distances;

int read_tsp_file(const char* fname, std::vector<std::vector<float>> &t_dist_matrix)
{
	std::ifstream file(fname);
	vector<double> xs, ys;

	if (file.is_open())
	{
		std::string line;

		std::getline(file, line);
		std::getline(file, line);
		std::getline(file, line);
		std::getline(file, line);
		std::getline(file, line);
		std::getline(file, line);
		std::getline(file, line);

		while (std::getline(file, line)) {
			if (line[0] == 'E')
				break;

			stringstream sin(line);
			int id;
			double x, y;
			sin >> id >> x >> y;

			xs.push_back(x);
			ys.push_back(y);
		}

		unsigned int n = xs.size();

		distances.resize(n * n);

		for(int i = 0; i < N; i++)
		{
			std::vector<float> row;
			for(int j = 0; j < N; j++)
			{
				row.push_back(FLT_MAX);
			}
			t_dist_matrix.push_back(row);
		}

		//TODO: calculate distance matrix
		for (int row = 0; row < N; row++)
		{
			for (int col = 0; col < N; col++)
			{
				if (row != col)
				{
					float xDist = (xs[col] - xs[row]) * (xs[col] - xs[row]);
					float yDist = (ys[col] - ys[row]) * (ys[col] - ys[row]);
					float distance = sqrtf(xDist + yDist);
					t_dist_matrix[row][col] = distance;
				}
			}
		}

		file.close();
	}
	else
	{
		return 0;
	}
	return 1;
}

/**
 * Funkce pro v�po�et TSP pomoc� brute force algoritmu
 *
 * Funkce je optimalizov�na pro paraleln� �e�en�, ov�em s�riov� pob�� takt�.
 *
 * V po��tku vytvo��m pole nejlevn�j��ch cest pro N - 1 m�st, proto�e po��te�n� m�sto ve v�po�tu permutac� neuva�uju, sta��
 * pouze p�ipo��st k v�sledn� hodnot� cesty cenu z po��tku do prvn�ho m�sta permutace a z posledn�ho m�sta permutace do
 * po��te�n�ho. Tak�e sta�� pole N - 1, proto�e vl�kna budou pracovat s permutac� s fixn� prvotn� hodnotou, kter� bude vl�knu
 * p�i�azena na z�klad� hodnoty cycle v paraleln�m for loopu, a permutovat budou jen zbytek,
 * tud� pot�ebuju pro dan� permutace N - 1 m�st, kde si m��u ukl�dat meziv�sledky, aby se mi v algoritmu neprojevil race condition.
 *
 * Pot� si napln�m vector m�sty, kter� mi budou pro permutace slou�it a vynech�m pouze po��te�n� m�sto.
 *
 * N�sledn� v cyklu for zrotuju podle hodnoty cycle vector permutac�, aby tedy ka�d� vl�kno pracovalo s tou prvnotn� hodnotou fixn� a
 * permutovalo a� od indexu 1.
 *
 * V hlavn� f�zi algoritmu v cyklu do while nejprve nastav�m hodnotu celkov� ceny cesty na 0 a nastav�m moment�ln� m�sto, ve kter�m
 * se nach�z�m na po��te�n� m�sto. Dal��m cyklem for traverzuju v�echny m�sta ve vectoru permutace a ceny hran mezi jednotliv�mi
 * m�sty hled�m v distance_matrix. Po nalezen� ceny cest mezi m�sty od po��tku a� do konce mus�m je�t� p�ipo��st hodnotu hrany mezi posledn�m
 * a po��te�n�m m�stem a pak u� mi sta�� pouze zkontrolovat, zda jsem nenaleznul lok�ln� minimum a pokud ano, tak si jej ulo��m.
 *
 * Po dokon�en� cyklu do while ulo��m do pole meziv�sledk� jednotliv�ch vl�ken lok�ln� minimum a po n�sledn�m skon�en� paraleln�ho foru
 * z pole lok�ln�ch minim�ln�ch hodnot naleznu glob�ln� minimum.
 *
 * \return V�sledn� cena nejkrat�� cesty
 */
float brute_force_tsp(std::vector<std::vector<float>>& t_distance_matrix, int starting_point, bool parallel)
{
	vector<int> permutations;
	float min_distances[N - 1];

	for (int i = 0; i < N - 1; i++)
	{
		min_distances[i] = FLT_MAX;
	}

	for (int i = 0; i < N; i++)
	{
		if (i == starting_point)
			continue;
		permutations.push_back(i);
	}


#pragma omp parallel for num_threads(16) if(parallel)
	for (int cycle = 0; cycle < N - 1; cycle++)
	{
		float local_min_dist = FLT_MAX;
		int local_start = starting_point;
		int local_cur_city = local_start;
		vector<int> local_perm = permutations;
		std::rotate(local_perm.begin(), local_perm.begin() + cycle, local_perm.begin() + cycle + 1);

		do
		{
			float local_total_dist = 0.0f;
			local_cur_city = local_start;
			for (int i = 0; i < local_perm.size(); i++)
			{
				int neighborCity = local_perm[i];
				local_total_dist += t_distance_matrix[local_cur_city][neighborCity];
				local_cur_city = neighborCity;
			}

			local_total_dist += t_distance_matrix[local_cur_city][local_start];

			if (local_total_dist <= local_min_dist)
			{
				local_min_dist = local_total_dist;
			}

		} while (next_permutation(local_perm.begin() + 1, local_perm.end()));
		min_distances[cycle] = local_min_dist;
	}
	float min_dist = FLT_MAX;
	for (int i = 0; i < N - 1; i++)
	{
		min_dist = min(min_dist, min_distances[i]);
	}

	return min_dist;
}

class CompareCost {
public:
	bool operator()(Node* t1, Node* t2)
	{
		if (t1->bound > t2->bound) return true;
		return false;
	}
};
float branch_n_bound_serial(std::vector<std::vector<float>> &t_distance_matrix, int starting_point)
{
	std::vector<int> vc = { };
	for (int i = 0; i < N; i++)
	{
		vc.push_back(i);
	}
	std::vector<std::vector<float>> temp_matrix = t_distance_matrix;
	Node* root = new Node(temp_matrix, vc, std::vector<int>{}, starting_point, 0, -1);
	vector<Node*> priority_queue;
	Node* current = nullptr;
	priority_queue.push_back(root);
	while (!priority_queue.empty())
	{
		std::sort(priority_queue.begin(), priority_queue.end(), CompareCost());
		current = priority_queue.back();
		priority_queue.pop_back();
		if(current->explorable.empty())
		{
			break;
		}
		std::vector<int> path = current->path;
		path.push_back(current->idx);

		std::vector<std::vector<float>> mat = current->distance_matrix;

		for (auto& city : current->explorable)
		{
			Node* child = new Node(mat, current->explorable, path, city, current->bound, current->idx);
			priority_queue.push_back(child);
		}
	}

	
	return current->computeTotalCost(t_distance_matrix).second;
}

float branch_n_bound_parallel(std::vector<std::vector<float>>& t_distance_matrix, int starting_point)
{
	std::vector<int> vc = { };
	for (int i = 0; i < N; i++)
	{
		vc.push_back(i);
	}
	std::vector<std::vector<float>> temp_matrix = t_distance_matrix;
	Node* root = new Node(temp_matrix, vc, std::vector<int>{}, starting_point, 0, -1);

	std::vector<std::pair<std::vector<int>, float>> final_results;
	for(int i = 0; i < root->explorable.size(); i++)
	{
		final_results.push_back(std::make_pair(std::vector<int>{}, 0.0f));
	}


#pragma omp parallel for
	for(int i = 0; i < root->explorable.size(); i++)
	{
		vector<Node*> priority_queue;
		Node* current = nullptr;

		std::vector<std::vector<float>> mat = root->distance_matrix;
		std::vector<int> path = root->path;
		path.push_back(root->idx);

		Node* local_root = new Node(mat, root->explorable, path, root->explorable[i], root->bound, root->idx);
		priority_queue.push_back(local_root);
		while (!priority_queue.empty())
		{
			std::sort(priority_queue.begin(), priority_queue.end(), CompareCost());
			current = priority_queue.back();
			priority_queue.pop_back();
			if (current->explorable.empty())
			{
				break;
			}
			std::vector<int> path = current->path;
			path.push_back(current->idx);

			std::vector<std::vector<float>> mat = current->distance_matrix;

			for (auto& city : current->explorable)
			{
				Node* child = new Node(mat, current->explorable, path, city, current->bound, current->idx);
				priority_queue.push_back(child);
			}
		}
		final_results[i] = current->computeTotalCost(t_distance_matrix);
	}
	int idx = 0;
	float final_cost = FLT_MAX;
	for(int i = 0; i < final_results.size(); i++)
	{
		if(final_results[i].second < final_cost)
		{
			final_cost = final_results[i].second;
			idx = i;
		}
	}



	return final_results[idx].second;
}

float branch_n_bound(float (&distance_matrix)[N][N], int starting_point)
{

	/* Udelat priority queue s rootem
	 * Kazdemu nodu pridat vektor s cestou, kterou k nemu musim dorazit
	 * Tento vektor pouziju taky na redukci radku v matici
	 * Kazdy node ted musi mit level
	 * Kazdy node taky musi mit vektor mest, ktere muze prozkoumat
	 *
	 * Udelam priority queue setrizenou podle costu
	 *
	 * Jak narazim na posledni level, vracim cestu
	 *
	 *
	 *
	 */
	/*std::vector<int> vc = {};
	for (int i = 0; i < N; i++)
	{
		if (i == starting_point)
			continue;
		vc.push_back(i);
	}
	float tmp[N][N];
	memcpy(tmp, distance_matrix, SIZE_BYTES);
	float bound = ops::reduce_matrix(distance_matrix);
	Node* root = new Node(starting_point, 0.0f, distance_matrix, std::vector<int>{}, bound);
	Node* curr_node = root;

	while (true)
	{
		Node* node = curr_node->solveChildren(vc);
		vc.erase(std::remove(vc.begin(), vc.end(), node->idx), vc.end());
		curr_node = node;
		if (vc.empty())
			break;
	}
	std::vector<int> path = root->getPath();
	path.push_back(0);
	float cost = 0.0f;
	for (int i = 0; i < path.size() - 1; i++)
	{
		cost += tmp[path[i]][path[i + 1]];
	}
	return cost;*/
	return 0.0f;
}

void test()
{
	std::vector<std::pair<std::vector<int>, float>> final_results = {
		std::make_pair(std::vector<int>{1, 2, 3}, 32.0f),
		std::make_pair(std::vector<int>{1, 2, 3}, 32.0f),
		std::make_pair(std::vector<int>{1, 2, 3}, 32.0f),
		std::make_pair(std::vector<int>{1, 2, 3}, 32.0f),
		std::make_pair(std::vector<int>{1, 2, 3}, 32.0f),
		std::make_pair(std::vector<int>{1, 2, 3}, 32.0f),
		std::make_pair(std::vector<int>{1, 2, 3}, 32.0f),
		std::make_pair(std::vector<int>{1, 2, 3}, 32.0f),
		std::make_pair(std::vector<int>{1, 2, 3}, 32.0f),
		std::make_pair(std::vector<int>{1, 2, 3}, 32.0f)
	};
#pragma omp parallel for
	for(int i = 0; i < 10; i++)
	{
		final_results[i] = std::make_pair(std::vector<int>{i, i, i}, 1.0f * i);
	}
	return;
}

void run_tsp_serial(std::vector<std::vector<float>>& t_dist_matrix)
{
	auto start = chrono::high_resolution_clock::now();
	float cost = brute_force_tsp(t_dist_matrix, 0, false);
	auto end = chrono::high_resolution_clock::now();
	auto duration = chrono::duration_cast<chrono::seconds>(end - start);
	cout << "Serial brute force ran: " << duration.count() << "s with total cost: " << cost << endl;
}

void run_tsp_parallel (std::vector<std::vector<float>>& t_dist_matrix)
{
	auto start = chrono::high_resolution_clock::now();
	float cost = brute_force_tsp(t_dist_matrix, 0, true);
	auto end = chrono::high_resolution_clock::now();
	auto duration = chrono::duration_cast<chrono::seconds>(end - start);
	cout << "Parallel brute force ran: " << duration.count() << "s with total cost: " << cost << endl;
}

void run_tsp_branch_n_bound_serial(std::vector<std::vector<float>> &t_dist_matrix)
{
	auto start = chrono::high_resolution_clock::now();
	float cost = branch_n_bound_serial(t_dist_matrix, 0);
	auto end = chrono::high_resolution_clock::now();
	auto duration = chrono::duration_cast<chrono::seconds>(end - start);
	cout << "Serial branch and bound ran: " << duration.count() <<"s with total cost: " << cost << endl;
}

void run_tsp_branch_n_bound_parallel(std::vector<std::vector<float>>& t_dist_matrix)
{
	auto start = chrono::high_resolution_clock::now();
	float cost = branch_n_bound_parallel(t_dist_matrix, 0);
	auto end = chrono::high_resolution_clock::now();
	auto duration = chrono::duration_cast<chrono::seconds>(end - start);
	cout << "Parallel and bound ran: " << duration.count() << "s with total cost: " << cost << endl;
}
int main()
{
	/*float distance_matrix1[N][N] =
	{
		{FLT_MAX, 20.0f, 30.0f, 10.0f, 11.0f},
		{15.0f, FLT_MAX, 16.0f, 4.0f, 2.0f},
		{3.0f, 5.0f, FLT_MAX, 2.0f, 4.0f},
		{19.0f, 6.0f, 18.0f, FLT_MAX, 3.0f},
		{16.0f, 4.0f, 7.0f, 16.0f, FLT_MAX}
	};*/
	std::vector<std::vector<float>> distance_matrix;
	if (!(read_tsp_file("ulysses22.tsp.txt", distance_matrix)))
	{
		std::cout << "Could not read the file, exiting with code -1" << std::endl;
		exit(-1);
	}

	//branch_n_bound_test(distance_matrix, 0);

	run_tsp_serial(distance_matrix);
	run_tsp_parallel(distance_matrix);
	run_tsp_branch_n_bound_serial(distance_matrix);
	run_tsp_branch_n_bound_parallel(distance_matrix);
	//test();
	return 0;
}
