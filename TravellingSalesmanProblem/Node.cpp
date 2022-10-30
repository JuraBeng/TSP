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
 * Kdy� se v ka�d�m uzlu rozhoduju, kter�m sm�rem se vydat, tak pot�ebuju proch�zet cestou potomka
 * s nejmen�� hodnotou cesty pro proch�zen�. Dalo by se efektivn�ji ud�lat p�es priority queue a t��dit si
 * potomky podle jejich hodnot
 *
 * \brief Vyhodnot� potomka s nejmen�� cenou proch�zen�
 * \return Pointer na potomka s nejmen�� cenou proch�zen�
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
 * V po��te�n� f�zi nastav� vector explore, kter� udr�uje hodnoty uzl�, resp index� m�st, kter� mus�m proj�t,
 * abych se dostal do moment�ln�ho uzlu. To slou�� pro pozd�j�� redukci matice.
 *
 * D�le ka�d� redukce matice vy�aduje redukovanou matici z p�edchoz�ho uzlu, tud� je vhodn� si matici moment�ln�ho
 * uzlu nechat tak jak je a pro redukci matice pro potomka pou�iju jeji kopii.
 *
 * Nastav�m v�echny ��dky koresponduj�c� s m�sty, kter� jsem musel proj�t, abych se dostal k potomkovi na INF a
 * sloupec koresponduj�c� s ��slem m�sta potomka taky na INF a posledn� jeden z�znam v matici s indexy
 * [potomek][po��tek] na INF
 *
 * Pot� provedu redukci matice, viz ops.h/ops.cpp, a sou�et redukc� na ��dc�ch a sloupc�ch, spole�n� s
 * hodnotou cesty mezi konkr�tn�mi dv�ma m�sty a bound hranice pro konkr�tn� v�choz� uzel mi d� bound hranici, respektive
 * cenu cesty pro potomka
 *
 * V posledn� �ad� nastav�m nov� redukovanou matici potomkovi a nastav�m jeho bound/cost a p�id�m do vectoru
 * potom� v�choz�ho uzlu
 *
 * \return Pointer na potomka s nejmen�� cenou proch�zen�
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
 * Traverze v�ech nejmen��ch potomk� pro z�sk�n� v�sledn� nejlevn�j�� cesty, v tomto kontextu
 * nejide�ln�j�� permutace v�ech m�st, abych posl�ze mohl zjistit cenu t�chto cest
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

