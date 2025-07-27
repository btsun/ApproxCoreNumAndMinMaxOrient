#include <cstdio>
#include <vector>
#include "MaxFlow.hpp"
using namespace std;

const int MAX_N_NODES = 5000000;
vector<int> deg;
int N, M, max_deg;
Network network;
vector<Network::Vertex> nodes;
vector<Network::Edge> edges_to_change;

int main(int argc, char **argv) {
  char *input_file_name = argv[1];
  char *output_file_name = argv[2];
  deg = vector<int>(MAX_N_NODES, 0);
  FILE *ifp = fopen(input_file_name, "r");
  int x, y;
  N = M = max_deg = 0;
  while (fscanf(ifp, "%d%d", &x, &y) != EOF) {
    N = max(N, x + 1);
    N = max(N, y + 1);
    max_deg = max(max_deg, ++deg[x]);
    max_deg = max(max_deg, ++deg[y]);
    ++M;
  }
  fclose(ifp);
  printf("Finish reading the graph. %d nodes, %d edges.\n", N, M);

  ifp = fopen(input_file_name, "r");
  for (int i = 0; i < N; ++i)
    nodes.push_back(network.AddVertex());
  Network::Vertex s = network.AddVertex();
  Network::Vertex t = network.AddVertex();
  while (fscanf(ifp, "%d%d", &x, &y) != EOF) {
    Network::Vertex v = network.AddVertex();
    network.AddEdge(s, v, 1);
    network.AddEdge(v, nodes[x], 1);
    network.AddEdge(v, nodes[y], 1);
  }
  for (int i = 0; i < N; ++i)
    edges_to_change.push_back(network.AddEdge(nodes[i], t, 0));
  fclose(ifp);

  int l = 0, u = max_deg;
  while (l + 1 < u) {
    int guess = (l + u) / 2;
    for (int i = 0; i < N; ++i)
      network.capacity[edges_to_change[i]] = guess;
    int max_flow = network.MaxFlow(s, t);
    if (max_flow == M) {
      u = guess;
      printf("%d yes\n", guess);
    }
    else {
      l = guess;
      printf("%d no\n", guess);
    }
  }
  printf("Min max in-degree: %d\n", u);
  if (output_file_name[0] != '_') {
    FILE *ofp = fopen(output_file_name, "w");
    fprintf(ofp, "%d\n", u);
    fclose(ofp);
  }
  return 0;
}
