#include <cstdio>
#include <cstdlib>
#include <vector>
#include <unordered_map>
using namespace std;

template <class T>
inline void fscand(FILE *ifp, T &ret) {
	char c;
	ret = 0;
	while ((c = fgetc(ifp)) < '0' || c > '9');
	while (c >= '0' && c <= '9')
		ret = ret * 10 + (c - '0'), c = getchar();
}

int main(int argc, char **argv) {
	if (argc <= 1) {
		printf("Usage: processInput [file name] [(loose) upper bound of number of vertices]");
		return 0;
	}
	FILE *ifp = fopen(argv[1], "r");
	int MAXN = (argc == 2) ? 1e8 : atoi(argv[2]), l = MAXN, r = -1, x, y;
	vector<bool> b(MAXN);
	int cnt = 0;
	while (fscanf(ifp, "%d", &x) != EOF) {
		b[x] = true, l = min(l, x), r = max(r, x);
		if (++cnt % 1000000 == 0)
			printf("%d numbers...\n", cnt);
	}
	fclose(ifp);
	cnt = 0;
	for (int i = l; i <= r; ++i) cnt+= b[i];
	if (cnt == r - l + 1) {
		printf("The graph is fine. IDs range from %d to %d.\n", l, r);
		return 0;
	}
	printf("IDs range from %d to %d, but only %d vertices appeared!\n", l, r, cnt);
	unordered_map<int, int> id;
	ifp = fopen(argv[1], "r");
	string newFilename = string(argv[1]) + ".new.txt";
	FILE *ofp = fopen(newFilename.c_str(), "w");
	int cntV = 0, cntE = 0;
	while (fscanf(ifp, "%d%d", &x, &y) != EOF) {
		if (!id.count(x))
			id[x] = cntV++;
		if (!id.count(y))
			id[y] = cntV++;
		++cntE;
		fprintf(ofp, "%d %d\n", id[x], id[y]);
	}
	printf("The relabeled edge list is written to %s.\n", newFilename.c_str());
	printf("%d vertices, %d edges.\n", cntV, cntE);
	fclose(ifp);
	fclose(ofp);
	return 0;
}
