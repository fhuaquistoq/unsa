#include <iostream>
#include <vector>
#include <map>
#include <queue>
#include <climits>

int main(int argc, char *argv[])
{
  int n, m;
  std::cin >> n >> m;

  std::map<std::string, int> name_to_index;
  std::vector<std::string> index_to_name;
  std::vector<std::vector<std::pair<int, int>>> adj(n);

  std::string s, t;
  int weight;

  int acc = 0;
  for (int i = 0; i < m; ++i)
  {
    std::cin >> s >> t >> weight;

    if (name_to_index.find(s) == name_to_index.end())
    {
      name_to_index[s] = acc++;
      index_to_name.push_back(s);
    }
    if (name_to_index.find(t) == name_to_index.end())
    {
      name_to_index[t] = acc++;
      index_to_name.push_back(t);
    }

    int u = name_to_index[s];
    int v = name_to_index[t];

    adj[u].push_back({v, weight});
  }

  // Dijkstra
  std::string start_name, end_name;
  std::cin >> start_name >> end_name;

  int start = name_to_index[start_name];

  std::vector<int> dist(n, INT_MAX);
  std::vector<int> parent(n, -1);

  std::priority_queue<std::pair<int, int>, std::vector<std::pair<int, int>>, std::greater<std::pair<int, int>>> pq;

  dist[start] = 0;
  pq.push({0, start});

  while (!pq.empty())
  {
    std::pair<int, int> current = pq.top();
    int d = current.first;
    int u = current.second;
    pq.pop();

    if (d > dist[u])
      continue;

    for (std::pair<int, int> edge : adj[u])
    {
      int v = edge.first;
      int w = edge.second;

      if (dist[u] + w < dist[v])
      {
        dist[v] = dist[u] + w;
        parent[v] = u;
        pq.push({dist[v], v});
      }
    }
  }

  // Generación de la tabla de resultados

  std::cout << "Algoritmo de Dijkstra\n";
  std::cout << "Nombre: Fabricio Jesus Huaquisto Quispe\n";
  std::cout << "Nodo Inicio: " << start_name << "\n";
  std::cout << "Nodo Final: " << end_name << "\n";
  std::cout << "X1: 17" << "\n";
  std::cout << "X2: 23" << "\n";
  std::cout << "\n";

  std::cout << "Node|Distance|Parent\n";

  for (int i = 0; i < acc; ++i)
  {
    std::cout << index_to_name[i] << "|";

    if (dist[i] == INT_MAX)
      std::cout << "INF|";
    else
      std::cout << dist[i] << "|";

    if (parent[i] == -1)
      std::cout << "null";
    else
      std::cout << index_to_name[parent[i]];

    std::cout << "\n";
  }

  return 0;
}