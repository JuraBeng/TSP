#include <queue>
#include "Node.h"

using namespace std;

vector<double> distances;

int read_tsp_file(const char* fname, float(&distance_matrix)[N][N])
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

		//TODO: calculate distance matrix
		for (int row = 0; row < N; row++)
		{
			for (int col = 0; col < N; col++)
			{
				if (row == col)
				{
					distance_matrix[row][col] = FLT_MAX;
				}
				else
				{
					float xDist = (xs[col] - xs[row]) * (xs[col] - xs[row]);
					float yDist = (ys[col] - ys[row]) * (ys[col] - ys[row]);
					float distance = sqrtf(xDist + yDist);
					distance_matrix[row][col] = distance;

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
 * Funkce pro výpoèet TSP pomocí brute force algoritmu
 *
 * Funkce je optimalizována pro paralelní øešení, ovšem sériovì pobìží taktéž.
 *
 * V poèátku vytvoøím pole nejlevnìjších cest pro N - 1 mìst, protože poèáteèní mìsto ve výpoètu permutací neuvažuju, staèí
 * pouze pøipoèíst k výsledné hodnotì cesty cenu z poèátku do prvního mìsta permutace a z posledního mìsta permutace do
 * poèáteèního. Takže staèí pole N - 1, protože vlákna budou pracovat s permutací s fixní prvotní hodnotou, která bude vláknu
 * pøiøazena na základì hodnoty cycle v paralelním for loopu, a permutovat budou jen zbytek,
 * tudíž potøebuju pro dané permutace N - 1 míst, kde si mùžu ukládat mezivýsledky, aby se mi v algoritmu neprojevil race condition.
 *
 * Poté si naplním vector mìsty, které mi budou pro permutace sloužit a vynechám pouze poèáteèní mìsto.
 *
 * Následnì v cyklu for zrotuju podle hodnoty cycle vector permutací, aby tedy každé vlákno pracovalo s tou prvnotní hodnotou fixní a
 * permutovalo až od indexu 1.
 *
 * V hlavní fázi algoritmu v cyklu do while nejprve nastavím hodnotu celkové ceny cesty na 0 a nastavím momentální mìsto, ve kterém
 * se nacházím na poèáteèní mìsto. Dalším cyklem for traverzuju všechny mìsta ve vectoru permutace a ceny hran mezi jednotlivými
 * mìsty hledám v distance_matrix. Po nalezení ceny cest mezi mìsty od poèátku až do konce musím ještì pøipoèíst hodnotu hrany mezi posledním
 * a poèáteèním mìstem a pak už mi staèí pouze zkontrolovat, zda jsem nenaleznul lokální minimum a pokud ano, tak si jej uložím.
 *
 * Po dokonèení cyklu do while uložím do pole mezivýsledkù jednotlivých vláken lokální minimum a po následném skonèení paralelního foru
 * z pole lokálních minimálních hodnot naleznu globální minimum.
 *
 * \return Výsledná cena nejkratší cesty
 */
float brute_force_tsp(float (& distance_matrix)[N][N], int starting_point, bool parallel)
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
				local_total_dist += distance_matrix[local_cur_city][neighborCity];
				local_cur_city = neighborCity;
			}

			local_total_dist += distance_matrix[local_cur_city][local_start];

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

/**
 * Nejprve inicializuje a naète vektor všech mìst, které nejsou poèáteèní mìsto
 *
 * Poté redukuje matici, vytvoøí koøenový uzel a postupnì v každém cyklu nalezne nejlevnìjšího potomka,
 * kterého pak prohledává. Za každým prohledáním z vectoru mìst odebere nejlevnìjší nalezené mìsto.
 * Cyklus konèí v pøípadì, že všechna mìsta byla prozkoumána, takže se pokraèuje traverzí nejlevnìjších mìst od
 * koøenového uzlu, abych si mohl naleznout výslednou permutaci mìst, které musím projít. Nesmím zapomenout pøidat
 * poèáteèní mìsto do permutace na konec, abych mohl také urèit cenu cesty z posledního mìsta do poèáteèního.
 *
 * \return Výsledná cena nejkratší cesty
 */

class CompareCost {
public:
	bool operator()(Node* t1, Node* t2)
	{
		if (t1->bound > t2->bound) return true;
		return false;
	}
};
float branch_n_bound_test(float(&distance_matrix)[N][N], int starting_point)
{
	std::vector<int> vc = { };
	for (int i = 0; i < N; i++)
	{
		vc.push_back(i);
	}
	float tmp[N][N];
	memcpy(tmp, distance_matrix, SIZE_BYTES);
	Node* root = new Node(tmp, vc, std::vector<int>{}, starting_point, 0, -1);
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

		float tmp_matrix[N][N];
		memcpy(tmp_matrix, current->matrix, SIZE_BYTES);

		for (auto& city : current->explorable)
		{
			Node* child = new Node(tmp_matrix, current->explorable, path, city, current->bound, current->idx);
			priority_queue.push_back(child);
		}
	}
	vector<int> final_path = current->path;
	final_path.push_back(current->idx);
	final_path.push_back(starting_point);
	float total_cost = 0.0f;
	for(auto&a : final_path)
	{ 
		std::cout << a << std::endl;
	}
	for(int i = 0; i < final_path.size() - 1; i++)
	{
		total_cost += distance_matrix[final_path[i]][final_path[i + 1]];
	}
	
	return total_cost;
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

void run_tsp_serial(float(&distance_matrix)[N][N])
{
	auto start = chrono::high_resolution_clock::now();
	float cost = brute_force_tsp(distance_matrix, 0, false);
	auto end = chrono::high_resolution_clock::now();
	auto duration = chrono::duration_cast<chrono::seconds>(end - start);
	cout << "Serial brute force ran: " << duration.count() << "s with total cost: " << cost << endl;
}

void run_tsp_parallel (float(&distance_matrix)[N][N])
{
	auto start = chrono::high_resolution_clock::now();
	float cost = brute_force_tsp(distance_matrix, 0, true);
	auto end = chrono::high_resolution_clock::now();
	auto duration = chrono::duration_cast<chrono::seconds>(end - start);
	cout << "Parallel brute force ran: " << duration.count() << "s with total cost: " << cost << endl;
}

void run_tsp_branch_n_bound(float(&distance_matrix)[N][N])
{
	auto start = chrono::high_resolution_clock::now();
	float cost = branch_n_bound_test(distance_matrix, 0);
	auto end = chrono::high_resolution_clock::now();
	auto duration = chrono::duration_cast<chrono::seconds>(end - start);
	cout << "Branch and bound ran: " << duration.count() <<"s with total cost: " << cost << endl;
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
	float distance_matrix[N][N];
	if (!(read_tsp_file("ulysses22.tsp.txt", distance_matrix)))
	{
		std::cout << "Could not read the file, exiting with code -1" << std::endl;
		exit(-1);
	}

	//branch_n_bound_test(distance_matrix, 0);

	//run_tsp_serial(distance_matrix);
	run_tsp_parallel(distance_matrix);
	//run_tsp_branch_n_bound(distance_matrix);
	return 0;
}
