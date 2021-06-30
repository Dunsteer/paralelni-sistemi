v = start;
sum = 0;
for (int i = 0; i < N; i++) {
	sum = sum + f(v);
	v = v + step;
}

//paralelno

sum = 0;

#pragma omp parallel for reduction(+:sum)
for (int i = 0; i < N; i++) {
	v = start + i * step;
	sum = sum + f(v);
}