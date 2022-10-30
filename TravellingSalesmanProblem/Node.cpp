#include "stdafx.h"
#include "Node.h"

Node::Node(int t_idx, float t_cost, float t_matrix[N][N], std::vector<int> t_explorable, float t_bound)
{
	bound = t_bound;
	explorable = t_explorable;
	idx = t_idx;
	memcpy(matrix, t_matrix, SIZE_BYTES);
}

Node::Node(int t_idx, std::vector<int> t_explorable)
{
	explorable = t_explorable;
	idx = t_idx;
}

Node::~Node()
{
	if (children.empty())
	{
		delete(this);
		return;
	}
	for (auto *child : children)
	{
		child->~Node();
	}
	delete(this);
}

void Node::addChild(Node* node)
{
	children.push_back(node);
}

/**
 * Když se v každém uzlu rozhoduju, kterým smìrem se vydat, tak potøebuju procházet cestou potomka
 * s nejmenší hodnotou cesty pro procházení. Dalo by se efektivnìji udìlat pøes priority queue a tøídit si
 * potomky podle jejich hodnot
 *
 * \brief Vyhodnotí potomka s nejmenší cenou procházení
 * \return Pointer na potomka s nejmenší cenou procházení
 */
Node* Node::getMinChild()
{
	float min = FLT_MAX;
	int idx = 0;
	if (children.empty())
		return nullptr;
	for (int i = 0; i < children.size(); i++)
	{
		if(children[i]->bound <= min)
		{
			min = children[i]->bound;
			idx = i;
		}
	}
	return children[idx];
}

/**
 * V poèáteèní fázi nastaví vector explore, který udržuje hodnoty uzlù, resp indexù mìst, které musím projít,
 * abych se dostal do momentálního uzlu. To slouží pro pozdìjší redukci matice.
 *
 * Dále každá redukce matice vyžaduje redukovanou matici z pøedchozího uzlu, tudíž je vhodné si matici momentálního
 * uzlu nechat tak jak je a pro redukci matice pro potomka použiju jeji kopii.
 *
 * Nastavím všechny øádky korespondující s mìsty, které jsem musel projít, abych se dostal k potomkovi na INF a
 * sloupec korespondující s èíslem mìsta potomka taky na INF a poslednì jeden záznam v matici s indexy
 * [potomek][poèátek] na INF
 *
 * Poté provedu redukci matice, viz ops.h/ops.cpp, a souèet redukcí na øádcích a sloupcích, spoleènì s
 * hodnotou cesty mezi konkrétními dvìma mìsty a bound hranice pro konkrétní výchozí uzel mi dá bound hranici, respektive
 * cenu cesty pro potomka
 *
 * V poslední øadì nastavím novì redukovanou matici potomkovi a nastavím jeho bound/cost a pøidám do vectoru
 * potomù výchozího uzlu
 *
 * \return Pointer na potomka s nejmenší cenou procházení
 */
Node* Node::solveChildren(std::vector<int> data)
{
	std::vector<int> explore = explorable;
	explore.push_back(idx);
	for(int i = 0; i < data.size(); i++)
	{
		Node* node = new Node(data[i], explore);

		float tmp_mat[N][N];
		memcpy(tmp_mat, matrix, SIZE_BYTES);

		//set rows of all predecessors to inf
		for(int i = 0; i < explore.size(); i++)
		{
			ops::set_inf_row(tmp_mat, explore[i]);
		}
		ops::set_inf_col(tmp_mat, node->idx);
		ops::set_mat_value(tmp_mat, node->idx, explore[0], FLT_MAX);

		float reduction = ops::reduce_matrix(tmp_mat);

		// compute cost and set all other things to node
		float cost = reduction + matrix[idx][node->idx] + bound;

		node->bound = cost;
		//tmp_mat[node->idx][explore[0]] = cache;
		node->setMatrix(tmp_mat);
		children.push_back(node);
	}

	return getMinChild();
}


void Node::setMatrix(float t_matrix[N][N])
{
	memcpy(matrix, t_matrix, SIZE_BYTES);
}


/**
 * Traverze všech nejmenších potomkù pro získání výsledné nejlevnìjší cesty, v tomto kontextu
 * nejideálnìjší permutace všech mìst, abych posléze mohl zjistit cenu tìchto cest
 *
 * \return vector perumutace
 */
std::vector<int> Node::getPath()
{
	std::vector<int> path_order;
	path_order.push_back(idx);
	Node* curr = getMinChild();
	while (curr != NULL)
	{
		path_order.push_back(curr->idx);
		curr = curr->getMinChild();
	}
	return path_order;
}

