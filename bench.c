#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include <sys/resource.h>
#include <sys/time.h>
#ifdef __cplusplus
#include <algorithm>
#endif

#ifndef STEP0
#define STEP0 1
#endif

#define KSORT2_CHECK(prefix, type_t, __sort_lt) \
	int prefix##_is_sorted(type_t *st, type_t *en) \
	{ \
		type_t *p; \
		for (p = st + 1; p < en; ++p) \
			if (__sort_lt(*p, *(p-1))) \
				return 0; \
		return 1; \
	}

#define KSORT2_INSERTION(prefix, type_t, __sort_lt) \
	void prefix##_insertion_sort(type_t *beg, type_t *end) \
	{ \
		type_t *i; \
		for (i = beg + 1; i < end; ++i) { \
			if (__sort_lt(*i, *(i-1))) { \
				type_t *j, tmp = *i; \
				for (j = i; j > beg && __sort_lt(tmp, *(j-1)); --j) \
					*j = *(j - 1); \
				*j = tmp; \
			} \
		} \
	}

#define KSORT2_MERGE(prefix, type_t, __sort_lt) \
	void prefix##_merge_sort(type_t *st, type_t *en, type_t *tmp) \
	{ \
		type_t *a2[2], *a, *b; \
		size_t n = en - st, step = STEP0; \
		int curr; \
		if (step > 1) { /* insertion sort to replace first few rounds of merge sort */ \
			for (a = st; a < en - step; a += step) \
				prefix##_insertion_sort(a, a + step); \
			prefix##_insertion_sort(a, en); \
		} \
		a2[0] = st, a2[1] = tmp; \
		for (curr = 0; step < n; step += step) { \
			size_t i; \
			a = a2[curr]; b = a2[1-curr]; \
			for (i = 0; i < n; i += step<<1) { \
				type_t *p, *j, *k; \
				type_t *ea = n < i + step? a + n : a + i + step; \
				type_t *eb = n < i + step? a : a + (n < i + (step<<1)? n : i + (step<<1)); \
				j = a + i; k = a + i + step; p = b + i; \
				while (j < ea && k < eb) { \
					if (__sort_lt(*k, *j)) *p++ = *k++; \
					else *p++ = *j++; \
				} \
				while (j < ea) *p++ = *j++; \
				while (k < eb) *p++ = *k++; \
			} \
			curr = 1 - curr; \
		} \
		if (curr == 1) memcpy(st, tmp, n * sizeof(*st)); \
	}

static int int_lt(const void *a, const void *b)
{
	return (*(const int*)a < *(const int*)b);
}

typedef int func_lt(const void *a, const void *b);

void insertion_sort_void(size_t n, size_t len, void *a_, func_lt lt)
{
	uint8_t tmp[len], *a = (uint8_t*)a_;
	size_t i, j;
	for (i = len; i < n * len; i += len) {
		if (lt(&a[i], &a[i-len])) {
			memcpy(tmp, &a[i], len);
			for (j = i; j > 0 && lt(tmp, &a[j-len]); j -= len)
				memcpy(&a[j], &a[j-len], len);
			memcpy(&a[j], tmp, len);
		}
	}
}

void merge_sort_void(size_t n, size_t len, void *array, void *tmp, func_lt lt)
{
	uint8_t *a2[2], *a = (uint8_t*)array, *b;
	size_t i, step = STEP0;
	int curr;
	if (step > 1) {
		for (i = 0; i < n - step; i += step)
			insertion_sort_void(step, len, &a[i * len], lt);
		insertion_sort_void(n - i, len, &a[i * len], lt);
	}
	a2[0] = (uint8_t*)array, a2[1] = (uint8_t*)tmp;
	for (curr = 0; step < n; step += step) {
		a = a2[curr]; b = a2[1-curr];
		for (i = 0; i < n; i += step<<1) {
			uint8_t *p, *j, *k, *ea, *eb;
			if (n < i + step) {
				ea = a + n * len; eb = a;
			} else {
				ea = a + (i + step) * len;
				eb = a + (n < i + (step<<1)? n : i + (step<<1)) * len;
			}
			j = a + i * len; k = a + (i + step) * len; p = b + i * len;
			while (j < ea && k < eb) {
				if (lt(k, j)) {
					memcpy(p, k, len);
					p += len, k += len;
				} else {
					memcpy(p, j, len);
					p += len, j += len;
				}
			}
			while (j < ea) {
				memcpy(p, j, len);
				p += len, j += len;
			}
			while (k < eb) {
				memcpy(p, k, len);
				p += len, k += len;
			}
		}
		curr = 1 - curr;
	}
	if (curr == 1) memcpy(array, tmp, n * len);
}

#define ks_generic_lt(a, b) ((a) < (b))
KSORT2_INSERTION(sc, int32_t, ks_generic_lt)
KSORT2_MERGE(sc, int32_t, ks_generic_lt)
KSORT2_CHECK(sc, int32_t, ks_generic_lt)

#define MALLOC(type, cnt) ((type*)malloc((cnt) * sizeof(type)))

static inline uint64_t splitmix64(uint64_t x)
{
	uint64_t z = (x += 0x9E3779B97F4A7C15ULL);
	z = (z ^ (z >> 30)) * 0xBF58476D1CE4E5B9ULL;
	z = (z ^ (z >> 27)) * 0x94D049BB133111EBULL;
	return z ^ (z >> 31);
}

double cputime(void)
{
	struct rusage r;
	getrusage(RUSAGE_SELF, &r);
	return r.ru_utime.tv_sec + r.ru_stime.tv_sec + 1e-6 * (r.ru_utime.tv_usec + r.ru_stime.tv_usec);
}

int main(int argc, char *argv[])
{
	int32_t i, N = 50000001, *a, *tmp;
	uint64_t x;
	double t;

	a = MALLOC(int32_t, N);
	tmp = MALLOC(int32_t, N);

	for (i = 0, x = 1; i < N; ++i) {
		a[i] = (int32_t)x;
		x = splitmix64(x);
	}
	t = cputime();
	sc_merge_sort(a, a + N, tmp);
	t = cputime() - t;
	printf("merge sort macro [%d]: %lf sec\n", sc_is_sorted(a, a + N), t);

	for (i = 0, x = 1; i < N; ++i) {
		a[i] = (int32_t)x;
		x = splitmix64(x);
	}
	t = cputime();
	merge_sort_void(N, sizeof(*a), a, tmp, int_lt);
	t = cputime() - t;
	printf("merge sort void [%d]: %lf sec\n", sc_is_sorted(a, a + N), t);

#ifdef __cplusplus
	for (i = 0, x = 1; i < N; ++i) {
		a[i] = (int32_t)x;
		x = splitmix64(x);
	}
	t = cputime();
	std::stable_sort(a, a + N);
	t = cputime() - t;
	printf("std::stable_sort [%d]: %lf sec\n", sc_is_sorted(a, a + N), t);
#endif

	free(tmp); free(a);
	return 0;
}
