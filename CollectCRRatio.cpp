#include <cstdio>

const int MAXN = 4000000;
double c[MAXN], r[MAXN];

int main() {
	int x, N = 0;
	double y;
	FILE *ifp = fopen("core_lj.txt", "r");
	while (fscanf(ifp, "%d%lf", &x, &y) != EOF) {
		c[x] = y;
		if (x > N) N = x;
	}
	++N;
	fclose(ifp);
	ifp = fopen("../WWW2017Code/rates_lj.txt", "r");
	while (fscanf(ifp, "%d%lf", &x, &y) != EOF)
		r[x] = y;
	fclose(ifp);
	FILE *ofp = fopen("crRatio.txt", "w");
	for (int i = 0; i < N; ++i)
		fprintf(ofp, "%.9f\n", c[i] / r[i]);
	fclose(ofp);
	return 0;
}
