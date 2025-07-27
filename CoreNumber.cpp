#include <cstdio>
#include <vector>
#include <algorithm>
#include <cstring>
#include <set>
#include <tuple>
using namespace std;

typedef long long weight_t;
typedef int id_t; // Will this change to long long when the graph is really huge?
struct Edge {
	id_t end;
	weight_t w;
	Edge(id_t end = 0, weight_t w = 0): end(end), w(w) {}
};
id_t N;
vector<vector<Edge> > eList;

void addEdge(id_t x, id_t y, weight_t w) {
	eList[x].push_back(Edge(y, w));
	eList[y].push_back(Edge(x, w));
}

void buildGraph(char *filename) {
	printf("Reading the graph...\n");
	N = 0;
	id_t x, y;
	FILE *ifp = fopen(filename, "r");
	while (fscanf(ifp, "%d", &x) != EOF)
		N = max(N, x);
	fclose(ifp);
	eList = vector<vector<Edge> >(++N);
	ifp = fopen(filename, "r");
	while (fscanf(ifp, "%d%d", &x, &y) != EOF)
		addEdge(x, y, 1);
	fclose(ifp);
}

void computeExactCore(char *filename) {
	printf("Computing the exact core values...\n");
	FILE *ofp = fopen(filename, "w");
	set<pair<weight_t, id_t> > S;
	weight_t maxWd = -1;
	vector<weight_t> wd(N, 0); // Weighted degree
	for (id_t i = 0; i < N; ++i) {
		for(const Edge &e: eList[i])
			wd[i] += e.w;
		S.insert(make_pair(wd[i], i));
	}
	while (!S.empty()) {
		pair<weight_t, id_t> p = *S.begin();
		S.erase(S.begin());
		maxWd = max(maxWd, p.first);
		fprintf(ofp, "%d %d\n", p.second, maxWd);
		for (const Edge &e: eList[p.second]) {
			id_t u = e.end;
			if (S.erase(make_pair(wd[u], u))) {
				wd[u] -= e.w;
				S.insert(make_pair(wd[u], u));
			}
		}
	}
	fclose(ofp);
}

pair<weight_t, weight_t> update(const vector<weight_t> &b, const vector<weight_t> &w, const id_t d) {
	// b[] is non-decreasing(; b >= w (element-wise) ???)
	weight_t s = 0;
	for (id_t i = d - 1; i > 0; --i) {
		s += w[i];
		if (b[i - 1] < s) {
      if (b[i] < s) {
        return make_pair(b[i], s - w[i]);
      } else {
        return make_pair(s, s);
      }
		}
	}
	s += w[0];
	if (b[0] < s) {
    return make_pair(b[0], s - w[0]);
	} else {
	  return make_pair(s, s);
	}
}

void plot(char *exactValFileName, char *minMaxInDegreeFileName, char *outFileName) {
	printf("Run the protocal and plot.\n");
	vector<weight_t> beta(N, N + 1), betaNew(N), b(N), w(N), workload(N);
	vector<double> val(N);
	vector<pair<weight_t, weight_t> > pairs(N);
	vector<bool> active(N, true), activeNew(N);
	double minMaxInDegree;

	// Read the exact values
	FILE *ifp = fopen(exactValFileName, "r");
	id_t x;
	double y;
	while (fscanf(ifp, "%d%lf", &x, &y) != EOF)
		val[x] = y;
	fclose(ifp);
	ifp = fopen(minMaxInDegreeFileName, "r");
	fscanf(ifp, "%lf", &minMaxInDegree);
	fclose(ifp);

	// Run the distributed algorithm
	FILE *ofp = fopen(outFileName, "w");
	bool updated = true;
	int nRound = 0;
	while (updated) {
		printf("Round %d...\n", ++nRound);
		updated = false;
		fill(activeNew.begin(), activeNew.end(), false);
		for (id_t i = 0; i < N; ++i) {
			if (!active[i]) continue;
			id_t d = eList[i].size();
			for (id_t j = 0; j < d; ++j) {
				pairs[j].first = beta[eList[i][j].end];
				pairs[j].second = eList[i][j].w;
			}
			sort(pairs.begin(), pairs.begin() + d);
			for (id_t j = 0; j < d; ++j)
				b[j] = pairs[j].first, w[j] = pairs[j].second;
			tie(betaNew[i], workload[i]) = update(b, w, d);
			if (betaNew[i] != beta[i]) {
				updated = true;
				for (id_t j = 0; j < d; ++j)
					activeNew[eList[i][j].end] = true;
			}
		}
		beta = betaNew;
		active = activeNew;
		if (plot) { // Output the average and maximum error for this round
			double s = 0, errMax = -1, maxWorkload = -1;
			for (id_t i = 0; i < N; ++i) {
				errMax = max(errMax, (double)beta[i] / val[i]);
				s += (double)beta[i] / val[i];
				maxWorkload = max(maxWorkload, workload[i] / minMaxInDegree);
			}
			fprintf(ofp, "%.9f\t%.9f\t%.9f\n", s / N, errMax, maxWorkload);
		}
	}
	fclose(ofp);
}

int main(int argc, char **argv) {
	if (argc <= 2 || (strcmp(argv[2], "plot") == 0 && argc <= 4)) {
		printf("Usage:\n"
			   "- CoreNumber [edge list file name] exact [output file name]\n"
			   "- CoreNumber [edge list file name] plot [exact values file name] [min max in-degree file name] [output file name]\n");
		return 0;
	}
	buildGraph(argv[1]);
	if (strcmp(argv[2], "exact") == 0)
		computeExactCore(argv[3]);
	else if (strcmp(argv[2], "plot") == 0)
		plot(argv[3], argv[4], argv[5]);
	else
		printf("Undefined command.\n");
	return 0;
}
